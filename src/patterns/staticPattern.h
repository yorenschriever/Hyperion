#pragma once
#include "pattern.h"

template<class T_COLOUR>
class StaticPattern : public LayeredPattern<T_COLOUR>
{
private:
    T_COLOUR value = T_COLOUR();

public:
    StaticPattern(T_COLOUR value)
    {
        this->value = value;
    }

    inline void Calculate(T_COLOUR *pixels, int width, bool active) override
    {
        if (active)
            for (int index = 0; index < width; index++)
                pixels[index] = value;
    }
};