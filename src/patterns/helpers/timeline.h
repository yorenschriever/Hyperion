
#pragma once

#include <Arduino.h>
#include <stdint.h>

struct TimelineEvent
{
    int Moment;
};

class Timeline
{

public:
    Timeline(int repeatAfter = 0)
    {
        this->duration = repeatAfter;
        reset();
    }

    bool Happened(TimelineEvent event)
    {
        return Happened(event.Moment);
    }

    bool Happened(int moment)
    {
        if (startInterval == endInterval)
            return false;

        if (startInterval < endInterval)
            return moment >= startInterval && moment < endInterval;

        //if (startInterval > endInterval) this is always true
        return moment >= startInterval || moment < endInterval;
    }

    void FrameStart()
    {
        startInterval = endInterval;
        endInterval = GetTimelinePosition();
    }

    //get the position on the timeline in ms.
    //if duration > 0 it loops back so that the result always fall between 0 and duration
    unsigned int GetTimelinePosition()
    {
        unsigned int result = millis() - startingpoint;
        if (duration > 0)
            result = result % duration;
        return result;
    }

    void reset()
    {
        this->startingpoint = millis();
    }

protected:
    unsigned long startingpoint;
    int duration;
    unsigned int startInterval;
    unsigned int endInterval = 0;
};
