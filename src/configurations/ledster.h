#pragma once

#include "outputs/spiOutput.h"
#include "outputs/udpOutput.h"
#include "inputs/layeredPatternInput.h"
#include "patterns/ledsterPatterns.h"
#include "luts/colourCorrectionLut.h"
#include "colours.h"
#include "pipe.h"
#include "configurationStruct.h"
#include "maps/ledsterMap.h"
#include "patterns/helpers/pixelMap.h"

LUT* PixelLut = new ColourCorrectionLUT(1.5, 255,255, 255, 240); 

void LoadConfiguration()
{
    Params::primaryColour = RGBA(255, 0, 0, 255);
    Params::secondaryColour = RGBA(0, 255, 0, 255);
    Params::highlightColour = RGBA(0, 0, 255, 255);
    Params::velocity = 0.3;

    Configuration.hostname = "ledster";
    
    Configuration.pipes = {

        new Pipe( 
            new LayeredPatternInput<RGBA>(
                481, //number of leds
                //new Ledster::PetalChasePattern(ledsterMap,2500)
                new Ledster::PetalChasePattern()
            ),
            new UDPOutput("192.168.0.76",9601,60),
            //new SpiOutput(0,1,500000)
            Pipe::transfer<RGBA, RGB>
            //PixelLut
        ),

    };

}

