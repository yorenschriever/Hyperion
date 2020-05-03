#include "dmx.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "strings.h"
#include <cstring>
#include <algorithm>
#include "debug.h"

#define DMX_SERIAL_INPUT_PIN    36          // pin for dmx rx
#define DMX_SERIAL_OUTPUT_PIN   32          // pin for dmx tx
#define DMX_UART_NUM            UART_NUM_1  // dmx uart

#define HEALTHY_TIME            500         // timeout in ms 

#define BUF_SIZE                1024        //  buffer size for rx events

#define DMX_IDLE                    0
#define DMX_BREAK                   1
#define DMX_DATA                    2



QueueHandle_t DMX::dmx_rx_queue;
SemaphoreHandle_t DMX::sync_dmx;
uint8_t DMX::dmx_state = DMX_IDLE;
uint16_t DMX::current_rx_addr = 0;
long DMX::last_dmx_packet = 0;
uint8_t DMX::dmx_data[513];
volatile unsigned int DMX::frameCount=0;

const int universeSize = 512;
bool DMX::fullframe = true; //always send the entire universe, instead of only the channels that were provided from the input
uint8_t DMX::dmx_tx_buffer[512];
uint8_t DMX::dmx_tx_frontbuffer[512];
xSemaphoreHandle DMX::tx_dirtySemaphore = xSemaphoreCreateBinary();
volatile bool DMX::tx_busy = false;
volatile int DMX::tx_size=0;
int DMX::minchannels = 0;
int DMX::trailingchannels = 0;

bool DMX::initialized = false;

DMX::DMX()
{

}

void DMX::Initialize()
{
    if (initialized)
        return;

    // configure UART for DMX
    uart_config_t uart_config =
    {
        .baud_rate = 250000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_2,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,

        .rx_flow_ctrl_thresh = 122, //not used
        .use_ref_tick = false //not used
    };

    ESP_ERROR_CHECK(uart_param_config(DMX_UART_NUM, &uart_config));

    // Set pins for UART
    uart_set_pin(DMX_UART_NUM, DMX_SERIAL_OUTPUT_PIN, DMX_SERIAL_INPUT_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // install queue
    uart_driver_install(DMX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &dmx_rx_queue, 0);

    // create receive task
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);

    // create mutex for syncronisation
    sync_dmx = xSemaphoreCreateMutex();

    //related to sending
    xTaskCreatePinnedToCore(SendDMXAsync, "SendDMXAsync", 10000, NULL, 1, NULL, 1);

    initialized=true;
}

uint8_t DMX::Read(uint16_t channel)
{
    // restrict acces to dmx array to valid values
    if(channel < 1)
    {
        channel = 1;
    }
    else if(channel > 512)
    {
        channel = 512;
    }
    // take data threadsafe from array and return
    xSemaphoreTake(sync_dmx, portMAX_DELAY);
    uint8_t tmp_dmx = dmx_data[channel];
    xSemaphoreGive(sync_dmx);
    return tmp_dmx;
}

uint8_t DMX::IsHealthy()
{
    // get timestamp of last received packet
    xSemaphoreTake(sync_dmx, portMAX_DELAY);
    long dmx_timeout = last_dmx_packet;
    xSemaphoreGive(sync_dmx);
    // check if elapsed time < defined timeout
    
    if(xTaskGetTickCount() - dmx_timeout < HEALTHY_TIME)
    {
        return 1;
    }
    return 0;
}

void DMX::uart_event_task(void *pvParameters)
{
    uart_event_t event;
    uint8_t* dtmp = (uint8_t*) malloc(BUF_SIZE);
    for(;;)
    {
        // wait for data in the dmx_queue
        if(xQueueReceive(dmx_rx_queue, (void * )&event, (portTickType)portMAX_DELAY))
        {
            bzero(dtmp, BUF_SIZE);
            switch(event.type)
            {
                case UART_DATA:
                    // read the received data
                    uart_read_bytes(DMX_UART_NUM, dtmp, event.size, portMAX_DELAY);
                    // check if break detected
                    if(dmx_state == DMX_BREAK)
                    {
                        // if not 0, then RDM or custom protocol
                        if(dtmp[0] == 0)
                        {
                        dmx_state = DMX_DATA;
                        // reset dmx adress to 0
                        current_rx_addr = 0;
                        xSemaphoreTake(sync_dmx, portMAX_DELAY);
                        // store received timestamp
                        last_dmx_packet = xTaskGetTickCount();

                        frameCount++;
                        xSemaphoreGive(sync_dmx);
                        }
                    }
                    // check if in data receive mode
                    if(dmx_state == DMX_DATA)
                    {
                        xSemaphoreTake(sync_dmx, portMAX_DELAY);
                        // copy received bytes to dmx data array
                        for(int i = 0; i < event.size; i++)
                        {
                            if(current_rx_addr < 513)
                            {
                                dmx_data[current_rx_addr++] = dtmp[i];
                            }
                        }
                        xSemaphoreGive(sync_dmx);
                    }
                    break;
                case UART_BREAK:
                    // break detected
                    // clear queue und flush received bytes                    
                    uart_flush_input(DMX_UART_NUM);
                    xQueueReset(dmx_rx_queue);
                    dmx_state = DMX_BREAK;
                    break;
                case UART_FRAME_ERR:
                case UART_PARITY_ERR:
                case UART_BUFFER_FULL:
                case UART_FIFO_OVF:
                default:
                    // error recevied, going to idle mode
                    uart_flush_input(DMX_UART_NUM);
                    xQueueReset(dmx_rx_queue);
                    dmx_state = DMX_IDLE;
                    break;
            }
        }
    }
}

int DMX::GetFrameNumber()
{
    return frameCount;
}

uint8_t* DMX::GetDataPtr()
{
    return dmx_data;
}

//portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;


//================ send

void DMX::Write(uint8_t* data, int size, int index)
{
    int copylength = std::min(size, universeSize - index);
    if (copylength>0)
        memcpy(dmx_tx_buffer + index, data, copylength);

    tx_size = std::max((int)tx_size, index + copylength);
}

void DMX::SendDMXAsync(void *param)
{
    while (true)
    {
        if (!xSemaphoreTake(tx_dirtySemaphore, portMAX_DELAY))
            continue;

        //vTaskDelay(5 / portTICK_PERIOD_MS);

        tx_busy = true;


        int frontBufferLen = universeSize;
        if (!fullframe) {
            frontBufferLen = min(max(minchannels,tx_size+trailingchannels),universeSize);
        }
        memcpy(dmx_tx_frontbuffer, dmx_tx_buffer, frontBufferLen);
        SendBuffer(dmx_tx_frontbuffer,frontBufferLen);
        tx_size=0;

        tx_busy = false;

    }
}

void DMX::SendBuffer(uint8_t* buf, int size)
{
    //if (wait)
      uart_wait_tx_done(DMX_UART_NUM, 500); 

    const char nullBuf = 0;
    uart_write_bytes_with_break(DMX_UART_NUM, &nullBuf,1, 25); //break time is in bit length
  
    //https://github.com/espressif/esp-idf/issues/703
    // this solution is only making it worse. somehow it changes some butes (eg channel 10 is always off)
    // uart_wait_tx_done(DMX_UART_NUM,100);
    // taskENTER_CRITICAL(&myMutex);
    // uart_set_line_inverse(DMX_UART_NUM, UART_INVERSE_TXD);
    // ets_delay_us(92);
    // uart_set_line_inverse(DMX_UART_NUM, UART_INVERSE_DISABLE);
    // ets_delay_us(12);
    // uart_write_bytes(DMX_UART_NUM, (const char*) data, 10);
    // taskEXIT_CRITICAL(&myMutex);
    
    uart_write_bytes(DMX_UART_NUM,  &nullBuf, 1); //send the start byte (0)
    uart_write_bytes(DMX_UART_NUM, (const char*) buf, size);
    //if (wait)
    //{
        vTaskDelay(5 / portTICK_PERIOD_MS); //wait a little the the tx has time to start https://www.esp32.com/viewtopic.php?t=10469 
        uart_wait_tx_done(DMX_UART_NUM, 500); //wait till completed. at most 100 RTOS ticks (=100ms?)
        //vTaskDelay(1 / portTICK_PERIOD_MS); //wait an additional inter frame period
    //}
}

void DMX::Show()
{
    xSemaphoreGive(tx_dirtySemaphore);
}

bool DMX::TxBusy(){
    return tx_busy;
}

void DMX::ClearTxBuffer()
{
    memset(dmx_tx_buffer,0,universeSize);
}

void DMX::SendFullFrame(bool ff){
    fullframe = ff;
}
void DMX::SetUniverseSize(int minchannels,int trailingchannels){
    DMX::minchannels = minchannels;
    DMX::trailingchannels = trailingchannels;
}