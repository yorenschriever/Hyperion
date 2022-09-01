#include "spi.h"
#include "esp32-hal-spi.h"
#include <driver/spi_master.h>
#include "debug.h"

int SPI::instance = 0;

SPI *SPI::CreateInstance()
{
    if (instance >= 2)
    {
        Debug.printf("More SPI devices are requested than are available. Available devices: %d\r\n", instance);
        return NULL;
    }
    return new SPI(instance++);
}

SPI::SPI(unsigned int number)
{
    this->hostNumber = number;
}

void SPI::Initialize(uint8_t dataPin, uint8_t clkPin, int frq)
{
    if (initialized)
        return;

    //Debug.printf("Initializing SPI device: %d\r\n", hostNumber);

    if (hostNumber >= 2)
    {
        Debug.printf("SPI not initialized, invalid number: %d\r\n", hostNumber);
        return;
    }

    spi_bus_config_t buscfg = {
        .mosi_io_num = dataPin,
        .miso_io_num = -1,
        .sclk_io_num = clkPin};

    spi_device_interface_config_t devcfg = {
        .mode = 0,
        .clock_speed_hz = frq,
        .spics_io_num = -1,
        .queue_size = 1,
        .post_cb = sendFinishedCallback};

    ESP_ERROR_CHECK(spi_bus_initialize(hostNumber == 0 ? HSPI_HOST : VSPI_HOST, &buscfg, hostNumber == 0 ? 1 : 2));
    ESP_ERROR_CHECK(spi_bus_add_device(hostNumber == 0 ? HSPI_HOST : VSPI_HOST, &devcfg, &spi));

    initialized = true;
}

void SPI::Send(uint8_t *data, int length)
{
    if (!IsReady())
        return;

    sendFinished = false;

    memset(&trans, 0, sizeof(spi_transaction_t));
    trans.length = length * 8;
    trans.user = (void *)this;
    trans.flags = 0;
    trans.tx_buffer = data;

    ESP_ERROR_CHECK_WITHOUT_ABORT(spi_device_queue_trans(spi, &trans, portMAX_DELAY));
}

void IRAM_ATTR SPI::sendFinishedCallback(spi_transaction_t *trans)
{
    auto instance = (SPI *)trans->user;
    instance->sendFinished = true;
    instance->sendFinishedAt = micros();
}

bool SPI::IsReady()
{
    return sendFinished;
}

bool SPI::IsReadyForUs(unsigned int offset)
{
    if (!sendFinished)
        return false;
    return (micros() - sendFinishedAt > offset);
}