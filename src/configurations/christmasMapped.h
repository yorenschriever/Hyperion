#pragma once

#include "pipe.h"
#include "colours.h"
#include "outputs/neopixelOutput.h"
#include "outputs/udpOutput.h"
#include "inputs/layeredPatternInput.h"
#include "inputs/inputSplitter.h"
#include "patterns/mappedPatterns.h"
#include "luts/colourCorrectionLut.h"
#include "configurationStruct.h"
#include "maps/tree2022-couch.h"
#include "maps/tree2022-desk.h"


LUT *NeopixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 255);

const std::vector<int> sizes = {200,200,200,200,200,150,150,200};

void LoadConfiguration()
{
    Configuration.hostname = "hyperion";

    Params::primaryColour = RGBA(255, 100, 0, 255);
    Params::secondaryColour = RGBA(100, 100, 75, 255);
    Params::velocity = 0.3;

    auto sizesInBytes = std::vector<int>();
    for(auto size : sizes) 
        sizesInBytes.push_back(size * sizeof(RGBA));

    //allocate on heap, splitter must remain available after this function is finished
    auto split = new InputSplitter(
        new LayeredPatternInput<RGBA>(
            1500,
            new Mapped::ConcentricWavePattern<SawDown>(treeDesk, 1, 5000)
        ),
        sizesInBytes
    );

    for (int i = 0; i < sizes.size(); i++)
    {
        Configuration.pipes.push_back(
            new Pipe(
                split->getInput(i),
                new NeopixelOutput<Kpbs800>(1 + i),
                Pipe::transfer<RGBA, GRB>,
                NeopixelLut));
    }
}
