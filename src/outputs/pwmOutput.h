#pragma once

#include "Output.h"
#include <Arduino.h>
#include <Wire.h>
#include "hardware/pca9685/pca9685.h"

const byte PWM_LED_ORDER[] = {5,  4, 3,10, 9, 8, 7, 6, 2, 0, 1, 11}; 

//This class controls the 12 pwm outputs on the back of the device
class PWMOutput : public Output
{
public:

    //startchannel 1-12
    PWMOutput(uint8_t startChannel=1) 
    {
        this->startChannel = startChannel-1;
    }

    //index and size are in bytes
    void SetData(uint8_t *data, int size, int index)
    {
        if (index%2!=0)
        {
            Debug.println("Cannot write to an uneven byte index. Did you convert to Monochrome12 or RGB12?");
            return;
        }

        if (size%2!=0)
        {
            Debug.println("Cannot write an uneven number of bytes to a pwm output. Did you convert to Monochrome12 or RGB12?");
            return;
        }

        uint8_t copyStartChannel = startChannel + index/2;
        uint8_t requestedNumChannels = size/2;
        uint8_t copyEndChannel = std::min(12, copyStartChannel + requestedNumChannels);
        uint16_t* dataPtr = (uint16_t*) data;

        for (uint8_t channel=copyStartChannel; channel < copyEndChannel; channel++)
        {
            uint16_t value = *(dataPtr++);
            PCA9685::Write(PWM_LED_ORDER[channel],value);
        }
    }

    boolean Ready()
    {
        return PCA9685::Ready();
    }

    void Show()
    {
        //set this static variable to indicate that any dmx instance has updated the data
        groupDirty = true;
    }

    void ShowGroup()
    {
        //read the static varaible to see if any instance has updated the data
        if (!groupDirty)
            return;

        PCA9685::Show();
        groupDirty = false;
    }

    void Begin() override
    {
        PCA9685::Initialize();
    }

    void Clear()
    {
        for (int i = 0; i < 12; i++)
            PCA9685::Write(PWM_LED_ORDER[i],0);
    }

    void SetLength(int len) override
    {
        //do nothing.
    }

private:
    uint8_t startChannel;
    static bool groupDirty;
};

bool PWMOutput::groupDirty = false;