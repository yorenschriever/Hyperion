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

const char *HostName = "HyperionMaster";

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

    // Pipe(
    //     //create an apcmini input that creates monochrome patterns
    //     new ApcminiInput<Monochrome>(
    //         30,
    //         1,
    //         new Pattern<Monochrome> *[8] {
    //             new SinPattern(),
    //                 new SawPattern(),
    //                 new RandomPattern(),
    //                 new RandomPattern2(),
    //                 new MeteorPattern(),
    //                 new RandomFadePattern(),
    //                 new SlowStrobePattern(),
    //                 new FastStrobePattern()
    //         }),
    //     new DMXOutput(31)),

    Pipe(
        new LayeredApcminiInput<Monochrome>(
            10,
            2,
            new LayeredPattern<Monochrome> *[8] {
                new Layered::OnPattern(100),
                new Layered::BeatMultiFadePattern(),
                new Layered::SinPattern(),
                new Layered::SinPattern(-1),
                new Layered::BeatShakePattern(),

                new Layered::BlinderPattern(fromCenter,fromCenter),
                new Layered::BeatAllFadePattern(),
                new Layered::GlitchPattern()
            }),
        //new PWMOutput(1500),
        //Pipe::transfer<Monochrome, Monochrome12>,
        //IncandescentLut
        new DMXOutput(128),
        Pipe::transfer<Monochrome, Monochrome>,
        IncandescentLut8),

    Pipe(
        new ApcminiInput<RGB>(
            500, //width of the pattern, in pixels
            3,  //button column on the apc to listen to (0-7)
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

    Pipe(
        new LayeredApcminiInput<Monochrome>(
            10, //width of the pattern, in pixels
            6,  //button column on the apc to listen to (0-7)
            //the patterns to attach to the buttons
            new LayeredPattern<Monochrome> *[8] {
                new Layered::OnPattern(),
                new Layered::SinPattern(),
                new Layered::BeatSingleFadePattern(),
                new Layered::BeatMultiFadePattern(),
                new Layered::BeatShakePattern(),
                new Layered::OnPattern(),
                new Layered::GlitchPattern(),
                new Layered::SlowStrobePattern()
            }),
        new UDPOutput("lasers.local",9619,100),
        Pipe::transfer<Monochrome,Monochrome12>),

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
                new Layered::OnPattern(),
                new Layered::GlitchPattern(),
                new Layered::FastStrobePattern()
            }),
        new UDPOutput("strobes.local",9619,100),
        Pipe::transfer<Monochrome,Monochrome12>
        //,GammaLut12
        ),

    // Pipe(
    //     new PatternInput<RGB>(16, 
    //     //new AnimatedMixingPattern()
    //     new BPMFillPattern()
    //     ),
    //     new NeopixelOutput<Kpbs800>(2),
    //     Pipe::transfer<RGB, GRB>,
    //     NeopixelLut
    // ),


    Pipe(
        new PatternInput<RGB>(1, new BPMIndicatorPattern()),
        new RotaryOutput()
        ),



    // Pipe(
    //     new PatternInput<RGB>(16, new WatcherPattern2()),
    //     new NeopixelOutput<Kpbs800>(1),
    //     Pipe::transfer<RGB, GRB>,
    //     NeopixelLut)

};