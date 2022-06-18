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

void updateParams()
{
    const int apcColumn = 0;
    if (APCMini::getStatus(apcColumn, 0))
    {
        // Orange
        Params::primaryColour = RGBA(255, 100, 0, 255);
        Params::secondaryColour = RGBA(255, 100, 0, 255);
        Params::highlightColour = RGBA(255, 0, 0, 255);
    }

    if (APCMini::getStatus(apcColumn, 1))
    {
        // Red
        Params::primaryColour = RGB(255, 0, 0);
        Params::secondaryColour = RGB(255, 0, 0);
        Params::highlightColour = RGB(255, 75, 0);
    }

    if (APCMini::getStatus(apcColumn, 2))
    {
        // Yellow
        Params::primaryColour = RGB(255, 255, 0);
        Params::secondaryColour = RGB(255, 100, 0);
        Params::highlightColour = RGB(255, 0, 10);
    }

    if (APCMini::getStatus(apcColumn, 3))
    {
        // Hot
        Params::primaryColour = RGB(255, 0, 0);
        Params::secondaryColour = RGB(255, 150, 0);
        Params::highlightColour = RGB(255, 0, 200);
    }

    if (APCMini::getStatus(apcColumn, 4))
    {
        // Cool
        Params::primaryColour = RGB(0, 100, 255);
        Params::secondaryColour = RGB(0, 0, 200);
        Params::highlightColour = RGB(150, 150, 150);
    }

    if (APCMini::getStatus(apcColumn, 5))
    {
        // Green blue
        Params::primaryColour = RGB(0, 0, 255);
        Params::secondaryColour = RGB(0, 255, 0);
        Params::highlightColour = RGB(0, 127, 127);
    }

    if (APCMini::getStatus(apcColumn, 6))
    {
        // blue/orange
        Params::primaryColour = RGB(255, 0, 0);
        Params::secondaryColour = RGB(255, 100, 0);
        Params::highlightColour = RGB(255, 127, 0);
    }

    if (APCMini::getStatus(apcColumn, 7))
    {
        // green red
        Params::primaryColour = RGB(255, 0, 255);
        Params::secondaryColour = RGB(0, 0, 255);
        Params::highlightColour = RGB(255, 0, 0);
    }
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
        // PAR & FAIRYLIGHTS
        ///////////////////////

        // PAR
        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                1,
                1,
                new LayeredPattern<Monochrome> *[8]
                {
                    new Layered::BlinderPattern(Transition::none, Transition::none, 255, 100, 1500),
                        new Layered::SinPattern(),
                        new Layered::EmptyPattern(),
                        new Layered::EmptyPattern(),
                        new Layered::EmptyPattern(),

                        new Layered::BlinderPattern(Transition::none, Transition::none, 255, 100, 1500),
                        new Layered::EmptyPattern(),
                        new Layered::EmptyPattern()
                }),
            new PWMOutput(12),
            Pipe::transfer<Monochrome, Monochrome12>,
            IncandescentLut),

        // FAIRY
        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                1,
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
        // DIMMERPACK 1
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                4,
                2,
                new LayeredPattern<Monochrome> *[8]
                {
                    new Layered::BlinderPattern(Transition::fromCenter, Transition::fromSides, 150, 1000, 1500),
                        new Layered::GlowPattern(),
                        new Layered::SinPattern(),
                        new Layered::BeatMultiFadePattern(500),
                        new Layered::LFOPattern<SawDown>(2, 1000),

                        new Layered::BlinderPattern(Transition::fromLeft, Transition::fromRight),
                        new Layered::GlitchPattern(),
                        new Layered::BeatAllFadePattern(600),
                }),
            new DMXOutput(1)),

        ///////////////////////
        // DIMMERPACK 2
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                4, // width of the pattern, in pixels
                3, // button column on the apc to listen to (0-7)
                // the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8]
                {
                    new Layered::BlinderPattern(Transition::fromLeft, Transition::fromLeft, 255),
                        new Layered::SinPattern(),
                        new Layered::BeatSingleFadePattern(),
                        new Layered::BeatMultiFadePattern(),
                        new Layered::BeatShakePattern(),
                        new Layered::BlinderPattern(Transition::fromLeft, Transition::fromLeft, 255),
                        new Layered::GlitchPattern(),
                        new Layered::SlowStrobePattern()
                }),
            new DMXOutput(5)),

        ///////////////////////
        // DIMMERPACK 3
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                4, // width of the pattern, in pixels
                4, // button column on the apc to listen to (0-7)
                // the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8]
                {
                    new Layered::BlinderPattern(Transition::fromLeft, Transition::fromLeft, 255),
                        new Layered::SinPattern(),
                        new Layered::BeatSingleFadePattern(),
                        new Layered::BeatMultiFadePattern(),
                        new Layered::BeatShakePattern(),
                        new Layered::BlinderPattern(Transition::fromLeft, Transition::fromLeft, 255),
                        new Layered::GlitchPattern(),
                        new Layered::SlowStrobePattern()
                }),
            new DMXOutput(9)),

        ///////////////////////
        // LEDPAR BALLON
        ///////////////////////
        new Pipe(
            new LayeredApcminiInput<RGBA>(
                3,
                5,
                new LayeredPattern<RGBA> *[8]
                {
                    new LedStrip::PalettePattern(),
                        new LedStrip::PalettePattern(),
                        new LedStrip::PalettePattern(),
                        new LedStrip::PalettePattern(),
                        new LedStrip::PalettePattern(),

                        new LedStrip::PalettePattern(),
                        new LedStrip::PalettePattern(),
                        new LedStrip::PalettePattern(),
                }),
            new DMXOutput(80),
            Pipe::transfer<RGBA, RGB>),

        ///////////////////////
        // DERBY
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<Derby>(
                1,
                6,
                new LayeredPattern<Derby> *[8]
                {
                    new StaticPattern<Derby>(Derby(200, RGB(0, 0, 255), 0)),
                        new StaticPattern<Derby>(Derby(200, RGB(255, 0, 0), 255)),
                        new StaticPattern<Derby>(Derby(200, RGB(0, 255, 0), 127)),
                        new StaticPattern<Derby>(Derby(200, RGB(0, 255, 255), 0)),
                        new StaticPattern<Derby>(Derby(200, RGB(255, 0, 255), 100)),
                        new StaticPattern<Derby>(Derby(200, RGB(255, 255, 0), 50)),
                        new StaticPattern<Derby>(Derby(200, RGB(255, 255, 255), 100)),
                        new StaticPattern<Derby>(Derby(200, RGB(255, 255, 255), 255)),
                }),
            new DMXOutput(15)),

        ///////////////////////
        // STROBE
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<Strobe>(
                1,
                7,
                new LayeredPattern<Strobe> *[8]
                {
                    new StaticPattern<Strobe>(Strobe(255, 50)),
                        new StaticPattern<Strobe>(Strobe(255, 100)),
                        new StaticPattern<Strobe>(Strobe(255, 175)),
                        new StaticPattern<Strobe>(Strobe(255, 200)),
                        new StaticPattern<Strobe>(Strobe(255, 255)),
                        new StaticPattern<Strobe>(Strobe(255, 50)),
                        new StaticPattern<Strobe>(Strobe(255, 127)),
                        new StaticPattern<Strobe>(Strobe(255, 255)),
                }),
            new DMXOutput(20)),

    };
}
