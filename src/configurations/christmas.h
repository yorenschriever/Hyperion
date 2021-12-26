#pragma once

#include "pipe.h"
#include "colours.h"

#include "outputs/pwmOutput.h"
#include "outputs/dmxOutput.h"
#include "outputs/apcminiOutput.h"
#include "outputs/neopixelOutput.h"
#include "inputs/udpInput.h"
#include "inputs/apcminiInput.h"
#include "inputs/dmxInput.h"
#include "inputs/fallbackInput.h"
#include "inputs/patternInput.h"
#include "inputs/bufferInput.h"
#include "inputs/websocketInput.h"
#include "patterns/rgbPatterns.h"
#include "patterns/monochromePatterns.h"
#include "patterns/ledstripPatterns.h"
#include "luts/colourCorrectionLut.h"
#include "luts/incandescentLut.h"
#include "configurationStruct.h"
#include "inputs/layeredPatternInput.h"
#include "wifipassword.h"


//I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
//http://fastled.io/docs/3.1/group___color_enums.html
//note the different order, fastled uses RGB, luts are in ouput order (GRB)
LUT *NeopixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 255);

const int channelCount = 8;
const int sizes[channelCount] = {100,100,150,150,150,150,100,200};

void LoadConfiguration()
{
    Params::primaryColour = RGBA(255, 100, 0, 255);
    Params::secondaryColour = RGBA(100, 100, 75, 255);
    Params::velocity = 0.3;

    for (int i = 0; i < channelCount; i++)
    {
        Configuration.pipes.push_back(
            new Pipe(
                new FallbackInput(
                    new UDPInput(9611 + i),
                    new PatternInput<RGB>(sizes[i], new GlowPulsePattern())),
                new NeopixelOutput<Kpbs800>(1 + i),
                Pipe::transfer<RGB, GRB>,
                NeopixelLut));
    }

}
