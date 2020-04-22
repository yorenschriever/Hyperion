#pragma once

#include <Arduino.h>
//#include <NeoPixelBus.h>
#include "../../.pio/libdeps/esp32-poe/NeoPixelBus_ID547/src/NeoPixelBus.h"
#include "output.h"

template <class T>
class NeoPixelBusOutput : public Output {
  public:
    NeoPixelBusOutput(int pin) {
        pinMode(pin,OUTPUT);
        this->pin=pin;
        this->length=4;
    }
    void SetPixelColor(int index, uint8_t r,uint8_t g,uint8_t b) override { 
      if (this->pixels != NULL)
        this->pixels->SetPixelColor(
          index, 
          RgbColor(
            this->gammaCurve[r],
            this->gammaCurve[g],
            this->gammaCurve[b]
          )
        ); 
    }
    boolean Ready(){
      return this->pixels->CanShow();
    }
    void Show() { 
      this->pixels->Show(); 
    }
    void Begin() override {
      if (this->pixels != NULL){
        //clear the old pixels
        this->Clear();
        this->Show();

        //delete the old object
        delete this->pixels;
      }
      this->pixels = new NeoPixelBus<NeoGrbFeature, T>(this->length, this->pin);
      this->pixels->Begin();
    }
    
    void Clear() override {
      this->pixels->ClearTo(RgbColor(0,0,0));
    }

    void SetLength(int len) override {
      if (len != this->length){
        this->length=len;
        this->Begin();
      }
    }
  private: 
    NeoPixelBus<NeoGrbFeature, T>* pixels=NULL;
};