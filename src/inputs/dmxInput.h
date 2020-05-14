#pragma once

#include <algorithm>
#include <cstring>
#include "input.h"
#include "lib/DMX/dmx.h"

//dmxInput listens to input on the dmx port.
class DMXInput : public Input
{

public:
    //startchannel = the first channel it will listen to (1-512)
    //length = the number of channels is will read
    DMXInput(int startChannel = 1, int length = 512)
    {
        this->startChannel = std::min(512, std::max(1, startChannel));
        this->length = std::min(513 - this->startChannel, length);
    }

    virtual void begin()
    {
        DMX::Initialize();
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        int frameNumber = DMX::GetFrameNumber();

        //check if there is a new frame since last time
        if (frameNumber == lastFrameNumber)
            return 0;

        usedframecount++;
        missedframecount += frameNumber - lastFrameNumber - 1;
        lastFrameNumber = frameNumber;

        memcpy(dataPtr, DMX::GetDataPtr() + startChannel, length);

        return length;
    }

private:
    int startChannel;
    int length;
    unsigned int lastFrameNumber = 0;
};