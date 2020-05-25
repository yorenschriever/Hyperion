#pragma once

#include <Arduino.h>

//I still have to write a proper debug class.
//currently i either throw away all message, or write them to the serial console,
//depending on the setting below

//uncomment this line to enable debuggen over the usb port. 
//this means that midi inputs wil be disabled, because they share
//the same uart.
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