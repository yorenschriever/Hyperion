#pragma once

#include <inttypes.h>
#include "esp32-hal-spi.h"
#include <driver/spi_master.h>
#include <set>

class SPI
{
public:
    static SPI *CreateInstance();
    void Initialize(uint8_t dataPin, uint8_t clkPin, int frq);

    void Send(uint8_t *data, int length);
    bool IsReady();
    bool IsReadyForUs(unsigned int offset);

private:
    bool initialized = false;
    int hostNumber;

    spi_device_handle_t spi;
    spi_transaction_t trans;

    volatile bool sendFinished = true;
    volatile unsigned long sendFinishedAt = 0;

    static void sendFinishedCallback(spi_transaction_t *trans);

    SPI(unsigned int number);
    static int instance;
};