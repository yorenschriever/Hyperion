#include <stdint.h>
#include <vector>
#include "pipe.h"

struct ConfigurationStruct
{
    using paramCalculatorType = void(*)();

    const char* hostname = "hyperion";
    int pwmFrequency = 1500;

    uint8_t tapMidiNote = -1;
    uint8_t tapStopMidiNote = -1;
    uint8_t tapAlignMidiNote = -1;
    uint8_t tapBarAlignMidiNote = -1;

    std::vector<Pipe*> pipes;

    paramCalculatorType paramCalculator = NULL;
} Configuration;

