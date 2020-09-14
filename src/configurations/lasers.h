#pragma once
#include "colours.h"
#include "pipe.h"
#include "inputs/udpInput.h"
#include "outputs/neopixelOutput.h"
#include "outputs/pwmOutput.h"
#include "outputs/dmxOutput.h"
#include "luts/colourCorrectionLut.h"
#include "luts/incandescentLut.h"
#include "configurationStruct.h"

LUT* LasersLut = new IncandescentLUT(2.5, 4096, 400);

void LoadConfiguration()
{

    Configuration.hostname = "lasers";
    Configuration.pwmFrequency = 1500;

    Configuration.pipes = {
        Pipe(
            new UDPInput(9619),
            new PWMOutput(),
            Pipe::transfer<Monochrome12,Monochrome12>,
            LasersLut),
    };

}

