#pragma once

#include <inttypes.h>

class Hysteresis
{
    const int hysteresisWidth = 2;

public:
    bool setValue(uint8_t newvalue)
    {
        if (newvalue > value && pickup <= value)
        {
            value = newvalue;
            pickup = value - hysteresisWidth;
            return true;
        }
        else if (newvalue < pickup && pickup <= value)
        {
            value = newvalue;
            pickup = value - hysteresisWidth;
            return true;
        }
        else if (newvalue < value && pickup >= value)
        {
            value = newvalue;
            pickup = value + hysteresisWidth;
            return true;
        }
        else if (newvalue > pickup && pickup >= value)
        {
            value = newvalue;
            pickup = value + hysteresisWidth;
            return true;
        }
        return false;
    }

    uint8_t getValue()
    {
        return value;
    }

private:
    uint8_t value;
    uint8_t pickup;
};
