#pragma once
#include <Arduino.h>
#include <inttypes.h>

using Pattern = uint8_t (*)(int index, int width, uint8_t* lastValue);

inline uint8_t sinPattern(int index, int width, uint8_t* lastValue){
    return (uint8_t) (127.f + 127.f * cos(float(index)/10.f - millis()/100.));
}

inline uint8_t sawPattern(int index, int width, uint8_t* lastValue){
    return millis()/50 + index*2;
}

inline uint8_t randomPattern(int index, int width, uint8_t* lastValue){
    return random(0,100) < 10 ? 255:0;
}

inline uint8_t randomPattern2(int index, int width, uint8_t* lastValue){
    return random(0,100) < 1 ? 255:0;
}

inline uint8_t randomFadePattern(int index, int width, uint8_t* lastValue){
    uint8_t lastPixelvalue = lastValue[index];
    if (random(0,100) < 1)
        return 255;
    return max(lastPixelvalue - 10,0);
}

inline uint8_t meteorPattern(int index, int width, uint8_t* lastValue){
    int leftindex = (index/3)*3; //lastvalue is current a list of rgb values, with rgb having the same value. 
    if (index!=leftindex)
        return lastValue[leftindex]; //only really update if 
    int lastSelfvalue = lastValue[leftindex];
    int lastNeighbourvalue = lastValue[(leftindex+3) % width];
    if (random(0,1000) < 1)
        return 255;
    return max(max(lastSelfvalue-25,lastNeighbourvalue-10),0);
}

inline uint8_t slowStrobePattern(int index, int width, uint8_t* lastValue){
    return millis()%100 < 25 ? 255:0;
}

inline uint8_t fastStrobePattern(int index, int width, uint8_t* lastValue){
    return millis()%50 < 25 ? 255:0;
}

