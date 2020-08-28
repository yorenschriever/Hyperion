#pragma once
#include "abstractTempo.h"
#include "hardware/rotary/rotary.h"
#include "debug.h"
#include "hardware/midi/midi.h"
#include "configurations/configuration.h"

//#define DEFAULTPERIOD 480000 //default at 125BPM
#define DEFAULTPERIOD 0 //default off

class TapTempo : public AbstractTempo
{

public:
    static TapTempo* getInstance()
    {
        static TapTempo instance;
        return &instance;
    }

    TempoTaskType Initialize() override
    {
        sourceName="Tap";
        Rotary::onPress([]() { TapTempo::getInstance()->Tap(); });
        Rotary::onLongPress([]() { TapTempo::getInstance()->Stop(); });
#ifdef TAPMIDINOTE //TODO proper config struct
        Midi::Initialize();
        Midi::onNoteOn([](uint8_t ch, uint8_t note, uint8_t velocity) {
            if (note == TAPMIDINOTE)
                TapTempo::getInstance()->Tap();
        });
#endif
        return TapTempoTask;
    }

private:
    const int tapTimeout = 1000000; //if no tap is detected for tapTimeout us, the next tap will be interpreted as a new tap session
    int period = DEFAULTPERIOD;     //default at 125BPM
    unsigned long startingpoint;
    unsigned long lastTap;
    unsigned long firstTap;
    int tapCount;

    //private constructors, singleton
    TapTempo() : AbstractTempo() {}
    TapTempo(TapTempo const &);         // Don't Implement
    void operator=(TapTempo const &); // Don't implement

    void Stop()
    {
        validSignal=false;
    }

    void Tap()
    {
        unsigned long now = micros();

        if (!isTapping())
        {
            //start a new tap session
            firstTap = now;
            startingpoint = now; //for now the Tempo counter starting point is linked to the first tap
            tapCount = 0;
        }

        tapCount++;
        lastTap = now;

        if (tapCount > 1)
        {
            period = (lastTap - firstTap) / (tapCount - 1);
            if (period < 1)
                period = DEFAULTPERIOD;
            validSignal=true;
        }

        timeBetweenBeats = period/1000;
    }

    bool isTapping()
    {
        return (micros() - lastTap) < tapTimeout;
    }

    static void TapTempoTask()
    {
        TapTempo* instance = TapTempo::getInstance();
        if (!instance->validSignal || instance->period==0)
            return;

        int newBeatNr = (micros() - instance->startingpoint) / instance->period;
        if (newBeatNr != instance->beatNumber)
            instance->beat(newBeatNr,instance->period/1000);
    }

};
