#pragma once

#include "input.h"
#include "hardware/DMX/dmx.h"
#include "../patterns/pattern.h"

///////
// This class is still untested!
///////

// this class attaches a provided set of 24 patterns to a dmx input.
// Patterns should all be the same 'Colour'. You have to provide this colour as template here
template <class T_COLOUR>
class DMXPatternInput : public Input
{

public:
    // length = the length in pixels
    // patterns = a list of patterns to attach to each channel
    DMXPatternInput(int length, std::vector<LayeredPattern<T_COLOUR>*> patterns, int startChannel=1)
    {
        this->length = length;
        this->patterns = patterns;
        this->startChannel = startChannel;
        this->leddata = (T_COLOUR *)malloc(length * sizeof(T_COLOUR));

        if (!this->leddata){
            Debug.printf("Unable to allocate memory for DMXPatternInput, free heap = %d\n",ESP.getFreeHeap());
            ESP.restart();
        }
    }

    virtual void begin()
    {
        DMX::Initialize();

        for (int i = 0; i < length; i++)
            this->leddata[i] = T_COLOUR();

        for (auto pattern: patterns)
            pattern->Initialize();
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        uint8_t* buffer = DMX::GetDataPtr();

        Params::velocity  = (float )buffer[startChannel + 0] / 0xff;
        Params::intensity = (float )buffer[startChannel + 1] / 0xff;
        Params::variant   = (float )buffer[startChannel + 2] / 0xff;

        Params::primaryColour   = RGB(buffer[startChannel + 3],buffer[startChannel + 4],buffer[startChannel + 5]);
        Params::secondaryColour = RGB(buffer[startChannel + 6],buffer[startChannel + 7],buffer[startChannel + 8]);
        Params::highlightColour = RGB(buffer[startChannel + 9],buffer[startChannel + 10],buffer[startChannel + 11]);
        uint8_t masterDim = buffer[12];

        for (int i = 0; i < length; i++)
            ((T_COLOUR *)dataPtr)[i] = T_COLOUR();

        for (auto pattern: patterns)
        {
            for (int i = 0; i < length; i++)
                leddata[i] = T_COLOUR();

            uint8_t channelDim = buffer[startChannel + 13 + patttern];
            pattern->Calculate(leddata, length, channelDim > 0);

            // apply dimming and copy to leddata buffer
            for (int i = 0; i < length; i++)
            {
                uint8_t dimValue = channelDim * masterDim >> 8;
                //TODO give a good thought about when we want dimming and when we want to make things transparent. 
                //how to support this in general?
                if (std::is_same<T_COLOUR,RGBA>::value) {
                    RGBA* l = &leddata[i];
                    l->A = (l->A * dimValue) >> 8;
                } else {
                    leddata[i].dim(dimValue);
                }
                ((T_COLOUR *)dataPtr)[i] += leddata[i];
            }
        } 

        usedframecount++;
        return length * sizeof(T_COLOUR);
    }

private:
    int length = 0;
    T_COLOUR *leddata;
    std::vector<LayeredPattern<T_COLOUR>*>patterns;
    int startChannel;
};