#pragma once

#include "outputs/spiOutput.h"
#include "outputs/udpOutput.h"
#include "outputs/rotaryOutput.h"
#include "inputs/patternCycleInput.h"
#include "inputs/nanoKontrolInput.h"
#include "inputs/patternInput.h"
#include "patterns/ledsterPatterns.h"
#include "patterns/rgbPatterns.h"
#include "patterns/helpers/tempo/constantTempo.h"
#include "luts/colourCorrectionLut.h"
#include "colours.h"
#include "pipe.h"
#include "configurationStruct.h"
#include "maps/ledsterMap.h"
#include "patterns/helpers/pixelMap.h"
#include "inputs/fallbackInput.h"
#include "inputs/udpInput.h"
#include "inputs/switchableInput.h"
#include "inputs/nullInput.h"

LUT* PixelLut = new ColourCorrectionLUT(1.5, 255,255, 255, 240); 

void LoadConfiguration()
{
    Params::primaryColour = RGBA(255, 0, 0, 255);
    Params::secondaryColour = RGBA(0, 255, 0, 255);
    Params::highlightColour = RGBA(0, 0, 255, 255);
    Params::velocity = 0.3;
    Params::intensity = 0.7;
    Params::variant = 0.7;

    Tempo::AddSource(ConstantTempo::getInstance()); 
    ConstantTempo::getInstance()->setBpm(120); 

    Configuration.hostname = "ledsterstandalone";

    auto sharedOutput = new SpiOutput(0,1,500000); //new UDPOutput("192.168.0.76",9601,60),

    Configuration.pipes = {

        //I cannot use FallbackInput here, because PatternCycleInput works with RGBA and
        //UDPInput works with RGB. This workaround with switchableInput also does the trick. 
        new Pipe(
            new SwitchableInput(
                new NullInput(),
                new PatternCycleInput<RGBA>(
                    481, //number of leds
                    std::vector<LayeredPattern<RGBA>*> {
                        new Ledster::RadarPattern(ledsterMap),
                        new Ledster::PetalChasePattern(),
                        new Ledster::ConcentricChaserPattern(),
                        new Ledster::SnakePattern(),
                        new Ledster::RadialFadePattern(ledsterMap),
                        new Ledster::RibbenClivePattern<SoftSquare>(),
                        new Ledster::SnowflakePattern(),
                        new Ledster::PetalRotatePattern(),
                        new Ledster::ClivePattern<LFOPause<SinFast>>(481,10000,5,0.25)
                    },
                    30000 //duration
                ),
                Ethernet::isConnected
            ),
            sharedOutput,
            Pipe::transfer<RGBA, RGB>,
            PixelLut
        ),

        new Pipe( 
            new UDPInput(9601),
            sharedOutput,
            Pipe::transfer<RGB, RGB>,
            PixelLut
        )
    };

}

