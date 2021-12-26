#pragma once
#include "input.h"

//Use bufferInput when you manually mantain a buffer with pixelData,
//and want to feed it into a pipe
class BufferInput : public Input
{

public:
    BufferInput(uint8_t *buffer, unsigned int length)
    {
        setBuffer(buffer, length);
    }

    void setBuffer(uint8_t *buffer, unsigned int length)
    {
        this->buffer = buffer;
        this->length = length;
    }

    //call this when you updated your buffer with new data
    void setFrameReady()
    {
        frameReady = true;
        missedframecount++;
    }

    virtual void begin()
    {
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        if (!frameReady)
            return 0;

        frameReady = false;

        memcpy(dataPtr, buffer, length);

        usedframecount++;
        missedframecount--;

        return length;
    }

private:
    uint8_t *buffer;
    unsigned int length = 0;
    bool frameReady = false;
};