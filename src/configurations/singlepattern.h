#pragma once

#include "outputs/dmxOutput.h"
#include "outputs/apcminiOutput.h"
#include "outputs/neopixelOutput.h"
#include "outputs/pwmOutput.h"
#include "metaoutputs/temperatureControlledOutput.h"
#include "inputs/udpInput.h"
#include "inputs/apcminiInput.h"
#include "inputs/dmxInput.h"
#include "inputs/patternInput.h"
#include "patterns/rgbPatterns.h"
#include "patterns/monochromePatterns.h"
#include "luts/colourCorrectionLut.h"
#include "luts/gammaLut.h"
#include "colours.h"

const char* HostName = "SinglePattern";

//I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
//http://fastled.io/docs/3.1/group___color_enums.html
//note the different order, fastled uses RGB, luts are in ouput order (GRB)
LUT* NeopixelLut = new ColourCorrectionLUT(1.5,255, 176, 255, 240); 
LUT* LedLut = new GammaLUT(2,4096); 

Pipe pipes[] = {

    Pipe(
        new PatternInput<RGB>(16,new AnimatedMixingPattern()),
        new NeopixelOutput<Kpbs800>(1),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut
    )
    
    // Pipe(
    //     new PatternInput<Monochrome>(1,new TemperatureTestPattern()),
    //     //new PWMOutput(150),
    //     new TemperatureControlledOutput<Monochrome12>(
    //         new PWMOutput(1500),
    //         0.77, 
    //         0.0058,  
    //         60),
    //     Pipe::transfer<Monochrome,Monochrome12>,
    //     LedLut
    // )
};