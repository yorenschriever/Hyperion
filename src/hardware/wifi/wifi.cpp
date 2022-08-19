#ifndef LWIP_OPEN_SRC
#define LWIP_OPEN_SRC
#endif
#define CONFIG_ETH_ENABLED

#include <functional>
#include "esp_wifi.h"

#include <Arduino.h>
#include "debug.h"
#include "mdns.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <lwip/netdb.h>
#include "wifi.h"

void Wifi::Initialize(const char* ssid, const char* psk, boolean isAccesspoint)
{
    if (isAccesspoint)
    {
        Debug.printf("Setting up wifi acess point: %s\r\n",ssid);
        WiFi.softAP(ssid,psk);
        IPAddress IP = WiFi.softAPIP();
        Debug.print("AP IP address: ");
        Debug.println(IP);
    } else {
        Debug.printf("Connecting to wifi: %s\r\n",ssid);
        WiFi.begin(ssid, psk);
    }
}

//for maximum compatibility with other hardware (routers, pioneer gear, (hyper)linked operation), 
//it is not advised to set a fixed ip address. The default approach is to let the device request an ip from
//the dhcp server, and self assign an autoip otherwise.
//if you want to target this device specifically, set HostName in the config
//this sets the hostname and a mdns entry.
bool Wifi::SetFixedIp(IPAddress ip, IPAddress gateway, IPAddress subnet)
{
    bool result = WiFi.config(ip, gateway, subnet);

    Debug.println("");
    Debug.println("Wifi fixed IP address: ");
    Debug.println(WiFi.localIP());

    return result;
}


bool Wifi::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

bool Wifi::isConnecting()
{
    return WiFi.status() == WL_IDLE_STATUS;
}

IPAddress Wifi::GetIp()
{
    return WiFi.localIP();
}