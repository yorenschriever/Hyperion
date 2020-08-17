#ifndef LWIP_OPEN_SRC
#define LWIP_OPEN_SRC
#endif
#define CONFIG_ETH_ENABLED

#include "WiFi.h"
#include <functional>
#include "esp_wifi.h"

#include <Arduino.h>
#include "ethernet.h"
#include "debug.h"
#include "mdns.h"
#include "WiFi.h"
#include <ESPmDNS.h>
#include <lwip/netdb.h>

bool Ethernet::eth_connected = false;
bool Ethernet::eth_connecting = false;
const char* Ethernet::hostname;
std::map<std::string, IPAddress> Ethernet::mdnsCache;

void Ethernet::Initialize(const char* hostname)
{
    Ethernet::hostname = hostname;

    //this delay makes sure the network hardware is property started up, it is unstable without it
    delay(500);

    //This delay makes sure that not all nodes will startup at exactly the same time when you flip the master power switch
    //Routers were having trouble when a large amount of nodes started communicating at exactly the same time.
    delay(100 + (esp_random() & 0xFF) * 5);

    WiFi.onEvent(EthEvent);

    ETH.begin();
}

//for maximum compatibility with other hardware (routers, pioneer gear, (hyper)linked operation), 
//it is not advised to set a fixed ip address. The default approach is to let the device request an ip from
//the dhcp server, and self assign an autoip otherwise.
//if you want to target this device specifically, set HostName in the config
//this sets the hostname and a mdns entry.
bool Ethernet::SetFixedIp(IPAddress ip, IPAddress gateway, IPAddress subnet)
{
    tcpip_adapter_ip_info_t info;
    info.ip.addr = static_cast<uint32_t>(ip);
    info.gw.addr = static_cast<uint32_t>(gateway);
    info.netmask.addr = static_cast<uint32_t>(subnet);

    ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_ETH));
    //ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_ETH, &info));
    bool result = (tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_ETH, &info) == ESP_OK);

    Debug.println("");

    Debug.println("Eth connected");
    Debug.println("IP address: ");
    Debug.println(ETH.localIP());

    return result;
}


bool Ethernet::isConnected()
{
    return eth_connected;
}

bool Ethernet::isConnecting()
{
    return eth_connecting;
}

IPAddress Ethernet::GetIp()
{
    return ETH.localIP();
}

void Ethernet::EthEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case SYSTEM_EVENT_ETH_START:
        Debug.println("ETH Started");
        //set eth hostname here
        if (Ethernet::hostname)
                ETH.setHostname(Ethernet::hostname);
        mdnsCache.clear();
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        Debug.println("ETH Connected");
        eth_connecting = true;
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
        eth_connecting = false;

        if (Ethernet::hostname){
            StartMdnsService(Ethernet::hostname);
        }

        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        Debug.println("ETH Disconnected");
        eth_connected = false;
        eth_connecting = false;
        break;
    case SYSTEM_EVENT_ETH_STOP:
        Debug.println("ETH Stopped");
        eth_connected = false;
        eth_connecting = false;
        break;
    default:
        break;
    }

}

void Ethernet::StartMdnsService(const char* name)
{
    if (!MDNS.begin(name)) {
        Debug.println("Error setting up MDNS responder!");
    } else {
        Debug.printf("Hostname set: %s\n", name);
    }
}

IPAddress Ethernet::Resolve(const char* hostname)
{
    if (!String(hostname).endsWith(".local"))
    {
        struct hostent *host;
        host = gethostbyname(hostname);
        if (host == NULL)
        {
            Debug.println("Cannot get host");
            return IPAddress((uint32_t)0);
        }
        return IPAddress((const uint8_t *)(host->h_addr_list[0]));
    }

    //strip ".local" from the hostname
    std::string localHostname = std::string(hostname);
    localHostname.resize(localHostname.size()  -6);

    std::map<std::string,IPAddress>::iterator it = mdnsCache.find(localHostname);
    if (it != mdnsCache.end())
        return it->second;
    
    Debug.printf("querying: %s\n",localHostname.c_str());

    struct ip4_addr addr;
    addr.addr = 0;
    esp_err_t err = mdns_query_a(localHostname.c_str(), 2000,  &addr);
    if(err){
        if(err == ESP_ERR_NOT_FOUND){
            Debug.println("Host was not found!");
            return IPAddress((uint32_t)0);
        }
        Debug.println("Query Failed");
        return IPAddress((uint32_t)0);
    }

    Debug.printf(IPSTR, IP2STR(&addr));

    IPAddress result = IPAddress(addr.addr);

    mdnsCache[localHostname] = result;

    return result;
}