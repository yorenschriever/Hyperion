
#pragma once

#include <Arduino.h>
#include <stdint.h>

template <class T_INTERVALTYPE>
class Interval
{

public:
    //create a stochastic interval. the provided average is in ms
    Interval(int averageInterval = 1000)
    {
        this->averageInterval = averageInterval;
        this->lastTime = millis();
    }

    void setAverageInterval(int averageInterval)
    {
        this->averageInterval = averageInterval;
    }

    bool Happened()
    {
        unsigned long now = millis();
        if (now - lastTime < currentDuration) //this method should handle the millis overflow correctly
            return false;

        lastTime = now;
        currentDuration = T_INTERVALTYPE::getRandomDuration(averageInterval);
        return true;
    }

protected:
    unsigned long lastTime;
    int currentDuration;
    int averageInterval;
};

class ConstantInterval
{
public:
    static int getRandomDuration(int averageInterval)
    {
        return averageInterval;
    }
};

class RandomInterval
{
public:
    static int getRandomDuration(int averageInterval)
    {
        return random(0, 2 * averageInterval);
    }
};

class PoissonInterval
{
public:
    static int getRandomDuration(int averageInterval)
    {
        float uniformRandom = random(0, 50000) / 50000.;
        float randomTime = -1 * log(1. - uniformRandom);
        return randomTime * averageInterval;
    }
};