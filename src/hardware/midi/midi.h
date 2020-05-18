#pragma once

#include <inttypes.h>
#include <Arduino.h>

#define MIDI_UART_NUM UART_NUM_0

//do not increase otherwise you will get a bug that the last byte cannot be read
//https://github.com/espressif/arduino-esp32/issues/1824
#define MIDI_BUF_SIZE 120

#define NUMBER_OF_CONTROLLERS 120
#define NUMBER_OF_NOTES 127

//This class will read midi messages coming from the atmega328, which in turn gets them from 
//the usb port on the front panel. 
//I currently only implemented parsing of noteon, noteof and contoller change messages. 
//More might be added when i need them. (eg read midi clock for tepo sync?)
class Midi
{
    //define the signature of the callback functions i expect
    using MidiEvent3 = void (*)(uint8_t, uint8_t, uint8_t);
    using MidiEvent = void (*)();

public:
    static void Initialize();
    static void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
    static void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
    static void sendControllerChange(uint8_t channel, uint8_t controller, uint8_t value);
    static bool waitTxDone(int timeout = 500);

    static void onNoteOn(MidiEvent3 handler);
    static void onNoteOff(MidiEvent3 handler);
    static void onControllerChange(MidiEvent3 handler);
    static void onConnect(MidiEvent handler);

    static uint8_t noteStatus(uint8_t notenumber);
    static uint8_t controllerValue(uint8_t channelnumber);

    static bool isConnected();

private:
    static QueueHandle_t midi_rx_queue;
    static void uart_event_task(void *pvParameters); // Event task
    static bool started;

    static MidiEvent3 noteOnHandler;
    static MidiEvent3 noteOffHandler;
    static MidiEvent3 controllerChangeHandler;
    static MidiEvent connectHandler;

    static unsigned long lastMessageTime;

    static uint8_t noteValues[NUMBER_OF_NOTES];
    static uint8_t controllerValues[NUMBER_OF_CONTROLLERS];
};