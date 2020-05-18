#pragma once

//config to get the olimex poe board working with ETH
#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
#define ETH_PHY_POWER 12

#include <Arduino.h>
#include <ETH.h>

class Ethernet
{
public:
    //starts the network with dhcp
    static void Initialize();

    //starts the network with fixed ip
    static bool SetFixedIp(IPAddress ip, IPAddress gateway, IPAddress subnet);

    //tells if the network is connected
    static bool isConnected();

protected:
    static void EthEvent(WiFiEvent_t event);
    static bool eth_connected;
};

//void NetworkBegin();
//boolean networkIsConnected();
