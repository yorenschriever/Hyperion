#pragma once
#include <Arduino.h>
#include <inttypes.h>
#include <Wire.h>

class PCA9685
{
public:
    static void Initialize();
    static void SetFrequency(int Hz);
    static void Write(uint8_t index, uint16_t value, bool invert=false);
    //static bool Write(uint8_t index, uint16_t* values, uint8_t numValues);
    static void Show();
    static bool Ready();
private:
    static bool isInitialized;

    static void Reset();
    static void Write8(uint8_t addr, uint8_t d);
    static uint8_t Read8(uint8_t addr);
    static TwoWire *_i2c;

    static volatile bool busy;
    static xSemaphoreHandle dirtySemaphore;
    static void PCA9685Task(void *param) ;
    static bool WritePWMData(uint8_t *data, int length, int maximumWait = 100);

    static uint16_t values[16];
    static uint16_t valuesBuf[16];
};
