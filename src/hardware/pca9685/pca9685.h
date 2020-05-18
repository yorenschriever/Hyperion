#pragma once
#include <Arduino.h>
#include <inttypes.h>
#include <Wire.h>

class PCA9685
{
public:
    static void Initialize();
    static void SetFrequency(int Hz);
    static bool WritePWMData(uint8_t *data, int length, int maximumWait = 100);

private:
    static bool isInitialized;

    static void Reset();
    static void Write8(uint8_t addr, uint8_t d);
    static uint8_t Read8(uint8_t addr);
    static TwoWire *_i2c;
};
