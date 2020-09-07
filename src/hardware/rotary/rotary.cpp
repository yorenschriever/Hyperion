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

#define DEBOUNCETIME 20   //ms
#define LONGPRESSTIME 350 //ms

LUT *Rotary::lut = new ColourCorrectionLUT(2.0, 4096, 150, 255, 255);

void Rotary::Initialize(bool createQueueTask)
{
    hasQueueTask = createQueueTask;

    PCA9685::Initialize();
    PCA9685::SetFrequency(1500);

    pinMode(CLKPIN, INPUT_PULLUP);
    pinMode(DATAPIN, INPUT_PULLUP);
    pinMode(BTNPIN, INPUT);

    attachInterrupt(CLKPIN, rotateISR, CHANGE);
    attachInterrupt(DATAPIN, rotateISR, CHANGE);
    attachInterrupt(BTNPIN, buttonISR, CHANGE);

    longPressTimer = xTimerCreate("LongpressTimer",LONGPRESSTIME,false,( void * ) 0,longpressISR);
}

bool Rotary::setColour(RGB colour)
{
    RGB12 col12 = static_cast<RGB12>(colour);
    col12.ApplyLut(lut);

    PCA9685::Write(13, col12.R, INVERT);
    PCA9685::Write(14, col12.G, INVERT);
    PCA9685::Write(15, col12.B, INVERT);
    PCA9685::Show();
    return true;
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

IRAM_ATTR
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
    }
    return; 
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

QueueHandle_t Rotary::eventQueue = NULL;
bool Rotary::hasQueueTask = false;
bool Rotary::queueStarted = false;

unsigned long Rotary::buttonDebounce = 0;
bool Rotary::buttonState = false;
unsigned long Rotary::buttonPressTime = 0;
TimerHandle_t Rotary::longPressTimer = NULL;

void Rotary::onRotate(RotationEvent evt, bool asTask)
{
    rotHandler = evt;
    rotHandlerAsTask = asTask;
    if (asTask)
        startQueHandler();
}

void Rotary::onPress(InputEvent evt, bool asTask)
{
    pressHandler = evt;
    pressHandlerAsTask = asTask;
    if (asTask)
        startQueHandler();
}
void Rotary::onRelease(InputEvent evt, bool asTask)
{
    releaseHandler = evt;
    releaseHandlerAsTask = asTask;
    if (asTask)
        startQueHandler();
}
void Rotary::onClick(InputEvent evt, bool asTask)
{
    clickHandler = evt;
    clickHandlerAsTask = asTask;
    if (asTask)
        startQueHandler();
}
void Rotary::onLongPress(InputEvent evt, bool asTask)
{
    longPressHandler = evt;
    longPressHandlerAsTask = asTask;
    if (asTask)
        startQueHandler();
}

IRAM_ATTR
void Rotary::buttonISR()
{   
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    unsigned long now = millis();
    if (now - buttonDebounce < DEBOUNCETIME)
        return;

    buttonDebounce = now;

    bool state = digitalRead(BTNPIN);

    if (state)
    {
        buttonPressTime = millis();
        xTimerResetFromISR(longPressTimer,&xHigherPriorityTaskWoken);
    }

    if (!state)
    {
        xTimerStopFromISR(longPressTimer,&xHigherPriorityTaskWoken);
    }

    if (state)
        handleEvent(pressHandler, pressHandlerAsTask);

    if (!state)
        handleEvent(releaseHandler, releaseHandlerAsTask);

    if (!state && now - buttonPressTime < LONGPRESSTIME)
        handleEvent(clickHandler, clickHandlerAsTask);

    buttonState = state;

    if (xHigherPriorityTaskWoken)
        portYIELD_FROM_ISR();
}

void Rotary::longpressISR(TimerHandle_t xTimer)
{
    handleEvent(longPressHandler, longPressHandlerAsTask);
}

inline void Rotary::handleEvent(InputEvent function, bool asTask)
{
    if (!function)
        return;

    if (!asTask)
    {
        function();
        return;
    }

    //add event to the event queue so it will be handled in the asynchronous task
    if (queueStarted)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        EventQueueItem item = EventQueueItem{function, NULL, 0};
        xQueueSendFromISR(eventQueue,(void *)&item,&xHigherPriorityTaskWoken);

        if( xHigherPriorityTaskWoken )
            portYIELD_FROM_ISR ();
    }
}

inline void Rotary::handleEvent(RotationEvent function, bool asTask, int amount)
{
    if (!function)
        return;

    if (!asTask)
    {
        function(amount);
        return;
    }

    //add event to the event queue so it will be handled in the asynchronous task
    if (queueStarted)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        EventQueueItem item = EventQueueItem{NULL, function, amount};
        xQueueSendFromISR(eventQueue,(void *)&item,&xHigherPriorityTaskWoken);

        if( xHigherPriorityTaskWoken )
            portYIELD_FROM_ISR ();
    }
}


void Rotary::queueHandlerTask(void *param)
{
    EventQueueItem item;

    while (true)
    {
        if (xQueueReceive(eventQueue, &item, (TickType_t) 50) == pdPASS)
        {
            if (item.inputfunction)
                item.inputfunction();
            else if (item.rotationfunction)
                item.rotationfunction(item.amount);
        }
    }
    vTaskDelete(NULL);
}

void Rotary::startQueHandler()
{
    if (queueStarted)
        return;

    eventQueue = xQueueCreate(100, sizeof(EventQueueItem));
    if (hasQueueTask)
        xTaskCreatePinnedToCore(queueHandlerTask, "RotaryEvent", 3000, NULL, 1, NULL, 1);

    queueStarted = true;
}

void Rotary::handleQueue()
{
    EventQueueItem item;

    while (xQueueReceive(eventQueue, &item, (TickType_t) 0) == pdTRUE)
    {
        if (item.inputfunction)
            item.inputfunction();
        else if (item.rotationfunction)
            item.rotationfunction(item.amount);
    }
}