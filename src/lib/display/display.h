#pragma once

//Use this class to show stuff on the display. It is far from finished yet.
class Display
{
public:
    static void Initialize();
    static void setFPS(int fpsin, int fpsout, int framemiss);
    static void setEthernet(bool connected);
    static void setDMX(bool connected);
    static void setMidi(bool connected);
    static void setLeds(int numleds);

    static void setDFU(bool dfu, int percentage);

private:
    Display(); // hide constructor
    static void displayTask(void *pvParameters);
    static void updateFrame();
    static volatile bool dirty;
    static int fpsin, fpsout, framemiss;
    static bool ethconnected;
    static bool dmxconnected;
    static bool midiconnected;
    static int numleds;

    static bool dfu;
    static int dfupercentage;

    static int renderConnectionStatus(int start, bool connected, String name);
};