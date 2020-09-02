#pragma once

#include <inttypes.h>

const int flashrows = 3;
const int width = 8;
const int height = 8;
const int midichannel = 0; //0 = channel 1

const int numfaders = 9;
const int firstfader = 48; //controller number of the first fader

//This class interfaces the APCmini. It creates 8 columns each of which work independently.
//You can select a cell for each column, by pressing the button. A led will indicate the
//selected cell. The bottom 3 rows will be in flash mode, which means that they release
//when you release the button.
class APCMini
{

public:
    static void Initialize();

    //(0,0) = top left, (7,7) is bottom right.
    static bool getStatus(uint8_t col, uint8_t row);

    //get fader position (0-127)
    static uint8_t getFader(uint8_t col);
    
private:
    APCMini();
    static void handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
    static void handleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
    static void handleControllerChange(uint8_t channel, uint8_t controller, uint8_t value);
    static void handleConnect();

    static bool allFlash;
    static bool allFlashStatus[width * height];
    static bool allStatus[width * height];
    static uint8_t faders[numfaders];

    static void handleKeyPress(uint8_t note, bool ison);
    static void toggleFlashAll();
    static void setAllOff();
    static void setNote(uint8_t note, bool ison, bool force);
    static void releaseGroup(uint8_t note, bool recursiveCall);

    static void displayStatusTask(void *pvParameters);
    static void displayStatus();
    static void displayButtonStatus(int8_t button);
    static void displayBottomStatus();
    static void displaySideStatus();

    static bool patternMode;
    static uint8_t patterns[width*height];
    static int8_t currentlyEditingPattern;
    static void togglePatternMode();
    static void switchPattern(uint8_t pat);
    static void togglePatternBeat(uint8_t beat);
};
