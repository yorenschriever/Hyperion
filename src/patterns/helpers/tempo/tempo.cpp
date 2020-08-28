#include "tempo.h"
#include "udpTempo.h"
#include "abstractTempo.h"

void Tempo::BroadcastBeat(AbstractTempo *source)
{
    if (getActive() != source)
        return;
    UdpTempo::getInstance()->broadcastBeat();
}

void Tempo::AddSource(AbstractTempo *source)
{
    sources.insert(source);
    registerTask(source->Initialize());
}

int Tempo::GetBeatNumber()
{
    AbstractTempo *source = getActive();
    if (source)
        return source->GetBeatNumber();
    return 0;
};

int Tempo::TimeBetweenBeats()
{
    AbstractTempo *source = getActive();
    if (source)
        return source->TimeBetweenBeats();
    return 0;
};

const char *Tempo::SourceName()
{
    AbstractTempo *source = getActive();
    if (source)
        return source->Name();
    return "None";
}

//gets a value between 0-1 that increases linearly in the given interval
//eg intervalsize=1  gives a value that fills between every beat
//   intervalsize=4  gives a value that fills between every measure
//   intervalsize=16 gives a value that fills between every phrase
float Tempo::GetProgress(int intervalsize)
{
    AbstractTempo *source = getActive();
    if (!source)
        return 0;

    if (source->TimeBetweenBeats() == 0)
        return 0;

    float coarse = float(source->GetBeatNumber() % intervalsize);
    float fine = float(millis() - source->TimeOfLastBeat()) / float(source->TimeBetweenBeats());
    float result = (coarse + fine) / intervalsize;

    if (result < 0)
        return 0;
    if (result > 1)
        return 1;
    return result;
}

AbstractTempo *Tempo::getActive()
{
    for (auto &&source : sources)
        if (source->HasSignal())
            return source;
    return NULL;
}

void Tempo::TempoTask(void *param)
{
    while (true)
    {
        for (auto &&task : tasks)
            task();
        delay(1);
    }
}

void Tempo::registerTask(TempoTaskType task)
{
    if (task == NULL)
        return;

    tasks.insert(task);
    if (!xHandle)
        xTaskCreatePinnedToCore(TempoTask, "TempoTask", 5000, NULL, 1, NULL, 0);
}

std::set<AbstractTempo *> Tempo::sources;
std::set<TempoTaskType> Tempo::tasks;
TaskHandle_t Tempo::xHandle = NULL;


