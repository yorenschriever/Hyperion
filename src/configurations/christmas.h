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
#include "patterns/rgbPatterns.h"
#include "patterns/monochromePatterns.h"
#include "patterns/ledstripPatterns.h"
#include "luts/colourCorrectionLut.h"
#include "luts/incandescentLut.h"
#include "configurationStruct.h"
#include "inputs/layeredPatternInput.h"

//I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
//http://fastled.io/docs/3.1/group___color_enums.html
//note the different order, fastled uses RGB, luts are in ouput order (GRB)
LUT* NeopixelLut = new ColourCorrectionLUT(1.5,255,255, 255, 255); 

void LoadConfiguration()
{
    Params::primaryColour = RGBA(255,100,0,255);
    Params::velocity = 0.3;

    Configuration.hostname = "hyperion";

    Configuration.pipes = {

    new Pipe(
        new FallbackInput(
            new UDPInput(9611),
            new LayeredPatternInput<RGBA>(150, new LedStrip::GlowPattern())
        ),
        new NeopixelOutput<Kpbs800>(1),
        Pipe::transfer<RGBA,GRB>,
        NeopixelLut),

    new Pipe(
        new FallbackInput(
            new UDPInput(9612),
            new LayeredPatternInput<RGBA>(150, new LedStrip::GlowPattern())
        ),
        new NeopixelOutput<Kpbs800>(2),
        Pipe::transfer<RGBA,GRB>,
        NeopixelLut),

    new Pipe(
        new FallbackInput(
            new UDPInput(9613),
            new LayeredPatternInput<RGBA>(150, new LedStrip::GlowPattern())
        ),
        new NeopixelOutput<Kpbs800>(3),
        Pipe::transfer<RGBA,GRB>,
        NeopixelLut),

    new Pipe(
        new FallbackInput(
            new UDPInput(9614),
            new LayeredPatternInput<RGBA>(150, new LedStrip::GlowPattern())
        ),
        new NeopixelOutput<Kpbs800>(4),
        Pipe::transfer<RGBA,GRB>,
        NeopixelLut),

    new Pipe(
        new FallbackInput(
            new UDPInput(9615),
            new LayeredPatternInput<RGBA>(150, new LedStrip::GlowPattern())
        ),
        new NeopixelOutput<Kpbs800>(5),
        Pipe::transfer<RGBA,GRB>,
        NeopixelLut),

    new Pipe(
        new FallbackInput(
            new UDPInput(9616),
            new LayeredPatternInput<RGBA>(150, new LedStrip::GlowPattern())
        ),
        new NeopixelOutput<Kpbs800>(6),
        Pipe::transfer<RGBA,GRB>,
        NeopixelLut),

    new Pipe(
        new FallbackInput(
            new UDPInput(9617),
            new LayeredPatternInput<RGBA>(150, new LedStrip::GlowPattern())
        ),
        new NeopixelOutput<Kpbs800>(7),
        Pipe::transfer<RGBA,GRB>,
        NeopixelLut),

    new Pipe(
        new FallbackInput(
            new UDPInput(9618),
            new LayeredPatternInput<RGBA>(150, new LedStrip::GlowPattern())
        ),
        new NeopixelOutput<Kpbs800>(8),
        Pipe::transfer<RGBA,GRB>,
        NeopixelLut),
        
    };

}
