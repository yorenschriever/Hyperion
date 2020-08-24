#pragma once

#include <Arduino.h>

enum FadeEase
{
    Linear,
    Quadratic,
    Cubic
};

enum FadeDirection
{
    Up,
    Down
};

enum FadeWaitPosition
{
    WaitAtStart=0,
    WaitAtEnd=1
};

class Fade
{

private:
    unsigned long startingpoint;

    FadeEase ease;
    FadeDirection direction;
    FadeWaitPosition waitPosition;

public:
    /***
     * Creates a fader
     * ease = ease curve
     * suration = duration of the fade in ms
     * direction: up or down
     * waitPosition: when using startDelay(), should the fade wait at start (phase=0) or at the end (phase=1)
     */ 
    Fade(FadeEase ease, unsigned int duration, FadeDirection direction = Down, FadeWaitPosition waitPosition = WaitAtStart)
    {
        this->ease = ease;
        this->duration = duration;
        this->direction = direction;
        this->waitPosition = waitPosition;
        reset();
    }

    unsigned int duration;

    float getPhase() { return getPhase(0); }
    float getPhase(int startDelay)
    {
        int phase = (millis() - startingpoint - startDelay);
        if (phase < 0)
            return waitPosition;

        float result = ((float)phase / (float)duration);
        if (result > 1)
            return 1;

        return result;
    }

    //value between 0-1
    float getValue() { return getValue(0); }
    float getValue(int startDelay)
    {
        float phase = getPhase(startDelay);

        if (direction == Down)
            phase = 1 - phase;

        switch (ease)
        {
        case (Linear):
            return phase;
            break;
        case (Quadratic):
            return pow(phase, 2);
            break;
        case (Cubic):
            return pow(phase, 4);
            break;
        default:
            return phase;
        }
    }

    void reset()
    {
        this->startingpoint = millis();
    }
};