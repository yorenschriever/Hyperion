#pragma once

#include "Output.h"
#include <Arduino.h>
#include <Wire.h>
#include "hardware/pca9685/pca9685.h"

#define PCA9685_LED0_ON_L 0x06 

const byte PWM_LED_ORDER[] = {9, 10, 8, 2, 1, 0, 7, 6, 5, 4, 3, 11}; //numbering on backside, pcb has a different numbering

//This class controls the 12 pwm outputs on the back of the device
class PWMOutput : public Output
{
public:
    //PWM frequency in Hz, choose 100 for incandescent and 1500 for led
    //A higher frequency looks better, because it produces less flicker.
    //This works great for leds, but if currents are high (indandescent)
    //The mosfets will heat up quickly. Also the produced noise will be
    //more audible. Incandescent lamps have a slow response, so you wont
    //see the flicker as much. Setting the pwm frequency lower is better
    //in that case.
    PWMOutput(int frequency) 
    {
        this->frequency = frequency;
    }

    //index and size are in bytes
    void SetData(uint8_t *data, int size, int index)
    {
        //this memcpy is not aware that it is actually copying uint16_ts byte by byte
        //(provided you actually converted to Monochrome12 format)
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
        PCA9685::Initialize();
        PCA9685::SetFrequency(frequency);

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
    int frequency;

    volatile boolean busy = false;
    xSemaphoreHandle dirtySemaphore;

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

                PCA9685::WritePWMData(buffer,sizeof(buffer),10);

                this2->busy = false;
            }
            delay(5);
        }
    }
};