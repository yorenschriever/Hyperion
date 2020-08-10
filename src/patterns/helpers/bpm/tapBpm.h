#pragma once
#include "bpm.h"
#include "hardware/rotary/rotary.h"
#include "debug.h"

//#define DEFAULTPERIOD 480000 //default at 125BPM
#define DEFAULTPERIOD 0 //default off

class TapBPM : public BPM
{

public:
    void Initialize() override
    {
        Rotary::onPress([]() { ((TapBPM *)BPM::GetInstance())->Tap(); });
    }

    int GetBeatNumber() override
    {
        if (period==0)
            return 0;
        return (micros() - startingpoint) / period;
    }

    int TimeToNextBeat() override { 
        if (period==0)
            return -1;
        return (period - ((micros() - startingpoint) % period)) / 1000;
    }

    int TimeSinceLastBeat() override { 
        if (period==0)
            return -1;
        return ((micros() - startingpoint) % period) / 1000;
    }

private:
    const int tapTimeout = 1000000; //if no tap is detected for tapTimeout us, the next tap will be interpreted as a new tap session
    int period = DEFAULTPERIOD; //default at 125BPM
    unsigned long startingpoint;
    unsigned long lastTap;
    unsigned long firstTap;
    int tapCount;
    bool isbeat;
    int lastBeatNr;

    void Tap()
    {
        unsigned long now = micros();

        if (!isTapping())
        {
            //start a new tap session
            firstTap = now;
            startingpoint = now; //for now the BPM counter starting point is linked to the first tap
            tapCount = 0;
        }

        tapCount++;
        lastTap = now;

        if (tapCount > 1)
        {
            period = (lastTap - firstTap) / (tapCount - 1);
            if (period < 1)
                period = DEFAULTPERIOD;
        }
    }

    bool isTapping()
    {
        return (micros() - lastTap) < tapTimeout;
    }
};
