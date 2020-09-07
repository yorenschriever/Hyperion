#pragma once
#include "colours.h"
#include "pipe.h"
#include "inputs/udpInput.h"
#include "outputs/neopixelOutput.h"
#include "outputs/pwmOutput.h"
#include "outputs/dmxOutput.h"
#include "luts/colourCorrectionLut.h"
#include "luts/incandescentLut.h"

const char* HostName = "lasers";
#define PWMFrequency 1500

LUT* LasersLut = new IncandescentLUT(2.5, 4096, 400);

Pipe pipes[] = {

    Pipe(
        new UDPInput(9619),
        new PWMOutput(),
        Pipe::transfer<Monochrome12,Monochrome12>,
        LasersLut),

};