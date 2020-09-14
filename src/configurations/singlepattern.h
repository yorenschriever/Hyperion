#pragma once

#include "outputs/dmxOutput.h"
#include "outputs/apcminiOutput.h"
#include "outputs/neopixelOutput.h"
#include "outputs/pwmOutput.h"
#include "outputs/udpOutput.h"
#include "outputs/rotaryOutput.h"
#include "metaoutputs/temperatureControlledOutput.h"
#include "inputs/udpInput.h"
#include "inputs/apcminiInput.h"
#include "inputs/dmxInput.h"
#include "inputs/patternInput.h"
#include "inputs/layeredPatternInput.h"
#include "patterns/rgbPatterns.h"
#include "patterns/ledstripPatterns.h"
#include "patterns/movingheadPatterns.h"
#include "patterns/monochromePatterns.h"
#include "luts/colourCorrectionLut.h"
#include "luts/gammaLut.h"
#include "colours.h"
#include "pipe.h"
#include "configurationStruct.h"

//I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
//http://fastled.io/docs/3.1/group___color_enums.html
//note the different order, fastled uses RGB, luts are in ouput order (GRB)
LUT* NeopixelLut = new ColourCorrectionLUT(1.5,255, 176, 255, 240); 
LUT* LedLut = new GammaLUT(2,4096); 

void LoadConfiguration()
{

    Configuration.hostname = "singlepattern";
    Configuration.pwmFrequency = 1500;

    Configuration.pipes = {
        Pipe(
            new LayeredPatternInput<MovingHead>(3,new MovingheadPatterns::TestPattern()),
            new DMXOutput(30),
            Pipe::transfer<MovingHead,Miniwash7>
        )
    };

}

