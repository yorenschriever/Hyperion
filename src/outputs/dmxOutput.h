#pragma once

#include "Output.h"
#include <Arduino.h>
#include <lib/dmx/dmx.h>

const int universeSize = 512;
const bool always512 = true; //always send the entire universe, instead of only the channels that were provided from the input

class DMXOutput : public Output {
  public:
  
    DMXOutput() {

    }
    
    void SetPixelColor(int index, uint8_t r,uint8_t g,uint8_t b) override 
    { 
       if (index<universeSize)
          this->buffer[index] = (r+g+b)/3;
    }

    boolean Ready()
    {
      return !this->busy;
    }
    
    void Show() 
    { 
        this->busy=true;
        xSemaphoreGive(dirtySemaphore);
    }
    
    void Begin() override 
    {
      dirtySemaphore = xSemaphoreCreateBinary();
      xTaskCreatePinnedToCore(SendDMXAsync,"SendDMXAsync",10000,this,1,NULL,1);
    }
    
    void Clear() 
    {
      for (int i=0; i<universeSize; i++)
        this->buffer[i]=0;
    }

    void SetLength(int len) override {
        this->bufferLen = min(len,universeSize);
    }

  private: 
    uint8_t buffer[universeSize+1];
    int bufferLen=0;
    xSemaphoreHandle dirtySemaphore;
    volatile boolean busy=false;

    static void SendDMXAsync(void* param)
    {
        DMXOutput* this2 = (DMXOutput*) param;
        uint8_t frontBuffer[universeSize+1];

        DMX::Initialize();

        while(true)
        {
            if (!xSemaphoreTake(this2->dirtySemaphore,0))
              continue;

            int frontBufferLen =  always512 ? universeSize : this2->bufferLen;
            memcpy(frontBuffer, this2->buffer, frontBufferLen);
            DMX::Write(frontBuffer, frontBufferLen, true);
            
            this2->busy=false;
        }

        
    }
};