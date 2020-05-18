#pragma once
#include "lut.h"

//incandescentlut is a lookuptable that applies gamma correction, but also 
//applies a base value. This means that even when driven with a value of 0,
//a little power will still be sent to keep the filament just hot enough to 
//quickly respond, but not enough to light up.
class IncandescentLUT  : public LUT
{
    public:
        IncandescentLUT(double gammaCorrection, int maxValue, int incandescentBase) 
        {
            Dimension = 1;
            luts = new uint16_t*[Dimension];
            for (int i = 0; i < 256; i++)
                lut[i] = incandescentBase + (maxValue-incandescentBase) * pow((double)i / 255., gammaCorrection);

            luts[0] = lut;
        }
    private:
        uint16_t lut[256];
};