#pragma once
#include "colours.h"
#include "pipe.h"
#include "inputs/udpInput.h"
#include "outputs/neopixelOutput.h"
#include "outputs/pwmOutput.h"
#include "outputs/dmxOutput.h"
#include "luts/colourCorrectionLut.h"
#include "luts/incandescentLut.h"
#include "configurationStruct.h"
#include "patterns/monochromePatterns.h"
#include "inputs/patternInput.h"
#include "inputs/combinedInput.h"
#include "luts/gammaLut.h"
#include "patterns/rgbPatterns.h"
#include "outputs/rotaryOutput.h"
#include "patterns/monochromeLayeredPatterns.h"
#include "inputs/layeredPatternInput.h"
#include "inputs/layeredApcminiInput.h"
#include "inputs/switchableInput.h"

LUT *SunstripLut = new GammaLUT(2.5, 255);

void LoadConfiguration()
{

    Configuration.hostname = "minimini";
    
    Configuration.pipes = {
        // new Pipe(
        //     // new CombinedInput({
        //     //     {input: new PatternInput<Monochrome>(5, new SinPattern()), offset:0},
        //     //     {input: new PatternInput<Monochrome>(5, new SinPattern()), offset:5}
        //     // },10),
        //     new LayeredPatternInput<Monochrome>(10,new Layered::ChaseFromCenterPattern(1200)),
        //     new NeopixelOutput<Kpbs800>(7),
        //     Pipe::transfer<Monochrome,Monochrome>,
        //     SunstripLut),


        new Pipe(
            new SwitchableInput(
            new LayeredApcminiInput<Monochrome>(
                34,
                3,
                new LayeredPattern<Monochrome> *[8] {
                    new Layered::BlinderPattern(Transition::fromCenter, Transition::fromSides,150,1000,1500),
                    new Layered::GlowPattern(),
                    new Layered::SinPattern(20),
                    new Layered::ChaseFromCenterPattern(1200),
                    new Layered::BeatMultiFadePattern(500),
                    
                    new Layered::BlinderPattern(Transition::fromLeft,Transition::fromRight),
                    new Layered::GlitchPattern(),
                    new Layered::BeatAllFadePattern(600),
                }),
                new LayeredPatternInput<Monochrome>(10,new Layered::GlowPattern()),
                Midi::isConnected
            ),
            new NeopixelOutput<Kpbs800>(7),
            Pipe::transfer<Monochrome,Monochrome>,
            SunstripLut),

        ///////////////////////
        // BPM PULSE INDICATOR
        ///////////////////////
        new Pipe(
            new PatternInput<RGB>(1, new BPMIndicatorPattern()),
            new RotaryOutput()),
    };

}

