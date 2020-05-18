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


#include "debug.h"
#include "pipe.h"
#include "colours.h"
#include "luts/lut.h"

#include "hardware/ethernet/ethernet.h"
#include "hardware/firmwareUpdate/firmwareUpdate.h"
#include "hardware/display/display.h"
#include "hardware/rotary/rotary.h"
#include "hardware/apcmini/apcmini.h"

#include "configurations/configuration.h"


void UpdateDisplay(void *parameter);
void clearall();
void animate(byte r, byte g, byte b);

void click() { 
    Debug.println("click"); 
    //Rotary::setRGB(0,0,0);
    //APCMini::handleNoteOn(0,58,127);
}
void press() { Debug.println("press"); }
void release() { Debug.println("release"); }
void longpress() { Debug.println("longpress"); }
void rotate(int amount) { Debug.printf("rotate: %d\n", amount); }

void setup()
{
    //Start 'safe mode' when rotary button is pressed during boot
    //only starts firmware update
    //Be aware that the constructors of all stack objects will
    //still execute before this point. If they crash you have to
    //update over usb
    pinMode(39,INPUT);
    if (digitalRead(39)){
        Display::Initialize(); 
        Display::setDFU(true,0);
        Ethernet::Initialize();
        FirmwareUpdate::Initialize();
        for(;;){
            FirmwareUpdate::Process();
            delay(1);
        }
    }

    Debug.begin(115200);

    Display::Initialize(); //initialize display before outputs

    Rotary::Initialize();
    Rotary::onClick(click);
    Rotary::onPress(press);
    Rotary::onRelease(release);
    Rotary::onLongPress(longpress);
    Rotary::onRotate(rotate);

    DMX::SendFullFrame(false); //speed up dmx transmission by only sending the bytes that have a value
    DMX::SetUniverseSize(37,7); //handle some quirks of the the midi device i use for testing

    Debug.println("Starting outputs");
    for (int j = 0; j < sizeof(pipes) / sizeof(Pipe); j++)
        pipes[j].out->Begin();

    clearall();

    PCA9685::Initialize();

    Debug.println("Starting network");
    Ethernet::Initialize();
    Ethernet::SetFixedIp(IPAddress(192,168,1,123), IPAddress(192,168,1,1), IPAddress(255,255,255,0));

    Debug.println("Starting inputs");
    for (int j = 0; j < sizeof(pipes) / sizeof(Pipe); j++)
        pipes[j].in->begin();

    Debug.println("Done");

    xTaskCreatePinnedToCore(UpdateDisplay, "UpdateDisplay", 3000, NULL, 0, NULL, 0);

    FirmwareUpdate::Initialize();
    Rotary::setColour(RGB(0,0,0));

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
    FirmwareUpdate::Process();

    //demo code that will scroll through the rainbow when rotating the rotary encoder
    static byte colour;
    int rot = Rotary::getRotation(); 
    if (rot != 0){
        colour += rot;
        Rotary::setColour(Hue(colour));
    }
}

void clearall()
{
    for (int j = 0; j < sizeof(pipes) / sizeof(Pipe); j++)
    {
        Pipe *pipe = &pipes[j];
        pipe->out->Clear();
        pipe->out->Show();
    }
}

void UpdateDisplay(void *parameter)
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
        Display::setEthernet(Ethernet::isConnected());
        Display::show();

        delay(500);
    }
    vTaskDelete(NULL);
}

#pragma GCC pop_options