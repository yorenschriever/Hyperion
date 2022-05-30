#pragma once

#include "outputs/spiOutput.h"
#include "outputs/udpOutput.h"
#include "outputs/rotaryOutput.h"
#include "inputs/patternCycleInput.h"
#include "inputs/nanoKontrolInput.h"
#include "inputs/patternInput.h"
#include "patterns/ledsterPatterns.h"
#include "patterns/rgbPatterns.h"
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
    Params::intensity = 0.7;
    Params::variant = 0.7;

    Configuration.tapMidiNote = 41;
    Configuration.tapStopMidiNote = 42;
    Configuration.tapAlignMidiNote = 43;
    Configuration.tapBarAlignMidiNote = 44;

    Configuration.hostname = "ledster";
    
    Configuration.pipes = {


        new Pipe( 
            new NanoKontrolInput<RGBA>(
                481, //number of leds
                new LayeredPattern<RGBA> *[24]{
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

                    new Ledster::MandalaPattern(),
                    new Ledster::RadialGlitterFadePattern(ledsterMap),
                    new Ledster::PixelGlitchPattern(),

                    new Ledster::SquareGlitchPattern(ledsterMap),
                    new Ledster::RibbenClivePattern<LFOPause<SinFast>>(10000,1,0.25),
                    new Ledster::ClivePattern<LFOPause<SinFast>>(481,10000,5,0.25),

                    new Ledster::KonamiFadePattern(ledsterMap),
                    new Ledster::RadialRainbowPattern(ledsterMap),

                    new Ledster::RadialRainbowPattern(ledsterMap),
                    new Ledster::RadialRainbowPattern(ledsterMap),
                    new Ledster::RadialRainbowPattern(ledsterMap),
                    new Ledster::ClivePattern<LFOPause<SinFast>>(481,10000,5,0.25),
                }
            ),
            //new UDPOutput("192.168.0.76",9601,60),
            new UDPOutput("ledsterstandalone.local",9601,60),
            //new SpiOutput(0,1,500000),
            Pipe::transfer<RGBA, RGB>
            //PixelLut
        ),


        ///////////////////////
        // BPM PULSE INDICATOR
        ///////////////////////
        new Pipe(
            new PatternInput<RGB>(1, new BPMIndicatorPattern()),
            new RotaryOutput()
        )
    };

}

