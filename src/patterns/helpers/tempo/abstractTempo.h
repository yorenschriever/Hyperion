#pragma once

#include "Arduino.h"
#include "tempo.h"

class AbstractTempo
{
public:
    virtual TempoTaskType Initialize();
    
    int GetBeatNumber() { return beatNumber; }

    //time in ms
    int TimeBetweenBeats() { return timeBetweenBeats; }

    //time in ms
    int TimeOfLastBeat() { return timeOfLastBeat; }

    bool HasSignal() { return validSignal && millis() - timeOfLastBeat < 2000; }

    const char* Name() { return sourceName; }

protected:
    int beatNumber=-1;
    int timeBetweenBeats;
    unsigned long timeOfLastBeat;
    unsigned long timeOfSecondToLastBeat;
    const char * sourceName="";
    bool validSignal=false;

    void beat()
    {
        beat(beatNumber+1);
    }

    void beat(int beatNumber, int timeBetweenBeats=0, bool broadcast=true)
    {
        this->beatNumber = beatNumber;
        this->timeOfSecondToLastBeat = this->timeOfLastBeat;
        this->timeOfLastBeat = millis();
        this->timeBetweenBeats = timeBetweenBeats ? timeBetweenBeats : this->timeOfLastBeat - this->timeOfSecondToLastBeat;

        if (broadcast)
            Tempo::BroadcastBeat(this);
    }
};
