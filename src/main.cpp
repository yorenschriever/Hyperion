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
uint16_t *RGBGamma[] = {gamma8, gamma8, gamma8};
uint16_t *RGBGamma12[] = {gamma12, gamma12, gamma12};

Pipe pipes[] = {
    Pipe(
        new ApcminiInput<Monochrome>(30, 0, new Pattern<Monochrome>[8]{sinPattern, sawPattern, randomPattern, randomPattern2, meteorPattern, randomFadePattern, slowStrobePattern, fastStrobePattern}),
        new DMXOutput(),
        Pipe::transfer<Monochrome, Monochrome>,
        NULL)
    // Pipe(Out(),Pipe::transfer<Monochrome,RGB>,RGBGamma),
    // Pipe(Out(),Pipe::transfer<RGB,Monochrome>,RGBGamma),
    // Pipe(Out(),Pipe::transfer,RGBGamma),
    // Pipe(Out(),Pipe::transfer<RGB,RGB12>,RGBGamma12)
};

// const int numOutputs = 9;
// #define forEach(output) for(int index=0;index<numOutputs;index++)

// Output* out[numOutputs] = {
//   new NeoPixelBusOutput<NeoEsp32Rmt0800KbpsMethod>(5),
//   new NeoPixelBusOutput<NeoEsp32Rmt1800KbpsMethod>(4),
//   new NeoPixelBusOutput<NeoEsp32Rmt2800KbpsMethod>(14),
//   new NeoPixelBusOutput<NeoEsp32Rmt3800KbpsMethod>(2),
//   new NeoPixelBusOutput<NeoEsp32Rmt4800KbpsMethod>(15),
//   //new NeoPixelBusOutput<NeoEsp32Rmt5800KbpsMethod>(32), //cant use together with dmx
//   new DMXOutput(),
//   new NeoPixelBusOutput<NeoEsp32Rmt6800KbpsMethod>(0),
//   new NeoPixelBusOutput<NeoEsp32Rmt7800KbpsMethod>(33),
//   //new APCMiniOutput(),
//   new PWMOutput()

// };

// Input* in[numOutputs] = {
//   new UDPInput(9611),
//   new UDPInput(9612),
//   new UDPInput(9613),
//   new UDPInput(9614),
//   new UDPInput(9615),
//   //new UDPInput(9616),
//   new ApcminiInput(90, 0, new Pattern[8] {sinPattern, sawPattern, randomPattern, randomPattern2, meteorPattern, randomFadePattern, slowStrobePattern, fastStrobePattern}),
//   new UDPInput(9617),
//   new UDPInput(9618),
//   //new UDPInput(9619)
//   new ApcminiInput(3*12, 1, new Pattern[8] {sinPattern, sawPattern, randomPattern, randomPattern2, meteorPattern, randomFadePattern, slowStrobePattern, fastStrobePattern}),
// };


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

    // Debug.println("Starting outputs");
    // //TODO call setLUT, do this when creating, not here
    // forEach(output)
    // {
    //   out[index]->Begin();
    //   out[index]->setGammaCurve(gamma8);
    // }
    // out[8]->setGammaCurve(gamma12);

    // Debug.println("Starting network");
    // clearall();
    // NetworkBegin();
    // clearall();

    Debug.println("Starting inputs");
    // forEach(output)
    //     in[index]->begin();

    for (int j = 0; j < sizeof(pipes) / sizeof(Pipe); j++)
    {
        pipes[j].in->begin();
        pipes[j].out->Begin();
    }

    Debug.println("Done");

    xTaskCreatePinnedToCore(DisplayFps, "DisplayFPS", 3000, NULL, 0, NULL, 0);

    //setupOta();

    Debug.printf("max udp connections: %d\n", MEMP_NUM_NETCONN);

    //APCMini::Initialize();
}

void loop()
{

    //forEach(output)
    //{
    for (int j = 0; j < sizeof(pipes) / sizeof(Pipe); j++)
    {
        Pipe *pipe = &pipes[j];
        pipe->process();

        // //check if the output is done sending the previous message
        // //TODO i dont know where to put this check
        // //if sending is the bottleneck, then i dont want to needlessly precalculate frames
        // //if calculating is the bottleneck, then i want to precalculate the frame while we are waiting for the send to complete
        // //is it possible to do the pre calculations with low prio?
        // if (!pipe->out->Ready())
        //     continue;

        // //load the new frame from the input, or quit if none was available
        // int datalength = pipe->in->loadData(data);
        // if (datalength == 0)
        //     continue;

        // //int numPixels = std::max(cb/3+1,4);
        // //out[index]->SetLength(numPixels);

        // //copy the pixel values
        // //for (int i=0;i<cb;i+=3)
        // //  out[index]->SetPixelColor(i/3, data[i],data[i+1],data[i+2]);

        // pipe->handleData(data, datalength);

        // pipe->out->Show();
    }

    //check for over-the-air firmware updates
    //handleOta();

    //demo code that will scroll through the rainbow when rotating the rotary encoder
    static byte color;
    color += Rotary::getRotation();
    Rotary::setWheel(color);
}

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

            totalLength += pipe->out->getLength();
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
