#pragma once
#include "abstractTempo.h"
#include "debug.h"
#include "hardware/midi/midi.h"

#define MIDICLOCKSTART 0xFA
#define MIDICLOCKSTOP 0xFC
#define MIDICLOCKCONTINUE 0xFB
#define MIDICLOCKTICK 0xF8

class MidiClockTempo : public AbstractTempo
{

public:
    static MidiClockTempo *getInstance()
    {
        static MidiClockTempo instance;
        return &instance;
    }

    TempoTaskType Initialize() override
    {
        sourceName = "Midi clock";

        Midi::Initialize();
        Midi::onSystemRealtime([](uint8_t msg) {
            MidiClockTempo *instance = MidiClockTempo::getInstance();
            if (msg == MIDICLOCKCONTINUE)
            {
                instance->stopped=false;
                instance->validSignal=true;
                instance->tickCount=0;
                //instance->beat(0);
                Debug.println("continue. aligned beat");
            }
            else if (msg == MIDICLOCKSTART)
            {
                instance->stopped=false;
                instance->validSignal=true;
                instance->tickCount=0;
                instance->beat(0);
                Debug.println("start. reset beat to 0");
            }
            else if (msg == MIDICLOCKSTOP)
            {
                if (instance->stopped){
                    instance->beat(-1); //reset the beatnumber, so we dont get an extra flash when the signal valid timeout resets the beat to 0
                    instance->validSignal=false;
                    Debug.println("stopped 2x. stopped signal");
                }
                instance->tickCount=0;
                instance->stopped=true;
                Debug.println("stop. aligned beat.");
            }
            else if (msg == MIDICLOCKTICK)
            {
                if (!instance->validSignal)
                    return;
                instance->tickCount = (instance->tickCount + 1) % 24;
                if (instance->tickCount == 0)
                {
                    instance->beat();
                }
            }
        });

        return NULL;
    }

private:
    int tickCount = 0;
    bool stopped=true;

    //private constructors, singleton
    MidiClockTempo() : AbstractTempo() {}
    MidiClockTempo(MidiClockTempo const &); // Don't Implement
    void operator=(MidiClockTempo const &); // Don't implement
};
