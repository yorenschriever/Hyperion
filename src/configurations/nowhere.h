#pragma once

#include "outputs/dmxOutput.h"
#include "outputs/apcminiOutput.h"
#include "outputs/neopixelOutput.h"
#include "outputs/pwmOutput.h"
#include "outputs/udpOutput.h"
#include "outputs/rotaryOutput.h"
#include "outputs/spiOutput.h"
#include "metaoutputs/temperatureControlledOutput.h"
#include "inputs/udpInput.h"
#include "inputs/apcminiInput.h"
#include "inputs/dmxInput.h"
#include "inputs/patternInput.h"
#include "inputs/layeredPatternInput.h"
#include "inputs/layeredApcminiInput.h"
#include "patterns/rgbPatterns.h"
#include "patterns/ledstripPatterns.h"
#include "patterns/movingheadPatterns.h"
#include "patterns/monochromePatterns.h"
#include "patterns/monochromeLayeredPatterns.h"
#include "patterns/staticPattern.h"
#include "patterns/mappedPatterns.h"
#include "patterns/derbyPatterns.h"
#include "patterns/singleLampPattern.h"
#include "patterns/helpers/params.h"
#include "patterns/helpers/pixelMap.h"
#include "luts/colourCorrectionLut.h"
#include "luts/gammaLut.h"
#include "luts/incandescentLut.h"
#include "luts/colourCorrectionLut.h"
#include "colours.h"
#include "pipe.h"
#include "configurationStruct.h"

// I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
// http://fastled.io/docs/3.1/group___color_enums.html
// note the different order, fastled uses RGB, luts are in ouput order (GRB)
LUT *PixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);
LUT *IncandescentLut = new IncandescentLUT(2.5, 4096, 200);
LUT *IncandescentLut8 = new IncandescentLUT(2.5, 255, 24);
LUT *GammaLut12 = new GammaLUT(2.5, 4096);
LUT *GammaLut8 = new GammaLUT(2.5, 255);

LFO<Sin> colourlfo = LFO<Sin>(5000);

void updateParams()
{
    const int apcColumn = 0;

    if (APCMini::getStatus(apcColumn, 0))
      Params::primaryColour = RGBA(255, 0, 0, 255);
    if (APCMini::getStatus(apcColumn, 1))
      Params::primaryColour = RGBA(0, 255, 0, 255);
    if (APCMini::getStatus(apcColumn, 2))
      Params::primaryColour = RGBA(0, 0, 255, 255);
    if (APCMini::getStatus(apcColumn, 3))
      Params::primaryColour = RGBA(255, 255, 0, 255);
    if (APCMini::getStatus(apcColumn, 4))
      Params::primaryColour = RGBA(colourlfo.getValue()*255, colourlfo.getValue(0.5)*255, 0, 255);

    if (APCMini::getStatus(apcColumn, 5))
      Params::primaryColour = RGBA(255, 100, 0, 255);
    if (APCMini::getStatus(apcColumn, 6))
      Params::primaryColour = Hue(colourlfo.getValue() * 255);
    if (APCMini::getStatus(apcColumn, 7))
      Params::primaryColour = RGBA(255, 255, 255, 255);
}

void LoadConfiguration()
{
    Params::primaryColour = RGBA(255, 100, 0, 255);
    Params::secondaryColour = RGBA(100, 100, 75, 255);
    Params::highlightColour = RGBA(255, 0, 0, 255);
    Params::velocity = 0.3;

    Configuration.hostname = "hyperion";

    Configuration.pwmFrequency = 1000;

    Configuration.tapMidiNote = 98;
    Configuration.tapStopMidiNote = 89;
    Configuration.tapAlignMidiNote = 88;
    Configuration.tapBarAlignMidiNote = 87;

    Configuration.paramCalculator = updateParams;

    Configuration.pipes = {

        ///////////////////////
        // BPM PULSE INDICATOR
        ///////////////////////
        new Pipe(
            new PatternInput<RGB>(1, new BPMIndicatorPattern()),
            new RotaryOutput()),

        ///////////////////////
        // FAIRYLIGHTS
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                2,
                1,
                new LayeredPattern<Monochrome> *[8]
                {
                    new Layered::BlinderPattern(Transition::none, Transition::none, 255, 100, 1500),
                        new Layered::SinPattern(4, 3.1415),
                        new Layered::BeatAllFadePattern(1000),
                        new Layered::BeatAllFadePattern(150),
                        new Layered::SlowStrobePattern(),

                        new Layered::BlinderPattern(Transition::none, Transition::none, 255, 100, 1500),
                        new Layered::SlowStrobePattern(),
                        new Layered::FastStrobePattern(),
                }),
            new PWMOutput(11),
            Pipe::transfer<Monochrome, Monochrome12>,
            GammaLut12),

        ///////////////////////
        // SWITCHPACK 1 
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                4,
                2,
                new LayeredPattern<Monochrome> *[8]
                {
                    new SingleLampPattern(0),
                    new SingleLampPattern(1),
                    new SingleLampPattern(2),
                    new SingleLampPattern(3),
                    new SingleLampPattern(-1),

                    new SingleLampPattern(0),
                    new SingleLampPattern(1),
                    new SingleLampPattern(2)
                }),
            new DMXOutput(11)),

        ///////////////////////
        // DIMMERPACK 2 SPIEGELBOLLEN
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                4, // width of the pattern, in pixels
                3, // button column on the apc to listen to (0-7)
                // the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8]
                {
                    new Layered::BlinderPattern(Transition::fromLeft, Transition::fromLeft, 200),
                    new Layered::LFOPattern<SoftSquare>(1,5000,0.25),
                    new Layered::SinPattern(),
                    new Layered::ChaseFromCenterPattern(100),
                    new Layered::BeatMultiFadePattern(),
                    
                    new Layered::BlinderPattern(Transition::none, Transition::none, 255),
                    new Layered::BeatSingleFadePattern(),
                    new Layered::BeatAllFadePattern(),
                }),
            new DMXOutput(15)),

        ///////////////////////
        // DIMMERPACK 3 DANSVLOER
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                4, // width of the pattern, in pixels
                4, // button column on the apc to listen to (0-7)
                // the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8]
                {
                    new Layered::BlinderPattern(Transition::fromLeft, Transition::fromLeft, 150),
                    new Layered::LFOPattern<SoftSquare>(1,5000,0.25),
                    new Layered::SinPattern(),
                    new Layered::BeatStepPattern(),
                    new Layered::BeatSingleFadePattern(),

                    new Layered::BlinderPattern(Transition::fromLeft, Transition::fromLeft, 200),
                    new Layered::BeatMultiFadePattern(),
                    new Layered::GlitchPattern(),
                }),
            new DMXOutput(19)),

        ///////////////////////
        // DIMMERPACK 4: PARREN NIELS
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                4, // width of the pattern, in pixels
                5, // button column on the apc to listen to (0-7)
                // the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8]
                {
                    new Layered::BlinderPattern(Transition::fromLeft, Transition::fromLeft, 150),
                    new Layered::SinPattern(),
                    new Layered::BeatStepPattern(),
                    new Layered::BeatSingleFadePattern(),
                    new Layered::BeatMultiFadePattern(),

                    new Layered::BlinderPattern(Transition::fromLeft, Transition::fromLeft, 200, 50,50),
                    new Layered::BeatSingleFadePattern(),
                    new Layered::GlitchPattern(),
                }),
            new DMXOutput(23)),

        // ///////////////////////
        // // LEDPAR BLIMP
        // ///////////////////////
        // new Pipe(
        //     new LayeredApcminiInput<RGBA>(
        //         5,
        //         5,
        //         new LayeredPattern<RGBA> *[8]
        //         {
        //             new LedStrip::PalettePattern(),
        //                 new LedStrip::PalettePattern(),
        //                 new LedStrip::PalettePattern(),
        //                 new LedStrip::PalettePattern(),
        //                 new LedStrip::PalettePattern(),

        //                 new LedStrip::PalettePattern(),
        //                 new LedStrip::PalettePattern(),
        //                 new LedStrip::PalettePattern(),
        //         }),
        //     new DMXOutput(35),
        //     Pipe::transfer<RGBA, RGB>),

        ///////////////////////
        // DERBY
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<Derby>(
                2,
                6,
                new LayeredPattern<Derby> *[8]
                {
                    new DerbyPatterns::OnPattern(),
                    new DerbyPatterns::MovePattern(),
                    new DerbyPatterns::BeatMovePattern(),
                    new DerbyPatterns::RandomPositionPattern(),
                    new DerbyPatterns::BeatFadePattern(),
                    
                    new DerbyPatterns::OnPattern(200),
                    new DerbyPatterns::StrobePattern(240),
                    new DerbyPatterns::StrobePattern(255)
                }),
            new DMXOutput(1)),

        // ///////////////////////
        // // STROBE
        // ///////////////////////

        // new Pipe(
        //     new LayeredApcminiInput<Strobe>(
        //         1,
        //         7,
        //         new LayeredPattern<Strobe> *[8]
        //         {
        //             new StaticPattern<Strobe>(Strobe(255, 50)),
        //                 new StaticPattern<Strobe>(Strobe(200, 100)),
        //                 new StaticPattern<Strobe>(Strobe(255, 175)),
        //                 new StaticPattern<Strobe>(Strobe(125, 200)),
        //                 new StaticPattern<Strobe>(Strobe(175, 255)),
        //                 new StaticPattern<Strobe>(Strobe(255, 50)),
        //                 new StaticPattern<Strobe>(Strobe(200, 127)),
        //                 new StaticPattern<Strobe>(Strobe(255, 255)),
        //         }),
        //     new DMXOutput(25)),

    };
}
