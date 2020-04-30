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

#include "outputs/neoPixelBusOutput.h"
#include "outputs/pwmOutput.h"
#include "outputs/dmxOutput.h"
#include "outputs/apcminiOutput.h"
#include "inputs/udpInput.h"
#include "inputs/apcminiInput.h"
#include "lib/display/display.h"
#include "lib/rotary/rotary.h"
#include "lib/apcmini/apcmini.h"
#include "patterns/patterns.h"

void DisplayFps(void *parameter);
void clearall();
void animate(byte r, byte g, byte b);

const float gammaCorrection8 = 2; //1.6 looks less quantized when dimming. 2.5 gives prettier colours. 2 is a compromise
const float gammaCorrection12 = 2.5;
const float gammaCorrection12rotary = 2; //2 looks best with the roataryled.
const float indancescentBase = 0.2;

uint16_t gamma8[256];
uint16_t gamma12[256];
uint16_t gamma12rotary[256];
uint16_t *RGBGamma8[] = {gamma8, gamma8, gamma8};
uint16_t *RGBGamma12[] = {gamma12, gamma12, gamma12};

Pipe pipes[] = {
    Pipe(
        //create an apcmini input that creates monochome patterns
        new ApcminiInput<Monochrome>(
            30, //width of the pattern
            0,  //button column on the apc to listen to (0-7)
            //the patterns to attach to the buttons
            new Pattern<Monochrome>[8]{sinPattern, sawPattern, randomPattern, randomPattern2, meteorPattern, randomFadePattern, slowStrobePattern, fastStrobePattern}),
        new DMXOutput()),

    Pipe(
        new UDPInput(9611),
        new NeoPixelBusOutput<NeoEsp32Rmt0800KbpsMethod>(5),
        Pipe::transfer<RGB,RGB>,
        RGBGamma8),

    // Pipe(
    //     new UDPInput(9612),
    //     new NeoPixelBusOutput<NeoEsp32Rmt1800KbpsMethod>(4),
    //     Pipe::transfer<RGB,RGB>,
    //     RGBGamma8),

    Pipe(
        new UDPInput(9613),
        new NeoPixelBusOutput<NeoEsp32Rmt2800KbpsMethod>(14),
        Pipe::transfer<RGB,RGB>,
        RGBGamma8),

    Pipe(
        new UDPInput(9614),
        new NeoPixelBusOutput<NeoEsp32Rmt3800KbpsMethod>(2),
        Pipe::transfer<RGB,RGB>,
        RGBGamma8),

    Pipe(
        new UDPInput(9615),
        new NeoPixelBusOutput<NeoEsp32Rmt4800KbpsMethod>(15),
        Pipe::transfer<RGB,RGB>,
        RGBGamma8),

    // //Shared with DMX!
    // Pipe(
    //     new UDPInput(9616),
    //     new NeoPixelBusOutput<NeoEsp32Rmt5800KbpsMethod>(32),
    //     //new DMXOutput(),
    //     Pipe::transfer<RGB,RGB>,
    //     RGBGamma),

    Pipe(
        new UDPInput(9617),
        new NeoPixelBusOutput<NeoEsp32Rmt6800KbpsMethod>(0),
        Pipe::transfer<RGB,RGB>,
        RGBGamma8),

    Pipe(
        new UDPInput(9618),
        new NeoPixelBusOutput<NeoEsp32Rmt7800KbpsMethod>(33),
        Pipe::transfer<RGB,RGB>,
        RGBGamma8),

    Pipe(
        new UDPInput(9619),
        new PWMOutput(),
        Pipe::transfer<RGB,Monochrome12>,
        RGBGamma12),

    Pipe(
        new ApcminiInput<Monochrome>(
            12, //width of the pattern, in pixels
            1,  //button column on the apc to listen to (0-7)
            //the patterns to attach to the buttons
            new Pattern<Monochrome>[8]{sinPattern, sawPattern, randomPattern, randomPattern2, meteorPattern, randomFadePattern, slowStrobePattern, fastStrobePattern}),
        new PWMOutput(),
        Pipe::transfer<Monochrome,Monochrome12>,
        RGBGamma12),

    Pipe(
        new ApcminiInput<RGB>(
            16, //width of the pattern, in pixels
            2,  //button column on the apc to listen to (0-7)
            //the patterns to attach to the buttons
            new Pattern<RGB>[8]{rainbowPattern, rainbowPattern, rainbowPattern, rainbowPattern, rainbowPattern, rainbowPattern, rainbowPattern, rainbowPattern}),
        new NeoPixelBusOutput<NeoEsp32Rmt1800KbpsMethod>(4),
        Pipe::transfer<RGB,RGB>,
        RGBGamma8),

};

void click() { Debug.println("click"); }
void press() { Debug.println("press"); }
void release() { Debug.println("release"); }
void longpress() { Debug.println("longpress"); }
void rotate(int amount) { Debug.printf("rotate: %d\n", amount); }

void setup()
{
    Debug.begin(115200);

    for (int i = 0; i < 256; i++)
    {
        gamma8[i] = pow((float)i / 255., gammaCorrection8) * 255;
        gamma12rotary[i] = pow((float)i / 255., gammaCorrection12rotary) * 4096; //4096 here, because the pca can also have a full on value, so 2^12+1 combinations are possible
        gamma12[i] = pow(((indancescentBase + (float)i / 255. * (1. - indancescentBase))), gammaCorrection12) * 4096;
    }

    Rotary::Initialize();
    Rotary::setLut(gamma12rotary, gamma12rotary, gamma12rotary);
    Rotary::onClick(click);
    Rotary::onPress(press);
    Rotary::onRelease(release);
    Rotary::onLongPress(longpress);
    Rotary::onRotate(rotate);

    Display::Initialize(); //initialize display before outputs

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
    for (int j = 0; j < sizeof(pipes) / sizeof(Pipe); j++)
    {
        Pipe *pipe = &pipes[j];
        pipe->process();
    }

    //check for over-the-air firmware updates
    handleOta();

    //demo code that will scroll through the rainbow when rotating the rotary encoder
    static byte color;
    color += Rotary::getRotation();
    Rotary::setWheel(color);
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
            if (pipe->in->getTotalFrameCount() > 0)
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

        Display::setFPS(outfps);

        delay(500);
    }
    vTaskDelete(NULL);
}
