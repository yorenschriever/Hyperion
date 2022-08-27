#pragma once

#include "outputs/spiOutput.h"
#include "luts/colourCorrectionLut.h"
#include "colours.h"
#include "pipe.h"
#include "configurationStruct.h"
#include "inputs/inputSplitter.h"
#include "outputs/udpOutput.h"
#include "inputs/patternInput.h"
#include "patterns/rgbPatterns.h"

LUT *PixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);

void LoadConfiguration()
{
    Configuration.hostname = "hoepels";

    auto split = new InputSplitter(
        new PatternInput<RGB>(500, new RainbowPattern()),
        {300, 300, 300, 300, 300},
        true);

    Configuration.pipes = {
        new Pipe(
            split->getInput(0),
            new UDPOutput("192.168.1.222", 9601, 50)),
        new Pipe(
            split->getInput(1),
            new UDPOutput("192.168.1.223", 9601, 50)),
        new Pipe(
            split->getInput(2),
            new UDPOutput("192.168.1.224", 9601, 50)),
        new Pipe(
            split->getInput(3),
            new UDPOutput("192.168.1.228", 9601, 50)),
        new Pipe(
            split->getInput(4),
            new UDPOutput("192.168.1.229", 9601, 50)),

    };
}