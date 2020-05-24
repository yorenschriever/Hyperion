#pragma once
#include "bpm.h"
#include "hardware/UDPFast/UDPFast.h"
#include "debug.h"

#include "hardware/ethernet/ethernet.h"
#include <ETH.h>


#define PRODJLINKPORTSYNC 50001
#define PRODJLINKPORTSTATUS 50002

class ProDJLinkBPM : public BPM
{

public:
    void Initialize() override
    {
        //todo check if network is initialized, otherwise device will crash
        udpFast.begin(PRODJLINKPORTSYNC);
        udpFaststatus.begin(PRODJLINKPORTSTATUS);
        xTaskCreatePinnedToCore(ProDJLinkTask, "ProDJLinkTask", 5000, this, 1, NULL, 1);;
    }

    int GetBeatNumber() override
    {
        //todo: beatnumber should always increase, not only 1-4
        return beatnumber;
    }

    int TimeToNextBeat() override { 
        return -1;
    }

    int TimeSinceLastBeat() override { 
        return -1;
    }

private:
    UDPFast udpFast;
    UDPFast udpFaststatus;
    volatile int beatnumber = 0;
    unsigned long lastKeepAlive = 0;
    int master=0;

    static void ProDJLinkTask(void *param) 
    {
        ProDJLinkBPM *this2 = (ProDJLinkBPM *)param;
        uint8_t buffer[1500];
        while(true){
            //https://djl-analysis.deepsymmetry.org/djl-analysis/beats.html#_footnoteref_1
            int len = this2->udpFast.waitPacketFast(buffer);
            if (len>0){
                int device = buffer[0x5f];
                if (len == 0x60 && buffer[0x0a]==0x28 && device == this2->master){
                    this2->beatnumber = buffer[0x5c];
                }
                Debug.printf("len=%d, id=%d, beat=%d, device=%d, master=%d\n",len,buffer[0x21],buffer[0x5c], buffer[0x5f], this2->master);
            }

            len = this2->udpFaststatus.waitPacketFast(buffer);
            if (len>0){
                Debug.printf("status %x, %x\n",len, buffer[0x0a]);
                // if (len == 0x60 && buffer[0x0a]==0x28){
                //     this2->beatnumber = buffer[0x5c];
                // }
                // Debug.printf("len=%d, id=%d, beat=%d, device=%d\n",len,buffer[0x21],buffer[0x5c], buffer[0x5f]);

                if (len==0x11c && buffer[0x0a]==0x0a){
                    int device = buffer[0x21];
                    int ismaster = buffer[0x9e] > 0;

                    Debug.printf("device=%d, master=%d (%x)\n",device,ismaster,buffer[0x9e]);

                    if (ismaster>0 && this2->master != device){
                        this2->master = device;
                        Debug.printf("master handoff to %d",device);

                    }
                }

            }

            if (millis() - this2->lastKeepAlive > 300 && Ethernet::isConnected())
            {

                uint8_t mac[6];
                //uint8_t ip[4];

                //ETHClass::macAddress(mac);
                //ETHClass::localIP();

                uint8_t deviceid = 5;
                uint8_t buffer[] = {
0x51, 
0x73,
0x70,
0x74,
0x31,
0x57,
0x6d,
0x4a,
0x4f,
0x4c,
0x06,
0x00,

0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 

1, 2, 0, 0x36, deviceid, 01, 

0,0,0,0,0,0, //mac
0,0,0,0, //ip

1, 0,0,0, 1, 0

                };

                //IPAddress ip = ETH.localIP();
                //uint8_t ip[4] = {169,254,67,123}; //TODO
                uint32_t ip = ETH.localIP();

                memcpy(buffer + 0x26,ETH.macAddress(mac),6);
                memcpy(buffer + 0x2c,&ip,4);

                this2->udpFaststatus.beginPacket(IPAddress(255,255,255,255),50000);
                this2->udpFaststatus.write(buffer,sizeof(buffer));
                this2->udpFaststatus.endPacket();

                this2->lastKeepAlive = millis();

                // for (int i=0; i<sizeof(buffer); i++){
                //     if (i%16==0)
                //         Debug.println();
                //     Debug.printf("%x    \t",buffer[i]);

                // }
                // Debug.println();
            }
        }
    }
};
