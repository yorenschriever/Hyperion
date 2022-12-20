#pragma once

#include "pipe.h"
#include "colours.h"
#include "outputs/neopixelOutput.h"
#include "outputs/udpOutput.h"
#include "inputs/layeredPatternInput.h"
#include "inputs/rotaryPatternInput.h"
#include "inputs/inputSplitter.h"
#include "patterns/mappedPatterns.h"
#include "patterns/ledstripPatterns.h"
#include "luts/colourCorrectionLut.h"
#include "configurationStruct.h"
#include "maps/tree2022-couch.h"
#include "maps/tree2022-desk.h"
#include "../hardware/rotary/rotary.h"

LUT *NeopixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 255);
const std::vector<int> sizes = {200, 200, 200, 200, 200, 150, 150, 200};

void rotaryClick()
{
    const int palettes = 4;
    static int selectedPalette = -1;
    selectedPalette = (selectedPalette + 1) % palettes;

    switch (selectedPalette)
    {
    case 0:
        //orange white
        Params::primaryColour = RGB(100, 100, 75);
        Params::secondaryColour = RGB(255, 100, 0);
        Params::highlightColour = RGB(100, 100, 75);
        break;
    case 1:
        //blue red
        Params::primaryColour = RGB(50, 0, 255);
        Params::secondaryColour =  RGB(255, 0, 50); 
        break;
    case 2:
        //blue cyan
        Params::primaryColour = RGB(0, 0, 255);
        Params::secondaryColour = RGB(0, 200, 255);
        break;
    case 3:
        //red yellow
        Params::primaryColour = RGB(255, 0, 0);
        Params::secondaryColour = RGB(255, 170, 0);
        break;
    }
}

void setRotary()
{
    Rotary::onPress(rotaryClick);
}

void LoadConfiguration()
{
    Configuration.hostname = "hyperion";

    Params::velocity = 0.3;

    auto sizesInBytes = std::vector<int>();
    for (auto size : sizes)
        sizesInBytes.push_back(size * sizeof(RGBA));

    // allocate on heap, splitter must remain available after this function is finished
    auto split = new InputSplitter(
        new RotaryPatternInput<RGBA>(
            1500,
            {
                new LedStrip::GlowPulsePattern(),
                new Mapped::HorizontalDitheredGradientPattern(treeDesk),
                new Mapped::HorizontalGradientPattern(treeDesk),
                new Mapped::ConcentricWavePattern<SawDown>(treeDesk, 1, 5000),
                new Mapped::DiagonalWavePattern<SoftPWM>(treeDesk,5000,-2,0.66)
            }),
        sizesInBytes);

    for (int i = 0; i < sizes.size(); i++)
    {
        Configuration.pipes.push_back(
            new Pipe(
                split->getInput(i),
                new NeopixelOutput<Kpbs800>(1 + i),
                Pipe::transfer<RGBA, GRB>,
                NeopixelLut));
    }

    rotaryClick();
    //a hack to set the rotary listener again after tampTempo claims it.
    Configuration.paramCalculator = setRotary;
}
