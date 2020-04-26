#include "semaphores.h"

xSemaphoreHandle i2cMutex = xSemaphoreCreateMutex();