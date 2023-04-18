#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/param.h>
#include "esp_attr.h"
#include "esp32/clk.h"
#include "soc/wdev_reg.h"
#include "freertos/FreeRTOSConfig.h"
#include "xtensa/core-macros.h"
#include "debug.h"

/*
the built-in random() crashes sometimes, because esp_clk_apb_freq()==0 and
a division by 0 exception is raised.
here is check for that contdition.
*/

uint32_t IRAM_ATTR esp_randomStable(void)
{
    /* The PRNG which implements WDEV_RANDOM register gets 2 bits
     * of extra entropy from a hardware randomness source every APB clock cycle
     * (provided WiFi or BT are enabled). To make sure entropy is not drained
     * faster than it is added, this function needs to wait for at least 16 APB
     * clock cycles after reading previous word. This implementation may actually
     * wait a bit longer due to extra time spent in arithmetic and branch statements.
     *
     * As a (probably unncessary) precaution to avoid returning the
     * RNG state as-is, the result is XORed with additional
     * WDEV_RND_REG reads while waiting.
     */

    /* This code does not run in a critical section, so CPU frequency switch may
     * happens while this code runs (this will not happen in the current
     * implementation, but possible in the future). However if that happens,
     * the number of cycles spent on frequency switching will certainly be more
     * than the number of cycles we need to wait here.
     */
    int esp_clk_apb_freq_val = esp_clk_apb_freq();
    if (esp_clk_apb_freq_val == 0)
        esp_clk_apb_freq_val = 1;
    uint32_t cpu_to_apb_freq_ratio = esp_clk_cpu_freq() / esp_clk_apb_freq_val;

    static uint32_t last_ccount = 0;
    uint32_t ccount;
    uint32_t result = 0;
    do
    {
        ccount = XTHAL_GET_CCOUNT();
        result ^= REG_READ(WDEV_RND_REG);
    } while (ccount - last_ccount < cpu_to_apb_freq_ratio * 16);
    last_ccount = ccount;
    return result ^ REG_READ(WDEV_RND_REG);
}

long stableRandom(long howbig)
{
    uint32_t x = esp_randomStable();
    uint64_t m = uint64_t(x) * uint64_t(howbig);
    uint32_t l = uint32_t(m);
    if (l < howbig)
    {
        uint32_t t = -howbig;
        if (t >= howbig)
        {
            t -= howbig;
            if (t >= howbig)
                t %= howbig;
        }
        while (l < t)
        {
            x = esp_randomStable();
            m = uint64_t(x) * uint64_t(howbig);
            l = uint32_t(m);
        }
    }
    return m >> 32;
}

float stableRandomFloat()
{
    const unsigned int max = 10000;
    return ((float)stableRandom(max)) / max;
}
