#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "sdkconfig.h"
#include <Arduino.h>

#include <../.pio/libdeps/esp32-poe/NeoPixelBus_ID547/src/NeoPixelBus.h>
//#include <NeoPixelBus.h>

const uint16_t PixelCount = 4; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 13;  // make sure to set this to the correct pin, ignored for Esp8266

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

void setup() {
    Serial.begin(115200);

    strip.Begin();
    strip.Show();
}

void loop() {
    Serial.printf("Max sockets: %d\n",CONFIG_LWIP_MAX_SOCKETS);

    strip.SetPixelColor(0, RgbColor(255,0,0));
    strip.SetPixelColor(1, RgbColor(0,255,0));
    strip.SetPixelColor(2, RgbColor(0,0,255));
    strip.SetPixelColor(3, RgbColor(255,255,255));

    strip.Show();

    delay(200);
}
