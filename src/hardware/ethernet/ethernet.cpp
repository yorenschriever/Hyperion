#include <Arduino.h>
#include "ethernet.h"
#include "debug.h"
#include "mdns.h"

bool Ethernet::eth_connected = false;
const char* Ethernet::hostname;

void Ethernet::Initialize(const char* hostname)
{
    //this delay makes sure the network hardware is property started up, it is unstable without it
    delay(500);

    //This delay makes sure that not all nodes will startup at exactly the same time when you flip the master power switch
    //Routers were having trouble when a large amount of nodes started communicating at exactly the same time.
    delay(100 + (esp_random() & 0xFF) * 5);

    WiFi.onEvent(EthEvent);
    ETH.begin();

    Ethernet::hostname = hostname;
    if (hostname)
        StartMdnsService(hostname);
    
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

void Ethernet::StartMdnsService(const char* name)
{
    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err) {
        Debug.printf("MDNS Init failed: %d\n", err);
        return;
    }

    //set hostname
    mdns_hostname_set(name);
    //set default instance
    mdns_instance_name_set(name);
}