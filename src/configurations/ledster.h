#pragma once

#include "outputs/spiOutput.h"
#include "inputs/patternInput.h"
#include "patterns/rgbPatterns.h"
#include "luts/colourCorrectionLut.h"
#include "colours.h"
#include "pipe.h"
#include "configurationStruct.h"

LUT* PixelLut = new ColourCorrectionLUT(1.5, 255,255, 255, 240); 

void LoadConfiguration()
{

    Configuration.hostname = "ledster";
    
    Configuration.pipes = {

        new Pipe( 
            new PatternInput<RGB>(
                512, //number of leds
                new RainbowPattern()
            ),
            new SpiOutput(0,1,500000)
            //Pipe::transfer<RGBA, RGB>,
            //PixelLut
        ),

    };

}

