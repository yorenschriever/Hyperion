#pragma once

#include "Output.h"
#include <Arduino.h>
#include "hardware/rotary/rotary.h"

class RotaryOutput : public Output
{
public:

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
        if (millis() - lastFrame < maxFps)
            return false; //limit the framerate to minimize the effect of temporal quantization (because we measure time in ms)
        return !this->busy;
    }

    void Show()
    {
        this->busy = true;
        xSemaphoreGive(dirtySemaphore);
        lastFrame = millis();
    }

    void Begin() override
    {
        PCA9685::Initialize();
        dirtySemaphore = xSemaphoreCreateBinary();
        xTaskCreatePinnedToCore(SendAsync, "SendRotaryAsync", 10000, this, 6, NULL, 1);
        lastFrame = millis();
    }

    void Clear()
    {
        for (int i = 0; i < 3; i++)
           this->values[i] = 0;
    }

    void SetLength(int len) override
    {
        //do nothing.
    }

private:
    uint8_t values[3];
    volatile boolean busy = false;
    xSemaphoreHandle dirtySemaphore;
    unsigned long lastFrame = 0;
    int maxFps = 30;

    static void SendAsync(void *param) 
    {
        RotaryOutput *this2 = (RotaryOutput *)param;
        while (true)
        {
            if (xSemaphoreTake(this2->dirtySemaphore, 0)) //wait for show() to be called
            {
                Rotary::setColour(RGB(this2->values[0],this2->values[1],this2->values[2]));
                this2->busy = false;
            }
            delay(5);
        }
    }
};