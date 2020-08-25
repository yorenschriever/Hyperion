
#pragma once
#include <Arduino.h>

//uncomment the first line to enable debugging over the usb port. This means that 
//midi inputs wil be disabled, because they share the same uart.
//#ifdef DEBUGOVERSERIAL is also used to block mini uart initalization and to remove the Midi 
//connection stats from the dusplay, so be careful when changing this name
//Uncomment the second line to enable debugging over udp. use ./debug command
//to start listening for debug messages

//#define DEBUGOVERSERIAL
#define DEBUGOVERUDP

class DebugClass
{
public:
    void begin() ;
    void print(const char *) ;
    void print(const unsigned char *) ;
    void print(unsigned char) ;
    void print(String) ;
    void println(const char *) ;
    void println(String) ;
    void println(const __FlashStringHelper *) ;
    void println(IPAddress) ;
    void printf(const char *, ...) ;

private:
    void transmit(const char *data) ;
    void transmit(const char *data, int len);
};

extern DebugClass Debug;
