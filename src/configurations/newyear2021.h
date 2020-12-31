#pragma once

#include "hardware/apcmini/apcmini.h"
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
#include "patterns/ledstripPatterns.h"
#include "patterns/monochromePatterns.h"
#include "patterns/monochromeLayeredPatterns.h"
#include "patterns/movingheadPatterns.h"
#include "patterns/helpers/params.h"
#include "luts/colourCorrectionLut.h"
#include "luts/incandescentLut.h"
#include "luts/gammaLut.h"
#include "outputs/udpOutput.h"
#include "pipe.h"
#include "configurationStruct.h"

//I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
//http://fastled.io/docs/3.1/group___color_enums.html
//note the different order, fastled uses RGB, luts are in ouput order (GRB)
//LUT *NeopixelLut = new ColourCorrectionLUT(1.5, 255, 176, 255, 240);

LUT *NeopixelLut = new ColourCorrectionLUT(1.2, 255, 176, 200, 255);
LUT *IncandescentLut = new IncandescentLUT(2.5, 4096, 200);
LUT *IncandescentLut8 = new IncandescentLUT(2.5, 255, 24);
LUT *GammaLut12 = new GammaLUT(2.5, 4096);

void updateParams()
{
        const int apcColumn = 0;
        if (APCMini::getStatus(apcColumn,0)){
            //Pink
            Params::primaryColour = RGBA(255,0,200,255);
            Params::secondaryColour = RGBA(170,0,255,255);
            Params::highlightColour = RGBA(0,255,0,255);
        }

        if (APCMini::getStatus(apcColumn,1)){ //OK
            //Red
            Params::primaryColour = RGB(255,0,0);
            Params::secondaryColour = RGB(255,75,0);
            Params::highlightColour = RGB(255,255,0);
        }

        if (APCMini::getStatus(apcColumn,2)){
            //Yellow
            Params::primaryColour = RGB(255,255,0);
            Params::secondaryColour = RGB(255,100,0);
            Params::highlightColour = RGB(255,175,10);
        }

        if (APCMini::getStatus(apcColumn,3)){
            //Hot
            Params::primaryColour = RGB(255,0,0);
            Params::secondaryColour = RGB(255,150,0);
            Params::highlightColour = RGB(255,200,0);
        }

        if (APCMini::getStatus(apcColumn,4)){
            //Cool
            Params::primaryColour = RGB(0,100,255);
            Params::secondaryColour = RGB(0,0,200);
            Params::highlightColour = RGB(150,150,150);
        }

        if (APCMini::getStatus(apcColumn,5)){
            //Green blue
            Params::primaryColour = RGB(0,0,255);
            Params::secondaryColour = RGB(0,255,0);
            Params::highlightColour = RGB(0,127,127);
        }

        if (APCMini::getStatus(apcColumn,6)){
            //blue/orange
            Params::primaryColour = RGB(255,0,0);
            Params::secondaryColour = RGB(255,100,0);
            Params::highlightColour = RGB(255,127,0);
        }

        if (APCMini::getStatus(apcColumn,7)){
            //blue red
            Params::primaryColour = RGB(0,0,255);
            Params::secondaryColour = RGB(255,0,255);
            Params::highlightColour = RGB(255,0,0);
        }
}

void LoadConfiguration()
{

    Configuration.hostname = "HyperionMaster";
    Configuration.pwmFrequency = 100;

    Configuration.tapMidiNote = 98;
    Configuration.tapStopMidiNote = 89;
    Configuration.tapAlignMidiNote = 88;
    Configuration.tapBarAlignMidiNote = 87;
    
    Configuration.paramCalculator = updateParams;


    Configuration.pipes = {
        // Pipe(
        //     //create an apcmini input that creates monochrome patterns
        //     new LayeredApcminiInput<Monochrome>(
        //         30,
        //         0,
        //         new LayeredPattern<Monochrome> *[8] {
        //             new Layered::SinPattern(),
        //             new Layered::SinPattern(1),
        //             new Layered::SinPattern(-1),
        //             new Layered::BeatShakePattern(),
        //             new Layered::GlitchPattern(),
        //             new Layered::SinPattern(),
        //             new Layered::SlowStrobePattern(),
        //             new Layered::BlinderPattern()
        //         }),
        //     new DMXOutput(1)
        //     //new NullOutput()
        //     ),

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
        // BPM PULSE INDICATOR
        ///////////////////////
        new Pipe(
            new PatternInput<RGB>(1, new BPMIndicatorPattern()),
            new RotaryOutput()),

            

        ///////////////////////
        // PAR & FAIRYLIGHTS
        ///////////////////////

        //PAR
        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                1,
                1,
                new LayeredPattern<Monochrome> *[8] {
                    new Layered::BlinderPattern(Transition::none, Transition::none,255,100,1500),
                    new Layered::SinPattern(),
                    new Layered::EmptyPattern(),
                    new Layered::EmptyPattern(),
                    new Layered::EmptyPattern(),

                    new Layered::BlinderPattern(Transition::none, Transition::none,255,100,1500),
                    new Layered::EmptyPattern(),
                    new Layered::EmptyPattern()
                }),
            new PWMOutput(12),
            Pipe::transfer<Monochrome, Monochrome12>,
            IncandescentLut),

        //FAIRY
        new Pipe(
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
            new PWMOutput(11),
            Pipe::transfer<Monochrome, Monochrome12>,
            GammaLut12),

        ///////////////////////
        // VERGIET
        ///////////////////////

        //on dmx for testing at home
        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                6,
                2,
                new LayeredPattern<Monochrome> *[8] {
                    new Layered::BlinderPattern(Transition::fromCenter, Transition::fromSides,150,1000,1500),
                    new Layered::GlowPattern(),
                    new Layered::SinPattern(),
                    new Layered::BeatMultiFadePattern(500),
                    new Layered::GlitchPattern(),
                    
                    new Layered::BlinderPattern(Transition::fromLeft,Transition::fromRight),
                    new Layered::GlitchPattern(),
                    new Layered::BeatAllFadePattern(600),
                }),
            new DMXOutput(128),
            Pipe::transfer<Monochrome, Monochrome>,
            IncandescentLut8),

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                6,
                2,
                new LayeredPattern<Monochrome> *[8] {
                    new Layered::BlinderPattern(Transition::fromCenter, Transition::fromSides,150,1000,1500),
                    new Layered::GlowPattern(),
                    new Layered::SinPattern(),
                    new Layered::BeatMultiFadePattern(500),
                    new Layered::GlitchPattern(),
                    
                    new Layered::BlinderPattern(Transition::fromLeft,Transition::fromRight),
                    new Layered::GlitchPattern(),
                    new Layered::BeatAllFadePattern(600),
                }),
            new DMXOutput(128+6),
            Pipe::transfer<Monochrome, Monochrome>,
            IncandescentLut8),

        // //on pwm output for on location
        // new Pipe(
        //     new LayeredApcminiInput<Monochrome>(
        //         10,
        //         2,
        //         new LayeredPattern<Monochrome> *[8] {
        //             new Layered::BlinderPattern(Transition::fromCenter, Transition::fromSides,150,1000,1500),
        //             new Layered::GlowPattern(),
        //             new Layered::SinPattern(),
        //             new Layered::BeatMultiFadePattern(500),
        //             new Layered::GlitchPattern(),
                    
        //             new Layered::BlinderPattern(Transition::fromLeft,Transition::fromRight),
        //             new Layered::GlitchPattern(),
        //             new Layered::BeatAllFadePattern(600),
        //         }),
        //     new PWMOutput(),
        //     Pipe::transfer<Monochrome, Monochrome12>,
        //     IncandescentLut),
            
        ///////////////////////
        // NEOPIXEL LEDSTRIPS
        ///////////////////////


        new Pipe( 
            new LayeredApcminiInput<RGBA>(
                50, //width of the pattern, in pixels
                3,   //button column on the apc to listen to (0-7)
                new LayeredPattern<RGBA> *[8] {
                //the patterns to attach to the buttons
                    new LedStrip::GradientPattern(),
                    new LedStrip::ClivePattern(),
                    new LedStrip::TripleFadePattern(),
                    new LedStrip::MeteorPattern(),
                    new LedStrip::GlitchPattern(),

                    new LedStrip::BlinderPattern(Transition::fromCenter, Transition::fromSides,150,1000,1500),
                    new LedStrip::GlitchPattern(),
                    new LedStrip::SlowStrobePattern()
                }),
            new NeopixelOutput<Kpbs800>(2),
            Pipe::transfer<RGBA, GRB>,
            NeopixelLut),

        new Pipe(
            new LayeredApcminiInput<RGBA>(
                50, //width of the pattern, in pixels
                3,   //button column on the apc to listen to (0-7)
                //the patterns to attach to the buttons
                new LayeredPattern<RGBA> *[8] {
                    new LedStrip::GradientPattern(),
                    new LedStrip::ClivePattern(),
                    new LedStrip::TripleFadePattern(),
                    new LedStrip::MeteorPattern(),
                    new LedStrip::GlitchPattern(),

                    new LedStrip::BlinderPattern(Transition::fromCenter, Transition::fromSides,150,1000,1500),
                    new LedStrip::GlitchPattern(),
                    new LedStrip::SlowStrobePattern()
                }),
            new NeopixelOutput<Kpbs800>(1),
            Pipe::transfer<RGBA, GRB>,
            NeopixelLut),

        new Pipe(
            new LayeredApcminiInput<RGBA>(
                50, //width of the pattern, in pixels
                3,   //button column on the apc to listen to (0-7)
                //the patterns to attach to the buttons
                new LayeredPattern<RGBA> *[8] {
                    new LedStrip::GradientPattern(),
                    new LedStrip::ClivePattern(),
                    new LedStrip::TripleFadePattern(),
                    new LedStrip::MeteorPattern(),
                    new LedStrip::GlitchPattern(),

                    new LedStrip::BlinderPattern(Transition::fromCenter, Transition::fromSides,150,1000,1500),
                    new LedStrip::GlitchPattern(),
                    new LedStrip::SlowStrobePattern()
                }),
            new NeopixelOutput<Kpbs800>(7),
            Pipe::transfer<RGBA, GRB>,
            NeopixelLut),

                new Pipe(
            new LayeredApcminiInput<RGBA>(
                50, //width of the pattern, in pixels
                3,   //button column on the apc to listen to (0-7)
                //the patterns to attach to the buttons
                new LayeredPattern<RGBA> *[8] {
                    new LedStrip::GradientPattern(),
                    new LedStrip::ClivePattern(),
                    new LedStrip::TripleFadePattern(),
                    new LedStrip::MeteorPattern(),
                    new LedStrip::GlitchPattern(),

                    new LedStrip::BlinderPattern(Transition::fromCenter, Transition::fromSides,150,1000,1500),
                    new LedStrip::GlitchPattern(),
                    new LedStrip::SlowStrobePattern()
                }),
            new NeopixelOutput<Kpbs800>(8),
            Pipe::transfer<RGBA, GRB>,
            NeopixelLut),

        ///////////////////////
        // MOVING HEAD
        ///////////////////////

        // new Pipe(
        //     new LayeredApcminiInput<MovingHead>(
        //         3,
        //         4,
        //         new LayeredPattern<MovingHead> *[8] {
        //             //
        //             //new MovingheadPatterns::TestPattern(),
        //             new MovingheadPatterns::WallPattern(),
        //             new MovingheadPatterns::WallDJPattern(),
        //             new MovingheadPatterns::SidesPattern(),
        //             new MovingheadPatterns::Flash360Pattern(),
        //             new MovingheadPatterns::GlitchPattern(),


        //             new MovingheadPatterns::FrontPattern(),
        //             new MovingheadPatterns::UVPattern(),
        //             new MovingheadPatterns::GlitchPattern(),
        //         }),
        //     new DMXOutput(30),
        //     Pipe::transfer<MovingHead,Miniwash7>
        // ),

        // /////////////////////
        // FAIRYLIGHTS
        // /////////////////////
        // Pipe(
        //     new LayeredApcminiInput<Monochrome>(
        //         1,
        //         5,
        //         new LayeredPattern<Monochrome> *[8] {
        //             new Layered::BlinderPattern(Transition::none, Transition::none,255,100,1500),
        //             new Layered::SinPattern(),
        //             new Layered::SinPattern(4,3.1415),
        //             new Layered::SlowStrobePattern(),
        //             new Layered::BeatAllFadePattern(1000),

        //             new Layered::BlinderPattern(Transition::none, Transition::none,255,100,1500),
        //             new Layered::BeatAllFadePattern(150),
        //             new Layered::FastStrobePattern(),
        //         }),
        //     new PWMOutput(11),
        //     Pipe::transfer<Monochrome, Monochrome12>,
        //     GammaLut12),

        ///////////////////////
        // LASERS
        ///////////////////////
        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                10, //width of the pattern, in pixels
                6,  //button column on the apc to listen to (0-7)
                //the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8] {
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::SinPattern(),
                new Layered::BeatSingleFadePattern(),
                new Layered::BeatMultiFadePattern(),
                new Layered::BeatShakePattern(),
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::GlitchPattern(),
                new Layered::SlowStrobePattern()
                }),
            new UDPOutput("lasers.local", 9619, 100),
            Pipe::transfer<Monochrome, Monochrome12>),

        ///////////////////////
        // STROBES
        ///////////////////////
        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                10, //width of the pattern, in pixels
                7,  //button column on the apc to listen to (0-7)
                //the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8] {
                new Layered::BlinderPattern(Transition::fromCenter,Transition::fromCenter,80),
                //new Layered::BeatSingleFadePattern(),
                new Layered::BeatSingleFadePattern(),
                new Layered::BeatShakePattern(),
                new Layered::BeatAllFadePattern(),
                new Layered::SlowStrobePattern(),

                new Layered::BlinderPattern(Transition::fromCenter,Transition::fromCenter),
                new Layered::GlitchPattern(),
                new Layered::FastStrobePattern()
                }),
            new UDPOutput("strobes.local", 9619, 100),
            Pipe::transfer<Monochrome, Monochrome12>
        ),

        ///////////////////////
        // BPM FILL LEDS
        ///////////////////////
        // Pipe(
        //     new PatternInput<RGB>(32,new BPMFillPattern()),
        //     new NeopixelOutput<Kpbs800>(2),
        //     Pipe::transfer<RGB, GRB>,
        //     NeopixelLut),


        // Pipe(
        //     new PatternInput<RGB>(16, new WatcherPattern2()),
        //     new NeopixelOutput<Kpbs800>(1),
        //     Pipe::transfer<RGB, GRB>,
        //     NeopixelLut)

    };
}

