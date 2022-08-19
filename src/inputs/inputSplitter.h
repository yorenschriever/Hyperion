#pragma once

#include <algorithm>
#include <cstring>
#include <vector>
#include "input.h"
#include "bufferInput.h"
#include "Arduino.h"

// InputSplitter splits the input into separate inputs that can be used in different pipelines.
// I wrote an InputSplitter instead of an OutputSplitter, so you have multiple pipes.
// This has advantages:
// - Each pipe can do its own colour conversion, so you can use multiple types of lights
// - Each pipe runs at its own fps, so the fastest output is not limited by the throughput of
//   the slowest Output.
class InputSplitter
{

public:
    InputSplitter(Input *sourceInput, std::vector<int> lengths)
    {
        this->sourceInput = sourceInput;
        int start = 0;
        for (int length : lengths)
        {
            auto bi = new BufferInput(this->buffer + start, length);
            bi->setCallbacks(LoadData, Begin, (void *)this);
            this->destinationInputs.push_back(bi);
            start += length;
        }
    }

    static void Begin(void *argument)
    {
        auto instance = (InputSplitter *)argument;
        instance->sourceInput->begin();
    }

    static void LoadData(void *argument)
    {
        auto instance = (InputSplitter *)argument;
        auto len = instance->sourceInput->loadData(instance->buffer);
        if (len > 0)
        {
            for (auto di : instance->destinationInputs)
            {
                di->setFrameReady();
            }
        }
    }

    Input *getInput(int index)
    {
        return destinationInputs[index];
    }

private:
    Input *sourceInput;
    std::vector<BufferInput *> destinationInputs;
    static const int MTU = 3100;
    uint8_t buffer[MTU];
};
