#include <Wire.h>
//#include "../../.pio/libdeps/esp32-poe/Adafruit GFX Library_ID13/Adafruit_GFX.h"
//#include "../../.pio/libdeps/esp32-poe/Adafruit SSD1306_ID571/Adafruit_SSD1306.h"

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "display.h"
#include "semaphores.h"
#include "debug.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define SDAPIN 13
#define SCLPIN 16

//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, NULL);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

volatile bool Display::dirty=false;
int Display::fpsin=1;
int Display::fpsout=1;
int Display::framemiss=0;
bool Display::ethconnected=false;
bool Display::dmxconnected=false;
bool Display::midiconnected=false;
int Display::numleds=0;
bool Display::dfu=false;
int Display::dfupercentage=0;

Display::Display()
{

}

void Display::Initialize()
{
    Wire.begin(SDAPIN,SCLPIN);
    //Wire.setClock(400000);

    display.setRotation(2);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
        Debug.println(F("SSD1306 allocation failed"));
        //for(;;); // Don't proceed, loop forever //TODO 
        return;
    }
    display.setRotation(2);
    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.clearDisplay(); //clear that logo
    display.display();

    TaskHandle_t xHandle = NULL;
    xTaskCreatePinnedToCore( 
        displayTask, "Display", 3000, NULL, 
        //tskIDLE_PRIORITY, 
        //100,
        5,
        &xHandle,
        1);
    configASSERT( xHandle ); 
}

void Display::setFPS(int newfpsin ,int newfpsout, int misses)
{
    if (fpsin == newfpsin && fpsout == newfpsout && misses == framemiss)
      return;
    fpsin = newfpsin;
    fpsout = newfpsout;
    framemiss = misses;
    dirty = true;
}

void Display::setEthernet(bool connected)
{
    if (ethconnected == connected)
        return;
    ethconnected = connected;
    dirty = true;
}

void Display::setDMX(bool connected)
{
    if (dmxconnected == connected)
        return;
    dmxconnected = connected;
    dirty = true;
}

void Display::setMidi(bool connected)
{
    if (midiconnected == connected)
        return;
    midiconnected = connected;
    dirty = true;
}

void Display::setLeds(int leds){
    if (numleds == leds)
        return;
    numleds = leds;
    dirty=true;
}

void Display::setDFU(bool dfu, int percentage){
    if (Display::dfu==dfu && Display::dfupercentage==percentage)
        return;
    Display::dfu=dfu;
    Display::dfupercentage=percentage;
    dirty=true;
}

void Display::displayTask( void * pvParameters )
{
 for( ;; )
 {
     if (dirty)
     {
            updateFrame();
            dirty=false;//todo only set to false if it actually updated
     }
     delay(10);
 }
}

void Display::updateFrame()
{
    display.clearDisplay();

        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.cp437(true);         // Use full 256 char 'Code Page 437' font

    if (dfu){

        display.setCursor(0, 0);  
        display.write("Firmware update");
        display.setCursor(0, 8);     
        display.write((String(dfupercentage) + "%").c_str());

    } else {



        display.setCursor(0, 0);    
        display.write((String(fpsout) + String("/") + String(fpsin) + String(" FPS (") + String(100-framemiss) + String("%)")).c_str());

        display.setCursor(0, 8);     
        display.write((String("Lights: ") + String(numleds)).c_str());

        display.setCursor(0, 16);    
        
        int pos=0;
        pos = renderConnectionStatus(pos,ethconnected,"ETH");
        pos = renderConnectionStatus(pos,dmxconnected,"DMX"); 
        pos = renderConnectionStatus(pos,midiconnected,"MIDI"); 

    }

    if(xSemaphoreTake( i2cMutex, ( TickType_t ) 100 ) == pdTRUE )
    {
        display.display();
        xSemaphoreGive(i2cMutex);
    }
}

int Display::renderConnectionStatus(int start, bool connected, String name){
    int width = name.length()*6+2;
    if (connected)
        display.fillRect(start,23, width, 9, SSD1306_WHITE);

    display.setCursor(start+1, 24); 
    display.setTextColor(SSD1306_INVERSE);
    display.write(name.c_str());

    return start+width+3;
}