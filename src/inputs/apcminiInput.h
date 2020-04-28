#pragma once

#include "input.h"
#include "../lib/apcmini/apcmini.h"
#include "../patterns/patterns.h"

class ApcminiInput : public Input{

    public:

        ApcminiInput(int length, int column, Pattern patterns[8]){
            this->length = length;
            this->leddata = (uint8_t*) malloc(length);
            this->patterns = patterns;
            this->column = column;
        }

        virtual void begin(){
            APCMini::Initialize();
        }

        virtual int loadData(uint8_t* dataPtr)
        {
            Pattern patternfunc=NULL;
            for (int i=0;i<8; i++){
                if (APCMini::getStatus(column,i))
                    patternfunc = patterns[i];
            }

            if (!patternfunc)
                for (int i=0;i<length;i++)
                    leddata[i] = 0;
            else
                for (int i=0; i<length; i+=3){
                    //todo colours
                    leddata[i] = (unsigned int) patternfunc(i,length,leddata) * APCMini::getFader(column) * APCMini::getFader(8) >> 7 >> 7;
                    leddata[i+1]= leddata[i] ;
                    leddata[i+2]= leddata[i] ;
                }
            
            memcpy(dataPtr,leddata, length);

            usedframecount++;

            return length;
        }

    private:
        int length=0;
        uint8_t* leddata; // leddata[90];
        Pattern* patterns;
        int column;

};