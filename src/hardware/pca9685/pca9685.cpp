#include "pca9685.h"

#include "../sharedResources.h"
#include "debug.h"

#define SDAPIN 13
#define SCLPIN 16

#define PCA9685_I2C_ADDRESS 0x40 /**< Default PCA9685 I2C Slave Address */
#define PCA9685_MODE1 0x00       /**< Mode Register 1 */
#define PCA9685_MODE2 0x01       /**< Mode Register 2 */
#define MODE1_RESTART 0x80       /**< Restart enabled */
#define PCA9685_LED0_ON_L 0x06   /**< LED0 on tick, low byte*/
#define PCA9685_PRESCALE_MIN 3   /**< minimum prescale value */
#define PCA9685_PRESCALE_MAX 255 /**< maximum prescale value */
#define MODE1_SLEEP 0x10         /**< Low power mode. Oscillator off */
#define MODE1_AI 0x20            /**< Auto-Increment enabled */
#define MODE1_EXTCLK 0x40        /**< Use EXTCLK pin clock */
#define MODE1_RESTART 0x80       /**< Restart enabled */
#define PCA9685_PRESCALE 0xFE    /**< Prescaler for PWM output frequency */

bool PCA9685::isInitialized = false;
TwoWire *PCA9685::_i2c = NULL;
volatile boolean PCA9685::busy = false;
xSemaphoreHandle PCA9685::dirtySemaphore;
uint16_t PCA9685::values[16];
uint16_t PCA9685::valuesBuf[16];

void PCA9685::Initialize()
{
    if (isInitialized)
        return;

    _i2c = &Wire;
    _i2c->begin(SDAPIN, SCLPIN);
    _i2c->setClock(1000000);
    Reset();

    SetFrequency(1500);

    dirtySemaphore = xSemaphoreCreateBinary();
    xTaskCreatePinnedToCore(PCA9685Task, "PCA9685Task", 10000, NULL, 6, NULL, 1);

    isInitialized = true;
}

void PCA9685::Reset()
{
    Write8(PCA9685_MODE1, MODE1_RESTART);
    delay(10);
}

void PCA9685::SetFrequency(int freq)
{
    if (!isInitialized)
        return;

    // Range output modulation frequency is dependant on oscillator
    if (freq < 1)
        freq = 1;
    if (freq > 3500)
        freq = 3500; // Datasheet limit is 3052=50MHz/(4*4096)

    float prescaleval = ((25000000 / (freq * 4096.0)) + 0.5) - 1;
    if (prescaleval < PCA9685_PRESCALE_MIN)
        prescaleval = PCA9685_PRESCALE_MIN;
    if (prescaleval > PCA9685_PRESCALE_MAX)
        prescaleval = PCA9685_PRESCALE_MAX;
    uint8_t prescale = (uint8_t)prescaleval;

    uint8_t oldmode = Read8(PCA9685_MODE1);
    uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
    Write8(PCA9685_MODE1, newmode);                             // go to sleep
    Write8(PCA9685_PRESCALE, prescale);                         // set the prescaler
    Write8(PCA9685_MODE1, oldmode);
    delay(5);
    // This sets the MODE1 register to turn on auto increment.
    Write8(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);
}

void PCA9685::Write8(uint8_t addr, uint8_t d)
{
    _i2c->beginTransmission(PCA9685_I2C_ADDRESS);
    _i2c->write(addr);
    _i2c->write(d);
    _i2c->endTransmission();
}

uint8_t PCA9685::Read8(uint8_t addr)
{
    _i2c->beginTransmission(PCA9685_I2C_ADDRESS);
    _i2c->write(addr);
    _i2c->endTransmission();

    _i2c->requestFrom((uint8_t)PCA9685_I2C_ADDRESS, (uint8_t)1);
    return _i2c->read();
}

bool PCA9685::WritePWMData(uint8_t *data, int length, int maximumWait)
{
    if (!isInitialized)
        return false;

    if (xSemaphoreTake(i2cMutex, (TickType_t)maximumWait) != pdTRUE)
        return false;

    _i2c->setClock(1000000);
    _i2c->writeTransmission(PCA9685_I2C_ADDRESS, data, length, true);

    xSemaphoreGive(i2cMutex);
    return true;
}

bool PCA9685::Ready()
{
    return !busy;
}

void PCA9685::Write(uint8_t index, uint16_t value, bool invert)
{
    if (index > 15)
        return;

    if (value > 4096)
        value = 4096;

    if (invert)
        value = 4096 - value;

    values[index] = value;
}

//untested
// bool PCA9685::Write(uint8_t index, uint16_t* newvalues, uint8_t numValues)
// {
//     int copylength = min(numValues * sizeof(uint16_t), (int)sizeof(PCA9685::values) - index * sizeof(uint16_t));
//     if (copylength > 0)
//         memcpy(values + index*sizeof(uint16_t), newvalues, copylength);
// }

void PCA9685::Show()
{
    busy = true;
    xSemaphoreGive(dirtySemaphore);
}

void PCA9685::PCA9685Task(void *param)
    {
        uint8_t buffer[16 * 4 + 1];
        buffer[0] = PCA9685_LED0_ON_L;
        
        while (true)
        {
            if (xSemaphoreTake(PCA9685::dirtySemaphore, portMAX_DELAY)) //wait for show() to be called
            {
                delay(1);//i dont know why, but this makes it more stable.
                PCA9685::busy=true;
                memcpy(PCA9685::valuesBuf, PCA9685::values, sizeof(PCA9685::valuesBuf));
                int edgepos = 0;

                //todo make this settings
                bool cascadedPWm = false;
                bool pcaCascaded = false;

                unsigned int on, off;

                for (int i = 0; i < 16; i++)
                {
                    int value = PCA9685::valuesBuf[i];
                    if (value==0) 
                    {
                        //this led is fully off, no pwm
                        on = 0;
                        off = 1<<12;
                    } 
                    else if (value >= 4096)
                    {
                        //this led is fully on, no pwm
                        on = 1 << 12;
                        off = 0;
                    }
                    else if (cascadedPWm)
                    {
                        //we start at the off-end of the previous led, to flatten out current
                        on = edgepos;
                        off = (edgepos + value) % 4096;
                        edgepos = off;
                    }
                    else if (pcaCascaded)
                    {
                        on = edgepos;
                        off = (on + value) % 4096;
                        edgepos += 40;
                    }
                    else
                    {
                        //we always start at the start of each pwm cycle, so we are sure to update the value when the led is off
                        on = 0;
                        off = value;
                    }
                    buffer[i * 4 + 1] = (on & 0xFF);
                    buffer[i * 4 + 2] = (on >> 8);
                    buffer[i * 4 + 3] = (off & 0xFF);
                    buffer[i * 4 + 4] = (off >> 8);
                }

                PCA9685::WritePWMData(buffer,sizeof(buffer),40);
                delay(1);
                PCA9685::busy = false;
            }
        }
    }