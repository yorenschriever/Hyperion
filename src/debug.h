
#pragma once
#include <Arduino.h>

//uncomment the first line to enable debugging over the usb port. This means that 
//midi inputs wil be disabled, because they share the same uart.
//#ifdef DEBUGOVERSERIAL is also used to block mini uart initalization and to remove the Midi 
//connection stats from the dusplay, so be careful when changing this name
//Uncomment the second line to enable debugging over udp. use ./debug command
//to start listening for debug messages
//uncomment the third line to send all debug commands to channel8 on the front panel.
//now you can attach your own serial->usb adapter to the front panel. this is useful when
//debugging midi, because it doesnt interfere with midi functions. it also prints
//system messages like stack traces, which udpdebug cannot do.

//#define DEBUGOVERSERIAL
//#define DEBUGOVERUDP
#define DEBUGOVERSERIALFRONT

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
