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

//This colour correction luts follows the colour order of the output. I.e.: If You use GRB as output colour,
//then colour1 will be G. 
class ColourCorrectionLUT  : public LUT
{
    public:
        ColourCorrectionLUT(float gammaCorrection, int maxValue, uint8_t Colour1, uint8_t Colour2, uint8_t Colour3) 
        {
            Dimension = 3;
            luts = new uint16_t*[Dimension];
            for (int i=0; i<256; i++){
                lut1[i] = pow((float)i*Colour1 / 255. / 255., gammaCorrection) * maxValue;
                lut2[i] = pow((float)i*Colour2 / 255. / 255., gammaCorrection) * maxValue;
                lut3[i] = pow((float)i*Colour3 / 255. / 255., gammaCorrection) * maxValue;
            }
            
            luts[0] = lut1;
            luts[1] = lut2;
            luts[2] = lut3;
        }
    private:
        uint16_t lut1[256];
        uint16_t lut2[256];
        uint16_t lut3[256];
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