#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "sdkconfig.h"
#include <Arduino.h>

void setup() {
    Serial.begin(115200);
}

void loop() {
    Serial.printf("Max sockets: %d\n",CONFIG_LWIP_MAX_SOCKETS);
    delay(200);
}

