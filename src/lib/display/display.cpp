#include <Wire.h>
//#include "../../.pio/libdeps/esp32-poe/Adafruit GFX Library_ID13/Adafruit_GFX.h"
//#include "../../.pio/libdeps/esp32-poe/Adafruit SSD1306_ID571/Adafruit_SSD1306.h"

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "display.h"
#include "semaphores.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define SDAPIN 13
#define SCLPIN 16

//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, NULL);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

volatile bool Display::dirty=false;
int Display::fps=1;

Display::Display()
{

}

void Display::Initialize()
{
    Wire.begin(SDAPIN,SCLPIN);
    //Wire.setClock(400000);

    display.setRotation(2);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
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

void Display::setFPS(int newfps)
{
    if (fps == newfps)
      return;
    fps = newfps;
    dirty = true;
}

void Display::displayTask( void * pvParameters )
{
 for( ;; )
 {
     if (dirty)
     {
            updateFrame();
            dirty=false;//todo only set to false it it actually updated
     }
     delay(10);
 }
}

void Display::updateFrame()
{
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  char buffer[5];
  itoa (fps,buffer,10);

  for(int16_t i=0; i<3 && buffer[i]!=0; i++) {
    display.write(buffer[i]);
  }
  display.write(" fps");

    if(xSemaphoreTake( i2cMutex, ( TickType_t ) 100 ) == pdTRUE )
    {
        display.display();
        
  
        //while(Wire.busy()){
        //    delay(1);
        //}
        xSemaphoreGive(i2cMutex);
        

        
    }
}