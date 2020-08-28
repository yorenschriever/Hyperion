#pragma once
#include "abstractTempo.h"
#include "hardware/UDPFast/UDPFast.h"
#include "debug.h"

#include "hardware/ethernet/ethernet.h"
#include <ETH.h>

#define PRODJLINKPORTKEEPALIVE 50000
#define PRODJLINKPORTSYNC 50001
#define PRODJLINKPORTSTATUS 50002

//ProDJLinkTempo will connect to Pioneer cdj/xdj's and sync to their beats.
//only works if the tracks are analyzed with recordbox.
class ProDJLinkTempo : public AbstractTempo
{

public:
    static ProDJLinkTempo *getInstance()
    {
        static ProDJLinkTempo instance;
        return &instance;
    }

    TempoTaskType Initialize() override
    {
        sourceName = "ProDJLink";
        syncSocket.begin(PRODJLINKPORTSYNC);
        statusSocket.begin(PRODJLINKPORTSTATUS);

        return ProDJLinkTask;
    }

private:
    UDPFast syncSocket;
    UDPFast statusSocket;
    unsigned long lastKeepAlive = 0; //when the last keep alive was sent to the other players
    int master = 0;                  //the cdplayer that is currently tempo master

    ProDJLinkTempo() : AbstractTempo() {}
    ProDJLinkTempo(ProDJLinkTempo const &); // Don't Implement
    void operator=(ProDJLinkTempo const &); // Don't implement

    //this task handles all communication with the cd players
    static void ProDJLinkTask() 
    {
        ProDJLinkTempo *this2 = ProDJLinkTempo::getInstance(); 
        uint8_t buffer[1500]; //TODO heap, and only 0x11c bytes

        //https://djl-analysis.deepsymmetry.org/djl-analysis/beats.html#_footnoteref_1
        int len = this2->syncSocket.waitPacketFast(buffer);
        if (len > 0)
        {
            int device = buffer[0x5f];
            if (len == 0x60 && buffer[0x0a] == 0x28 && device == this2->master)
            {
                this2->beat();

                //adjust the beat counter if it is not in sync with the beat counter of the cdj anymore.
                int beatmod4cdj = buffer[0x5c] - 1;        //beat number 0-3 from the cdj
                int beatmod4local = this2->beatNumber % 4; //beat number 0-3 from this Tempo class
                int diff = beatmod4cdj - beatmod4local;
                if (diff != 0)
                {
                    if (diff == -2)
                        diff = 2; // prefer skipping +2 over -2
                    else if (diff == -3)
                        diff = 1; // adjust skips to the nearest match
                    else if (diff == 3)
                        diff = -1; // adjust skips to the nearest match

                    this2->beatNumber += diff;
                }
            }
            this2->validSignal=true;
            Debug.printf("len=%d, id=%d, beat=%d, device=%d, master=%d\n", len, buffer[0x21], buffer[0x5c], buffer[0x5f], this2->master);
        }

        len = this2->statusSocket.waitPacketFast(buffer);
        if (len > 0)
        {
            //Debug.printf("status %x, %x\n", len, buffer[0x0a]);

            if (len == 0x11c && buffer[0x0a] == 0x0a)
            {
                int device = buffer[0x21];
                int ismaster = buffer[0x9e] > 0;
                if (ismaster > 0 && this2->master != device)
                {
                    this2->master = device;
                    Debug.printf("master handoff to %d", device);
                }
            }
        }

        if (millis() - this2->lastKeepAlive > 300 && Ethernet::isConnected())
            SendKeepAlive(this2);
    }

    static void SendKeepAlive(ProDJLinkTempo *this2)
    {
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

            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

            1, 2, 0, 0x36, deviceid, 01,

            0, 0, 0, 0, 0, 0, //mac
            0, 0, 0, 0,       //ip

            1, 0, 0, 0, 1, 0};

        uint8_t mac[6];
        uint32_t ip = ETH.localIP();

        memcpy(buffer + 0x26, ETH.macAddress(mac), 6);
        memcpy(buffer + 0x2c, &ip, 4);

        this2->statusSocket.sendPacketFast(
            IPAddress(255, 255, 255, 255),
            PRODJLINKPORTKEEPALIVE,
            buffer,
            sizeof(buffer));

        // this2->statusSocket.beginPacket(IPAddress(255, 255, 255, 255), PRODJLINKPORTKEEPALIVE);
        // this2->statusSocket.write(buffer, sizeof(buffer));
        // this2->statusSocket.endPacket();

        this2->lastKeepAlive = millis();
    }
};
