#pragma once
#include "lut.h"

//laserlut is a lookuptable that applies gamma correction, but also 
//applies a base value to compensate for the non linear response of the laser.
//values of 0 are still 0
class LaserLUT  : public LUT
{
    public:
        LaserLUT(double gammaCorrection = 1.6, int maxValue = 4096, int base = 1500) 
        {
            Dimension = 1;
            luts = new uint16_t*[Dimension];
            for (int i = 0; i < 256; i++)
                lut[i] = base + (maxValue-base) * pow((double)i / 255., gammaCorrection);
            lut[0] = 0;

            luts[0] = lut;
        }
    private:
        uint16_t lut[256];
};