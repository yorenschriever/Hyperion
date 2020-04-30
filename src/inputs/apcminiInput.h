#pragma once

#include "input.h"
#include "../lib/apcmini/apcmini.h"
#include "../patterns/patterns.h"

template <class T>
class ApcminiInput : public Input{

    public:

        ApcminiInput(int length, int column, Pattern<T> patterns[8]){
            this->length = length;
            this->leddata = (T*) malloc(length * sizeof(T));
            this->patterns = patterns;
            this->column = column;
        }

        virtual void begin(){
            APCMini::Initialize();
        }

        virtual int loadData(uint8_t* dataPtr)
        {
            Pattern<T> patternfunc=NULL;
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
                    leddata[i] = T();

                alloff = true;
            } else {
                alloff=false;
                for (int i=0; i<length; i++){
                    //todo leddata is already partly updated when we pass it here. use 2 buffers
                    leddata[i] = patternfunc(i,length,leddata);

                    //combine and scale back the 2 faders to a range of 0-255 and feed to the dimmer function
                    leddata[i].dim(APCMini::getFader(column) * APCMini::getFader(8) >> 6); //TODO magic const
                }
            }
            
            memcpy(dataPtr,leddata, length * sizeof(T));

            usedframecount++;

            return length * sizeof(T);
        }

    private:
        int length=0;
        T* leddata;
        Pattern<T>* patterns;
        int column;
        boolean alloff=true;

};