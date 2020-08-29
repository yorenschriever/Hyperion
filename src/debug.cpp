
#include "debug.h"
#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>
#include <Udp.h>
#include <cbuf.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include "driver/uart.h"
#include "hardware/ethernet/ethernet.h"

#if defined(DEBUGOVERSERIAL) or defined(DEBUGOVERSERIALFRONT)

#if defined(DEBUGOVERSERIALFRONT)
void DebugClass::begin() { 
    Serial.begin(115200); 
    uart_set_pin(UART_NUM_0, 33, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}
#else 
void DebugClass::begin() { Serial.begin(115200); }
#endif
void DebugClass::print(const char *arg) { Serial.print(arg); }
void DebugClass::print(const unsigned char *arg) { Serial.print((const char *)arg); }
void DebugClass::print(unsigned char arg) { Serial.print(arg); }
void DebugClass::print(String arg) { Serial.print(arg); }
void DebugClass::println(const char *arg) { Serial.println(arg); }
void DebugClass::println(String arg) { Serial.println(arg); }
void DebugClass::println(const __FlashStringHelper *arg) { Serial.println(arg); }
void DebugClass::println(IPAddress arg) { Serial.println(arg); }
void DebugClass::printf(const char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
    va_end(copy);
    if (len < 0)
    {
        va_end(arg);
        return;
    };
    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            va_end(arg);
            return;
        }
        len = vsnprintf(temp, len + 1, format, arg);
    }
    va_end(arg);
    len = Serial.write((uint8_t *)temp, len);
    if (temp != loc_buf)
    {
        free(temp);
    }
}

#elif defined(DEBUGOVERUDP)

void DebugClass::begin() {}
void DebugClass::print(const char *arg) { transmit(arg); }
void DebugClass::print(const unsigned char *arg) { transmit((const char *)arg); }
void DebugClass::print(unsigned char arg) { transmit((const char *)&arg, 1); }
void DebugClass::print(String arg) { transmit(arg.c_str()); }
void DebugClass::println(const char *arg) { transmit((String(arg) + "\n").c_str()); }
void DebugClass::println(String arg) { transmit((arg + "\n").c_str()); }
void DebugClass::println(const __FlashStringHelper *arg) { transmit((String(arg) + "\n").c_str()); }
void DebugClass::println(IPAddress arg) { transmit(arg.toString().c_str()); }
void DebugClass::printf(const char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
    va_end(copy);
    if (len < 0)
    {
        va_end(arg);
        return;
    };
    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            va_end(arg);
            return;
        }
        len = vsnprintf(temp, len + 1, format, arg);
    }
    va_end(arg);
    transmit(temp, len);
    if (temp != loc_buf)
    {
        free(temp);
    }

    // char str[256];
    // va_list argptr;
    // va_start(argptr, data);
    // vsnprintf(str, 255, data, argptr);
    // transmit(str);
    // va_end(argptr);
}

void DebugClass::transmit(const char *data) { transmit(data, strlen(data)); }
void DebugClass::transmit(const char *data, int len)
{
    if (!Ethernet::isConnected())
        return;

    static int udp_server = -1;
    if (udp_server == -1)
    {
        if ((udp_server = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            return;
        }
    }

    fcntl(udp_server, F_SETFL, O_NONBLOCK);

    struct sockaddr_in recipient;
    recipient.sin_addr.s_addr = IPAddress(255, 255, 255, 255);
    recipient.sin_family = AF_INET;
    recipient.sin_port = htons(9000);
    sendto(udp_server, data, len, 0, (struct sockaddr *)&recipient, sizeof(recipient));
}

#else

void DebugClass::begin() {}
void DebugClass::print(const char *arg) {}
void DebugClass::print(const unsigned char *arg) {}
void DebugClass::print(unsigned char arg) {}
void DebugClass::print(String arg) {}
void DebugClass::println(const char *arg) {}
void DebugClass::println(String arg) {}
void DebugClass::println(const __FlashStringHelper *arg) {}
void DebugClass::println(IPAddress arg) {}
void DebugClass::printf(const char *data, ...){}

#endif
