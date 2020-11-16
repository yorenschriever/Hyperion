#pragma once

#include <algorithm>
#include <cstring>
#include "input.h"
#include "Arduino.h"

//FallbackInput gets its input from one of the available sources
class FallbackInput : public Input
{

public:
    //input A, high prio input
    //input B, fallback
    //timeout: the delay before switching to B after a stops giving a signal, milliseconds
    FallbackInput(Input *inputA, Input *inputB, int timeout = 500)
    {
        this->inputA = inputA;
        this->inputB = inputB;
        this->timeout = timeout;
        this->aLastValid = 0;
    }

    virtual void begin()
    {
        inputA->begin();
        inputB->begin();
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        int result = inputA->loadData(dataPtr);
        if (result > 0)
        {
            aLastValid = millis();
            return result;
        }

        if (millis() - aLastValid > timeout)
            return inputB->loadData(dataPtr);

        return 0;
    }

private:
    Input *inputA;
    Input *inputB;
    int timeout;
    unsigned long aLastValid;
};