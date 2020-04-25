#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "sdkconfig.h"
#include <Arduino.h>

#include "ota.h"
#include "network.h"

#include "outputs/neoPixelBusOutput.h"
#include "outputs/pwmOutput.h"
#include "outputs/dmxOutput.h"
#include "lib/UDPFast/UDPFast.h"


// #include <../.pio/libdeps/esp32-poe/NeoPixelBus_ID547/src/NeoPixelBus.h>
//#include <NeoPixelBus.h>

void DisplayFps( void * parameter );
void clearall();
void animate(byte r, byte g, byte b);

// const uint16_t PixelCount = 4; // this example assumes 4 pixels, making it smaller will cause a failure
// const uint8_t PixelPin = 13;  // make sure to set this to the correct pin, ignored for Esp8266

// NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

const int numOutputs = 9;
#define forEach(output) for(int index=0;index<numOutputs;index++)

volatile int packets=0;
volatile int dropped=0;
volatile int busy=0;
volatile int success=0;
unsigned long lastFpsUpdate=0;
volatile boolean channelActive[numOutputs];

volatile int stripsize=0;

Output* out[numOutputs] = {
  new NeoPixelBusOutput<NeoEsp32Rmt0800KbpsMethod>(5),
  new NeoPixelBusOutput<NeoEsp32Rmt1800KbpsMethod>(4),
  new NeoPixelBusOutput<NeoEsp32Rmt2800KbpsMethod>(14),
  new NeoPixelBusOutput<NeoEsp32Rmt3800KbpsMethod>(2),
  new NeoPixelBusOutput<NeoEsp32Rmt4800KbpsMethod>(15),
  //new NeoPixelBusOutput<NeoEsp32Rmt5800KbpsMethod>(32), //cant use together with dmx
  new DMXOutput(),
  new NeoPixelBusOutput<NeoEsp32Rmt6800KbpsMethod>(0),
  new NeoPixelBusOutput<NeoEsp32Rmt7800KbpsMethod>(33),
  new PWMOutput()
  
};

UDPFast* udp[numOutputs] = {
  new UDPFast(),
  new UDPFast(),
  new UDPFast(),
  new UDPFast(),
  new UDPFast(),
  new UDPFast(),
  new UDPFast(),
  new UDPFast(),
  new UDPFast(),
};


const int MTU = 1500;
uint8_t data[MTU]; //i used to malloc this, but the MTU is 1500, so i just set it to the upped bound

const float gammaCorrection8 = 2; //1.6 looks less quantized when dimming. 2.5 gives prettier colours. 2 is a compromise
const float gammaCorrection12 = 2.5;
const float indancescentBase=0.2;

uint16_t gamma8[256];
uint16_t gamma12[256];

const unsigned int startPort = 9611; 

void setup() {
  Serial.begin(115200);
  Serial.println("Code started");
  //delay(500);

  for(int i=0;i<256; i++)
  {
    gamma8[i] = pow((float)i/255.,gammaCorrection8)*255;
    //gamma12[i] = pow((float)i/255.,gammaCorrection12)*4096; //4096 here, because the pca can also have a full on value, so 2^12+1 combinations are possible
    gamma12[i] = pow(((indancescentBase + (float)i/255.*(1.-indancescentBase))),gammaCorrection12)*4096;
  }
  
  Serial.println("Starting outputs");
  forEach(output)
  {
    out[index]->Begin();
    out[index]->setGammaCurve(gamma8);
  }
  out[8]->setGammaCurve(gamma12);

  Serial.println("Starting network");
  clearall();
  NetworkBegin();
  clearall();

  Serial.println("Starting inputs");
  forEach(output)
    udp[index]->begin(startPort+index);

  Serial.println("Done");
  xTaskCreate(DisplayFps,"DisplayFps",3000,NULL,0,NULL);

  //stopAllSockets();

  setupOta();

  Serial.printf("max udp connections: %d",MEMP_NUM_NETCONN);
}

void loop() {

  forEach(output)
  {
    int cb, cbr;
    boolean gotFrame=false;

    //read all packets, throw them away, but keep the last one.
    //there must a more elegant way to empty the queue, but i didnt find any
    //without having to recompile the lwip driver 
    while((cbr = udp[index]->parsePacketFast(data))) { gotFrame=true; dropped++; packets++; cb=cbr; }

//    cb=MTU;
//    gotFrame=true;
//    if (!out[index]->Ready()){
//      busy++;
//      continue;
//    }

    if (!gotFrame)
      continue;

    dropped--;

    int numPixels = std::max(cb/3+1,4);
    out[index]->SetLength(numPixels);

    stripsize = numPixels;

    for (int i=0;i<cb;i+=3) 
      out[index]->SetPixelColor(i/3, data[i],data[i+1],data[i+2]); 

    if (!out[index]->Ready()){
      busy++;
      continue;
    }

    out[index]->Show();
    
    success++;
    channelActive[index]=true;
  }

    handleOta();
}


void animate(byte r, byte g, byte b)
{
  byte pos = (millis()/200)%4;

  for (int i=0;i<4;i++) {
    forEach(output)
      out[index]->SetPixelColor(i, r,g,i<pos?b:0);
  }
  forEach(output)
    out[index]->Show();
}

void clearall()
{
  forEach(output) {
    out[index]->Clear();
    out[index]->Show();
  }
}



void DisplayFps( void * parameter )
{
  while(true){

    
    unsigned long now = millis();
    unsigned long elapsedTime = now-lastFpsUpdate;

    int activeChannels=0; 
      forEach(output)
        if (channelActive[index])
          activeChannels++;


    float fps = activeChannels==0? 0 : (float)1000.*success/(elapsedTime)/activeChannels;
    float misses = packets==0? 0 : 100.0*(dropped+busy)/packets;

    Serial.printf("Packets: %d, Dropped: %d,  Busy: %d. Success: %d\n",packets,dropped, busy, success);
    
    packets=0;
    dropped=0;
    busy=0;
    success=0;
    lastFpsUpdate = now;
    forEach(output)
      channelActive[index]=false;

    Serial.printf("elapsed: %d, #channels: %d, AVG FPS: %d (miss: %d%%)\n",(int)elapsedTime,activeChannels, (int)fps, (int)misses);
    Serial.printf("Free heap: %d\n",ESP.getFreeHeap());
    Serial.printf("strip size: %d\n",stripsize);
    delay(500);
  }
  vTaskDelete( NULL );
}
