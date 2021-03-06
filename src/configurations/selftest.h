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
#include "inputs/patternInput.h"
#include "patterns/rgbPatterns.h"
#include "patterns/monochromePatterns.h"
#include "luts/colourCorrectionLut.h"
#include "luts/incandescentLut.h"
#include "configurationStruct.h"

//I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
//http://fastled.io/docs/3.1/group___color_enums.html
//note the different order, fastled uses RGB, luts are in ouput order (GRB)
LUT* NeopixelLut = new ColourCorrectionLUT(1.5,255, 176, 255, 240); 
LUT* IncandescentLut = new IncandescentLUT(2.5, 4096, 400);

void LoadConfiguration()
{
    Configuration.hostname = "selftest";

    Configuration.pipes = {

    new Pipe(
        new PatternInput<RGB>(16, new RainbowPattern()),
        new NeopixelOutput<Kpbs800>(1),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    new Pipe(
        new PatternInput<RGB>(16, new RainbowPattern()),
        new NeopixelOutput<Kpbs800>(2),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    new Pipe(
        new PatternInput<RGB>(16, new RainbowPattern()),
        new NeopixelOutput<Kpbs800>(3),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    new Pipe(
        new PatternInput<RGB>(16, new RainbowPattern()),
        new NeopixelOutput<Kpbs800>(4),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    new Pipe(
        new PatternInput<RGB>(16, new RainbowPattern()),
        new NeopixelOutput<Kpbs800>(5),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    new Pipe(
        new PatternInput<RGB>(16, new RainbowPattern()),
        new NeopixelOutput<Kpbs800>(6),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    new Pipe(
        new PatternInput<RGB>(16, new RainbowPattern()),
        new NeopixelOutput<Kpbs800>(7),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    new Pipe(
        new PatternInput<RGB>(16, new RainbowPattern()),
        new NeopixelOutput<Kpbs800>(8),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),
        
    new Pipe(
        new PatternInput<Monochrome>(12, new SinPattern()),
        new PWMOutput(),
        Pipe::transfer<Monochrome,Monochrome12>,
        IncandescentLut
    ),

    //puts output in dmx output
    new Pipe(
        new PatternInput<RGB>(16, new RainbowPattern()),
        new DMXOutput(1)
    ),

    //initializes dmx input, so the indicator will light up
    new Pipe(
        new DMXInput(1),
        new DMXOutput(1)
    ),

    //initializes midi input, so the indicator will light up
    new Pipe(
        new ApcminiInput<RGB>(
            16, //width of the pattern, in pixels
            2,  //button column on the apc to listen to (0-7)
            //the patterns to attach to the buttons
            new Pattern<RGB>*[8]{
                new RainbowPattern(), 
                new ColourOrderPattern(), 
                new MixingPattern(), 
                new AnimatedMixingPattern(), 
                new RainbowPattern(), 
                new RainbowPattern(), 
                new RainbowPattern(), 
                new RainbowPattern()
            }),
        new DMXOutput(1)
    )
    };

}
