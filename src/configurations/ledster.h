#pragma once

#include "outputs/spiOutput.h"
#include "outputs/udpOutput.h"
#include "inputs/patternCycleInput.h"
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
            new PatternCycleInput<RGBA>(
                481, //number of leds
                std::vector<LayeredPattern<RGBA>*> {
                    new Ledster::RadarPattern(ledsterMap),
                    new Ledster::PetalChasePattern(),
                    new Ledster::ConcentricChaserPattern(),
                    new Ledster::SnakePattern(),
                    new Ledster::RadialFadePattern(ledsterMap),
                    new Ledster::RibbenClivePattern<SoftSquare>(),
                    new Ledster::RibbenClivePattern<SawDown>(500),
                    new Ledster::RibbenClivePattern<SinFast>(3000),
                    new Ledster::AdidasPattern(),
                    new Ledster::ChevronsPattern(ledsterMap),
                    new Ledster::SnowflakePattern(),
                    new Ledster::PetalRotatePattern(),
                    new Ledster::MandalaPattern()
                },
                5000 //duration
            ),
            new UDPOutput("192.168.0.76",9601,60),
            //new SpiOutput(0,1,500000)
            Pipe::transfer<RGBA, RGB>
            //PixelLut
        ),

    };

}

