#include "rotary.h"
#include <Arduino.h>
#include <Wire.h>
#include "luts/colourCorrectionLut.h"
#include "../sharedResources.h"

#define CLKPIN 35
#define DATAPIN 34
#define BTNPIN 39

#define PCA9685_LED0_ON_L 0x06

#define INVERT true //the leds are inverted, so writing 4096 turns them off

#define DEBOUNCETIME 50   //ms
#define LONGPRESSTIME 350 //ms

LUT *Rotary::lut = new ColourCorrectionLUT(2.0, 4096, 150,255,255);

void Rotary::Initialize()
{
    PCA9685::Initialize();
    PCA9685::SetFrequency(1500);

    pinMode(CLKPIN, INPUT_PULLUP);
    pinMode(DATAPIN, INPUT_PULLUP);
    pinMode(BTNPIN, INPUT);

    attachInterrupt(CLKPIN, rotateISR, CHANGE);
    attachInterrupt(DATAPIN, rotateISR, CHANGE);
    attachInterrupt(BTNPIN, buttonISR, CHANGE);
}

bool Rotary::setColour(RGB colour)
{
    uint8_t buffer[3 * 4 + 1];
    buffer[0] = PCA9685_LED0_ON_L + 13 * 4;
    fillLedBuffer(buffer + 1, lut->luts[0], colour.R, INVERT);
    fillLedBuffer(buffer + 5, lut->luts[1], colour.G, INVERT);
    fillLedBuffer(buffer + 9, lut->luts[2], colour.B, INVERT);

    return PCA9685::WritePWMData(buffer,sizeof(buffer),100);    
}

void Rotary::fillLedBuffer(uint8_t *buffer, uint16_t *lut, uint8_t value, bool invert)
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
        buffer[3] = 1 << 4;
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

void Rotary::setLut(LUT *lut)
{
    Rotary::lut = lut;
}

uint8_t Rotary::prevNextCode = 0;
uint16_t Rotary::store = 0;
unsigned long Rotary::lastChange = 0;
int Rotary::rotation = 0;

int Rotary::getRotation()
{
    int result = rotation;
    rotation = 0;
    return result;
}

bool Rotary::isButtonPressed()
{
    return buttonState;
}

void Rotary::rotateISR()
{
    static int8_t rot_enc_table[] = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0};

    prevNextCode <<= 2;
    if (digitalRead(DATAPIN))
        prevNextCode |= 0x02;
    if (digitalRead(CLKPIN))
        prevNextCode |= 0x01;
    prevNextCode &= 0x0f;

    // If valid then store as 16 bit data.
    if (rot_enc_table[prevNextCode])
    {
        store <<= 4;
        store |= prevNextCode;

        int result = 0;
        if ((store & 0xff) == 0x2b)
            result = -1;
        else if ((store & 0xff) == 0x17)
            result = 1;
        else
            return; // 0;

        //add some acceleration
        unsigned long now = millis();
        if (now - lastChange < 10)
            result *= 4;
        else if (now - lastChange < 50)
            result *= 2;

        lastChange = now;
        rotation += result;

        handleEvent(rotHandler, rotHandlerAsTask, result);

        return;
        //return result;
    }
    return; // 0;
}

Rotary::RotationEvent Rotary::rotHandler = NULL;
Rotary::InputEvent Rotary::pressHandler = NULL;
Rotary::InputEvent Rotary::releaseHandler = NULL;
Rotary::InputEvent Rotary::clickHandler = NULL;
Rotary::InputEvent Rotary::longPressHandler = NULL;

bool Rotary::rotHandlerAsTask = true;
bool Rotary::pressHandlerAsTask = true;
bool Rotary::releaseHandlerAsTask = true;
bool Rotary::clickHandlerAsTask = true;
bool Rotary::longPressHandlerAsTask = true;

unsigned long Rotary::buttonDebounce = 0;
bool Rotary::buttonState = false;
unsigned long Rotary::buttonPressTime = 0;
hw_timer_t *Rotary::longPressTimer = NULL;

void Rotary::onRotate(RotationEvent evt, bool asTask)
{
    rotHandler = evt;
    rotHandlerAsTask = asTask;
}

void Rotary::onPress(InputEvent evt, bool asTask)
{
    pressHandler = evt;
    pressHandlerAsTask = asTask;
}
void Rotary::onRelease(InputEvent evt, bool asTask)
{
    releaseHandler = evt;
    releaseHandlerAsTask = asTask;
}
void Rotary::onClick(InputEvent evt, bool asTask)
{
    clickHandler = evt;
    clickHandlerAsTask = asTask;
}
void Rotary::onLongPress(InputEvent evt, bool asTask)
{
    longPressHandler = evt;
    longPressHandlerAsTask = asTask;
}

void Rotary::buttonISR()
{
    unsigned long now = millis();
    if (now - buttonDebounce < DEBOUNCETIME)
        return;

    buttonDebounce = now;

    bool state = digitalRead(BTNPIN);

    if (state)
    {
        buttonPressTime = millis();

        if (longPressTimer)
            timerEnd(longPressTimer);
        longPressTimer = timerBegin(rotaryButtonTimer, 80, true);                     // timer_id = 0; divider=80; countUp = true;
        timerAttachInterrupt(longPressTimer, &longpressISR, true);    // edge = true
        timerAlarmWrite(longPressTimer, 1000 * LONGPRESSTIME, false); //1000 ms
        timerAlarmEnable(longPressTimer);
    }

    if (!state)
    {
        if (longPressTimer)
            timerEnd(longPressTimer);
        longPressTimer = NULL;
    }

    if (state)
        handleEvent(pressHandler,pressHandlerAsTask);

    if (!state)
        handleEvent(releaseHandler,releaseHandlerAsTask);

    if (!state && now - buttonPressTime < LONGPRESSTIME)
        handleEvent(clickHandler, clickHandlerAsTask);

    buttonState = state;
}

void Rotary::longpressISR()
{
    handleEvent(longPressHandler,longPressHandlerAsTask);
}

void Rotary::handleEvent(InputEvent function, bool asTask)
{
    if (!function)
        return;

    if (!asTask)
    {
        function();
        return;
    }

    //spin up a thread to handle the task. this has more overhead, 
    //but allows you to do stuff that would not be possible from an interrupt.
    //ideally you create your handler in such a way that it only sets a flag and
    //the actual handling takes place in the main loop. If you did that you can 
    //pass asTask=false when setting the handlers. 
    xTaskCreate(inputEventTask,"RotaryEvent",3000,(void*) function,1,NULL);
}


void Rotary::handleEvent(RotationEvent function, bool asTask, int amount)
{
    if (!function)
        return;

    if (!asTask)
    {
        function(amount);
        return;
    }

    //spin up a thread to handle the task. this has more overhead, 
    //but allows you to do stuff that would not be possible from an interrupt.
    //ideally you create your handler in such a way that it only sets a flag and
    //the actual handling takes place in the main loop. If you did that you can 
    //pass asTask=false when setting the handlers. 
    RotationEventParams* params = new RotationEventParams {function, amount};
    xTaskCreate(rotationEventTask,"RotaryEvent",3000,params,1,NULL);
}


void Rotary::inputEventTask(void * param)
{
    InputEvent func = (InputEvent) param;
    func();
    vTaskDelete(NULL);
}

void Rotary::rotationEventTask(void * param)
{
    RotationEventParams* params = (RotationEventParams*) param;
    params->function(params->amount);
    delete params;
    vTaskDelete(NULL);
}

