#pragma once

#include "input.h"
#include "../hardware/midi/midi.h"
#include "../patterns/pattern.h"

// this class attaches a provided set of 24 patterns to a korg Nanocontrol.
// Patterns should all be the same 'Colour'. You have to provide this colour as template here
template <class T_COLOUR>
class NanoKontrolInput : public Input
{

public:
    // length = the length in pixels
    // patterns = a list of 24 patterns to attach to each button
    NanoKontrolInput(int length, LayeredPattern<T_COLOUR> *patterns[24])
    {
        this->length = length;
        this->patterns = patterns;
        this->leddata = (T_COLOUR *)malloc(length * sizeof(T_COLOUR));
    }

    virtual void begin()
    {
        Midi::Initialize();

        for (int i = 0; i < length; i++)
            this->leddata[i] = T_COLOUR();

        for (int i = 0; i < 24; i++)
            patterns[i]->Initialize();
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        for (int i = 0; i < length; i++)
            ((T_COLOUR *)dataPtr)[i] = T_COLOUR();

        for (int pattern = 0; pattern < 24; pattern++)
        {
            for (int i = 0; i < length; i++)
                leddata[i] = T_COLOUR();

            patterns[pattern]->Calculate(leddata, length, Midi::controllerValue(buttonMapping[pattern]) > 0);

            // apply dimming and copy to leddata buffer
            for (int i = 0; i < length; i++)
            {
                leddata[i].dim((Midi::controllerValue(firstFader + pattern / 3) * Midi::controllerValue(masterDimController)) >> 6);
                ((T_COLOUR *)dataPtr)[i] += leddata[i];
            }
        } 

        usedframecount++;
        return length * sizeof(T_COLOUR);
    }

private:
    int length = 0;
    T_COLOUR *leddata;
    LayeredPattern<T_COLOUR> **patterns;

    const int buttonMapping[24] = {32,48,64, 33,49,65, 34,50,66, 35,51,67, 36,52,68, 37,53,69, 38,54,70, 39,55,71};
    const int firstFader = 0;
    const int masterDimController = 23;
};