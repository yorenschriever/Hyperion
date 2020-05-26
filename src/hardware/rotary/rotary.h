#pragma once

#include <inttypes.h>
#include <Arduino.h>
#include "luts/lut.h"
#include "colours.h"
#include "hardware/pca9685/pca9685.h"

//Rotary interfaces the rotary encoder on the front panel.
class Rotary
{
    using InputEvent = void (*)();
    using RotationEvent = void (*)(int);

    using EventQueueItem = struct {
        InputEvent inputfunction;
        RotationEvent rotationfunction;
        int amount;
    };

public:
    //if you pass createQueueTask=true, a new thread will be started to handle 
    //all async handlers. The safer method is to set createQueueTask=false
    //and call Rotary::handleQueue() regularly at times it suits you. This avoids
    //concurrency issues and glitches
    static void Initialize(bool createQueueTask=false);

    //You can apply a lookup table to RGB leds
    static void setLut(LUT *lut);

    //set a colour. 
    static bool setColour(RGB colour);

    //get the rotation since the last time this function was called
    //you can use this if you dont want to keep track of the rotation with onRotate()
    static int getRotation();
    static bool isButtonPressed();

    //use these functions to attach events to the button.
    //be aware that the handlers are called from inside an interrupt, so they should be kept short.
    //only 1 handler can be attached. if you attach another handler, the first one is removed. 
    //it is strongly advised to attach very short eventhandlers (only set a flag in your code)
    //and set asTask to true. Doing otherwise will result in glitches of the led outputs
    //or crashes of the application.
    static void onRotate(RotationEvent evt, bool async=true);
    static void onPress(InputEvent evt, bool async=true);
    static void onRelease(InputEvent evt, bool async=true);
    static void onClick(InputEvent evt, bool async=true);
    static void onLongPress(InputEvent evt, bool async=true);

    static void handleQueue();

private:
    Rotary();

    static LUT *lut;

    static void fillLedBuffer(uint8_t *buffer, uint16_t *lut, uint8_t value, bool invert);
    static void rotateISR();
    static void buttonISR();
    static void longpressISR(TimerHandle_t xTimer);

    static QueueHandle_t eventQueue;
    static bool hasQueueTask;
    static bool queueStarted;
    static void startQueHandler();
    static void queueHandlerTask(void *param);
    static void handleEvent(InputEvent function, bool asTask);
    static void handleEvent(RotationEvent function, bool asTask, int amount);

    static uint8_t prevNextCode;
    static uint16_t store;
    static unsigned long lastChange;
    static int rotation;

    static RotationEvent rotHandler;
    static InputEvent pressHandler;
    static InputEvent releaseHandler;
    static InputEvent clickHandler;
    static InputEvent longPressHandler;

    static bool rotHandlerAsTask;
    static bool pressHandlerAsTask;
    static bool releaseHandlerAsTask;
    static bool clickHandlerAsTask;
    static bool longPressHandlerAsTask;

    static unsigned long buttonDebounce;
    static bool buttonState;
    static unsigned long buttonPressTime;
    static TimerHandle_t longPressTimer;
};
