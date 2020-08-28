#pragma once

#include "Arduino.h"
#include <set>

using TempoTaskType = void (*)();

class AbstractTempo;

class Tempo
{
public:
    static void AddSource(AbstractTempo *source);

    static int GetBeatNumber();

    static int TimeBetweenBeats();

    static const char *SourceName();

    //gets a value between 0-1 that increases linearly in the given interval
    //eg intervalsize=1  gives a value that fills between every beat
    //   intervalsize=4  gives a value that fills between every measure
    //   intervalsize=16 gives a value that fills between every phrase
    static float GetProgress(int intervalsize);

    static void BroadcastBeat(AbstractTempo *source);

private:
    
    static std::set<AbstractTempo *> sources;
    static std::set<TempoTaskType> tasks;
    static TaskHandle_t xHandle;
    static TempoTaskType broadcastTask;

    static void TempoTask(void *param);
    static AbstractTempo *getActive();
    static void registerTask(TempoTaskType task);
};