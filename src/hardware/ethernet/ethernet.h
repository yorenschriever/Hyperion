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
    static uint8_t* GetMac(uint8_t* mac);

    //in case of a .local address it resolves using mdns, and caches the result
    //otherwise it will do a regular hostname lookup
    static IPAddress* ResolveNoWait(const char* hostname);
    static IPAddress* Resolve(const char* hostname);

protected:
    static esp_eth_handle_t eth_handle;
    static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static void StartMdnsService(const char *name);
    static bool eth_connected;
    static bool eth_connecting;
    static const char* hostname;

    static void ResolveTask(void * pvParameters);
    struct hostnameCacheItem {IPAddress ip; unsigned long updated; bool found;};
    static std::map<std::string, hostnameCacheItem> hostnameCache;
};
