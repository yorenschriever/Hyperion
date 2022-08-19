#pragma once

#include "outputs/spiOutput.h"
#include "luts/colourCorrectionLut.h"
#include "colours.h"
#include "pipe.h"
#include "configurationStruct.h"
#include "inputs/udpInput.h"
#include "inputs/inputSplitter.h"

LUT *PixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);

auto split = InputSplitter(
    new UDPInput(9601),
    {271,210}
);

void LoadConfiguration()
{
    Configuration.hostname = "ledsterstandalone";

    Configuration.pipes = {
        new Pipe(
            split.getInput(0),
            new SpiOutput(0, 1, 500000)
        ),
        new Pipe(
            split.getInput(1),
            new SpiOutput(4, 5, 500000)
        )
    };
}