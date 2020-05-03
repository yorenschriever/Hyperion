#pragma once

#include <inttypes.h>

const int flashrows = 3;
const int width=8;
const int height=8;
const int midichannel = 0; //0 = channel 1

const int numfaders=9;
const int firstfader=48; //controller number of the first fader

class APCMini {

    public:
        static void Initialize();

        //(0,0) = top left, (7,7) is bottom right.
        static bool getStatus(uint8_t col, uint8_t row);
        static uint8_t getFader(uint8_t col);

static void handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
        

    private:
        APCMini();
        //static void handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
        static void handleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
        static void handleControllerChange(uint8_t channel, uint8_t controller, uint8_t value);

        static bool allFlash;
        static bool allStatus[width*height];
        static uint8_t faders[numfaders];

        static void handleKeyPress(uint8_t note, bool ison);
        static void toggleFlashAll();
        static void setAllOff();
        static void setNote(uint8_t note, bool ison, bool force);
        static void releaseGroup(uint8_t note, bool recursiveCall);

};
