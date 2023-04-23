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

void LoadConfiguration()
{

    Configuration.hostname = "hyperslave2";
    Configuration.pwmFrequency = 1500;

    Configuration.pipes = {

    new Pipe(
        new UDPInput(9611),
        new NeopixelOutput<Kpbs800>(1),
        Pipe::transfer<RGB,BGR>
        ),

    new Pipe(
        new UDPInput(9612),
        new NeopixelOutput<Kpbs800>(2),
        Pipe::transfer<RGB,BGR>),

    new Pipe(
        new UDPInput(9613),
        new NeopixelOutput<Kpbs800>(3),
        Pipe::transfer<RGB,BGR>),

    new Pipe(
        new UDPInput(9614),
        new NeopixelOutput<Kpbs800>(4),
        Pipe::transfer<RGB,BGR>),

    new Pipe(
        new UDPInput(9615),
        new NeopixelOutput<Kpbs800>(5),
        Pipe::transfer<RGB,BGR>),

    //Shared with DMX!
    new Pipe(
        new UDPInput(9616),
        new NeopixelOutput<Kpbs800>(6),
        //new DMXOutput(),
        Pipe::transfer<RGB,BGR>),

    //new  Pipe(
    //     new UDPInput(9619),
    //     new DMXOutput(1)),

    new Pipe(
        new UDPInput(9617),
        new NeopixelOutput<Kpbs800>(7),
        Pipe::transfer<RGB,BGR>),

    new Pipe(
        new UDPInput(9618),
        new NeopixelOutput<Kpbs800>(8),
        Pipe::transfer<RGB,BGR>),

    //new  Pipe(
    //     new UDPInput(9619),
    //     new PWMOutput(),
    //     Pipe::transfer<RGB,Monochrome12>,
    //     IncandescentLut),
    };

};
