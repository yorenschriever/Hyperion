#pragma once

#include <inttypes.h>
#include <math.h>

class LUT 
{
public:

    int Dimension;
    uint16_t** luts;
};

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

class ColourCorrectionLUT  : public LUT
{
    public:
        ColourCorrectionLUT(float gammaCorrection, int maxValue, uint8_t R, uint8_t G, uint8_t B) 
        {
            Dimension = 3;
            luts = new uint16_t*[Dimension];
            for (int i=0; i<256; i++){
                lutR[i] = pow((float)i*R / 255. / 255., gammaCorrection) * maxValue;
                lutG[i] = pow((float)i*G / 255. / 255., gammaCorrection) * maxValue;
                lutB[i] = pow((float)i*B / 255. / 255., gammaCorrection) * maxValue;
            }
            
            luts[0] = lutR;
            luts[1] = lutG;
            luts[2] = lutB;
        }
    private:
        uint16_t lutR[256];
        uint16_t lutG[256];
        uint16_t lutB[256];
};

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