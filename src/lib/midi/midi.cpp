#include "midi.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "debug.h"

//https://www.midi.org/specifications/item/table-1-summary-of-midi-message

QueueHandle_t Midi::midi_rx_queue;

#define NOTEON 0x90
#define NOTEOFF 0x80
#define CONTROLLERCHANGE 0xB0

//the code on the atmega will insert active sense messages every 300ms
//if nothing else was sent. we use this to check if the connection is still alive
#define HEALTHY_TIME 500

Midi::MidiEvent3 Midi::noteOnHandler=NULL;
Midi::MidiEvent3 Midi::noteOffHandler=NULL;
Midi::MidiEvent3 Midi::controllerChangeHandler=NULL;
Midi::MidiEvent Midi::connectHandler=NULL;
bool Midi::started=false;
unsigned long Midi::lastMessageTime=0;


void Midi::Initialize()
{

    #ifdef DEBUGSERIAL
        Debug.println("USB midi driver is not installed because the debugger is already using this serial port to print debug data. Incoming and outgoing midi data is ignored.");
        started=false;
        return;
    #endif

    //no need to configure it twice.
    if (started)
        return;

    // configure UART for DMX
    uart_config_t uart_config =
    {
        .baud_rate = 31250,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_2,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,

        .rx_flow_ctrl_thresh = 122, //not used
        .use_ref_tick = false //not used
    };

    ESP_ERROR_CHECK(uart_param_config(MIDI_UART_NUM, &uart_config));

    // Set pins for UART
    //uart_set_pin(MIDI_UART_NUM, MIDI_SERIAL_OUTPUT_PIN, MIDI_SERIAL_INPUT_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // install queue
    started = uart_driver_install(MIDI_UART_NUM, MIDI_BUF_SIZE * 2, MIDI_BUF_SIZE * 2, 20, &midi_rx_queue, 0 ) == ESP_OK;
    if (!started)
        return;

    // create receive task
    xTaskCreatePinnedToCore(uart_event_task, "Midi", 2048, NULL, 1, NULL, 1);
}

void Midi::uart_event_task(void *pvParameters)
{
    byte messageposition=0;
    byte message[4];
    while(true)
    {
        delay(5);
        uint8_t data[MIDI_BUF_SIZE];
        int length = 0;
        ESP_ERROR_CHECK(uart_get_buffered_data_len(MIDI_UART_NUM, (size_t*)&length));
        if (length==0)
            continue;

        length = uart_read_bytes(MIDI_UART_NUM, data, length, 100);

        if (length>0)
        {
            if (!isConnected() && connectHandler)
                connectHandler();
            lastMessageTime = xTaskGetTickCount();
        }

        for(int i=0; i<length; i++){
            if (data[i] >= B10000000){ //detect start of a message, start filling the buffer
                message[0] = data[i];
                messageposition=1;
            } else if (messageposition < sizeof(message)){ //otherwise: store bytes untill buffer is full
                message[messageposition] = data[i];
                messageposition++;
            }

            uint8_t channel = message[0] & 0x0F;
            uint8_t messagetype = message[0] & 0xF0;

            if (messagetype == NOTEON && messageposition==3 && noteOnHandler)
                noteOnHandler(channel, message[1], message[2]);

            if (messagetype == NOTEOFF && messageposition==3 && noteOffHandler)
                noteOffHandler(channel, message[1], message[2]);

            if (messagetype == CONTROLLERCHANGE && messageposition==3 && controllerChangeHandler)
                controllerChangeHandler(channel, message[1], message[2]);
        }
    }

}

void Midi::sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity){
    if (!started)
        return;
    uint8_t buffer[3];
    buffer[0] = NOTEON | channel;
    buffer[1] = note;
    buffer[2] = velocity;
    uart_write_bytes(MIDI_UART_NUM, (const char*)buffer, sizeof(buffer));
}

void Midi::sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity){
    if (!started)
        return;
    uint8_t buffer[3];
    buffer[0] = NOTEOFF | channel;
    buffer[1] = note;
    buffer[2] = velocity;
    uart_write_bytes(MIDI_UART_NUM, (const char*)buffer, sizeof(buffer));
}

void Midi::sendControllerChange(uint8_t channel, uint8_t controller, uint8_t value){
    if (!started)
        return;
    uint8_t buffer[3];
    buffer[0] = CONTROLLERCHANGE | channel;
    buffer[1] = controller;
    buffer[2] = value;
    uart_write_bytes(MIDI_UART_NUM, (const char*)buffer, sizeof(buffer));
}

bool Midi::waitTxDone(int timeout)
{
    if (!started)
        return false;
    return uart_wait_tx_done(MIDI_UART_NUM, timeout) == ESP_OK;
}

void Midi::onNoteOn(MidiEvent3 handler){
    noteOnHandler = handler;
}
void Midi::onNoteOff(MidiEvent3 handler){
    noteOffHandler = handler;
}
void Midi::onControllerChange(MidiEvent3 handler){
    controllerChangeHandler = handler;
}

void Midi::onConnect(MidiEvent handler){
    connectHandler = handler;
}

bool Midi::isConnected(){
    if (!started)
        return false;

    if(xTaskGetTickCount() - lastMessageTime < HEALTHY_TIME)
        return true;

    return false;
}