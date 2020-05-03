#pragma once

class Display
{
    public:
        static void Initialize();
        static void setFPS(int fpsin, int fpsout, int framemiss);
        static void setEthernet(bool connected);
        static void setDMX(bool connected);
        static void setMidi(bool connected);
        static void setLeds(int numleds);

    private:
        Display();                                              // hide constructor
        static void displayTask( void * pvParameters );
        static void updateFrame();
        static volatile bool dirty;
        static int fpsin, fpsout, framemiss;
        static bool ethconnected;
        static bool dmxconnected;
        static bool midiconnected;
        static int numleds;

        static int renderConnectionStatus(int start, bool connected, String name);


};