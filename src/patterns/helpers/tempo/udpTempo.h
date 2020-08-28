#pragma once
#include "abstractTempo.h"
#include "tempo.h"
#include "debug.h"
#include "hardware/UDPFast/UDPFast.h"
#include "hardware/ethernet/ethernet.h"
#include <ETH.h>

#define UDPTEMPOPORT 50010

#define PACKETID 0x12

struct UdpTempoPacket
{
    uint8_t id;
    uint8_t version;
    int beatNumber;
    int timeBetweenBeats;
};

class UdpTempo : public AbstractTempo
{

public:
    static UdpTempo *getInstance()
    {
        static UdpTempo instance;
        return &instance;
    }

    TempoTaskType Initialize() override
    {
        sourceName = "UDPTempo";
        socket.begin(UDPTEMPOPORT);
        return UDPTempoTask;
    }

    static void broadcastBeat()
    {
        UdpTempoPacket packet;
        packet.id = PACKETID;
        packet.beatNumber = Tempo::GetBeatNumber();
        packet.timeBetweenBeats = Tempo::TimeBetweenBeats();
        getInstance()->socket.sendPacketFast(
            IPAddress(255, 255, 255, 255),
            UDPTEMPOPORT,
            (uint8_t *)&packet,
            sizeof(packet));
    }

private:
    UDPFast socket;

    UdpTempo() : AbstractTempo() {}
    UdpTempo(UdpTempo const &);       // Don't Implement
    void operator=(UdpTempo const &); // Don't implement

    UdpTempoPacket recvPacket;

    static void UDPTempoTask()
    {
        UdpTempo *this2 = UdpTempo::getInstance();

        int len = this2->socket.waitPacketFast((uint8_t *)&this2->recvPacket, sizeof(UdpTempoPacket));
        if (len == sizeof(UdpTempoPacket) && this2->recvPacket.id == PACKETID)
        {
            this2->validSignal = true;
            this2->beat(this2->recvPacket.beatNumber, this2->recvPacket.timeBetweenBeats, false);
            Debug.printf("got udp beat %d %d\n", this2->recvPacket.beatNumber, this2->recvPacket.timeBetweenBeats);
        }
    }
};