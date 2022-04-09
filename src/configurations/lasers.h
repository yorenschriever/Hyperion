#pragma once
#include "colours.h"
#include "pipe.h"
#include "inputs/udpInput.h"
#include "outputs/neopixelOutput.h"
#include "outputs/pwmOutput.h"
#include "outputs/dmxOutput.h"
#include "luts/colourCorrectionLut.h"
#include "luts/incandescentLut.h"
#include "luts/laserLut.h"
#include "configurationStruct.h"

LUT *LaserLut = new LaserLUT();

void LoadConfiguration()
{

    Configuration.hostname = "lasers";
    Configuration.pwmFrequency = 1500;

    Configuration.pipes = {
        new Pipe(
            new UDPInput(9619),
            new PWMOutput(),
            Pipe::transfer<Monochrome12,Monochrome12>,
            LaserLut),
    };

}

