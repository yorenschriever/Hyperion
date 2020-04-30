#pragma once

#include <Arduino.h>

//#define DEBUGOVERSERIAL

#ifdef DEBUGOVERSERIAL

    #define DEBUGSERIAL
    #define Debug Serial

#else

    //dummy class that basically throws away all debug data
    class DebugClass {
        public:
        void begin(int){}
        void print(const char*){}
        void print(const unsigned char*){}
        void print(unsigned char){}
        void print(String){}
        void println(const char*){}
        void println(String){}
        void println(const __FlashStringHelper*){}
        void println(IPAddress){}
        void printf(const char* , ...){}
    };

    extern DebugClass Debug;
#endif