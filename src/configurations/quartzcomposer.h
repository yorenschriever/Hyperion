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
//LUT* NeopixelLut = new ColourCorrectionLUT(1.5,255, 176, 255, 240); 
LUT* NeopixelLut = new ColourCorrectionLUT(1.5, 255, 255, 225, 225); 
LUT* IncandescentLut = new IncandescentLUT(2.5, 4096, 400);


void LoadConfiguration()
{
    Params::primaryColour = RGBA(255, 200, 0, 255);
    Params::secondaryColour = RGBA(0, 255, 255, 255);
    Params::highlightColour = RGBA(0, 0, 255, 255);
    Params::velocity = 0.3;
    Params::intensity = 0.7;
    Params::variant = 0.7;
    
    Configuration.hostname = "hyperion";
    Configuration.pwmFrequency = 1500;

    Configuration.pipes = {

    new Pipe(
        new UDPInput(9611),
        new NeopixelOutput<Kpbs800>(1),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    new Pipe(
        new UDPInput(9612),
        new NeopixelOutput<Kpbs800>(2),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    new Pipe(
        new UDPInput(9613),
        new NeopixelOutput<Kpbs800>(3),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    new Pipe(
        new UDPInput(9614),
        new NeopixelOutput<Kpbs800>(4),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    new Pipe(
        new UDPInput(9615),
        new NeopixelOutput<Kpbs800>(5),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    //Shared with DMX!
    new Pipe(
        new UDPInput(9616),
        new NeopixelOutput<Kpbs800>(6),
        //new DMXOutput(),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    //new  Pipe(
    //     new UDPInput(9619),
    //     new DMXOutput(1)),

    new Pipe(
        new UDPInput(9617),
        new NeopixelOutput<Kpbs800>(7),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    new Pipe(
        new UDPInput(9618),
        new NeopixelOutput<Kpbs800>(8),
        Pipe::transfer<RGB,GRB>,
        NeopixelLut),

    //new  Pipe(
    //     new UDPInput(9619),
    //     new PWMOutput(),
    //     Pipe::transfer<RGB,Monochrome12>,
    //     IncandescentLut),
    };

};
