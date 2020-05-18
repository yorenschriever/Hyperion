#include "sharedResources.h"

//Use this file to define and share resources 

//a mutex to indicate someone is using i2c. 
//I use this because the display lib splits up the frame update over multiple
//i2c transactions, so i cannot trust the i2c mutex here
xSemaphoreHandle i2cMutex = xSemaphoreCreateMutex();

//the rotary button is using one the the hardware timers to detect a long press.
//this defines the timer number it uses.
int rotaryButtonTimer = 0;