#pragma once

#include "input.h"

//NullInput never provides any data. You can use this for debugging,
//or use it together with SwitchableInput
class NullInput : public Input
{
public:
    virtual void begin() {}

    virtual int loadData(uint8_t *dataPtr)
    {
        return 0;
    }
};