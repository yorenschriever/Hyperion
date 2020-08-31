#pragma once
#include "pattern.h"

class SinPattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = (uint8_t)(127.f + 127.f * cos(float(index) / float(width) * (2*3.1415) - millis() / 250.));
    }
};

class SawPattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = millis() / 50 + index * 2;
    }
};

class RandomPattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = random(0, 100) < 10 ? 255 : 0;
    }
};

class RandomPattern2 : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = random(0, 100) < 1 ? 255 : 0;
    }
};

class RandomFadePattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
        {
            uint8_t lastPixelvalue = pixels[index].L;
            if (random(0, 100) < 1)
                pixels[index] = 255;
            else
                pixels[index] = max(lastPixelvalue - 10, 0);
        }
    }
};

// class MeteorPattern : public Pattern<Monochrome>
// {
//     inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
//     {
//         for (int index = 0; index < width; index++)
//         {
//             int lastSelfvalue = pixels[index].L;
//             int lastNeighbourvalue = pixels[(index + 1) % width].L;
//             if (random(0, 300) < 1)
//                 pixels[index] = 255;
//             else
//                 pixels[index] = max(max(lastSelfvalue - 25, lastNeighbourvalue - 10), 0);
//         }
//     }
// };

class SlowStrobePattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = millis() % 100 < 25 ? 255 : 0;
    }
};

class FastStrobePattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = millis() % 50 < 25 ? 255 : 0;
    }
};


class TemperatureTestPattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = 255*Rotary::isButtonPressed();
    }
};

