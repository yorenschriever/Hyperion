#pragma once

#include "Output.h"
#include <Arduino.h>
#include "../lib/midi/midi.h"

#define NUMBUTTONS 64

//this is merely a peculiarity. it outputs the patterns on the leds of the apc mini.
//it was used to test the midi interface
//it's not really stable. for that to happen you should create an async thread to send the midi data, just like in dmxoutput


class APCMiniOutput : public Output {
  public:
  
    APCMiniOutput() {

    }
    
    void SetPixelColor(int index, uint8_t r,uint8_t g,uint8_t b) override 
    { 
       if (index<NUMBUTTONS)
          this->buffer[index] = (r+g+b)/3;
    }

    boolean Ready()
    {
      return !this->busy;
    }
    
    void Show() 
    { 
        for (int i=0; i<NUMBUTTONS; i++){
            if (this->buffer[i] != this->lastbuffer[i])
                Midi::sendNoteOn(0,i,this->buffer[i] > 127 ? 100 : 0);
        }
        memcpy(lastbuffer,buffer,NUMBUTTONS);
        Midi::waitTxDone();
        delay(2);
    }
    
    void Begin() override 
    {

    }
    
    void Clear() 
    {
      for (int i=0; i<NUMBUTTONS; i++)
        this->buffer[i]=0;
    }

    void SetLength(int len) override {
        //nothing
    }

  private: 
    uint8_t buffer[NUMBUTTONS];
    uint8_t lastbuffer[NUMBUTTONS];

    volatile boolean dirty=false;
    volatile boolean busy=false;

};