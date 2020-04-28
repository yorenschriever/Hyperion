#pragma once

#include "input.h"
#include "../lib/midi/midi.h"

class ApcminiInput : public Input{

    public:
        virtual void begin(){}

        virtual int loadData(uint8_t* dataPtr)
        {
            for (int i=0;i<90;i++){
                leddata[i] = millis()/50 + i*2;
            }
            memcpy(dataPtr,leddata, 90);

            usedframecount++;

            return 90;
        }

    private:
        uint8_t leddata[90];

};