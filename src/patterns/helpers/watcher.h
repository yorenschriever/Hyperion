#pragma once

#include "tempo/tempo.h"

enum WatcherType
{
    Rising,
    Falling,
    Changing
};

template <class T>
class Watcher
{

    using WatcherFunc = T (*)();

public:
    Watcher(WatcherFunc getValue, WatcherType type)
    {
        this->type = type;
        this->getValue = getValue;
    }

    bool Triggered()
    {
        T value = getValue();
        bool result = false;
        if ((type == Rising || type == Changing) && value > lastValue)
            result = true;
        if ((type == Falling || type == Changing) && value < lastValue)
            result = true;
        lastValue = value;
        return result;
    }

private:
    T lastValue;
    WatcherFunc getValue;
    WatcherType type;
};

class TempoWatcher : public Watcher<int>
{
public: 
    TempoWatcher() : 
        Watcher(
            Tempo::GetBeatNumber ,
            Changing
        )
    {}
};