#pragma once

#include "Output.h"
#include <Arduino.h>
#include <hardware/dmx/dmx.h>
#include "debug.h"

//DMXoutput writes leddata to the dmx output. You can use multiple dmxoutputs
//with different startchannels. Their data will be combined into a single dmx frame 
//before it is sent out. 
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
        DMX::Write(data, size, index + startChannel - 1);
    }

    boolean Ready()
    {
        return !DMX::TxBusy();
    }

    void Show()
    {
        //set this static variable to indicate that any dmx instace has updated the data
        groupDirty = true;
    }

    void ShowGroup()
    {
        //read the static varaible to see if any instance has updated the data
        if (!groupDirty)
            return;

        DMX::Show();
        groupDirty = false;
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

    static bool groupDirty;
};

bool DMXOutput::groupDirty = false;