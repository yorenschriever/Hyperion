#pragma once

#include <Arduino.h>

class Output {

  public:

    virtual void SetLength(int len) =0; 

    //depricated
    virtual void SetPixelColor(int index, uint8_t r,uint8_t g,uint8_t b)= 0; 

    virtual void SetData(uint8_t* data, int size, int index);
    virtual void Show() = 0; 
    virtual void Begin() = 0;
    virtual void Clear() = 0;
    virtual boolean Ready() = 0;

    //depricated
    void setGammaCurve(uint16_t* curve){
      this->gammaCurve = curve;
    }
    //depricated, there is no concept of length anymore in outputs. maybe introduce something like this in pipe?
    int getLength(){ return length; }
    
  protected:
    
    //TODO both are not generic
    int length=0;
    int pin=0;

    //depricated
    uint16_t* gammaCurve;
};
