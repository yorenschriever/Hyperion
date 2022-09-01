#pragma once

//#include "outputs/spiOutput.h"
//#include "luts/colourCorrectionLut.h"
#include "colours.h"
#include "pipe.h"
#include "configurationStruct.h"
#include "inputs/inputSplitter.h"
#include "outputs/udpOutput.h"
#include "inputs/patternInput.h"
#include "patterns/rgbPatterns.h"
#include "patterns/hoepelPatterns.h"
#include "inputs/nanoKontrolInput.h"
#include "outputs/rotaryOutput.h"
#include "inputs/patternInput.h"
#include "patterns/rgbPatterns.h"

//LUT *PixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);

void LoadConfiguration()
{
    Configuration.hostname = "hoepels";

    Params::primaryColour = RGBA(255, 0, 0, 255);
    Params::secondaryColour = RGBA(0, 255, 0, 255);
    Params::highlightColour = RGBA(0, 0, 255, 255);
    Params::velocity = 0.3;
    Params::intensity = 0.7;
    Params::variant = 0.7;

    Configuration.tapMidiNote = 41; //play
    Configuration.tapStopMidiNote = 42; //stop
    Configuration.tapAlignMidiNote = 43; //back
    Configuration.tapBarAlignMidiNote = 44; //forward

    auto split = new InputSplitter(
            new NanoKontrolInput<RGBA>(
                500, //number of leds
                new LayeredPattern<RGBA> *[24]{
                    //
                    new Hoepels::MonochromePattern(),
                    new Hoepels::StereochromePattern(),
                    new Hoepels::GradientPattern(),

                    //
                    new Hoepels::Rainbow(),
                    new Hoepels::Rainbow2(),
                    new Hoepels::HoepelsBase(),

                    //inner
                    new Hoepels::HoepelsTransition(),
                    new Hoepels::SinStripPattern(),
                    new Hoepels::SinStripPattern2(),

                    //outer
                    new Hoepels::OnbeatFadePattern(),
                    new Hoepels::FirePattern(),
                    new Hoepels::FireworkPattern(),

                    //sparse
                    new Hoepels::SinPattern(),
                    new Hoepels::AntichasePattern(),
                    new Hoepels::ClivePattern<SawDown>(50,1000),

                    //clive
                    new Hoepels::ClivePattern<SinFast>(500,2000),
                    new Hoepels::ClivePattern<SinFast>(10,2000),
                    new Hoepels::ClivePattern<SoftSquare>(50,10000,1,0.1),

                    //flashes
                    new Hoepels::SquareGlitchPattern(),
                    new Hoepels::GlowPulsePattern(),//dubbel
                    new Hoepels::SquareGlitchPattern(),

                    //strobes
                    new Hoepels::SlowStrobePattern(),
                    new Hoepels::SquareGlitchPattern(),//dubbel
                    new Hoepels::GlowPulsePattern(),

                }
            ),
        {400, 400, 400, 400, 400},
        true);

    Configuration.pipes = {

        // BPM PULSE INDICATOR
        new Pipe(
            new PatternInput<RGB>(1, new BPMIndicatorPattern()),
            new RotaryOutput()),

        new Pipe(
            split->getInput(0),
            new UDPOutput("192.168.1.222", 9601, 50),
            Pipe::transfer<RGBA, RGB>),
        new Pipe(
            split->getInput(1),
            new UDPOutput("192.168.1.223", 9601, 50),
            Pipe::transfer<RGBA, RGB>),
        new Pipe(
            split->getInput(2),
            new UDPOutput("192.168.1.224", 9601, 50),
            Pipe::transfer<RGBA, RGB>),
        new Pipe(
            split->getInput(3),
            new UDPOutput("192.168.1.226", 9601, 50),
            Pipe::transfer<RGBA, RGB>),
        new Pipe(
            split->getInput(4),
            new UDPOutput("192.168.1.229", 9601, 50),
            Pipe::transfer<RGBA, RGB>),
    };
}