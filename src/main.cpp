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

#include "patterns/helpers/tempo/tempo.h"
#include "patterns/helpers/tempo/tapTempo.h"
#include "patterns/helpers/tempo/proDJLinkTempo.h"
#include "patterns/helpers/tempo/midiClockTempo.h"
#include "patterns/helpers/tempo/udpTempo.h"
#include "patterns/helpers/params.h"

#include "hardware/midi/midi.h"
#include "hardware/ethernet/ethernet.h"
#include "hardware/firmwareUpdate/firmwareUpdate.h"
#include "hardware/display/display.h"
#include "hardware/rotary/rotary.h"
#include "hardware/apcmini/apcmini.h"

#include "configurations/configuration.h"

#include "metaOutputs/temperatureControlledOutput.h"

void UpdateDisplay(void *parameter);
void clearall();
void animate(byte r, byte g, byte b);

void click() { 
    Rotary::setColour(RGB(0,0,0));
    Debug.println("click"); 
    //APCMini::handleNoteOn(0,58,127);
}
void press() { Debug.println("press"); }
void release() { Debug.println("release"); }
void longpress() { Debug.println("longpress"); }
void rotate(int amount) { Debug.printf("rotate: %d\n", amount); }

DebugClass Debug;

void setup()
{
    Debug.begin();
    Debug.println("Checking safemode");

    //Start 'safe mode' when rotary button is pressed during boot
    //only starts firmware update
    //Be aware that the constructors of all stack objects will
    //still execute before this point. If they crash you have to
    //update over usb
    pinMode(39,INPUT);
    if (digitalRead(39)){
        Debug.println("\n\n================\n\nStarting safemode\n\n=======================\n\n");
        Display::Initialize(); 
        Display::setDFU(true,0);
        Ethernet::Initialize("Hyperion");
        FirmwareUpdate::Initialize();
        for(;;){
            FirmwareUpdate::Process();
            delay(1);
        }
    }

    Debug.println("Skipped safemode, normal boot");
    
    Debug.println("Loading configuration");
    LoadConfiguration();

    //PWM frequency in Hz, choose 100 for incandescent and 1500 for led
    //A higher frequency looks better, because it produces less flicker.
    //This works great for leds, but if currents are high (indandescent)
    //The mosfets will heat up quickly. Also the produced noise will be
    //more audible. Incandescent lamps have a slow response, so you wont
    //see the flicker as much. Setting the pwm frequency lower is better
    //in that case.
    PCA9685::Initialize();
    PCA9685::SetFrequency(Configuration.pwmFrequency);

    Display::Initialize(); //initialize display before outputs

    Rotary::Initialize();
    Rotary::onClick(click);
    Rotary::onPress(press);
    Rotary::onRelease(release);
    Rotary::onLongPress(longpress);
    Rotary::onRotate(rotate);

    DMX::SendFullFrame(false); //speed up dmx transmission by only sending the bytes that have a value
    DMX::SetUniverseSize(37,37); //handle some quirks of the the midi device i use for testing

    Debug.println("Starting outputs");
    for (Pipe* pipe : Configuration.pipes)
        pipe->out->Begin();

    clearall();



    Debug.println("Starting network");
    Ethernet::Initialize(Configuration.hostname);

    //add tempo sources in order of importance. first has highest priority
    Tempo::AddSource(ProDJLinkTempo::getInstance());
    Tempo::AddSource(MidiClockTempo::getInstance());
    Tempo::AddSource(TapTempo::getInstance()); 
    Tempo::AddSource(UdpTempo::getInstance()); 

    Midi::Initialize();
    Midi::onNoteOn([](uint8_t ch, uint8_t note, uint8_t velocity) {
        if (note == Configuration.tapMidiNote) TapTempo::getInstance()->Tap();
        if (note == Configuration.tapStopMidiNote) TapTempo::getInstance()->Stop();
        if (note == Configuration.tapAlignMidiNote) TapTempo::getInstance()->Align();
        if (note == Configuration.tapBarAlignMidiNote) Tempo::AlignPhrase();
    });

    Rotary::onPress([]() { TapTempo::getInstance()->Tap(); });
    Rotary::onLongPress([]() { TapTempo::getInstance()->Stop(); });

    Debug.println("Starting inputs");
    for (Pipe* pipe : Configuration.pipes)
        pipe->in->begin();

    Debug.println("Done");

    xTaskCreatePinnedToCore(UpdateDisplay, "UpdateDisplay", 3000, NULL, 0, NULL, 0);

    FirmwareUpdate::Initialize();
    Rotary::setColour(RGB(0,0,0));

    Debug.printf("max udp connections: %d\n", MEMP_NUM_NETCONN);
}

void loop()
{
    //The main process loop
    for (Pipe* pipe : Configuration.pipes)
        pipe->process();

    for (Pipe* pipe : Configuration.pipes)
        pipe->out->ShowGroup();
   
    //check for over-the-air firmware updates (also works over ETH)
    FirmwareUpdate::Process();

    Rotary::handleQueue();

    if (Configuration.paramCalculator)
        Configuration.paramCalculator();

    //demo code that will scroll through the rainbow when rotating the rotary encoder
    // static byte colour;
    // int rot = Rotary::getRotation(); 
    // if (rot != 0){
    //     colour += rot;
    //     Rotary::setColour(Hue(colour));
    // }
}

void clearall()
{
    for (Pipe* pipe : Configuration.pipes)
    {
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
        for (Pipe* pipe : Configuration.pipes)
        {
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

        Debug.printf("FPS: %d of %d (%d%% miss)\t interval: %dms \t freeHeap: %d \t avg length: %d \t channels: %d \n", (int)outfps, (int)infps, (int)misses, (int)elapsedTime, ESP.getFreeHeap(), avglength, activeChannels);
        //Debug.printf("IPAddress: %s\n", Ethernet::GetIp().toString().c_str());
        //Debug.printf("Tempo source: %s\n", Tempo::SourceName());

        Display::setFPS(infps,outfps,misses);
        Display::setLeds(totalLength);
        Display::setDMX(DMX::IsHealthy());
        Display::setMidi(Midi::isConnected());
        Display::setEthernet(Ethernet::isConnected(),Ethernet::isConnecting());
        Display::show();

        delay(500);
    }
    vTaskDelete(NULL);
}

#pragma GCC pop_options