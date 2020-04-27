#pragma once

#include <inttypes.h>

class Rotary
{
    public:
        static void Initialize();                  
        
        static void setLut(uint16_t* curveR, uint16_t* curveG, uint16_t* curveB);
        static bool setRGB(uint8_t r,uint8_t g,uint8_t b);
        static bool setWheel(uint8_t angle);
        
        static void tick();

        static void onPress();
        static void onRelease();
        static void onLongPress();
        static void onMove();

    private:
        Rotary(); 
        
        static uint16_t* lutR;
        static uint16_t* lutG;
        static uint16_t* lutB;
         
        static void fillLedBuffer(uint8_t* buffer, uint16_t* lut, uint8_t value, bool invert);                                         

};
