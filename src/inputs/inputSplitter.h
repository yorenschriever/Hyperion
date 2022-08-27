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
    /*
    * - sourceInput is the input to distribute over the destination inputs
    * - length is in bytes, not in lights
    * - if sync is on, a new frame will not be loaded until all outputs have finished sending their data.
    * This is useful when the sourceinput is an input that generates data on demand instead of listening
    * to incoming data. Without sync it would generate a new frame when any of the destination outputs
    * requests new data.
    */
    InputSplitter(Input *sourceInput, std::vector<int> lengths, bool sync = false)
    {
        this->sourceInput = sourceInput;
        this->sync = sync;
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

        //check all outputs, and ask if there is still a frame pending
        if (instance->sync){
            for (auto di : instance->destinationInputs)
            {
                if (di->getFrameReady()){
                    return;
                }
            }
        }

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
    Input *sourceInput = NULL;
    std::vector<BufferInput *> destinationInputs;
    static const int MTU = 3100;
    uint8_t buffer[MTU];
    bool sync;
};
