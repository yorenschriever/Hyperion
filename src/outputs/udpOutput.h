#pragma once

#include "Output.h"

#include "hardware/UDPFast/UDPFast.h"
#include "debug.h"

//UDPoutput writes leddata to another device over UDP.
class UDPOutput : public Output
{
public:
    UDPOutput(const char* hostname, int port, unsigned int fps)
    {
        this->hostname = hostname;
        this->port = port;
        this->frameInterval = 1000/fps;

        this->length=12;
        buffer = (uint8_t*) malloc(12);
    }

    //index and size are in bytes
    void SetData(uint8_t *data, int size, int index)
    {
        int copylength = min(size, length - index);
        if (copylength>0)
            memcpy(this->buffer + index, data, copylength);
    }

    boolean Ready()
    {
        return (millis() - lastFrame >= frameInterval);
    }

    void Show()
    {
        lastFrame = millis();
        udpFast.sendPacketFast(hostname, port, buffer, length);
    }

    void ShowGroup()
    {

    }

    void Begin() override
    {
        lastFrame = millis();
    }

    void Clear()
    {
        memset(this->buffer,0,this->length);
    }

    //length is in bytes
    void SetLength(int len) override
    {
        if (len != this->length){
            //wait for the frontbuffer to be sent before we are going to change its size
            while(!Ready())
                delay(1);

            buffer = (uint8_t*) realloc(buffer,len);

            this->length = len;
        }
    }

private:
    const char* hostname;
    int port;
    int frameInterval;
    unsigned long lastFrame=0;

    int length;
    uint8_t* buffer;

    static UDPFast udpFast;
};

UDPFast UDPOutput::udpFast;