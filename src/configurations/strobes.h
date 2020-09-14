#pragma once
#include "colours.h"
#include "pipe.h"
#include "inputs/udpInput.h"
#include "outputs/neopixelOutput.h"
#include "outputs/pwmOutput.h"
#include "outputs/dmxOutput.h"
#include "luts/colourCorrectionLut.h"
#include "luts/gammaLut.h"
#include "metaoutputs/temperatureControlledOutput.h"
#include "outputs/rotaryOutput.h"
#include "inputs/patternInput.h"
#include "patterns/rgbPatterns.h"
#include "configurationStruct.h"

LUT *LedLut = new GammaLUT(2, 4096);

void LoadConfiguration()
{

    Configuration.hostname = "strobes";
    Configuration.pwmFrequency = 1500;

    Configuration.pipes = {
        Pipe(
            new UDPInput(9619),
            new TemperatureControlledOutput<Monochrome12>(
                new PWMOutput(),
                0.77,
                0.0058,
                60),
            Pipe::transfer<Monochrome12, Monochrome12>,
            LedLut),

        Pipe(
            new PatternInput<RGB>(1, new BPMIndicatorPattern()),
            new RotaryOutput())};
}