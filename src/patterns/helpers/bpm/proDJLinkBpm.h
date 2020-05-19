#pragma once
#include "bpm.h"
#include "hardware/UDPFast/UDPFast.h"
#include "debug.h"

#define PRODJLINKPORT 50001

class ProDJLinkBPM : public BPM
{

public:
    void Initialize() override
    {
        udpFast.begin(PRODJLINKPORT);
        xTaskCreatePinnedToCore(ProDJLinkTask, "ProDJLinkTask", 1500, this, 1, NULL, 1);;
    }

    int GetBeatNumber() override
    {
        return beatnumber;
    }

    int TimeToNextBeat() override { 
        
    }

    int TimeSinceLastBeat() override { 
        
    }

private:
    UDPFast udpFast;
    volatile int beatnumber = 0;

    static void ProDJLinkTask(void *param) 
    {
        ProDJLinkBPM *this2 = (ProDJLinkBPM *)param;
        uint8_t buffer[0x60];
        while(true){
            //https://djl-analysis.deepsymmetry.org/djl-analysis/beats.html#_footnoteref_1
            int len = this2->udpFast.waitPacketFast(buffer);
            if (len == 0x60 && buffer[0x21]==0x21 && buffer[0x0a]==0x28){
                this2->beatnumber = buffer[0x5c];
            }
        }
    }
};
