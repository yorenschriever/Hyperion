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

class Fade
{

private:
    unsigned long startingpoint;

    FadeEase ease;
    FadeDirection direction;

public:
    Fade(FadeEase ease, unsigned int duration, FadeDirection direction)
    {
        this->ease = ease;
        this->duration = duration;
        this->direction = direction;
        reset();
    }

    unsigned int duration;

    float getPhase() { return getPhase(0); }
    float getPhase(int startDelay)
    {
        int phase = (millis() - startingpoint - startDelay);
        if (phase < 0)
            return 0;

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