#pragma once

#include "Output.h"
#include "debug.h"
#include <Arduino.h>
#include <driver/rmt.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp32-hal.h"
#include "esp_intr_alloc.h"
#include "driver/gpio.h"
#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "freertos/semphr.h"
#include "soc/rmt_struct.h"

#include "esp_log.h"

#ifdef __cplusplus
}
#endif


#define RMT_MAX_CHANNELS 8
#define MAX_PULSES 64      /* A channel has a 64 "pulse" buffer */
#define PULSES_PER_FILL 24 /* One pixel's worth of pulses */

struct TransmissionContext
{
    bool active = false;
    uint8_t *buffer = NULL;
    int length = 0;
    volatile int bufferposition = 0;
    volatile uint32_t *mRMT_mem_ptr = NULL;
    uint16_t mCurPulse = 0;
    uint32_t val1;
    uint32_t val0;
    uint32_t RmtDurationReset = 0;
};

static intr_handle_t gRMT_intr_handle = NULL;
TransmissionContext transmissionContext[RMT_MAX_CHANNELS];

//Writes the ledata to the octows2811 style connector on the front panel.
//It used esp32's RMT peripheral to do this.
//I used parts of NeoPixelBus and Fastled to get the flexibility and performance i need
template <typename T_SPEED>
class NeopixelOutput : public Output
{
public:
    const int Orange1 = 1;
    const int Blue1 = 2;
    const int Green1 = 3;
    const int Brown1 = 4;
    const int Orange2 = 5;
    const int Blue2 = 6;
    const int Green2 = 7;
    const int Brown2 = 8;

    //channel goes from 1-8
    NeopixelOutput(int channel)
    {
        this->channel = channel;

        //initialize with 3 pixels by default
        this->length = 12;
        buffer = (uint8_t *)malloc(12);
        frontBuffer = (uint8_t *)malloc(12);
    }

    //index and size are in bytes
    void SetData(uint8_t *data, int size, int index)
    {
        int copylength = min(size, length - index);
        if (copylength > 0)
            memcpy(this->buffer + index, data, copylength);
    }

    boolean Ready()
    {
        return !transmissionContext[rmtchannel].active;
    }

    void Show()
    {
        //check if we are ready to send, or
        if (transmissionContext[rmtchannel].active)
            return;

        // now start the RMT transmit with the editing buffer before we swap
        startRMT(buffer, length);

        // swap so the user can modify without affecting the async operation
        std::swap(buffer, frontBuffer);
    }

    void Begin() override
    {
        if (channel < 1 || channel > RMT_MAX_CHANNELS)
        {
            Debug.printf("Invalid neopixel channel: %d", channel);
            return;
        }

        //there are 8 outputs and 8 rmt channels, so this mapping is trivial
        rmtchannel = static_cast<rmt_channel_t>(channel - 1);

        rmt_config_t config;

        config.rmt_mode = RMT_MODE_TX;
        config.channel = rmtchannel;
        config.gpio_num = static_cast<gpio_num_t>(pins[channel - 1]);
        config.mem_block_num = 1;
        config.tx_config.loop_en = false;

        config.tx_config.idle_output_en = true;
        config.tx_config.idle_level = T_SPEED::IdleLevel;

        config.tx_config.carrier_en = false;
        config.tx_config.carrier_level = RMT_CARRIER_LEVEL_LOW;

        config.clk_div = T_SPEED::RmtClockDivider;

        ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_config(&config));
        ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_set_tx_thr_intr_en(rmtchannel, true, PULSES_PER_FILL));
        if (gRMT_intr_handle == NULL)
            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_intr_alloc(ETS_RMT_INTR_SOURCE, ESP_INTR_FLAG_LEVEL3, interruptHandler, 0, &gRMT_intr_handle));
    }

    void Clear()
    {
        memset(this->buffer, 0, this->length);
    }

    //length is in bytes
    void SetLength(int len) override
    {
        if (len != this->length)
        {
            //wait for the frontbuffer to be sent before we are going to change its size
            while (!Ready())
                delay(1);

            buffer = (uint8_t *)realloc(buffer, len);
            frontBuffer = (uint8_t *)realloc(frontBuffer, len);

            memset(frontBuffer, 0x00, len);

            this->length = len;
        }
    }

private:
    int channel = 1;
    rmt_channel_t rmtchannel;
    int length;

    uint8_t *buffer;      //data we are receiving / editing
    uint8_t *frontBuffer; //data we are sending

    //the way the project is currently setup we dont need 2 buffers.
    //because we wait for (Ready) before sending new data

    void IRAM_ATTR startRMT(uint8_t *data, int len)
    {
        transmissionContext[rmtchannel].active = true;
        transmissionContext[rmtchannel].buffer = data;
        transmissionContext[rmtchannel].length = len;
        transmissionContext[rmtchannel].bufferposition = 0;
        transmissionContext[rmtchannel].mRMT_mem_ptr = &(RMTMEM.chan[rmtchannel].data32[0].val);
        transmissionContext[rmtchannel].mCurPulse = 0;
        transmissionContext[rmtchannel].val0 = T_SPEED::RmtBit0;
        transmissionContext[rmtchannel].val1 = T_SPEED::RmtBit1;
        transmissionContext[rmtchannel].RmtDurationReset = T_SPEED::RmtDurationReset;

        //fill the buffer a bit. currenty fillnext will add 3 bytes of pixel data to the pulse buffer.
        //the pulse buffer can hold 8 bytes of pulse data in total
        fillNext(rmtchannel);
        fillNext(rmtchannel);

        rmt_set_tx_intr_en(rmtchannel, true);
        rmt_tx_start(rmtchannel, true);
    }

    static void IRAM_ATTR interruptHandler(void *arg)
    {
        // -- The basic structure of this code is borrowed from the
        //    interrupt handler in esp-idf/components/driver/rmt.c
        uint32_t intr_st = RMT.int_st.val;
        uint8_t channel;

        for (channel = 0; channel < RMT_MAX_CHANNELS; channel++)
        {
            int tx_done_bit = channel * 3;
            int tx_next_bit = channel + 24;

            if (transmissionContext[channel].active)
            {

                // -- More to send on this channel
                if (intr_st & BIT(tx_next_bit))
                {
                    RMT.int_clr.val |= BIT(tx_next_bit);
                    fillNext(channel);
                }
                else
                {
                    // -- Transmission is complete on this channel
                    if (intr_st & BIT(tx_done_bit))
                    {
                        RMT.int_clr.val |= BIT(tx_done_bit);
                        transmissionContext[channel].active = false;
                        //rmt_tx_stop((rmt_channel_t)channel); //todo doesnt it stop automatically?
                    }
                }
            }
        }
    }

    static void IRAM_ATTR fillNext(uint8_t channel)
    {
        TransmissionContext* tc = &transmissionContext[channel];
        uint32_t one_val = tc->val1;
        uint32_t zero_val = tc->val0;

        //fill 3 bytes, or less if the end of the buffer is reached
        //we have the memory to do this 4 at a time, but i copied the concept
        //from fastled, and they use 3 (to align with their pixel data)
        //i could test it with 4 someday
        int filltill = std::min(tc->bufferposition + 3, tc->length);

        while (tc->bufferposition < filltill)
        {
            // -- Use locals for speed
            volatile register uint32_t *pItem = tc->mRMT_mem_ptr;
            register uint16_t curPulse = tc->mCurPulse;

            register uint8_t pixel = tc->buffer[tc->bufferposition++];
            //transmissionContext[channel].bufferposition++;

            // Shift bits out, MSB first, setting RMTMEM.chan[n].data32[x] to the
            // rmt_item32_t value corresponding to the buffered bit value
            for (register uint32_t j = 0; j < 8; j++)
            {
                uint32_t val = (pixel & B10000000) ? one_val : zero_val;
                *pItem++ = val;// Replaces: RMTMEM.chan[mRMT_channel].data32[mCurPulse].val = val;
                pixel <<= 1;
            }

            curPulse += 8;
            if (curPulse == MAX_PULSES)
            {
                pItem = &(RMTMEM.chan[channel].data32[0].val);
                curPulse = 0;
            }

            // -- Store the new values back into the object
            tc->mCurPulse = curPulse;
            tc->mRMT_mem_ptr = pItem;
        }

        if (tc->bufferposition == tc->length)
        {
            //set the last bit time to the break time
            //the pulse buffer pointer can be at the start, in that case the last pulse it at the end of the buffer
            if (tc->mRMT_mem_ptr == &(RMTMEM.chan[channel].data32[0].val))
                ((rmt_item32_t *)&RMTMEM.chan[channel].data32[MAX_PULSES - 1].val)->duration1 = tc->RmtDurationReset;
            else 
                ((rmt_item32_t *)transmissionContext[channel].mRMT_mem_ptr - 1)->duration1 = tc->RmtDurationReset;
            
            // -- No more data; signal to the RMT we are done
            //we are at least 8 pulses from the end of the buffer, otherwise we would have wrapped already
            for (uint32_t j = 0; j < 8; j++)
            {
                *tc->mRMT_mem_ptr++ = 0; //todo handle overflow
            }
        }
    }

};

class NeoEsp32RmtSpeed
{
public:
    // ClkDiv of 2 provides for good resolution and plenty of reset resolution; but
    // a ClkDiv of 1 will provide enough space for the longest reset and does show
    // little better pulse accuracy
    const static DRAM_ATTR uint8_t RmtClockDivider = 2;

    inline constexpr static IRAM_ATTR uint32_t FromNs(uint32_t ns)
    {
        return ns / NsPerRmtTick;
    }

protected:
    const static DRAM_ATTR uint32_t RmtCpu = 80000000L; // 80 mhz RMT clock
    const static DRAM_ATTR uint32_t NsPerSecond = 1000000000L;
    const static DRAM_ATTR uint32_t RmtTicksPerSecond = (RmtCpu / RmtClockDivider);
    const static DRAM_ATTR uint32_t NsPerRmtTick = (NsPerSecond / RmtTicksPerSecond); // about 25
};

class NeoEsp32RmtSpeedBase : public NeoEsp32RmtSpeed
{
public:
    // this is used rather than the rmt_item32_t as you can't correctly initialize
    // it as a static constexpr within the template
    inline constexpr static IRAM_ATTR uint32_t Item32Val(uint16_t nsHigh, uint16_t nsLow)
    {
        return (FromNs(nsLow) << 16) | (1 << 15) | (FromNs(nsHigh));
    }

    const static DRAM_ATTR rmt_idle_level_t IdleLevel = RMT_IDLE_LEVEL_LOW;
};

class NeoEsp32RmtSpeedWs2811 : public NeoEsp32RmtSpeedBase
{
public:
    const static DRAM_ATTR uint32_t RmtBit0 = Item32Val(300, 950);
    const static DRAM_ATTR uint32_t RmtBit1 = Item32Val(900, 350);
    const static DRAM_ATTR uint16_t RmtDurationReset = FromNs(300000); // 300us
};

class Ws2812x : public NeoEsp32RmtSpeedBase
{
public:
    const static DRAM_ATTR uint32_t RmtBit0 = Item32Val(400, 850);
    const static DRAM_ATTR uint32_t RmtBit1 = Item32Val(800, 450);
    const static DRAM_ATTR uint16_t RmtDurationReset = FromNs(300000); // 300us
};

class Sk6812 : public NeoEsp32RmtSpeedBase
{
public:
    const static DRAM_ATTR uint32_t RmtBit0 = Item32Val(400, 850);
    const static DRAM_ATTR uint32_t RmtBit1 = Item32Val(800, 450);
    const static DRAM_ATTR uint16_t RmtDurationReset = FromNs(80000); // 80us
};

// // normal is inverted signal
// class NeoEsp32RmtSpeedTm1814 : public NeoEsp32RmtInvertedSpeedBase
// {
// public:
//     const static uint32_t RmtBit0 = Item32Val(360, 890);
//     const static uint32_t RmtBit1 = Item32Val(720, 530);
//     const static uint16_t RmtDurationReset = FromNs(200000); // 200us
// };

class Kpbs800 : public NeoEsp32RmtSpeedBase
{
public:
    const static DRAM_ATTR uint32_t RmtBit0 = Item32Val(400, 850);
    const static DRAM_ATTR uint32_t RmtBit1 = Item32Val(800, 450);
    const static DRAM_ATTR uint16_t RmtDurationReset = FromNs(50000); // 50us
};

class Kbps400 : public NeoEsp32RmtSpeedBase
{
public:
    const static DRAM_ATTR uint32_t RmtBit0 = Item32Val(800, 1700);
    const static DRAM_ATTR uint32_t RmtBit1 = Item32Val(1600, 900);
    const static DRAM_ATTR uint16_t RmtDurationReset = FromNs(50000); // 50us
};

class Apa106 : public NeoEsp32RmtSpeedBase
{
public:
    const static DRAM_ATTR uint32_t RmtBit0 = Item32Val(400, 1250);
    const static DRAM_ATTR uint32_t RmtBit1 = Item32Val(1250, 400);
    const static DRAM_ATTR uint16_t RmtDurationReset = FromNs(50000); // 50us
};