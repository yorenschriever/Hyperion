#pragma once

#include <inttypes.h>
#include <Arduino.h>

//#define MIDI_SERIAL_INPUT_PIN    36          
//#define MIDI_SERIAL_OUTPUT_PIN   32          
#define MIDI_UART_NUM            UART_NUM_0  

//do not increase otherwise you will get a bug that the last byte cannot be read
//https://github.com/espressif/arduino-esp32/issues/1824
#define MIDI_BUF_SIZE            120 

class Midi {

    using MidiEvent3 = void (*)(uint8_t, uint8_t, uint8_t);

    public:
        static void Initialize();
        static void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
        static void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
        static void sendControllerChange(uint8_t channel, uint8_t controller, uint8_t value);
        static bool waitTxDone(int timeout = 500);

        static void onNoteOn(MidiEvent3 handler);
        static void onNoteOff(MidiEvent3 handler);
        static void onControllerChange(MidiEvent3 handler);

    private:
        static QueueHandle_t midi_rx_queue;
        static void uart_event_task(void *pvParameters);    // Event task

        static MidiEvent3 noteOnHandler;
        static MidiEvent3 noteOffHandler;
        static MidiEvent3 controllerChangeHandler;

};