#pragma once

#include <algorithm>
#include <cstring>
#include "input.h"
#include "Arduino.h"
#include <vector>
#include "debug.h"

struct CombinedInputPart {
    Input* input;
    int offset;
};

//CombinedInput reads the input from multiple sources and glues them together
class CombinedInput : public Input
{

public:
    CombinedInput(std::vector<CombinedInputPart> parts, int bufferSize)
    {
        this->parts = parts;
        this->bufferSize = bufferSize;
        dataBuffer = (uint8_t*) malloc(bufferSize);

        if (!dataBuffer){
            Debug.printf("Unable to allocate memory for patternInput, free heap = %d\n",ESP.getFreeHeap());
            ESP.restart();
        }
    }

    virtual void begin()
    {
        for(auto part: parts)
            part.input->begin();
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        int totalCopied = 0;
        for(auto part: parts)
            totalCopied += part.input->loadData(dataBuffer + part.offset);

        if (totalCopied == 0)
            return 0;

        usedframecount++;
        memcpy(dataPtr, dataBuffer, bufferSize);
        return bufferSize;
    }

private:
    std::vector<CombinedInputPart> parts;
    int bufferSize;
    uint8_t* dataBuffer;
};