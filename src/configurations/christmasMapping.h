#pragma once

#include "pipe.h"
#include "colours.h"

#include "outputs/pwmOutput.h"
#include "outputs/dmxOutput.h"
#include "outputs/apcminiOutput.h"
#include "outputs/neopixelOutput.h"
#include "inputs/udpInput.h"
#include "inputs/apcminiInput.h"
#include "inputs/dmxInput.h"
#include "inputs/fallbackInput.h"
#include "inputs/patternInput.h"
#include "inputs/bufferInput.h"
#include "inputs/websocketInput.h"
#include "patterns/rgbPatterns.h"
#include "patterns/monochromePatterns.h"
#include "patterns/ledstripPatterns.h"
#include "luts/colourCorrectionLut.h"
#include "luts/incandescentLut.h"
#include "configurationStruct.h"
#include "inputs/layeredPatternInput.h"

//I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
//http://fastled.io/docs/3.1/group___color_enums.html
//note the different order, fastled uses RGB, luts are in ouput order (GRB)
LUT *NeopixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 255);


//This config can receive mapping info over websocket, as well as display 
//udp input. It is not very stable.
//In the future it might be worth considering only receiving udp input,
//and make a separate device to transform ble/websocket call into udp.

const int channelCount = 8;

//number of leds on each output
const int sizes[channelCount] = {200,200,200,200,200,150,150,200};

//websocket created below will put mapping data into this buffer
//this buffer is then displayed on the leds because of the fallbackinput and
//bufferinputs
//i cannot connect 8 websockets at the same time, so i use one socket, and one 
//buffer. I then assign different parts of the buffer to the individual bufferInputs.
uint8_t* mappingBuffer;
int mappingBufferSize = 0;
BufferInput* bufferInputs[channelCount];
bool socketInitialized = false;

void initBufferInputs()
{
    mappingBufferSize = 0;
    for (int i = 0; i < channelCount; i++)
        mappingBufferSize += sizes[i] * 3;

    mappingBuffer = (uint8_t*) malloc(mappingBufferSize * 3);

    int bufferStart = 0;
    for (int i = 0; i < channelCount; i++)
    {
        bufferInputs[i] = new BufferInput(mappingBuffer + bufferStart, sizes[i] * 3);
        bufferStart += sizes[i] * 3;
    }
}

void initWebsocket()
{
    WebsocketServer* sock = new WebsocketServer("/map");
    sock->onMessage([](RemoteWebsocketClient *client, WebsocketServer* server, std::string msg) {

        for (int i=0; i < msg.length() && i < mappingBufferSize/3; i++)
        {
            mappingBuffer[i*3+0] = msg[i];
            mappingBuffer[i*3+1] = msg[i];
            mappingBuffer[i*3+2] = msg[i];
        }

        for (int i = 0; i < channelCount; i++)
        {
            bufferInputs[i]->setFrameReady();
        }

        server->sendAll("OK");
    });
}

void updateParams()
{
    if (!socketInitialized)
    {
        initWebsocket();
        socketInitialized = true;
    }
    WebsocketContainer::Run();
}

void LoadConfiguration()
{
    Params::primaryColour = RGBA(255, 100, 0, 255);
    Params::secondaryColour = RGBA(100, 100, 75, 255);
    Params::velocity = 0.3;

    Configuration.hostname = "hyperion";

    Configuration.paramCalculator = updateParams;

    initBufferInputs();

    for (int i = 0; i < channelCount; i++)
    {
        Configuration.pipes.push_back(
            new Pipe(
                new FallbackInput(
                    bufferInputs[i],
                    new FallbackInput(
                        new UDPInput(9611 + i),
                        new PatternInput<RGB>(sizes[i], new GlowPulsePattern())),
                    5000),
                new NeopixelOutput<Kpbs800>(1 + i),
                Pipe::transfer<RGB, GRB>,
                NeopixelLut));
    }

}
