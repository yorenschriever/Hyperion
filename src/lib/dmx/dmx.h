//based on https://github.com/luksal/ESP32-DMX-RX

#ifndef DMX_h
#define DMX_h

#include <inttypes.h>
#include "driver/uart.h"

class DMX
{
    public:
        static void Initialize();                           // initialize library
        static uint8_t Read(uint16_t channel);              // returns the dmx value for the givven address (values from 1 to 512)
        static uint8_t IsHealthy();                            // returns true, when a valid DMX signal was received within the last 500ms
        static void Write(uint8_t* data, int length, bool wait); 
        
    private:
        DMX();                                              // hide constructor
        static QueueHandle_t dmx_rx_queue;                  // queue for uart rx events
        static SemaphoreHandle_t sync_dmx;                  // semaphore for synchronising access to dmx array
        static uint8_t dmx_state;                           // status, in which received state we are
        static uint16_t current_rx_addr;                    // last received dmx channel
        static long last_dmx_packet;                        // timestamp for the last received packet
        static uint8_t dmx_data[513];                       // stores the received dmx data
        static void uart_event_task(void *pvParameters);    // Event task

};

#endif