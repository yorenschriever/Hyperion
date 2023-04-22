#pragma once

#include <vector>
#include "input.h"
#include "../patterns/pattern.h"
#include "../hardware/rotary/rotary.h"

// RotaryPatternInput lets you select a pattern with the rotary control.
template <class T_COLOUR>
class RotaryPatternInput : public Input
{

public:
    // length = the length in pixels
    // patterns = the patterns to display
    RotaryPatternInput(int length, const std::vector<LayeredPattern<T_COLOUR> *> patterns)
    {
        this->length = length;
        this->patterns = patterns;
        this->leddata = (T_COLOUR *)malloc(length * sizeof(T_COLOUR));

        if (!this->leddata)
        {
            Debug.printf("Unable to allocate memory for rotaryPatternInput, free heap = %d\n", ESP.getFreeHeap());
            ESP.restart();
        }
    }

    virtual void begin()
    {
        for (int i = 0; i < length; i++)
            leddata[i] = T_COLOUR();

        for (auto pattern : patterns)
            pattern->Initialize();
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        if (patterns.size() == 0)
            return 0;

        selectedPattern = (selectedPattern + Rotary::getRotation() + patterns.size()) % patterns.size();

        // reset everything to 0
        for (int i = 0; i < length; i++)
            leddata[i] = T_COLOUR();

        patterns[selectedPattern]->Calculate(leddata, length, true);

        memcpy(dataPtr, leddata, length * sizeof(T_COLOUR));

        usedframecount++;

        return length * sizeof(T_COLOUR);
    }

private:
    int length = 0;
    T_COLOUR *leddata;
    std::vector<LayeredPattern<T_COLOUR> *> patterns;
    int selectedPattern = 0;
};