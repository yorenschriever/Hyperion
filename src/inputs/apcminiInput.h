#pragma once

#include "input.h"
#include "../lib/apcmini/apcmini.h"
#include "../patterns/pattern.h"

//this class attaches a provided set of 8 patterns to a column on the APCmini.
//you can provide the column and the patterns when initializing this class.
//Patterns should all be the same 'Colour'. You have to provide this colour as templace here
template <class T_COLOUR>
class ApcminiInput : public Input{

    public:

        //length = the length in pixels
        //column = the colunm on the apc to listen to
        //patterns = a list of 8 patterns to attach to each button
        ApcminiInput(int length, int column, Pattern<T_COLOUR> patterns[8]){
            this->length = length;
            this->leddata = (T_COLOUR*) malloc(length * sizeof(T_COLOUR));
            this->lastLeddata = (T_COLOUR*) malloc(length * sizeof(T_COLOUR));
            this->patterns = patterns;
            this->column = column;
        }

        virtual void begin(){
            APCMini::Initialize();

            for (int i=0;i<length;i++)
                    lastLeddata[i] = T_COLOUR();
        }

        virtual int loadData(uint8_t* dataPtr)
        {
            Pattern<T_COLOUR> patternfunc=NULL;
            for (int i=0;i<8; i++){ //TODO use const from APCMini class instead of hardcoded 8
                if (APCMini::getStatus(column,i))
                    patternfunc = patterns[i];
            }

            if (!patternfunc){                
                //stop sending new empty frames if we already have sent an empty frame because no patterns were selected, 
                //tell the pipe that we dont have anything to process.
                if (alloff) 
                    return 0;
                
                //load empty colour if no pattern is selected. this will set everything to black after you unselected
                //the last pattern
                for (int i=0;i<length;i++)
                    leddata[i] = T_COLOUR();

                alloff = true;
            } else {
                alloff=false;
                for (int i=0; i<length; i++){
                    //todo leddata is already partly updated when we pass it here. use 2 buffers
                    leddata[i] = patternfunc(i,length,leddata);

                    //combine and scale back the 2 faders to a range of 0-255 and feed to the dimmer function
                    leddata[i].dim(APCMini::getFader(column) * APCMini::getFader(8) >> 6); //TODO magic const
                }

                //i only copy to lastLedData if a pattern was on, not if not patternfunc was selected
                //this might be useful when writing patches, so they continue where they were
                //not sure though, we will have to see
                memcpy(lastLeddata,leddata, length * sizeof(T_COLOUR));
            }
            
            memcpy(dataPtr,leddata, length * sizeof(T_COLOUR));
            
            usedframecount++;

            return length * sizeof(T_COLOUR);
        }

    private:
        int length=0;
        T_COLOUR* leddata;
        T_COLOUR* lastLeddata;
        Pattern<T_COLOUR>* patterns;
        int column;
        boolean alloff=true;

};