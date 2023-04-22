#pragma once

#include "outputs/spiOutput.h"
#include "outputs/udpOutput.h"
#include "outputs/rotaryOutput.h"
#include "inputs/patternCycleInput.h"
#include "inputs/nanoKontrolInput.h"
#include "inputs/patternInput.h"
#include "patterns/ledsterPatterns.h"
#include "patterns/rgbPatterns.h"
#include "patterns/helpers/tempo/constantTempo.h"
#include "luts/colourCorrectionLut.h"
#include "colours.h"
#include "pipe.h"
#include "configurationStruct.h"
#include "maps/ledsterMap.h"
#include "patterns/helpers/pixelMap.h"
#include "inputs/fallbackInput.h"
#include "inputs/udpInput.h"
#include "inputs/switchableInput.h"
#include "inputs/nullInput.h"

LUT *PixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);
const int numLeds = 481;

int dynamicColorConversionTransfer(uint8_t *data, int length, Output *out, LUT *lut)
{
    if (length == numLeds * sizeof(RGBA))
        return Pipe::transfer<RGBA, RGB>(data, length, out, lut);
    else
        return Pipe::transfer(data, length, out, lut);
}

void LoadConfiguration()
{
    Params::primaryColour = RGBA(255, 0, 0, 255);
    Params::secondaryColour = RGBA(0, 255, 0, 255);
    Params::highlightColour = RGBA(0, 0, 255, 255);
    Params::velocity = 0.3;
    Params::intensity = 0.7;
    Params::variant = 0.7;

    Tempo::AddSource(ConstantTempo::getInstance());
    ConstantTempo::getInstance()->setBpm(120);

    Configuration.hostname = "ledsterstandalone";

    Configuration.pipes = {

        new Pipe(
            new UDPInput(9601),
            new SpiOutput(0, 1, 500000)
        )

        // new Pipe(
        //     new FallbackInput(
        //         new UDPInput(9601),
        //         new PatternCycleInput<RGBA>(
        //             numLeds,
        //             std::vector<LayeredPattern<RGBA> *>{
        //                 new Ledster::RadarPattern(ledsterMap),
        //                 new Ledster::PetalChasePattern(),
        //                 new Ledster::ConcentricChaserPattern(),
        //                 new Ledster::SnakePattern(),
        //                 new Ledster::RadialFadePattern(ledsterMap),
        //                 new Ledster::RibbenClivePattern<SoftSquare>(),
        //                 new Ledster::SnowflakePattern(),
        //                 new Ledster::PetalRotatePattern(),
        //                 new Ledster::ClivePattern<LFOPause<SinFast>>(numLeds, 10000, 5, 0.25)},
        //             30000 // duration
        //             )
        //         ),
        //     new SpiOutput(0, 1, 500000),
        //     dynamicColorConversionTransfer,
        //     PixelLut
        // )
    };
}