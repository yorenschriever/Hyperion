#pragma once

#include "outputs/pwmOutput.h"
#include "outputs/dmxOutput.h"
#include "outputs/apcminiOutput.h"
#include "outputs/neopixelOutput.h"
#include "outputs/rotaryOutput.h"
#include "inputs/udpInput.h"
#include "inputs/apcminiInput.h"
#include "inputs/dmxInput.h"
#include "inputs/patternInput.h"
#include "patterns/rgbPatterns.h"
#include "patterns/monochromePatterns.h"
#include "luts/colourCorrectionLut.h"
#include "luts/incandescentLut.h"
#include "outputs/udpOutput.h"

const char *HostName = "HyperionMaster";

//I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
//http://fastled.io/docs/3.1/group___color_enums.html
//note the different order, fastled uses RGB, luts are in ouput order (GRB)
LUT *NeopixelLut = new ColourCorrectionLUT(1.5, 255, 176, 255, 240);
LUT *IncandescentLut = new IncandescentLUT(2.5, 4096, 400);

//TODO proper config struct
#define TAPMIDINOTE 98
#define TAPSTOPMIDINOTE 89

Pipe pipes[] = {


    Pipe(
        //create an apcmini input that creates monochrome patterns
        new ApcminiInput<Monochrome>(
            30,
            0,
            new Pattern<Monochrome> *[8] {
                new SinPattern(),
                    new SawPattern(),
                    new RandomPattern(),
                    new RandomPattern2(),
                    new MeteorPattern(),
                    new RandomFadePattern(),
                    new SlowStrobePattern(),
                    new FastStrobePattern()
            }),
        new DMXOutput(1)),

    Pipe(
        //create an apcmini input that creates monochrome patterns
        new ApcminiInput<Monochrome>(
            30,
            1,
            new Pattern<Monochrome> *[8] {
                new SinPattern(),
                    new SawPattern(),
                    new RandomPattern(),
                    new RandomPattern2(),
                    new MeteorPattern(),
                    new RandomFadePattern(),
                    new SlowStrobePattern(),
                    new FastStrobePattern()
            }),
        new DMXOutput(31)),

    Pipe(
        new ApcminiInput<Monochrome>(
            12,
            2,
            new Pattern<Monochrome> *[8] {
                new OnPattern(),
                new BeatMultiFadePattern(),
                new SinPattern(),
                new MeteorPattern(),
                new BeatShakePattern(),

                new OnPattern(),
                new BeatAllFadePattern(),
                new GlitchPattern()
            }),
        new PWMOutput(1500),
        Pipe::transfer<Monochrome, Monochrome12>,
        IncandescentLut),

    Pipe(
        new ApcminiInput<RGB>(
            16, //width of the pattern, in pixels
            3,  //button column on the apc to listen to (0-7)
            //the patterns to attach to the buttons
            new Pattern<RGB> *[8] {
                new RainbowPattern(),
                    new ColourOrderPattern(),
                    new MixingPattern(),
                    new AnimatedMixingPattern(),
                    new PoissonPattern(),
                    new RainbowPattern(),
                    new RainbowPattern(),
                    new RainbowPattern()
            }),
        new NeopixelOutput<Kpbs800>(1),
        Pipe::transfer<RGB, GRB>,
        NeopixelLut),

    Pipe(
        new ApcminiInput<Monochrome>(
            12, //width of the pattern, in pixels
            6,  //button column on the apc to listen to (0-7)
            //the patterns to attach to the buttons
            new Pattern<Monochrome> *[8] {
                new OnPattern(),
                new SinPattern(),
                new BeatSingleFadePattern(),
                new BeatMultiFadePattern(),
                new BeatShakePattern(),
                new OnPattern(),
                new GlitchPattern(),
                new SlowStrobePattern()
            }),
        new UDPOutput("lasers.local",9619,100),
        Pipe::transfer<Monochrome,Monochrome12>),

    Pipe(
        new ApcminiInput<Monochrome>(
            12, //width of the pattern, in pixels
            7,  //button column on the apc to listen to (0-7)
            //the patterns to attach to the buttons
            new Pattern<Monochrome> *[8] {
                new BeatAllFadePattern(),
                new BeatSingleFadePattern(),
                new GlitchPattern(),
                new BeatShakePattern(),
                new SlowStrobePattern(),
                new OnPattern(),
                new GlitchPattern(),
                new FastStrobePattern()
            }),
        new UDPOutput("strobes.local",9619,100),
        Pipe::transfer<Monochrome,Monochrome12>),


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