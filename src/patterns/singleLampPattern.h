#pragma once
#include "pattern.h"

class SingleLampPattern : public LayeredPattern<Monochrome>
{
private:
    int activeIndex = 0;

public:
    SingleLampPattern(int activeIndex)
    {
        this->activeIndex = activeIndex;
    }

    inline void Calculate(Monochrome *pixels, int width, bool active) override
    {
        if (active)
            for (int index = 0; index < width; index++)
                if (index == this->activeIndex || this->activeIndex == -1)
                    pixels[index] = 255;
    }
};