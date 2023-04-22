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

//I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
//http://fastled.io/docs/3.1/group___color_enums.html
//note the different order, fastled uses RGB, luts are in ouput order (GRB)
//when i calibrated myself i found different values, see standalone.h
//LUT* HaloLut = new ColourCorrectionLUT(1.5,255, 176, 255, 240); 

void LoadConfiguration()
{

    Configuration.hostname = "haloslave";
    Configuration.pwmFrequency = 1500;

    Configuration.pipes = {

    new Pipe(
        new UDPInput(9611),
        new NeopixelOutput<Kpbs800>(1)),

    new Pipe(
        new UDPInput(9612),
        new NeopixelOutput<Kpbs800>(2)),

    new Pipe(
        new UDPInput(9613),
        new NeopixelOutput<Kpbs800>(3)),

    new Pipe(
        new UDPInput(9614),
        new NeopixelOutput<Kpbs800>(4)),

    new Pipe(
        new UDPInput(9615),
        new NeopixelOutput<Kpbs800>(5)),

    //Shared with DMX!
    new Pipe(
        new UDPInput(9616),
        new NeopixelOutput<Kpbs800>(6)),

    //new  Pipe(
    //     new UDPInput(9619),
    //     new DMXOutput(1)),

    new Pipe(
        new UDPInput(9617),
        new NeopixelOutput<Kpbs800>(7)),

    new Pipe(
        new UDPInput(9618),
        new NeopixelOutput<Kpbs800>(8)),

    //new  Pipe(
    //     new UDPInput(9619),
    //     new PWMOutput(),
    //     Pipe::transfer<RGB,Monochrome12>,
    //     IncandescentLut),
    };

};
