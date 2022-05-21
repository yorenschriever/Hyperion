#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

//Use this class to show stuff on the display. It is far from finished yet.
class Display
{
public:
    static void Initialize();
    static void setFPS(int fpsin, int fpsout, int framemiss);
    static void setEthernet(bool connected, bool connecting);
    static void setWifi(bool enabled, bool connected, bool connecting);
    static void setDMX(bool connected);
    static void setMidi(bool connected, bool started);
    static void setLeds(int numleds);
    static void show();

    static void setDFU(bool dfu, int percentage);

private:
    Display(); // hide constructor
    static void displayTask(void *pvParameters);
    static void updateFrame();
    //static volatile bool dirty;
    static xSemaphoreHandle dirtySemaphore;
    static int fpsin, fpsout, framemiss;
    static bool ethconnected;
    static bool ethconnecting;
    static bool wificonnected;
    static bool wificonnecting;
    static bool wifienabled;
    static bool dmxconnected;
    static bool midiconnected;
    static bool midistarted;
    static int numleds;

    static bool dfu;
    static int dfupercentage;

    static int renderConnectionStatus(int start, bool connected, String name);
};