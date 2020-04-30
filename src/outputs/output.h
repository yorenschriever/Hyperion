#pragma once

#include <Arduino.h>

class Output {

  public:

    virtual void SetLength(int len) =0; 
    virtual void SetData(uint8_t* data, int size, int index);
    virtual void Show() = 0; 
    virtual void Begin() = 0;
    virtual void Clear() = 0;
    virtual boolean Ready() = 0;

};
