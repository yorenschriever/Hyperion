#pragma once

#include "input.h"
#include "../patterns/pattern.h"

//LayeredPatternInput displays 1 pattern continuously.
template <class T_COLOUR>
class LayeredPatternInput : public Input
{

public:
    //length = the length in pixels
    //pattern = the pattern to display
    LayeredPatternInput(int length, LayeredPattern<T_COLOUR> *pattern)
    {
        this->length = length;
        this->pattern = pattern;
        this->leddata = (T_COLOUR *)malloc(length * sizeof(T_COLOUR));

        if (!this->leddata){
            Debug.printf("Unable to allocate memory for layeredPatternInput, free heap = %d\n",ESP.getFreeHeap());
            ESP.restart();
        }
    }

    virtual void begin()
    {
        for (int i = 0; i < length; i++)
            leddata[i] = T_COLOUR();

        pattern->Initialize();
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        //reset everything to 0
        for (int i = 0; i < length; i++)
            leddata[i] = T_COLOUR();

        pattern->Calculate(leddata, length, true);

        memcpy(dataPtr, leddata, length * sizeof(T_COLOUR));

        usedframecount++;

        return length * sizeof(T_COLOUR);
    }

private:
    int length = 0;
    T_COLOUR *leddata;
    LayeredPattern<T_COLOUR> *pattern;
};