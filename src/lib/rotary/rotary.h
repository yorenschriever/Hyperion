#pragma once

#include <inttypes.h>
#include <Arduino.h>
#include "lut.h"

//Rotary interfaces the rotary encoder on the front panel.
class Rotary
{
    using InputEvent = void (*)();
    using RotationEvent = void (*)(int);

public:
    static void Initialize();

    //You can apply a lookup table to RGB leds
    static void setLut(LUT *lut);

    //set a colour. TODO: accept RGB and Hue colours here, instead of separate arguments
    static bool setRGB(uint8_t r, uint8_t g, uint8_t b);
    static bool setWheel(uint8_t angle);

    //get the rotation since the last time this function was called
    //you can use this if you dont want to keep track of the rotation with onRotate()
    static int getRotation();
    static bool isButtonPressed();

    //use these functions to attach events to the button.
    //be aware that the handlers are called from inside an interrupt, so they should be kept short.
    //only 1 handler can be attached. if you attach another handler, the first one is removed. 
    //TODO handle them from a task
    static void onRotate(RotationEvent evt);
    static void onPress(InputEvent evt);
    static void onRelease(InputEvent evt);
    static void onClick(InputEvent evt);
    static void onLongPress(InputEvent evt);

private:
    Rotary();

    static LUT *lut;

    static void fillLedBuffer(uint8_t *buffer, uint16_t *lut, uint8_t value, bool invert);
    static void rotateISR();
    static void buttonISR();
    static void longpressISR();

    static uint8_t prevNextCode;
    static uint16_t store;
    static unsigned long lastChange;
    static int rotation;

    static RotationEvent rotHandler;
    static InputEvent pressHandler;
    static InputEvent releaseHandler;
    static InputEvent clickHandler;
    static InputEvent longPressHandler;
    static unsigned long buttonDebounce;
    static bool buttonState;
    static unsigned long buttonPressTime;
    static hw_timer_t *longPressTimer;
};
