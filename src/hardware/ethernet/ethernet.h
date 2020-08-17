#pragma once

//config to get the olimex poe board working with ETH
#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
#define ETH_PHY_POWER 12

#include <Arduino.h>
#include <ETH.h>
#include <string>
#include <map>

class Ethernet
{
public:
    //starts the network
    static void Initialize(const char* hostname);

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

    //in case of a .local address it resolves using mdns, and caches the result
    //otherwise it will do a regular hostname lookup
    static IPAddress Resolve(const char* hostname);

protected:
    static void EthEvent(WiFiEvent_t event);
    static void StartMdnsService(const char *name);
    static bool eth_connected;
    static bool eth_connecting;
    static const char* hostname;

    static std::map<std::string, IPAddress> mdnsCache;
};
