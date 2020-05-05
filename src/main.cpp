//todo find out why the build option -O2 in platform.ini doest work. 
//this optimization increases fps from 35 to 41
#pragma GCC optimize ("-O3")
#pragma GCC push_options

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "sdkconfig.h"
#include <Arduino.h>

#include "ota.h"
#include "network.h"
#include "debug.h"
#include "pipe.h"
#include "colours.h"
#include "lut.h"

#include "outputs/pwmOutput.h"
#include "outputs/dmxOutput.h"
#include "outputs/apcminiOutput.h"
#include "outputs/neopixelOutput.h"
#include "inputs/udpInput.h"
#include "inputs/apcminiInput.h"
#include "inputs/dmxInput.h"
#include "lib/display/display.h"
#include "lib/rotary/rotary.h"
#include "lib/apcmini/apcmini.h"
#include "patterns/monochromePatterns.h"
#include "patterns/rgbPatterns.h"

void DisplayFps(void *parameter);
void clearall();
void animate(byte r, byte g, byte b);

LUT* NeopixelLut = new ColourCorrectionLUT(2.0,255, 255,255,255);
LUT* IncandescentLut = new IncandescentLUT(2.5, 4096, 400);
LUT* RotaryLut = new ColourCorrectionLUT(2.0, 4096, 150,255,255);

Pipe pipes[] = {
    // Pipe(
    //     new DMXInput(1),
    //     new DMXOutput(1)
    // ),

    Pipe(
        //create an apcmini input that creates monochome patterns
        new ApcminiInput<Monochrome>(
            10, //width of the pattern
            0,  //button column on the apc to listen to (0-7)
            //the patterns to attach to the buttons
            new Pattern<Monochrome>[8]{sinPattern, sawPattern, randomPattern, randomPattern2, meteorPattern, randomFadePattern, slowStrobePattern, fastStrobePattern}),
        new DMXOutput(1)),

    // Pipe(
    //     //create an apcmini input that creates monochome patterns
    //     new ApcminiInput<Monochrome>(
    //         10, //width of the pattern
    //         1,  //button column on the apc to listen to (0-7)
    //         //the patterns to attach to the buttons
    //         new Pattern<Monochrome>[8]{sinPattern, sawPattern, randomPattern, randomPattern2, meteorPattern, randomFadePattern, slowStrobePattern, fastStrobePattern}),
    //     new DMXOutput(15)),

    // Pipe(
    //     new UDPInput(9611),
    //     new NeopixelOutput<Kpbs800>(1),
    //     Pipe::transfer<RGB,GRB>,
    //     NeopixelLut),

    // Pipe(
    //     new UDPInput(9612),
    //     new NeopixelOutput<Kpbs800>(2),
    //     Pipe::transfer<RGB,GRB>,
    //     NeopixelLut),

    // Pipe(
    //     new UDPInput(9613),
    //     new NeopixelOutput<Kpbs800>(3),
    //     Pipe::transfer<RGB,GRB>,
    //     NeopixelLut),

    // Pipe(
    //     new UDPInput(9614),
    //     new NeopixelOutput<Kpbs800>(4),
    //     Pipe::transfer<RGB,GRB>,
    //     NeopixelLut),

    // Pipe(
    //     new UDPInput(9615),
    //     new NeopixelOutput<Kpbs800>(5),
    //     Pipe::transfer<RGB,GRB>,
    //     NeopixelLut),

    // // //Shared with DMX!
    // // Pipe(
    // //     new UDPInput(9616),
    // //     new NeopixelOutput<Kpbs800>(6),
    // //     //new DMXOutput(),
    // //     Pipe::transfer<RGB,GRB>,
    // //     NeopixelLut),

    // Pipe(
    //     new UDPInput(9617),
    //     new NeopixelOutput<Kpbs800>(7),
    //     Pipe::transfer<RGB,GRB>,
    //     NeopixelLut),

    // Pipe(
    //     new UDPInput(9618),
    //     new NeopixelOutput<Kpbs800>(8),
    //     Pipe::transfer<RGB,GRB>,
    //     NeopixelLut),

    Pipe(
        new UDPInput(9619),
        new PWMOutput(1500),
        Pipe::transfer<RGB,Monochrome12>,
        IncandescentLut),


    // Pipe(
    //     new ApcminiInput<RGB>(
    //         16, //width of the pattern, in pixels
    //         2,  //button column on the apc to listen to (0-7)
    //         //the patterns to attach to the buttons
    //         new Pattern<RGB>[8]{rainbowPattern, rainbowPattern, rainbowPattern, rainbowPattern, rainbowPattern, rainbowPattern, rainbowPattern, rainbowPattern}),
    //     new NeopixelOutput<NeoEsp32RmtSpeed800Kbps>(1),
    //     Pipe::transfer<RGB,RGB>,
    //     RGBGamma8),

};

void click() { 
    Debug.println("click"); 
    //APCMini::handleNoteOn(0,56,127);
}
void press() { Debug.println("press"); }
void release() { Debug.println("release"); }
void longpress() { Debug.println("longpress"); }
void rotate(int amount) { Debug.printf("rotate: %d\n", amount); }

void setup()
{
    Debug.begin(115200);

    Rotary::Initialize();
    Rotary::setLut(RotaryLut);
    Rotary::setRGB(255,255,255);
    Rotary::onClick(click);
    Rotary::onPress(press);
    Rotary::onRelease(release);
    Rotary::onLongPress(longpress);
    Rotary::onRotate(rotate);

    Display::Initialize(); //initialize display before outputs

    DMX::SendFullFrame(false); //speed up dmx transmission by only sending the bytes that have a value
    DMX::SetUniverseSize(37,7); //handle some quirks of the the midi device i use for testing

    Debug.println("Starting outputs");
    for (int j = 0; j < sizeof(pipes) / sizeof(Pipe); j++)
        pipes[j].out->Begin();

    Debug.println("Starting network");
    clearall();
    NetworkBegin();
    clearall();

    Debug.println("Starting inputs");
    for (int j = 0; j < sizeof(pipes) / sizeof(Pipe); j++)
        pipes[j].in->begin();

    Debug.println("Done");

    xTaskCreatePinnedToCore(DisplayFps, "DisplayFPS", 3000, NULL, 0, NULL, 0);

    setupOta();

    Debug.printf("max udp connections: %d\n", MEMP_NUM_NETCONN);
}

void loop()
{
    //The main process loop
    for (int j = 0; j < sizeof(pipes) / sizeof(Pipe); j++)
    {
        Pipe *pipe = &pipes[j];
        pipe->process();
    }

    for (int j = 0; j < sizeof(pipes) / sizeof(Pipe); j++)
    {
        Pipe *pipe = &pipes[j];
        pipe->out->ShowGroup();
    }

    //check for over-the-air firmware updates (also works over ETH)
    handleOta();

    //demo code that will scroll through the rainbow when rotating the rotary encoder
    static byte color;
    int rot = Rotary::getRotation(); 
    if (rot != 0){
        color += rot;
        Rotary::setWheel(color);
    }
}

//function that would display a loading animation on the ledstrips while eth connecting 
//- 1. should we display that on the outs now that we have a display?
//- 2. why wait for eth at all? there also also other input channels, so eth might never even connect
// void animate(byte r, byte g, byte b)
// {
//   byte pos = (millis()/200)%4;
//   for (int i=0;i<4;i++) {
//     forEach(output)
//       out[index]->SetPixelColor(i, r,g,i<pos?b:0);
//   }
//   forEach(output)
//     out[index]->Show();
// }

void clearall()
{
    for (int j = 0; j < sizeof(pipes) / sizeof(Pipe); j++)
    {
        Pipe *pipe = &pipes[j];
        pipe->out->Clear();
        pipe->out->Show();
    }
}

void DisplayFps(void *parameter)
{
    unsigned long lastFpsUpdate = 0;
    while (true)
    {

        unsigned long now = millis();
        unsigned long elapsedTime = now - lastFpsUpdate;

        int activeChannels = 0;
        int totalUsedframes = 0;
        int totalMissedframes = 0;
        int totalTotalframes = 0;
        int totalLength = 0;
        for (int j = 0; j < sizeof(pipes) / sizeof(Pipe); j++)
        {
            Pipe *pipe = &pipes[j];
            if (pipe->in->getTotalFrameCount() == 0)
                continue;
            
            activeChannels++;
            totalUsedframes += pipe->in->getUsedFramecount();
            totalMissedframes += pipe->in->getMissedFrameCount();
            totalTotalframes += pipe->in->getTotalFrameCount();
            pipe->in->resetFrameCount();

            totalLength += pipe->getNumPixels();
        }
 
        float outfps = activeChannels == 0 ? 0 : (float)1000. * totalUsedframes / (elapsedTime) / activeChannels;
        float infps = activeChannels == 0 ? 0 : (float)1000. * totalTotalframes / (elapsedTime) / activeChannels;
        float misses = totalTotalframes == 0 ? 0 : 100.0 * (totalMissedframes) / totalTotalframes;
        int avglength = activeChannels == 0 ? 0 : totalLength / activeChannels;

        lastFpsUpdate = now;

        Debug.printf("FPS: %d of %d (%d%% miss)\t interval: %dms \t freeHeap: %d \t avg length: %d \t channel: %d \n", (int)outfps, (int)infps, (int)misses, (int)elapsedTime, ESP.getFreeHeap(), avglength, activeChannels);

        Display::setFPS(infps,outfps,misses);
        Display::setLeds(totalLength);
        Display::setDMX(DMX::IsHealthy());
        Display::setMidi(Midi::isConnected());

        delay(500);
    }
    vTaskDelete(NULL);
}

#pragma GCC pop_options