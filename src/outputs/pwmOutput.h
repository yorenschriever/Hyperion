#pragma once

#include "Output.h"
#include <Arduino.h>
#include <Wire.h>

#include "semaphores.h"

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

const byte PWM_LED_ORDER[] = {9, 10, 8, 2, 1, 0, 7, 6, 5, 4, 3, 11}; //numbering on backside, pcb has a different numbering

#define SDAPIN 13
#define SCLPIN 16

//This class controls the 12 pwm outputs on the back of the device
class PWMOutput : public Output
{
public:
    //PWM frequency in Hz, choose 100 for incandescent and 1500 for led
    //A higher frequency looks better, because it produces less flicker.
    //This works great for leds, but if currents are high (indandescent)
    //The mosfets will heat up quickly. Also the produced noise will be
    //more audible. Indandescent lamps have a slow response, so you wont
    //see the flicker as mucht. Setting the pwm frequency lower is better
    //in that case.
    PWMOutput(int frequency) : _i2c(&Wire)
    {
        this->frequency = frequency;
    }

    //index and size are in bytes
    void SetData(uint8_t *data, int size, int index)
    {
        //this memcpy is not aware that is it actually copying uint16_ts byte by byte
        //(provided you actually converted to RGB12 format)
        int copylength = min(size, (int)sizeof(this->values) - index);
        if (copylength > 0)
            memcpy((uint8_t *)this->values + index, data, copylength);
    }

    boolean Ready()
    {
        return !this->busy;
    }

    void Show()
    {
        this->busy = true;
        xSemaphoreGive(dirtySemaphore);
    }

    void Begin() override
    {

        _i2c->begin(SDAPIN, SCLPIN);
        _i2c->setClock(1000000);
        this->reset();

        //todo make setting
        //this->setPWMFreq(1500);
        //this->setPWMFreq(100);
        setPWMFreq(frequency);

        dirtySemaphore = xSemaphoreCreateBinary();
        xTaskCreatePinnedToCore(SendAsync, "SendPWMAsync", 10000, this, 6, NULL, 1);
    }

    void Clear()
    {
        for (int i = 0; i < 12; i++)
            this->values[i] = 0;
    }

    void SetLength(int len) override
    {
        //do nothing.
    }

private:
    uint16_t values[12];
    uint16_t valuesBuf[12];
    TwoWire *_i2c;
    int frequency;

    volatile boolean busy = false;
    xSemaphoreHandle dirtySemaphore;

    void reset()
    {
        write8(PCA9685_MODE1, MODE1_RESTART);
        delay(10);
    }

    void write8(uint8_t addr, uint8_t d)
    {
        _i2c->beginTransmission(PCA9685_I2C_ADDRESS);
        _i2c->write(addr);
        _i2c->write(d);
        _i2c->endTransmission();
    }

    uint8_t read8(uint8_t addr)
    {
        _i2c->beginTransmission(PCA9685_I2C_ADDRESS);
        _i2c->write(addr);
        _i2c->endTransmission();

        _i2c->requestFrom((uint8_t)PCA9685_I2C_ADDRESS, (uint8_t)1);
        return _i2c->read();
    }

    void setPWMFreq(float freq)
    {

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

        uint8_t oldmode = read8(PCA9685_MODE1);
        uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
        write8(PCA9685_MODE1, newmode);                             // go to sleep
        write8(PCA9685_PRESCALE, prescale);                         // set the prescaler
        write8(PCA9685_MODE1, oldmode);
        delay(5);
        // This sets the MODE1 register to turn on auto increment.
        write8(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);
    }

    static void SendAsync(void *param) //todo 2 functions: the background task and the actual sending part
    {
        uint8_t buffer[12 * 4 + 1];
        buffer[0] = PCA9685_LED0_ON_L;
        PWMOutput *this2 = (PWMOutput *)param;
        while (true)
        {
            if (xSemaphoreTake(this2->dirtySemaphore, 0)) //wait for show() to be called
            {

                memcpy(this2->valuesBuf, this2->values, sizeof(this2->valuesBuf));
                int edgepos = 0;

                //todo make this settings
                bool cascadedPWm = false;
                bool pcaCascaded = false;

                unsigned int on, off;

                for (int i = 0; i < 12; i++)
                {

                    //int gammaCorrected = this2->gammaCurve[this2->valuesBuf[i]];
                    int value = this2->valuesBuf[PWM_LED_ORDER[i]];
                    if (value >= 4096)
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
                    buffer[i * 4 + 1] = (on % 0xff);
                    buffer[i * 4 + 2] = (on >> 8);
                    buffer[i * 4 + 3] = (off % 0xff);
                    buffer[i * 4 + 4] = (off >> 8);
                }

                if (xSemaphoreTake(i2cMutex, (TickType_t)10) != pdTRUE)
                {
                    this2->busy = false;
                    continue; //wait wait most 10 ticks (ms) to get the semaphore, otherwise give up
                }

                this2->_i2c->setClock(1000000);
                this2->_i2c->writeTransmission(PCA9685_I2C_ADDRESS, buffer, sizeof(buffer), true);

                xSemaphoreGive(i2cMutex);
                this2->busy = false;
            }
            delay(5);
        }
    }
};