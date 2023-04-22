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
#include "maps/ledsterMap.h"
#include "patterns/helpers/pixelMap.h"
#include "patterns/ledsterPatterns.h"
#include "inputs/patternCycleInput.h"
#include "patterns/helpers/tempo/constantTempo.h"

LUT *SunstripLut = new GammaLUT(2.6, 255);
LUT *GammaLut12 = new GammaLUT(2.5, 4096);
LUT *PixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 255);

void LoadConfiguration()
{
    Params::primaryColour = RGBA(255, 100, 0, 255);
    Params::secondaryColour = RGBA(255, 0, 255, 255);
    Params::highlightColour = RGBA(0, 0, 255, 255);
    Params::velocity = 0.3;
    Params::intensity = 0.7;
    Params::variant = 0.7;

    Configuration.hostname = "minimini";
    
    Tempo::AddSource(ConstantTempo::getInstance());
    ConstantTempo::getInstance()->setBpm(120);

    Configuration.tapMidiNote = 98;
    Configuration.tapStopMidiNote = 89;
    Configuration.tapAlignMidiNote = 88;
    Configuration.tapBarAlignMidiNote = 87;

    Configuration.pipes = {

        //FAIRY
        new Pipe(
            new SwitchableInput(
                new LayeredApcminiInput<Monochrome>(
                1,
                1,
                new LayeredPattern<Monochrome> *[8] {
                    new Layered::BlinderPattern(Transition::none, Transition::none,255,100,1500),
                    new Layered::SinPattern(4,3.1415),
                    new Layered::BeatAllFadePattern(1000),
                    new Layered::BeatAllFadePattern(150),
                    new Layered::SlowStrobePattern(),

                    new Layered::BlinderPattern(Transition::none, Transition::none,255,100,1500),
                    new Layered::SlowStrobePattern(),
                    new Layered::FastStrobePattern(),
                }),
                new LayeredPatternInput<Monochrome>(1,new Layered::BlinderPattern()),
                Midi::isConnected
            ),
            new PWMOutput(1),
            Pipe::transfer<Monochrome, Monochrome12>,
            GammaLut12),


        // new Pipe(
        //     // new CombinedInput({
        //     //     {input: new PatternInput<Monochrome>(5, new SinPattern()), offset:0},
        //     //     {input: new PatternInput<Monochrome>(5, new SinPattern()), offset:5}
        //     // },10),
        //     new LayeredPatternInput<Monochrome>(10,new Layered::ChaseFromCenterPattern(1200)),
        //     new NeopixelOutput<Kpbs800>(7),
        //     Pipe::transfer<Monochrome,Monochrome>,
        //     SunstripLut),

        //SUNSTRIP

        new Pipe(
            new SwitchableInput(
            new LayeredApcminiInput<Monochrome>(
                34,
                3,
                new LayeredPattern<Monochrome> *[8] {
                    new Layered::BlinderPattern(Transition::fromCenter, Transition::fromSides,150,1000,1500),
                    new Layered::GlowPattern(3),
                    new Layered::SinPattern(20),
                    new Layered::ChaseFromCenterPattern(1200),
                    new Layered::BeatMultiFadePattern(500),
                    
                    new Layered::BlinderPattern(Transition::fromLeft,Transition::fromRight),
                    new Layered::GlitchPattern(),
                    new Layered::BeatAllFadePattern(600),
                }),
                new LayeredPatternInput<Monochrome>(34,new Layered::SinPattern(20)),
                Midi::isConnected
            ),
            new NeopixelOutput<Kpbs800>(7),
            Pipe::transfer<Monochrome,Monochrome>,
            SunstripLut),

        //LEDSTER

        new Pipe(
            //new SwitchableInput(
            //    new UDPInput(9601),
                new PatternCycleInput<RGBA>(
                    481,
                    std::vector<LayeredPattern<RGBA> *>{
                        new Ledster::RadarPattern(ledsterMap),
                        new Ledster::PetalChasePattern(),
                        //new Ledster::ConcentricChaserPattern(),
                        new Ledster::SnakePattern(),
                        new Ledster::RadialFadePattern(ledsterMap),
                        new Ledster::RibbenClivePattern<SoftSquare>(),
                        new Ledster::SnowflakePattern(),
                        //new Ledster::PetalRotatePattern(),
                        new Ledster::ClivePattern<LFOPause<SinFast>>(481, 10000, 5, 0.25)},
                    30000 // duration
             //       )
                ),
            new NeopixelOutput<Kpbs800>(1),
            Pipe::transfer<RGBA,GRB>,
            PixelLut
        ),

        ///////////////////////
        // BPM PULSE INDICATOR
        ///////////////////////
        new Pipe(
            new PatternInput<RGB>(1, new BPMIndicatorPattern()),
            new RotaryOutput()),
    };

}

