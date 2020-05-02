#pragma once
#include "pattern.h"

inline RGB rainbowPattern(int index, int width, RGB* lastValue){
    int WheelPos = (0xFF * index / width + (millis()/10)) %0xFF;
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85)
    {
        return RGB(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else if(WheelPos < 170)
    {
        WheelPos -= 85;
        return RGB(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    else
    {
        WheelPos -= 170;
        return RGB(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
}