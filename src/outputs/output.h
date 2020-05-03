#pragma once

#include <Arduino.h>

class Output {

  public:

    virtual void SetLength(int len) =0; 
    virtual void SetData(uint8_t* data, int size, int index);
    virtual void Show() = 0; 
    virtual void ShowGroup() {}; 
    virtual void Begin() = 0;
    virtual void Clear() = 0; //TODO clear should be part of the pipe, because it depends on the colour. setting all bytes to 0 doesnt necessarily mean turning everything off
    virtual boolean Ready() = 0;

};
