#pragma once

#include <Arduino.h>
#include <WiFi.h>

#include <string>
#include <map>

class Wifi
{
public:
    //starts the network
    static void Initialize(const char* ssid, const char* psk, boolean isAccesspoint);

    //starts the network with fixed ip
    //for maximum compatibility with other hardware (routers, pioneer gear, (hyper)linked operation),
    //it is not advised to set a fixed ip address. The default approach is to let the device request an ip from
    //the dhcp server, and self assign an autoip otherwise.
    //if you want to target this device specifically, set HostName in the config
    //this sets the hostname and a mdns entry.
    static bool SetFixedIp(IPAddress ip, IPAddress gateway, IPAddress subnet);

    //tells if the network is connected
    static bool isConnected();  
    static bool isConnecting();

    static IPAddress GetIp();
protected:

};
