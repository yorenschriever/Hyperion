#pragma once
#include "pattern.h"

class RainbowPattern : public Pattern<RGB> {
    inline void Calculate(RGB* pixels, int width, bool firstFrame) override
    {
        for (int index=0;index<width; index++)
            pixels[index] = Hue(0xFF * index / width + (millis()/10));
    }
};

//You can use this pattern to test the order of the colors in your ledstrip. It will flash Red, green, blue, off.
class ColourOrderPattern : public Pattern<RGB> {
    inline void Calculate(RGB* pixels, int width, bool firstFrame) override
    {
        RGB col;
        int tick = millis()/1000 % 4; 
        if (tick ==0)
          col = RGB(255,0,0);
        if (tick ==1)
          col = RGB(0,255,0);
        if (tick ==2)
          col = RGB(0,0,255);
        if (tick ==3)
          col = RGB(0,0,0);

        for (int index=0;index<width; index++)
            pixels[index] = col;
    }
};

///Demonstation how to mix colours. It displays a fade from red to green
class MixingPattern : public Pattern<RGB> {
    inline void Calculate(RGB* pixels, int width, bool firstFrame) override
    {
        //create colors as RGBA objects, so we can use the Alpha channel to blend
        RGBA red = RGBA(255,0,0 ,255);
        RGBA green = RGBA(0,255,0, 255);

        for (int index=0;index<width; index++)
        {
            //There is a lot going on in the next line:
            //1. green is multiplied with a float, multiplying RGBA with float changes the alpha channel
            //2. the result is added to red. RGBA + RGBA will apply alpha blending to get a resulting RGBA colour
            //3. the RGBA colour is implicitly casted to an RGB colour, because pixels is RGB. The compiler will automatically do this for you
            pixels[index] = red + green * (float(index)/width);
        }
    }
};

///Demonstation how to mix colours. It displays a fade from red to green
class AnimatedMixingPattern : public Pattern<RGB> {
    inline void Calculate(RGB* pixels, int width, bool firstFrame) override
    {
        for (int index=0;index<width; index++)
        {
            RGBA red   = RGBA(255,0,0 , index*16);
            RGBA green = RGBA(0,255,0, millis()/300);
            RGBA blue  = RGBA(0,0,255, millis()/-210);
            pixels[index] = red; // + green + blue;
        }
    }
};