#pragma once

#include "input.h"
#include "../hardware/apcmini/apcmini.h"
#include "../patterns/pattern.h"

//this class attaches a provided set of 8 patterns to a column on the APCmini.
//you can provide the column and the patterns when initializing this class.
//Patterns should all be the same 'Colour'. You have to provide this colour as template here
template <class T_COLOUR>
class LayeredApcminiInput : public Input
{

public:
    //length = the length in pixels
    //column = the colunm on the apcmini to listen to
    //patterns = a list of 8 patterns to attach to each button
    LayeredApcminiInput(int length, int column, LayeredPattern<T_COLOUR> *patterns[8])
    {
        this->length = length;
        this->patterns = patterns;
        this->column = column;
        this->leddata = (T_COLOUR *)malloc(length * sizeof(T_COLOUR));
    }

    virtual void begin()
    {
        APCMini::Initialize();

        for (int i = 0; i < length; i++)
            this->leddata[i] = T_COLOUR();

        for (int i=0;i<8;i++)
            patterns[i]->Initialize();
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        for (int i = 0; i < length; i++)
            leddata[i] = T_COLOUR();

        for (int i = 0; i < 8; i++)
            patterns[i]->Calculate(leddata, length, APCMini::getStatus(column, i));

        memcpy(dataPtr, leddata, length * sizeof(T_COLOUR));

        //apply dimming.
        for (int i = 0; i < length; i++)
            ((T_COLOUR *)dataPtr)[i].dim(APCMini::getFader(column) * APCMini::getFader(8) >> 6); //TODO magic const

        usedframecount++;

        return length * sizeof(T_COLOUR);
    }

private:
    int length = 0;
    T_COLOUR *leddata;
    LayeredPattern<T_COLOUR> **patterns;
    int column;
    boolean alloff = true;
    int lastSelectedPattern = 1;
};