#pragma once

#include "input.h"
#include "../hardware/apcmini/apcmini.h"
#include "../patterns/pattern.h"

//this class attaches a provided set of 8 patterns to a column on the APCmini.
//you can provide the column and the patterns when initializing this class.
//Patterns should all be the same 'Colour'. You have to provide this colour as template here
template <class T_COLOUR>
class ApcminiInput : public Input
{

public:
    //length = the length in pixels
    //column = the colunm on the apcmini to listen to
    //patterns = a list of 8 patterns to attach to each button
    ApcminiInput(int length, int column, Pattern<T_COLOUR> *patterns[8])
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
        int selectedPattern = -1;
        for (int i = 0; i < 8; i++)
        { //TODO use const from APCMini class instead of hardcoded 8
            if (APCMini::getStatus(column, i))
                selectedPattern = i;
        }

        if (selectedPattern == -1)
        {
            //stop sending new empty frames if we already have sent an empty frame because no patterns were selected,
            //tell the pipe that we dont have anything to process.
            if (alloff)
                return 0;

            //load empty colour if no pattern is selected. this will set everything to black after you unselected
            //the last pattern
            for (int i = 0; i < length; i++)
                leddata[i] = T_COLOUR();

            alloff = true;
        }
        else
        {
            alloff = false;
            patterns[selectedPattern]->Calculate(leddata, length, selectedPattern == lastSelectedPattern);
        }

        lastSelectedPattern = selectedPattern;

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
    Pattern<T_COLOUR> **patterns;
    int column;
    boolean alloff = true;
    int lastSelectedPattern = 1;
};