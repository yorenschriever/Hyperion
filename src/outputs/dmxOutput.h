#pragma once

#include "Output.h"
#include <Arduino.h>
#include <lib/dmx/dmx.h>
#include "debug.h"

class DMXOutput : public Output
{
public:
    //startchannel 1-512
    DMXOutput(int startChannel)
    {
        if (startChannel < 1)
            startChannel = 1;
        if (startChannel > 512)
            startChannel = 512;
        this->startChannel = startChannel;
    }

    //index and size are in bytes
    void SetData(uint8_t *data, int size, int index)
    {
        DMX::Write(data, size, index + startChannel-1);
    }

    boolean Ready()
    {
        return !DMX::TxBusy();
    }

    void Show()
    {
        DMX::Show();
    }

    void Begin() override
    {
        DMX::Initialize();
    }

    void Clear()
    {
        DMX::ClearTxBuffer();
    }

    //length is in bytes
    void SetLength(int len) override
    {
        //do nothing. DMX automatically detects the length based on the last byte written
    }

private:
    int startChannel;
};
