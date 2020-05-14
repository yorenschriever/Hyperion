#pragma once

#include "input.h"
#include "lib/UDPFast/UDPFast.h"

//UdpInput opens an UDP port, and listens for incoming packages. Each package
//will correspond to 1 new frame. I enabled IP reassembly in sdkconfig.defaults,
//and that seems to work for 2 packets, but not more than that. For this reason
//UDP input is limited to 975  RGB leds/port. More than enough for now
class UDPInput : public Input
{

public:
    UDPInput(int port)
    {
        this->port = port;
    }

    virtual void begin()
    {
        udpFast.begin(this->port);
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        int cb, cbr;
        bool gotFrame = false;
        //read all packets, throw them away, but keep the last one.
        //there must a more elegant way to empty the queue, but i didnt find any
        //without having to recompile the lwip driver
        while ((cbr = udpFast.parsePacketFast(dataPtr)))
        {
            gotFrame = true;
            cb = cbr;
            missedframecount++;
        }
        if (!gotFrame)
            return 0;

        usedframecount++;
        missedframecount--;
        return cb;
    }

private:
    int port;
    UDPFast udpFast;
};