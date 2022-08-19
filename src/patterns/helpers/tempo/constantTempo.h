#pragma once
#include "abstractTempo.h"
#include "debug.h"

#define DEFAULTPERIOD 0 // default off

class ConstantTempo : public AbstractTempo
{

public:
    static ConstantTempo *getInstance()
    {
        static ConstantTempo instance;
        return &instance;
    }

    TempoTaskType Initialize() override
    {
        sourceName = "Constant";
        return ConstantTempoTask;
    }

    void setBpm(int bpm)
    {
        period = 60000000/bpm;
        startingpoint = micros();
        validSignal = true;
    }

private:
    int period = DEFAULTPERIOD;     
    unsigned long startingpoint;
    
    // private constructors, singleton
    ConstantTempo() : AbstractTempo() {}
    ConstantTempo(ConstantTempo const &);       // Don't Implement
    void operator=(ConstantTempo const &); // Don't implement

    static void ConstantTempoTask()
    {
        ConstantTempo *instance = ConstantTempo::getInstance();
        if (!instance->validSignal || instance->period == 0)
            return;

        int newBeatNr = (micros() - instance->startingpoint) / instance->period;
        if (newBeatNr != instance->beatNumber)
            instance->beat(newBeatNr, instance->period / 1000, false); //use the beat locally, but don't broadcast it.
    }
};
