#pragma once

#include "input.h"
#include "../patterns/pattern.h"
#include <vector>
#include "Arduino.h"

//PatternCycleInput shows each pattern for a while.
template <class T_COLOUR>
class PatternCycleInput : public Input
{

public:
    //length = the length in pixels
    //patterns = a list of 8 patterns to attach to each button
    //duration = the time each pattern is displayed
    PatternCycleInput(int length, const std::vector<LayeredPattern<T_COLOUR>*> patterns, int duration)
    {
        this->length = length;
        this->patterns = patterns;
        this->leddata = (T_COLOUR *)malloc(length * sizeof(T_COLOUR));
        this->duration = duration;

        if (!this->leddata){
            Debug.printf("Unable to allocate memory for patternCycleInput, free heap = %d\n",ESP.getFreeHeap());
            ESP.restart();
        }
    }

    virtual void begin()
    {
        for (int i = 0; i < length; i++)
            leddata[i] = T_COLOUR();

        for (auto pattern:patterns)
            pattern->Initialize();

        start = millis();
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        for (int i = 0; i < length; i++)
            leddata[i] = T_COLOUR();

        for (int i=0; i < patterns.size(); i++) {
            bool active = ((millis() - start) / duration) % patterns.size() == i;
            patterns[i]->Calculate(leddata, length, active);
        }

        memcpy(dataPtr, leddata, length * sizeof(T_COLOUR));

        usedframecount++;

        return length * sizeof(T_COLOUR);
    }

private:
    int length = 0;
    T_COLOUR *leddata;
    std::vector<LayeredPattern<T_COLOUR>*> patterns;
    unsigned long start;
    unsigned int duration;
};