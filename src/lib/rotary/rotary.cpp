#include "rotary.h"
#include <Arduino.h>
#include <Wire.h>

#include "semaphores.h"

#define SDAPIN 13
#define SCLPIN 16

#define CLKPIN 35
#define DATAPIN 34
#define BTNPIN 39

#define PCA9685_I2C_ADDRESS 0x40
#define PCA9685_LED0_ON_L 0x06

#define INVERT true

uint16_t* Rotary::lutR = NULL;
uint16_t* Rotary::lutG = NULL;
uint16_t* Rotary::lutB = NULL;

void Rotary::Initialize()
{
    Wire.begin(SDAPIN,SCLPIN);

    //pinMode(CLKPIN, INPUT);
    pinMode(CLKPIN, INPUT_PULLUP);
    //pinMode(DATAPIN, INPUT);
    pinMode(DATAPIN, INPUT_PULLUP);
    pinMode(BTNPIN, INPUT);
}

bool Rotary::setRGB(uint8_t r,uint8_t g,uint8_t b)
{
    uint8_t buffer[3*4+1];
    buffer[0] = PCA9685_LED0_ON_L + 13*4;
    fillLedBuffer(buffer+1,lutR,r,INVERT);
    fillLedBuffer(buffer+5,lutG,g,INVERT);
    fillLedBuffer(buffer+9,lutB,b,INVERT);

    if(xSemaphoreTake( i2cMutex, ( TickType_t ) 100 ) != pdTRUE ){
        return false;
    }

    Wire.setClock(1000000);
    Wire.writeTransmission(PCA9685_I2C_ADDRESS, buffer, sizeof(buffer),true);

    xSemaphoreGive( i2cMutex );
    return true;
}

void Rotary::fillLedBuffer(uint8_t* buffer, uint16_t* lut, uint8_t value, bool invert)
{
    uint16_t lutVal = lut[value];

    if (lutVal > 4096)
      lutVal = 4096;

    if (invert)
      lutVal = 4096 - lutVal;

    if (lutVal == 0)
    {
        buffer[0] = 0;
        buffer[1] = 0;
        buffer[2] = 0;
        buffer[3] = 1<<4;
        return;
    }

    if (lutVal >= 4096)
    {
        buffer[0] = 0;
        buffer[1] = 1 << 4;
        buffer[2] = 0;
        buffer[3] = 0;
        return;
    }
    
    buffer[0] = 0;
    buffer[1] = 0;
    buffer[2] = lutVal & 0xFF;
    buffer[3] = lutVal >> 8;
}

bool Rotary::setWheel(uint8_t wheelpos)
{
  byte WheelPos = 255 - wheelpos;
  if(WheelPos < 85) {
    return setRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  }else   if(WheelPos < 170) {
    WheelPos -= 85;
    return setRGB(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return setRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

void Rotary::setLut(uint16_t* lutR, uint16_t* lutG, uint16_t* lutB){
    Rotary::lutR = lutR;
    Rotary::lutG = lutG;
    Rotary::lutB = lutB;
}