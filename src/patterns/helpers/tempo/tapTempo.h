#pragma once
#include "abstractTempo.h"
#include "hardware/rotary/rotary.h"
#include "debug.h"

//#define DEFAULTPERIOD 480000 //default at 125BPM
#define DEFAULTPERIOD 0 // default off

class TapTempo : public AbstractTempo
{

public:
    static TapTempo *getInstance()
    {
        static TapTempo instance;
        return &instance;
    }

    TempoTaskType Initialize() override
    {
        sourceName = "Tap";
        return TapTempoTask;
    }

    void Stop()
    {
        validSignal = false;
        beatNumber = -1;
        period = 0;
    }

    // this aligns the tempo to your tap without resetting the beat count or interval
    void Align()
    {
        if (period == 0)
            return;

        long offset = (micros() - startingpoint) % period;
        if (offset > period / 2)
            offset -= period; // syncing to the other side is closer
        startingpoint += offset;
    }

    //
    // void BarAlign()
    // {
    //     unsigned long timepassed = micros()-startingpoint;
    //     unsigned long timeinbar = timepassed % (period*4);
    //     startingpoint = micros()-timeinbar; //micros() - timesincebarstart;
    // }

    void Tap()
    {
        unsigned long now = micros();

        if (!isTapping())
        {
            // start a new tap session
            firstTap = now;
            startingpoint = now; // for now the Tempo counter starting point is linked to the first tap
            tapCount = 0;
            validSignal = true;
            beat(0, period / 1000); // this wont be visible, because if there is no signal, the beatnumber will be 0, so setting it to 0 again doesnt trigger the watcher
        }

        tapCount++;
        lastTap = now;

        if (tapCount > 1)
        {
            period = (lastTap - firstTap) / (tapCount - 1);
            if (period < 1)
                period = DEFAULTPERIOD;
            validSignal = true;
        }

        timeBetweenBeats = period / 1000;
    }

private:
    const int tapTimeout = 1000000; // if no tap is detected for tapTimeout us, the next tap will be interpreted as a new tap session
    int period = DEFAULTPERIOD;     // default at 125BPM
    unsigned long startingpoint;
    unsigned long lastTap;
    unsigned long firstTap;
    int tapCount;

    // private constructors, singleton
    TapTempo() : AbstractTempo() {}
    TapTempo(TapTempo const &);       // Don't Implement
    void operator=(TapTempo const &); // Don't implement

    bool isTapping()
    {
        return (micros() - lastTap) < tapTimeout;
    }

    static void TapTempoTask()
    {
        TapTempo *instance = TapTempo::getInstance();
        if (!instance->validSignal || instance->period == 0)
            return;

        int newBeatNr = (micros() - instance->startingpoint) / instance->period;
        if (newBeatNr != instance->beatNumber)
            instance->beat(newBeatNr, instance->period / 1000);
    }
};
