#pragma once

#include <inttypes.h>

const int flashrows = 3;
const int width=8;
const int height=8;
const int midichannel = 0; //0 = channel 1

class APCMini {

    public:
        static void Initialize();

    private:
        APCMini();
        static void handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
        static void handleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
        static void handleControllerChange(uint8_t channel, uint8_t note, uint8_t velocity);

        static bool allFlash;
        static bool allStatus[width*height];

        static void handleKeyPress(uint8_t note, bool ison);
        static void toggleFlashAll();
        static void setAllOff();
        static void setNote(uint8_t note, bool ison, bool force);
        static void releaseGroup(uint8_t note, bool recursiveCall);

};
