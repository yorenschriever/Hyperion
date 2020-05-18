#pragma once
#include "lut.h"

//ColourCorrectionLut applies the same gamma correction to all channels, and also
//gives you the option to scale the output of the 3 channels. You can use this if
//get a better white balance.
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