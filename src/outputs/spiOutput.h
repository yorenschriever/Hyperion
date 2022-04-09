#pragma once

#include "Output.h"
#include <Arduino.h>
#include "hardware/spi/spi.h"
#include "debug.h"

//SpiOutput sends the output over spi.
//Can be used for pixel led strands.
class SpiOutput : public Output
{
public:
    SpiOutput(uint8_t clkPin, uint8_t dataPin, int frq = 1000000)
    {
        this->clkPin = clkPin;
        this->dataPin = dataPin;
        this->frq = frq;

        this->length = 12;
        buffer = (uint8_t *)heap_caps_malloc(12, MALLOC_CAP_DMA | MALLOC_CAP_32BIT);
    }

    //index and size are in bytes
    void SetData(uint8_t *data, int size, int index)
    {
        int copylength = min(size, length - index);
        if (copylength > 0)
            memcpy(this->buffer + index, data, copylength);
    }

    boolean Ready()
    {
        return SPI::IsReadyForUs(500);
    }

    void Show()
    {
        SPI::Send(buffer, length);
    }

    void ShowGroup() {}

    void Begin() override
    {
        SPI::Initialize(pins[dataPin], pins[clkPin], frq);
    }

    void Clear()
    {
        memset(buffer, 0, length);
    }

    //length is in bytes
    void SetLength(int len) override
    {
        if (len != this->length)
        {
            //wait for the buffer to be sent before we are going to change its size
            while (!Ready())
                delay(1);

            buffer = (uint8_t *)heap_caps_realloc(buffer, len , MALLOC_CAP_DMA | MALLOC_CAP_32BIT);

            this->length = len;
        }
    }

private:
    uint8_t clkPin;
    uint8_t dataPin;
    int frq;

    int length;
    uint8_t *buffer;
};
