#pragma once

class Display
{
    public:
        static void Initialize();
        static void setFPS(int fps);
        
    private:
        Display();                                              // hide constructor
        static void displayTask( void * pvParameters );
        static void updateFrame();
        static volatile bool dirty;
        static int fps;


};