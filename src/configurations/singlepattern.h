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
#include "patterns/rgbPatterns.h"
#include "patterns/monochromePatterns.h"
#include "luts/colourCorrectionLut.h"
#include "luts/gammaLut.h"
#include "colours.h"
#include "pipe.h"

const char* HostName = "SinglePattern";

//I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
//http://fastled.io/docs/3.1/group___color_enums.html
//note the different order, fastled uses RGB, luts are in ouput order (GRB)
LUT* NeopixelLut = new ColourCorrectionLUT(1.5,255, 176, 255, 240); 
LUT* LedLut = new GammaLUT(2,4096); 

Pipe pipes[] = {

    // Pipe(
    //     new PatternInput<RGB>(16,new AnimatedMixingPattern()),
    //     new NeopixelOutput<Kpbs800>(1),
    //     Pipe::transfer<RGB,GRB>,
    //     NeopixelLut
    // )
    
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
/*

RandomFadePattern()
SlowStrobePattern();
FastStrobePattern
BeatAllFadePattern
BeatShakePattern
BeatSingleFadePattern
BeatSinglePattern
GlitchPattern
OnPattern
SinPattern
BeatSomeFadePattern
*/

    // Pipe(
    //     new PatternInput<Monochrome>(12,new GlitchPattern()),
    //     new UDPOutput("strobes.local",9619,500),
    //     Pipe::transfer<Monochrome,Monochrome12>
    // )

    // Pipe(
    //     new PatternInput<Monochrome>(12,new BeatAllFadePattern()),
    //     new PWMOutput(1500),
    //     Pipe::transfer<Monochrome,Monochrome12>
    // )

    Pipe(
        new PatternInput<Monochrome>(1,new SinPattern()),
        new PWMOutput(1),
        Pipe::transfer<Monochrome,Monochrome12>
    ),

    Pipe(
        new PatternInput<Monochrome>(3,new SinPattern()),
        new PWMOutput(7),
        Pipe::transfer<Monochrome,Monochrome12>
    ),

    Pipe(
        new PatternInput<Monochrome>(12,new SinPattern()),
        new PWMOutput(11),
        Pipe::transfer<Monochrome,Monochrome12>
    ),

    // Pipe(
    //     new PatternInput<RGB>(32,new BPMFillPattern()),
    //     new NeopixelOutput<Kpbs800>(2),
    //     Pipe::transfer<RGB, GRB>,
    //     NeopixelLut),

    // Pipe(
    //     new PatternInput<RGB>(1, 
    //     new BPMIndicatorPattern()
    //     //new RainbowPattern()
    //     ),
    //     new RotaryOutput()
    // ),
};