#pragma once

#include "input.h"
#include "../patterns/pattern.h"

//PatternInput displays 1 pattern continuously.
template <class T_COLOUR>
class PatternInput : public Input
{

public:
    //length = the length in pixels
    //patterns = the pattern to display
    PatternInput(int length, Pattern<T_COLOUR> *pattern)
    {
        this->length = length;
        this->pattern = pattern;
        this->leddata = (T_COLOUR *)malloc(length * sizeof(T_COLOUR));

        if (!this->leddata){
            Debug.printf("Unable to allocate memory for patternInput, free heap = %d\n",ESP.getFreeHeap());
            ESP.restart();
        }
    }

    virtual void begin()
    {
        for (int i = 0; i < length; i++)
            this->leddata[i] = T_COLOUR();

        pattern->Initialize();
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        pattern->Calculate(leddata, length, first);

        memcpy(dataPtr, leddata, length * sizeof(T_COLOUR));

        first = false;
        usedframecount++;

        return length * sizeof(T_COLOUR);
    }

private:
    int length = 0;
    T_COLOUR *leddata;
    Pattern<T_COLOUR> *pattern;
    bool first = true;
};