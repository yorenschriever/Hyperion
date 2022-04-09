#pragma once

#include <inttypes.h>
#include "esp32-hal-spi.h"
#include <driver/spi_master.h>
#include <set>

class SPI
{
public:
    static void Initialize(uint8_t dataPin, uint8_t clkPin, int frq);

    static void Send(uint8_t *data, int length);
    static bool IsReady();
    static bool IsReadyForUs(unsigned int offset);

private:
    static bool initialized;

    static spi_device_handle_t spi;
    static spi_transaction_t trans;

    static volatile bool sendFinished;
    static volatile unsigned long sendFinishedAt;

    static void sendFinishedCallback(spi_transaction_t *trans);
};