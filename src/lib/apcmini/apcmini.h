#pragma once

#include <inttypes.h>

class APCMini {

    public:
        static void Initialize();

    private:
        APCMini();
        static void handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
        static void handleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
        static void handleControllerChange(uint8_t channel, uint8_t note, uint8_t velocity);

};