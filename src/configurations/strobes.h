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

const char* HostName = "strobes";

LUT* LedLut = new GammaLUT(2,4096); 

Pipe pipes[] = {

    Pipe(
        new UDPInput(9619),
        new TemperatureControlledOutput<Monochrome12>(
            new PWMOutput(1500),
            0.77, 
            0.0058,  
            60),
        Pipe::transfer<Monochrome12,Monochrome12>,
        LedLut
    )

};