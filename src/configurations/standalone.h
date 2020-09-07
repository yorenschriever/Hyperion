#pragma once

#include "outputs/pwmOutput.h"
#include "outputs/dmxOutput.h"
#include "outputs/apcminiOutput.h"
#include "outputs/neopixelOutput.h"
#include "outputs/rotaryOutput.h"
#include "outputs/nullOutput.h"
#include "inputs/udpInput.h"
#include "inputs/apcminiInput.h"
#include "inputs/layeredApcminiInput.h"
#include "inputs/dmxInput.h"
#include "inputs/patternInput.h"
#include "patterns/rgbPatterns.h"
#include "patterns/monochromePatterns.h"
#include "patterns/monochromeLayeredPatterns.h"
#include "luts/colourCorrectionLut.h"
#include "luts/incandescentLut.h"
#include "luts/gammaLut.h"
#include "outputs/udpOutput.h"
#include "pipe.h"

const char *HostName = "HyperionMaster";
#define PWMFrequency 100

//I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
//http://fastled.io/docs/3.1/group___color_enums.html
//note the different order, fastled uses RGB, luts are in ouput order (GRB)
LUT *NeopixelLut = new ColourCorrectionLUT(1.5, 255, 176, 255, 240);
LUT *IncandescentLut = new IncandescentLUT(2.5, 4096, 400);
LUT *IncandescentLut8 = new IncandescentLUT(2.5, 255, 24);
LUT *GammaLut12 = new GammaLUT(2.5, 4096);

//TODO proper config struct
#define TAPMIDINOTE 98
#define TAPSTOPMIDINOTE 89
#define TAPALIGNMIDINOTE 88
#define TAPBARALIGNMINIDNOTE 87

Pipe pipes[] = {

    Pipe(
        //create an apcmini input that creates monochrome patterns
        new LayeredApcminiInput<Monochrome>(
            30,
            0,
            new LayeredPattern<Monochrome> *[8] {
                new Layered::SinPattern(),
                new Layered::SinPattern(1),
                new Layered::SinPattern(-1),
                new Layered::BeatShakePattern(),
                new Layered::GlitchPattern(),
                new Layered::SinPattern(),
                new Layered::SlowStrobePattern(),
                new Layered::BlinderPattern()
            }),
        new DMXOutput(1)
        //new NullOutput()
        ),

    // // Pipe(
    // //     //create an apcmini input that creates monochrome patterns
    // //     new ApcminiInput<Monochrome>(
    // //         30,
    // //         1,
    // //         new Pattern<Monochrome> *[8] {
    // //             new SinPattern(),
    // //                 new SawPattern(),
    // //                 new RandomPattern(),
    // //                 new RandomPattern2(),
    // //                 new MeteorPattern(),
    // //                 new RandomFadePattern(),
    // //                 new SlowStrobePattern(),
    // //                 new FastStrobePattern()
    // //         }),
    // //     new DMXOutput(31)),

    ///////////////////////
    // PAR
    ///////////////////////
    Pipe(
        new LayeredApcminiInput<Monochrome>(
            1,
            1,
            new LayeredPattern<Monochrome> *[8] {
                new Layered::BlinderPattern(none, none,255,100,1500),
                new Layered::SinPattern(),
                new Layered::SinPattern(4,3.1415),
                new Layered::SinPattern(),
                new Layered::BeatAllFadePattern(1000),

                new Layered::BlinderPattern(none, none,255,100,1500),
                new Layered::BeatAllFadePattern(600),
                new Layered::BeatAllFadePattern(150)
            }),
        new PWMOutput(12),
        Pipe::transfer<Monochrome, Monochrome12>,
        IncandescentLut),

    ///////////////////////
    // VERGIET
    ///////////////////////
    Pipe(
        new LayeredApcminiInput<Monochrome>(
            10,
            2,
            new LayeredPattern<Monochrome> *[8] {
                new Layered::BlinderPattern(fromCenter, fromSides,150,1000,1500),
                new Layered::BeatMultiFadePattern(500),
                new Layered::SinPattern(),
                new Layered::GlowPattern(),
                new Layered::BeatShakePattern(),

                new Layered::BlinderPattern(fromLeft,fromRight),
                new Layered::BeatAllFadePattern(600),
                new Layered::GlitchPattern()
            }),
        //new PWMOutput(),
        //Pipe::transfer<Monochrome, Monochrome12>,
        //IncandescentLut
        new DMXOutput(128),
        Pipe::transfer<Monochrome, Monochrome>,
        IncandescentLut8),

    ///////////////////////
    // NEOPIXEL LEDSTRIPS
    ///////////////////////
    Pipe(
        new ApcminiInput<RGB>(
            500, //width of the pattern, in pixels
            3,   //button column on the apc to listen to (0-7)
            //the patterns to attach to the buttons
            new Pattern<RGB> *[8] {
                new RainbowPattern(),
                    new ColourOrderPattern(),
                    new MixingPattern(),
                    new AnimatedMixingPattern(),
                    //new PoissonPattern(),
                    new BPMFillPattern(),
                    new RainbowPattern(),
                    new RainbowPattern(),
                    new RainbowPattern()
            }),
        new NeopixelOutput<Kpbs800>(1),
        Pipe::transfer<RGB, GRB>,
        NeopixelLut),

    ///////////////////////
    // FAIRYLIGHTS
    ///////////////////////
    Pipe(
        new LayeredApcminiInput<Monochrome>(
            1,
            5,
            new LayeredPattern<Monochrome> *[8] {
                new Layered::BlinderPattern(none, none,255,100,1500),
                new Layered::SinPattern(),
                new Layered::SinPattern(4,3.1415),
                new Layered::SlowStrobePattern(),
                new Layered::BeatAllFadePattern(1000),

                new Layered::BlinderPattern(none, none,255,100,1500),
                new Layered::BeatAllFadePattern(150),
                new Layered::FastStrobePattern(),
            }),
        new PWMOutput(11),
        Pipe::transfer<Monochrome, Monochrome12>,
        GammaLut12),

    ///////////////////////
    // LASERS
    ///////////////////////
    Pipe(
        new LayeredApcminiInput<Monochrome>(
            10, //width of the pattern, in pixels
            6,  //button column on the apc to listen to (0-7)
            //the patterns to attach to the buttons
            new LayeredPattern<Monochrome> *[8] {
            new Layered::BlinderPattern(fromLeft,fromLeft,255),
            new Layered::SinPattern(),
            new Layered::BeatSingleFadePattern(),
            new Layered::BeatMultiFadePattern(),
            new Layered::BeatShakePattern(),
            new Layered::BlinderPattern(fromLeft,fromLeft,255),
            new Layered::GlitchPattern(),
            new Layered::SlowStrobePattern()
            }),
        new UDPOutput("lasers.local", 9619, 100),
        Pipe::transfer<Monochrome, Monochrome12>),

    ///////////////////////
    // STROBES
    ///////////////////////
    Pipe(
        new LayeredApcminiInput<Monochrome>(
            10, //width of the pattern, in pixels
            7,  //button column on the apc to listen to (0-7)
            //the patterns to attach to the buttons
            new LayeredPattern<Monochrome> *[8] {
            new Layered::BeatAllFadePattern(),
            new Layered::BeatSingleFadePattern(),
            new Layered::GlitchPattern(),
            new Layered::BeatShakePattern(),
            new Layered::SlowStrobePattern(),
            new Layered::BlinderPattern(fromCenter,fromCenter),
            new Layered::GlitchPattern(),
            new Layered::FastStrobePattern()
            }),
        new UDPOutput("strobes.local", 9619, 100),
        Pipe::transfer<Monochrome, Monochrome12>),

    ///////////////////////
    // BPM FILL LEDS
    ///////////////////////
    Pipe(
        new PatternInput<RGB>(32,new BPMFillPattern()),
        new NeopixelOutput<Kpbs800>(2),
        Pipe::transfer<RGB, GRB>,
        NeopixelLut),

    ///////////////////////
    // BPM INDICATOR
    ///////////////////////
    Pipe(
        new PatternInput<RGB>(1, new BPMIndicatorPattern()),
        new RotaryOutput()),

    // Pipe(
    //     new PatternInput<RGB>(16, new WatcherPattern2()),
    //     new NeopixelOutput<Kpbs800>(1),
    //     Pipe::transfer<RGB, GRB>,
    //     NeopixelLut)

};