#include "midi.h"

#define CONFIG_UART_ISR_IN_IRAM 1

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "debug.h"
#include "hardware/uart_iram/uart_iram.h"

// https://www.midi.org/specifications/item/table-1-summary-of-midi-message

QueueHandle_t Midi::midi_rx_queue;

#define NOTEON 0x90
#define NOTEOFF 0x80
#define CONTROLLERCHANGE 0xB0

// the code on the atmega will insert active sense messages every 300ms
// if nothing else was sent. we use this to check if the connection is still alive
#define HEALTHY_TIME 500

std::set<Midi::MidiEvent3> Midi::noteOnHandler;
std::set<Midi::MidiEvent3> Midi::noteOffHandler;
std::set<Midi::MidiEvent3> Midi::controllerChangeHandler;
std::set<Midi::MidiEvent> Midi::connectHandler;
std::set<Midi::MidiEvent1> Midi::systemRealTimeHandler;
bool Midi::started = false;
unsigned long Midi::lastMessageTime = 0;
uint8_t Midi::noteValues[NUMBER_OF_NOTES];
uint8_t Midi::controllerValues[NUMBER_OF_CONTROLLERS];
Midi::Route Midi::route = Usb;

void Midi::Initialize()
{
    // no need to configure it twice.
    if (started)
        return;

#ifdef DEBUGOVERSERIAL
    if (route == Usb)
    {
        Debug.println("USB midi driver is not installed because the debugger is already using this serial port to print debug data. Incoming and outgoing midi data is ignored.");
        started = false;
        return;
    }
#endif

    memset(noteValues, 0, NUMBER_OF_NOTES);
    memset(controllerValues, 0, NUMBER_OF_CONTROLLERS);

    // configure UART for DMX
    uart_config_t uart_config =
        {
            .baud_rate = 31250,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_2,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,

            .rx_flow_ctrl_thresh = 122, // not used
            .use_ref_tick = false       // not used
        };

    ESP_ERROR_CHECK(uart_param_config_iram(MIDI_UART_NUM, &uart_config));

    // Set pins for UART
    if (route == Usb)
    {
        // note that i use uart 2 for midi here, but connect it to pin 1 and 3, the pin where uart0 would normally
        // be. uart 0 can still be used as serial output for debug messages/core dumps by the chip of you route them somewhere else.
        started = uart_set_pin_iram(MIDI_UART_NUM, MIDI_SERIAL_OUTPUT_PIN_USB, MIDI_SERIAL_INPUT_PIN_USB, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) == ESP_OK;
    }
    else if (route == Din)
    {
        // Route to the external midi connector on the front panel
        started = uart_set_pin_iram(MIDI_UART_NUM, MIDI_SERIAL_OUTPUT_PIN_DIN, MIDI_SERIAL_INPUT_PIN_DIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) == ESP_OK;
    }
    if (!started)
        return;

    // install queue
    // even though the documentation says i cannot use ESP_INTR_FLAG_IRAM here, it seems to solve the neopixelOutput flicker issue
    started = uart_driver_install_iram(MIDI_UART_NUM, MIDI_BUF_SIZE * 2, MIDI_BUF_SIZE * 2, 20, &midi_rx_queue, ESP_INTR_FLAG_IRAM) == ESP_OK;
    // started = uart_driver_install(MIDI_UART_NUM, MIDI_BUF_SIZE * 2, MIDI_BUF_SIZE * 2, 20, &midi_rx_queue, 0) == ESP_OK;
    if (!started)
        return;

    // create receive task
    xTaskCreatePinnedToCore(uart_event_task, "Midi", 2048, NULL, 1, NULL, 1);
}

void Midi::setRoute(Route route)
{
    if (started){
        Debug.println("You cannot call Midi::setRoute after initialization.");
        return;
    }
    Midi::route = route;
}

Midi::Route Midi::getRoute()
{
    return route;
}

void Midi::uart_event_task(void *pvParameters)
{
    byte messageposition = 0;
    byte message[4];
    while (true)
    {
        uint8_t data[1];
        int length = uart_read_bytes_iram(MIDI_UART_NUM, data, 1, 1000);

        if (length <= 0)
            continue;

        if (!isConnected())
        {
            for (MidiEvent handler : connectHandler)
                handler();
        }
        lastMessageTime = xTaskGetTickCount();

        if (data[0] >= 0xF8)
        {
            // this is a system realtime message, it is always 1 byte long, and come in between
            // other messages, so they should not reset the messageposition.
            for (auto &&handler : systemRealTimeHandler)
                handler(data[0]);
            continue;
        }

        if (data[0] >= B10000000)
        {
            // detect start of a message, start filling the buffer
            message[0] = data[0];
            messageposition = 1;
        }
        else if (messageposition < sizeof(message))
        {
            // otherwise: store bytes untill buffer is full
            message[messageposition] = data[0];
            messageposition++;
        }

        uint8_t channel = message[0] & 0x0F;
        uint8_t messagetype = message[0] & 0xF0;

        if (messagetype == NOTEON && messageposition == 3 && message[1] < NUMBER_OF_NOTES)
        {
            noteValues[message[1]] = message[2];
            for (auto &&handler : noteOnHandler)
                handler(channel, message[1], message[2]);
        }

        if (messagetype == NOTEOFF && messageposition == 3 && message[1] < NUMBER_OF_NOTES)
        {
            noteValues[message[1]] = 0;
            for (auto &&handler : noteOffHandler)
                handler(channel, message[1], message[2]);
        }

        if (messagetype == CONTROLLERCHANGE && messageposition == 3 && message[1] < NUMBER_OF_CONTROLLERS)
        {
            //Debug.printf("controller %d %d\n", message[1],message[2]);
            controllerValues[message[1]] = message[2];
            for (auto &&handler : controllerChangeHandler)
                handler(channel, message[1], message[2]);
        }
    }
}

void Midi::sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
    if (!started)
        return;
    uint8_t buffer[3];
    buffer[0] = NOTEON | channel;
    buffer[1] = note;
    buffer[2] = velocity;
    uart_write_bytes_iram(MIDI_UART_NUM, (const char *)buffer, sizeof(buffer));
}

void Midi::sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
    if (!started)
        return;
    uint8_t buffer[3];
    buffer[0] = NOTEOFF | channel;
    buffer[1] = note;
    buffer[2] = velocity;
    uart_write_bytes_iram(MIDI_UART_NUM, (const char *)buffer, sizeof(buffer));
}

void Midi::sendControllerChange(uint8_t channel, uint8_t controller, uint8_t value)
{
    if (!started)
        return;
    uint8_t buffer[3];
    buffer[0] = CONTROLLERCHANGE | channel;
    buffer[1] = controller;
    buffer[2] = value;
    uart_write_bytes_iram(MIDI_UART_NUM, (const char *)buffer, sizeof(buffer));
}

bool Midi::waitTxDone(int timeout)
{
    if (!started)
        return false;
    return uart_wait_tx_done_iram(MIDI_UART_NUM, timeout) == ESP_OK;
}

void Midi::onNoteOn(MidiEvent3 handler)
{
    noteOnHandler.insert(handler);
}
void Midi::onNoteOff(MidiEvent3 handler)
{
    noteOffHandler.insert(handler);
}
void Midi::onControllerChange(MidiEvent3 handler)
{
    controllerChangeHandler.insert(handler);
}

void Midi::onConnect(MidiEvent handler)
{
    connectHandler.insert(handler);
}

void Midi::onSystemRealtime(MidiEvent1 handler)
{
    systemRealTimeHandler.insert(handler);
}

bool Midi::isConnected()
{
    if (!started)
        return false;

    if (xTaskGetTickCount() - lastMessageTime < HEALTHY_TIME)
        return true;

    return false;
}

bool Midi::isStarted()
{
    return started;
}

uint8_t Midi::noteStatus(uint8_t notenumber)
{
    if (notenumber >= NUMBER_OF_NOTES)
        return 0;
    return noteValues[notenumber];
}
uint8_t Midi::controllerValue(uint8_t controllernumber)
{
    if (controllernumber >= NUMBER_OF_CONTROLLERS)
        return 0;
    return controllerValues[controllernumber];
}