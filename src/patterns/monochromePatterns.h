#pragma once
#include "pattern.h"

inline Monochrome sinPattern(int index, int width, Monochrome* lastValue){
    return (uint8_t) (127.f + 127.f * cos(float(index)/10.f - millis()/100.));
}

inline Monochrome sawPattern(int index, int width, Monochrome* lastValue){
    return millis()/50 + index*2;
}

inline Monochrome randomPattern(int index, int width, Monochrome* lastValue){
    return random(0,100) < 10 ? 255:0;
}

inline Monochrome randomPattern2(int index, int width, Monochrome* lastValue){
    return random(0,100) < 1 ? 255:0;
}

inline Monochrome randomFadePattern(int index, int width, Monochrome* lastValue){
    uint8_t lastPixelvalue = lastValue[index].L;
    if (random(0,100) < 1)
        return 255;
    return max(lastPixelvalue - 10,0);
}

inline Monochrome meteorPattern(int index, int width, Monochrome* lastValue){
    int lastSelfvalue = lastValue[index].L;
    int lastNeighbourvalue = lastValue[(index+1) % width].L;
    if (random(0,300) < 1)
        return 255;
    return max(max(lastSelfvalue-25,lastNeighbourvalue-10),0);
}

inline Monochrome slowStrobePattern(int index, int width, Monochrome* lastValue){
    return millis()%100 < 25 ? 255:0;
}

inline Monochrome fastStrobePattern(int index, int width, Monochrome* lastValue){
    return millis()%50 < 25 ? 255:0;
}
