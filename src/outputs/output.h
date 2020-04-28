#pragma once

#include <Arduino.h>

class Output {

  public:

    virtual void SetLength(int len) =0; 
    virtual void SetPixelColor(int index, uint8_t r,uint8_t g,uint8_t b)= 0; 
    virtual void Show() = 0; 
    virtual void Begin() = 0;
    virtual void Clear() = 0;
    virtual boolean Ready() = 0;
    void setGammaCurve(uint16_t* curve){
      this->gammaCurve = curve;
    }
    int getLength(){ return length; }
    
  protected:
    
    int length=0;
    int pin=0;
    uint16_t* gammaCurve;
};
