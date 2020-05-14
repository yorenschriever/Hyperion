#ifndef DMX_h
#define DMX_h

#include <inttypes.h>
#include "driver/uart.h"

//The DMX reception code is based on
//based on https://github.com/luksal/ESP32-DMX-RX
//I added the functions to send DMX.
class DMX
{
public:
    static void Initialize();              // initialize library
    
    static uint8_t Read(uint16_t channel); // returns the dmx value for the givven address (values from 1 to 512)
    static uint8_t IsHealthy();            // returns true, when a valid DMX signal was received within the last 500ms
    static int GetFrameNumber();           // get the number of frames received
    static uint8_t *GetDataPtr();          // get a pointer to the incoming databuffer

    static void Write(uint8_t *data, int len, int index); //write to the dmx output buffer. After all data is written, use Show() to send out the frame.
    static void Show();                                   // sends the dmx frame
    static bool TxBusy();
    static void ClearTxBuffer();
    static void SendFullFrame(bool);                                // always send all 512, or stop after the last one
    static void SetUniverseSize(int minsize, int trailingchannels); //if not sending full frame you can set a minimum frame size here.

private:
    DMX();                                           // hide constructor
    static QueueHandle_t dmx_rx_queue;               // queue for uart rx events
    static SemaphoreHandle_t sync_dmx;               // semaphore for synchronising access to dmx array
    static uint8_t dmx_state;                        // status, in which received state we are
    static uint16_t current_rx_addr;                 // last received dmx channel
    static long last_dmx_packet;                     // timestamp for the last received packet
    static uint8_t dmx_data[513];                    // stores the received dmx data
    static void uart_event_task(void *pvParameters); // Event task
    volatile static unsigned int frameCount;

    static void SendDMXAsync(void *param);          //Task to send DMX
    static void SendBuffer(uint8_t *buf, int size); //artual send function
    static xSemaphoreHandle tx_dirtySemaphore;
    static uint8_t dmx_tx_buffer[512], dmx_tx_frontbuffer[512];
    static volatile bool tx_busy;
    static volatile int tx_size;
    static bool fullframe;
    static int minchannels;
    static int trailingchannels;

    static bool initialized;
};

#endif