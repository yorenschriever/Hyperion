#pragma once
#include "pattern.h"

class LayeredSinPattern : public LayeredPattern<Monochrome>
{
    int direction;

    Transition transition = Transition(
        400,none,0,
        400,none,0
    );

public:
    LayeredSinPattern(int direction=1){
        this->direction=direction;
    }

    inline void Calculate(Monochrome *pixels, int width, bool active) override
    {
        if (!transition.Calculate(active))
            return; //the fade out is done. we can skip calculating pattern data

        for (int index = 0; index < width; index++)
            pixels[index] += Monochrome((uint8_t)(127.f + 127.f * cos(float(index) / float(width) * (2*3.1415) - direction * millis() / 250.))) * transition.getValue(index,width);
    }

};

class LayeredSlowStrobePattern : public LayeredPattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool active) override
    {
        if (!active)
            return;

        for (int index = 0; index < width; index++)
            pixels[index] = millis() % 100 < 25 ? 255 : 0;
    }
};



class LayeredBlinderPattern : public LayeredPattern<Monochrome>
{
    Transition transition = Transition(
        400,fromLeft,600,
        400,fromCenter,600
    );

    inline void Calculate(Monochrome *pixels, int width, bool active) override
    {
        if (!transition.Calculate(active))
            return; //the fade out is done. we can skip calculating pattern data

        for (int index = 0; index < width; index++)
            pixels[index] += Monochrome(255) * transition.getValue(index,width);
    }
};

