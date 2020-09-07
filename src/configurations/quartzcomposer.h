#pragma once
#include "colours.h"
#include "pipe.h"
#include "inputs/udpInput.h"
#include "outputs/neopixelOutput.h"
#include "outputs/pwmOutput.h"
#include "outputs/dmxOutput.h"
#include "luts/colourCorrectionLut.h"
#include "luts/incandescentLut.h"

const char* HostName = "Hyperion";

//I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
//http://fastled.io/docs/3.1/group___color_enums.html
//note the different order, fastled uses RGB, luts are in ouput order (GRB)
LUT* NeopixelLut = new ColourCorrectionLUT(1.5,255, 176, 255, 240); 
LUT* IncandescentLut = new IncandescentLUT(2.5, 4096, 400);

Pipe pipes[] = {

    Pipe(
        new UDPInput(9611),
        new NeopixelOutput<Kpbs800>(1),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    Pipe(
        new UDPInput(9612),
        new NeopixelOutput<Kpbs800>(2),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    Pipe(
        new UDPInput(9613),
        new NeopixelOutput<Kpbs800>(3),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    Pipe(
        new UDPInput(9614),
        new NeopixelOutput<Kpbs800>(4),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    Pipe(
        new UDPInput(9615),
        new NeopixelOutput<Kpbs800>(5),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    // //Shared with DMX!
    // Pipe(
    //     new UDPInput(9616),
    //     new NeopixelOutput<Kpbs800>(6),
    //     //new DMXOutput(),
    //     Pipe::transfer<RGB,GRB>,
    //     NeopixelLut),

    Pipe(
        new UDPInput(9619),
        new DMXOutput(1)),

    Pipe(
        new UDPInput(9617),
        new NeopixelOutput<Kpbs800>(7),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    Pipe(
        new UDPInput(9618),
        new NeopixelOutput<Kpbs800>(8),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    Pipe(
        new UDPInput(9619),
        new PWMOutput(),
        Pipe::transfer<RGB,Monochrome12>,
        IncandescentLut),



};