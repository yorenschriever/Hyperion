#pragma once

class BPM
{
public:
    static BPM *GetInstance() { return instance; }
    static void SetInstance(BPM *instance)
    {
        BPM::instance = instance;
        instance->Initialize();
    }

    virtual void Initialize(){};
    virtual int GetBeatNumber() { return 0; };

    //time in ms
    virtual int TimeToNextBeat() { return -1; };

    //time in ms
    virtual int TimeSinceLastBeat() { return -1; };

private:
    static BPM *instance;
};

BPM *BPM::instance = NULL;