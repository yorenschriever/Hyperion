#pragma once

#include <Arduino.h>
#include "../../.pio/libdeps/esp32-poe/NeoPixelBus_ID547/src/NeoPixelBus.h"
#include "output.h"

template <class T>
class NeoPixelBusOutput : public Output
{
public:

    //TODO use 1-8 corresponding to the output numbering on the front instead of the raw pin numbers
    //ideally also remove the templace and use the RMT based on the output number
    NeoPixelBusOutput(int pin)
    {
        pinMode(pin, OUTPUT);
        this->pin = pin;
        this->length = 4;
    }

    void SetData(uint8_t* data, int size, int index)
    {
        if (this->pixels == NULL)
            return;

        //for now i ignore datatypes that do not have a with of 3 bytes, because 
        //i dont feel like remapping everything to 3 bytes to call pixels->setPixelcolor.
        //hopefully soon i will switch to my own implementation of the RMT drivers, so
        //i can feed any string of byte to it, regardless is we are dealing with an
        //RGB, RGBW strip or whatever
        if (size != 3)
            return;

        this->pixels->SetPixelColor(index/3,RgbColor(data[0],data[1],data[2]));
    }

    boolean Ready()
    {
        return this->pixels->CanShow();
    }

    void Show()
    {
        this->pixels->Show();
    }

    void Begin() override
    {
        if (this->pixels != NULL)
        {
            //clear the old pixels
            this->Clear();
            this->Show();

            //delete the old object
            delete this->pixels;
        }
        this->pixels = new NeoPixelBus<NeoGrbFeature, T>(this->length, this->pin);
        this->pixels->Begin();
    }

    void Clear() override
    {
        this->pixels->ClearTo(RgbColor(0, 0, 0));
    }

    void SetLength(int len) override
    {
        if (len/3 != this->length)
        {
            this->length = len/3; //SetLength works in bytes, but this class thinks in number of leds.
            this->Begin();
        }
    }

private:
    NeoPixelBus<NeoGrbFeature, T> *pixels = NULL;
    int length=0;
    int pin=0;
};