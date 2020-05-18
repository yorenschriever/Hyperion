#include "pca9685.h"

#include "../sharedResources.h"

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

void PCA9685::Initialize()
{
    if (isInitialized)
        return;

    _i2c = &Wire;
    _i2c->begin(SDAPIN, SCLPIN);
    _i2c->setClock(1000000);
    Reset();

    SetFrequency(1500);

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