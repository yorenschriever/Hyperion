#ifndef LWIP_OPEN_SRC
#define LWIP_OPEN_SRC
#endif
#define CONFIG_ETH_ENABLED

#include <functional>
#include "esp_wifi.h"

#include <Arduino.h>
#include "ethernet.h"
#include "debug.h"
#include "mdns.h"
#include <ESPmDNS.h>
#include <lwip/netdb.h>

#include "esp_system.h"
#include "esp_event.h"
#include "esp_eth.h"
#include "esp_eth_phy.h"
#include "esp_eth_mac.h"
#include "esp_eth_com.h"
#include "lwip/err.h"
#include "lwip/dns.h"

esp_eth_handle_t Ethernet::eth_handle = NULL;
bool Ethernet::eth_connected = false;
bool Ethernet::eth_connecting = false;
const char *Ethernet::hostname;
std::map<std::string, Ethernet::hostnameCacheItem> Ethernet::hostnameCache;

extern void tcpipInit();

void Ethernet::Initialize(const char *hostname)
{
    Ethernet::hostname = hostname;

    // This delay makes sure that not all nodes will startup at exactly the same time when you flip the master power switch
    // Routers were having trouble when a large amount of nodes started communicating at exactly the same time.
    delay(100 + (esp_random() & 0xFF) * 5);

    tcpipInit();

    tcpip_adapter_set_default_eth_handlers();
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    mac_config.smi_mdc_gpio_num = 23;
    mac_config.smi_mdio_gpio_num = 18;
    mac_config.sw_reset_timeout_ms = 1000; // this timeout is increased for stability
    esp_eth_mac_t *eth_mac = NULL;

    eth_mac = esp_eth_mac_new_esp32(&mac_config);

    if (eth_mac == NULL)
    {
        Debug.println("Eth error: new mac failed");
        log_e("esp_eth_mac_new_esp32 failed");
        return;
    }

    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = 0;
    phy_config.reset_gpio_num = 12;
    esp_eth_phy_t *eth_phy = NULL;

    eth_phy = esp_eth_phy_new_lan8720(&phy_config);

    if (eth_phy == NULL)
    {
        Debug.println("Eth error: new phy failed");
        log_e("esp_eth_phy_new failed");
        return;
    }

    eth_handle = NULL;
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(eth_mac, eth_phy);

    if (esp_eth_driver_install(&eth_config, &eth_handle) != ESP_OK || eth_handle == NULL)
    {
        Debug.println("Eth error: driver install failed");
        log_e("esp_eth_driver_install failed");
        return;
    }

    if (esp_eth_start(eth_handle) != ESP_OK)
    {
        Debug.println("Eth error: eth start failed");
        log_e("esp_eth_start failed");
        return;
    }

    //     ESP_ERROR_CHECK(esp_event_loop_create_default());
    //     //ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    //     //ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    // esp_event_loop_init(event_handler2, nullptr);

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_CONNECTED, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_START, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_START, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_STOP, &event_handler, NULL));

    Debug.println("Eth initialized");
}

// for maximum compatibility with other hardware (routers, pioneer gear, (hyper)linked operation),
// it is not advised to set a fixed ip address. The default approach is to let the device request an ip from
// the dhcp server, and self assign an autoip otherwise.
// if you want to target this device specifically, set HostName in the config
// this sets the hostname and a mdns entry.
bool Ethernet::SetFixedIp(IPAddress ip, IPAddress gateway, IPAddress subnet)
{
    tcpip_adapter_ip_info_t info;
    info.ip.addr = static_cast<uint32_t>(ip);
    info.gw.addr = static_cast<uint32_t>(gateway);
    info.netmask.addr = static_cast<uint32_t>(subnet);

    ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_ETH));
    // ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_ETH, &info));
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
    tcpip_adapter_ip_info_t ip;
    if (tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_ETH, &ip))
    {
        return IPAddress();
    }
    return IPAddress(ip.ip.addr);
}

uint8_t *Ethernet::GetMac(uint8_t *mac)
{
    if (!mac)
    {
        return NULL;
    }
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac);
    return mac;
}

void Ethernet::event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
        case IP_EVENT_ETH_GOT_IP:
            Debug.print("Ethernet IPv4: ");
            Debug.print(GetIp());
            eth_connected = true;
            eth_connecting = false;

            if (Ethernet::hostname)
            {
                StartMdnsService(Ethernet::hostname);
            }

            break;
        case IP_EVENT_STA_GOT_IP:
            Debug.printf("Wifi connected.\r\n");
            if (Ethernet::hostname)
            {
                StartMdnsService(Ethernet::hostname);
            }
            break;
        default:
            break;
        }
    }

    if (event_base == ETH_EVENT)
    {
        switch (event_id)
        {
        case ETHERNET_EVENT_START:
            Debug.println("ETH Started");
            // set eth hostname here
            if (Ethernet::hostname)
                ETH.setHostname(Ethernet::hostname);
            hostnameCache.clear();
            break;
        case ETHERNET_EVENT_CONNECTED:
            Debug.println("ETH Connected");
            eth_connecting = true;
            break;
        case ETHERNET_EVENT_DISCONNECTED:
            Debug.println("ETH Disconnected");
            eth_connected = false;
            eth_connecting = false;
            break;
        case ETHERNET_EVENT_STOP:
            Debug.println("ETH Stopped");
            eth_connected = false;
            eth_connecting = false;
            break;
        default:
            break;
        }
    }

    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_AP_START:
            Debug.printf("Wifi connected.\r\n");
            if (Ethernet::hostname)
            {
                StartMdnsService(Ethernet::hostname);
            }
            break;
        default:
            break;
        }
    }
}

void Ethernet::StartMdnsService(const char *name)
{
    if (!MDNS.begin(name))
    {
        Debug.println("Error setting up MDNS responder!");
    }
    else
    {
        Debug.printf("Hostname set: %s\r\n", name);
    }
}

IPAddress *Ethernet::ResolveNoWait(const char *hostname)
{
    std::map<std::string, hostnameCacheItem>::iterator it = hostnameCache.find(hostname);
    if (it == hostnameCache.end())
    {
        // start lookup
        hostnameCache[hostname] = hostnameCacheItem{IPAddress((uint32_t)0), millis(), false};
        // Debug.println("hostname item created");
        xTaskCreate(ResolveTask, "ResolveTask", 3000, (void *)hostname, 0, NULL);
        return NULL;
    }

    if (!it->second.found)
    {
        if ((millis() - it->second.updated) < 5000)
        {
            // last hostname query returned nothing, and was less than 5 seconds ago. Do not ask again yet
            // Debug.println("less than 5 sec. waiting");
            return NULL;
        }
        // Debug.println("item was queried before, but not found, retry");
        hostnameCache[hostname].updated = millis();
        xTaskCreate(ResolveTask, "ResolveTask", 3000, (void *)hostname, 0, NULL);
        return NULL;
    }

    if (millis() - it->second.updated > 6000)
    {
        // entry is older than 1 minute, refresh
        // Debug.println("refreshing hostname item");
        hostnameCache[hostname].updated = millis();
        xTaskCreate(ResolveTask, "ResolveTask", 3000, (void *)hostname, 0, NULL);
    }

    // Debug.printf("return cached ip %s\r\n",it->second.ip.toString().c_str());
    return &it->second.ip;
}

void Ethernet::ResolveTask(void *pvParameters)
{
#ifdef DEBUGOVERSERIAL
    // give the serial some time to send the other messages
    vTaskDelay(10);
#endif
    Resolve(static_cast<const char *>(pvParameters));
    vTaskDelete(NULL);
}

IPAddress *Ethernet::Resolve(const char *hostname)
{

    // there is a built-in mdns resovler, but i cant get it to work
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html#config-lwip-dns-support-mdns-queries

    if (!String(hostname).endsWith(".local"))
    {
        struct hostent *host;
        host = gethostbyname(hostname);
        if (host == NULL)
        {
            Debug.println("Cannot get host");
            // return IPAddress((uint32_t)0);
            hostnameCache[hostname].updated = millis();
            hostnameCache[hostname].found = false;
            return NULL;
        }

        hostnameCache[hostname].ip = IPAddress((const uint8_t *)(host->h_addr_list[0]));
        hostnameCache[hostname].updated = millis();
        hostnameCache[hostname].found = true;
        return &hostnameCache[hostname].ip;
    }

    // strip ".local" from the hostname
    std::string localHostname = std::string(hostname);
    localHostname.resize(localHostname.size() - 6);

    // Debug.printf("querying: %s\r\n",localHostname.c_str());

    ip4_addr addr;
    addr.addr = 0;
    esp_err_t err = mdns_query_a(localHostname.c_str(), 2000, &addr);
    if (err)
    {
        Debug.println(err == ESP_ERR_NOT_FOUND ? "Host was not found!" : "Error in mdns query");

        hostnameCache[hostname].updated = millis();
        hostnameCache[hostname].found = false;
        return NULL;
    }

    // Debug.printf(IPSTR, IP2STR(&addr));
    // Debug.printf("ip found: %s\r\n",IPAddress(addr.addr).toString().c_str());

    hostnameCache[hostname].ip = IPAddress(addr.addr);
    hostnameCache[hostname].updated = millis();
    hostnameCache[hostname].found = true;
    return &hostnameCache[hostname].ip;
}