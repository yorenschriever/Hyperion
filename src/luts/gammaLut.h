#pragma once
#include "lut.h"

//simple LUT that applies gamma correction only
class GammaLUT : public LUT
{
    public:
        GammaLUT(float gammaCorrection, int maxValue) 
        {
            Dimension = 1;
            luts = new uint16_t*[Dimension];
            for (int i=0; i<256; i++)
                lut[i] = pow((float)i / 255., gammaCorrection) * maxValue;
            luts[0] = lut;
        }
    private:
        uint16_t lut[256];
};