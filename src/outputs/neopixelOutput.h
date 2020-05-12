#pragma once

#include "Output.h"
#include "debug.h"
#include <Arduino.h>
#include <driver/rmt.h>

//mapping of esps gpio pins to channel 1-8 on the front panel
const int pins[] = {5,4,14,2,15,32,0,33};

//Based on NeoPixelBus

template<typename T_SPEED>
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
        this->channel=channel;

        //initialize with 3 pixels by default
        this->length=12;
        buffer = (uint8_t*) malloc(12);
        frontBuffer = (uint8_t*) malloc(12);
    }

    //index and size are in bytes
    void SetData(uint8_t *data, int size, int index)
    {
        int copylength = min(size, length - index);
        if (copylength>0)
            memcpy(this->buffer + index, data, copylength);
    }

    boolean Ready()
    {
        return (ESP_OK == rmt_wait_tx_done(rmtchannel, 0));
    }

    void Show()
    {
        //check if we are ready to send, or 
        if (ESP_OK != rmt_wait_tx_done(rmtchannel, 0))
            return;

        // now start the RMT transmit with the editing buffer before we swap
        ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_write_sample(rmtchannel, buffer, length, false));

        // swap so the user can modify without affecting the async operation
        std::swap(buffer, frontBuffer);
    }

    void Begin() override
    {
        if (channel < 1 || channel > 8){
            Debug.printf("Invalid neopixel channel: %d",channel);
            return;
        }

        //there are 8 outputs and 8 rmt channels, so this mapping is trivial
        rmtchannel = static_cast<rmt_channel_t>(channel-1);

        rmt_config_t config;

        config.rmt_mode = RMT_MODE_TX;
        config.channel = rmtchannel;
        config.gpio_num = static_cast<gpio_num_t>(pins[channel-1]);
        config.mem_block_num = 1;
        config.tx_config.loop_en = false;
        
        config.tx_config.idle_output_en = true;
        config.tx_config.idle_level = T_SPEED::IdleLevel;

        config.tx_config.carrier_en = false;
        config.tx_config.carrier_level = RMT_CARRIER_LEVEL_LOW;

        config.clk_div = T_SPEED::RmtClockDivider;

        ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_config(&config));
        ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_driver_install(rmtchannel, 0, ESP_INTR_FLAG_IRAM));
        ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_translator_init(rmtchannel, _translate));
    }

    void Clear()
    {
        memset(this->buffer,0,this->length);
    }

    //length is in bytes
    void SetLength(int len) override
    {
        if (len != this->length){
            //wait for the frontbuffer to be sent before we are going to change its size
            while(!Ready())
                delay(1);

            buffer = (uint8_t*) realloc(buffer,len);
            frontBuffer = (uint8_t*) realloc(frontBuffer,len);

            memset(frontBuffer, 0x00, len);

            this->length = len;
        }
        //return len;
    }

private:
    int channel=1;
    rmt_channel_t rmtchannel;
    int length;

    uint8_t* buffer;  //data we are receiving / editing
    uint8_t* frontBuffer; //data we are sending
    
    static bool driverInstalled[8];

    //the way the project is currently setup we dont need 2 buffers.
    //because we wait for (Ready) before sending new data

    // stranslate NeoPixelBuffer into RMT buffer
    // this is done on the fly so we don't require a send buffer in raw RMT format
    // which would be 32x larger than the primary buffer
    static void IRAM_ATTR _translate(const void* src,
        rmt_item32_t* dest,
        size_t src_size,
        size_t wanted_num,
        size_t* translated_size,
        size_t* item_num)
    {
        if (src == NULL || dest == NULL) 
        {
            *translated_size = 0;
            *item_num = 0;
            return;
        }

        size_t size = 0;
        size_t num = 0;
        const uint8_t* psrc = static_cast<const uint8_t*>(src);
        rmt_item32_t* pdest = dest;

        for (;;)
        {
            uint8_t data = *psrc;

            for (uint8_t bit = 0; bit < 8; bit++)
            {
                pdest->val = (data & 0x80) ? T_SPEED::RmtBit1 : T_SPEED::RmtBit0;
                pdest++;
                data <<= 1;
            }
            num += 8;
            size++;

            // if this is the last byte we need to adjust the length of the last pulse
            if (size >= src_size)
            {
                // extend the last bits LOW value to include the full reset signal length
                pdest--;
                pdest->duration1 = T_SPEED::RmtDurationReset;
                // and stop updating data to send
                break; 
            }

            if (num >= wanted_num)
            {
                // stop updating data to send
                break;
            }

            psrc++;
        }

        *translated_size = size;
        *item_num = num;
    }
};


class NeoEsp32RmtSpeed
{
public:
    // ClkDiv of 2 provides for good resolution and plenty of reset resolution; but
    // a ClkDiv of 1 will provide enough space for the longest reset and does show
    // little better pulse accuracy
    const static uint8_t RmtClockDivider = 2; 

    inline constexpr static uint32_t FromNs(uint32_t ns)
    {
        return ns / NsPerRmtTick;
    }


protected:
    const static uint32_t RmtCpu = 80000000L; // 80 mhz RMT clock
    const static uint32_t NsPerSecond = 1000000000L;
    const static uint32_t RmtTicksPerSecond = (RmtCpu / RmtClockDivider);
    const static uint32_t NsPerRmtTick = (NsPerSecond / RmtTicksPerSecond); // about 25 
};

class NeoEsp32RmtSpeedBase : public NeoEsp32RmtSpeed
{
public:
	// this is used rather than the rmt_item32_t as you can't correctly initialize
    // it as a static constexpr within the template
	inline constexpr static uint32_t Item32Val(uint16_t nsHigh, uint16_t nsLow)
	{
		return (FromNs(nsLow) << 16) | (1 << 15) | (FromNs(nsHigh));
	}

	const static rmt_idle_level_t IdleLevel = RMT_IDLE_LEVEL_LOW;
};

class NeoEsp32RmtSpeedWs2811 : public NeoEsp32RmtSpeedBase
{
public:
    const static uint32_t RmtBit0 = Item32Val(300, 950); 
    const static uint32_t RmtBit1 = Item32Val(900, 350); 
    const static uint16_t RmtDurationReset = FromNs(300000); // 300us
};

class Ws2812x : public NeoEsp32RmtSpeedBase
{
public:
	const static uint32_t RmtBit0 = Item32Val(400, 850);
	const static uint32_t RmtBit1 = Item32Val(800, 450);
	const static uint16_t RmtDurationReset = FromNs(300000); // 300us
};

class Sk6812 : public NeoEsp32RmtSpeedBase
{
public:
    const static uint32_t RmtBit0 = Item32Val(400, 850); 
    const static uint32_t RmtBit1 = Item32Val(800, 450); 
    const static uint16_t RmtDurationReset = FromNs(80000); // 80us
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
    const static uint32_t RmtBit0 = Item32Val(400, 850); 
    const static uint32_t RmtBit1 = Item32Val(800, 450); 
    const static uint16_t RmtDurationReset = FromNs(50000); // 50us
};

class Kbps400 : public NeoEsp32RmtSpeedBase
{
public:
    const static uint32_t RmtBit0 = Item32Val(800, 1700); 
    const static uint32_t RmtBit1 = Item32Val(1600, 900); 
    const static uint16_t RmtDurationReset = FromNs(50000); // 50us
};

class Apa106 : public NeoEsp32RmtSpeedBase
{
public:
	const static uint32_t RmtBit0 = Item32Val(400, 1250);
	const static uint32_t RmtBit1 = Item32Val(1250, 400);
	const static uint16_t RmtDurationReset = FromNs(50000); // 50us
};