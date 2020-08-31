#pragma once

#include "Output.h"
#include <Arduino.h>
#include <hardware/dmx/dmx.h>
#include "debug.h"

//nulloutput does not use the output. 
//you can use this for performance testing 
class NullOutput : public Output
{
public:
    NullOutput()
    {}

    //index and size are in bytes
    void SetData(uint8_t *data, int size, int index){}

    boolean Ready()
    {
        return true;
    }

    void Show(){}

    void ShowGroup(){}

    void Begin() override{}

    void Clear(){}

    //length is in bytes
    void SetLength(int len) override{}

private:

};