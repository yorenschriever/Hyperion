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

LUT *IncandescentLut8 = new IncandescentLUT(2.5, 255, 24);

void LoadConfiguration()
{

    Configuration.hostname = "Hyperion";
    Configuration.pwmFrequency = 100;

    Configuration.tapMidiNote = 98;
    Configuration.tapStopMidiNote = 89;
    Configuration.tapAlignMidiNote = 88;
    Configuration.tapBarAlignMidiNote = 87;

    Configuration.pipes = {

        ///////////////////////
        // BPM PULSE INDICATOR
        ///////////////////////
        new Pipe(
            new PatternInput<RGB>(1, new BPMIndicatorPattern()),
            new RotaryOutput()),


        ///////////////////////
        // VERGIET
        ///////////////////////

        //on dmx
        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                20,
                2,
                new LayeredPattern<Monochrome> *[8] {
                    new Layered::BlinderPattern(Transition::fromCenter, Transition::fromSides,150,1000,1500),
                    new Layered::GlowPattern(5),
                    new Layered::GlowPattern(),
                    new Layered::BeatMultiFadePattern(500),
                    new Layered::BeatShakePattern(),
                    
                    new Layered::BlinderPattern(Transition::fromLeft,Transition::fromRight),
                    new Layered::GlitchPattern(),
                    new Layered::BeatAllFadePattern(600),
                }),
            new DMXOutput(128),
            Pipe::transfer<Monochrome, Monochrome>,
            IncandescentLut8),
         
    };
}

