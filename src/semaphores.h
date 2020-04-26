#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

extern xSemaphoreHandle i2cMutex;
//extern xSemaphoreHandle i2cMutex = xSemaphoreCreateMutex();
