#pragma once

#include <algorithm>
#include <cstring>
#include "input.h"
#include "Arduino.h"

class SwitchableInput : public Input
{

public:
    using SwitchFunc = boolean (*)(); 

    //input A, input when midi is avaialble
    //input B, fallback when no midi is connected
    //SwitchFunc, function returning a boolean to select input A or B
    SwitchableInput(Input *inputA, Input *inputB, SwitchFunc switchFunc)
    {
        this->inputA = inputA;
        this->inputB = inputB;
        this->switchFunc = switchFunc;
    }

    virtual void begin()
    {
        inputA->begin();
        inputB->begin();
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        return switchFunc() ? inputA->loadData(dataPtr) : inputB->loadData(dataPtr);
    }

    virtual void resetFrameCount(){
        this->inputA->resetFrameCount();
        this->inputB->resetFrameCount();
    }

    virtual int getUsedFramecount() { return switchFunc() ? inputA->getUsedFramecount() : inputB->getUsedFramecount(); }
    virtual int getMissedFrameCount() { return switchFunc() ? inputA->getMissedFrameCount() : inputB->getMissedFrameCount(); }
    virtual int getTotalFrameCount() { return switchFunc() ? inputA->getTotalFrameCount() : inputB->getTotalFrameCount(); }

private:
    Input *inputA;
    Input *inputB;
    SwitchFunc switchFunc;
};