#include <Arduino.h>
#include "network.h"
#include "debug.h"

const unsigned int nodeid = 123;

IPAddress ip(192, 168, 1, nodeid);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void EthEvent(WiFiEvent_t event);
static bool eth_connected = false;

byte flipByte(byte c);
bool config_eth(IPAddress local_ip, IPAddress gateway, IPAddress subnet);

void NetworkBegin()
{
    //This delay makes sure that not all nodes will startup at exactly the same time when you flip the master power switch
    //Routers were having trouble when a large amount of nodes started communicating at exactly the same time.
    delay(100 + flipByte(nodeid) * 5);

    WiFi.onEvent(EthEvent);
    ETH.begin();

    bool staticip = config_eth(ip, gateway, subnet);
    Debug.print(staticip ? "static ip configured" : "static ip failed");

    while (!eth_connected)
    {
        Debug.print(".");
        //animate(0,0,50);
        delay(50);
    }
    Debug.println("");

    Debug.println("Eth connected");
    Debug.println("IP address: ");
    Debug.println(ETH.localIP());
}

boolean networkIsConnected()
{
    return eth_connected;
}

bool config_eth(IPAddress local_ip, IPAddress gateway, IPAddress subnet)
{
    tcpip_adapter_ip_info_t info;
    info.ip.addr = static_cast<uint32_t>(local_ip);
    info.gw.addr = static_cast<uint32_t>(gateway);
    info.netmask.addr = static_cast<uint32_t>(subnet);

    tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_ETH);
    return (tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_ETH, &info) == ESP_OK);
}

void EthEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case SYSTEM_EVENT_ETH_START:
        Debug.println("ETH Started");
        //set eth hostname here
        char buf[20];
        sprintf(buf, "Hyperion ip= %d", nodeid);
        ETH.setHostname(buf);
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        Debug.println("ETH Connected");
        //eth_connected = true;
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        Debug.print("ETH MAC: ");
        Debug.print(ETH.macAddress());
        Debug.print(", IPv4: ");
        Debug.print(ETH.localIP());
        if (ETH.fullDuplex())
        {
            Debug.print(", FULL_DUPLEX");
        }
        Debug.print(", ");
        Debug.print(ETH.linkSpeed());
        Debug.println("Mbps");
        eth_connected = true;
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        Debug.println("ETH Disconnected");
        eth_connected = false;
        break;
    case SYSTEM_EVENT_ETH_STOP:
        Debug.println("ETH Stopped");
        eth_connected = false;
        break;
    default:
        break;
    }
}

byte flipByte(byte c)
{
    char r = 0;
    for (byte i = 0; i < 8; i++)
    {
        r <<= 1;
        r |= c & 1;
        c >>= 1;
    }
    return r;
}