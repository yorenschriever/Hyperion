#include "apcmini.h"
#include "../midi/midi.h"
#include "debug.h"
#include "patterns/helpers/tempo/tempo.h"

bool APCMini::allFlash = false;
bool APCMini::allStatus[width * height];
bool APCMini::allFlashStatus[width * height];
uint8_t APCMini::faders[numfaders];
bool APCMini::patternMode = false;
uint8_t APCMini::patterns[width * height];
int8_t APCMini::currentlyEditingPattern = -1;

#define FLASHALLNOTE 82
#define PATTERNMODENOTE 83

void APCMini::Initialize()
{
    for (int i = 0; i < width * height; i++)
        allStatus[i] = false;
    for (int i = 0; i < numfaders; i++)
        faders[i] = 127;

    Midi::Initialize();
    Midi::onNoteOn(handleNoteOn);
    Midi::onNoteOff(handleNoteOff);
    Midi::onControllerChange(handleControllerChange);
    Midi::onConnect(handleConnect);
}

void APCMini::handleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
    if (channel != midichannel)
        return;
    handleKeyPress(note, false);
}

void APCMini::handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
    if (channel != midichannel)
        return;
    handleKeyPress(note, velocity > 0);
}

void APCMini::handleConnect()
{
    //send all note statusses on connect, so the lights correspond to the active patterns after disconnecting/reconnecting
    xTaskCreate(displayStatusTask, "APCMini status", 2048, NULL, 1, NULL);
}

void APCMini::displayStatusTask(void *pvParameters)
{
    displayStatus();
    displaySideStatus();
    displayBottomStatus();
    vTaskDelete(NULL);
}

void APCMini::displayStatus()
{
    for (byte i = 0; i < height * width; i++)
    {
        displayButtonStatus(i);
        if (i % 8 == 0)
            vTaskDelay(10); //give things some time to breathe
    }
}

void APCMini::displayButtonStatus(int8_t button)
{
    if (button < 0 || button > width * height)
        return;

    uint8_t ledvalue = 0;
    if (allFlashStatus[button])
        ledvalue = 1;
    else if (!allFlash && patternMode && currentlyEditingPattern == button) //we are editing this pattern
        ledvalue = 6;                                                       //blinking orange
    else if (allStatus[button])                                             //note is on
        ledvalue = 1;                                                       //green
    else if (patterns[button] > 0)                                          //this note has a pattern
        ledvalue = 5;                                                       //orange

    Midi::sendNoteOn(midichannel, button, ledvalue);
    Midi::waitTxDone();
}

void APCMini::displayBottomStatus()
{
    if (patternMode && currentlyEditingPattern != -1)
    {
        for (byte beat = 0; beat < 8; beat++)
        {
            Midi::sendNoteOn(midichannel, beat + 64, (patterns[currentlyEditingPattern] & (1 << beat)) ? 1 : 0);
            Midi::waitTxDone();
        }
        return;
    }

    for (byte beat = 0; beat < 8; beat++)
    {
        Midi::sendNoteOn(midichannel, beat + 64, 0);
        Midi::waitTxDone();
    }
}

void APCMini::displaySideStatus()
{
    Midi::sendNoteOn(midichannel, 82, allFlash);
    Midi::waitTxDone();

    Midi::sendNoteOn(midichannel, 83, patternMode);
    Midi::waitTxDone();
    for (byte i = 84; i <= 89; i++)
    {
        Midi::sendNoteOn(midichannel, i, 0);
        Midi::waitTxDone();
        if (i % 8 == 0)
            vTaskDelay(10); //give things some time to breathe
    }
}

void APCMini::handleControllerChange(uint8_t channel, uint8_t controller, uint8_t value)
{

    if (channel != midichannel)
        return;
    int fadernum = controller - firstfader;
    if (fadernum < 0 || fadernum >= numfaders)
        return;
    faders[fadernum] = value;
}

void APCMini::handleKeyPress(uint8_t note, bool ison)
{
    if (note == PATTERNMODENOTE)
    {
        if (ison)
            togglePatternMode();
        return;
    }

    if (note == FLASHALLNOTE)
    {
        if (ison)
            toggleFlashAll();
        return;
    }

    if (allFlash && note < width * height)
    {
        allFlashStatus[note] = ison;
        displayButtonStatus(note);
        return;
    }

    if (patternMode)
    {
        if (!ison)
            return;
        if (note < width * height)
            switchPattern(note);
        else if (note >= 64 && note <= 71)
            togglePatternBeat(note - 64);
        return;
    }

    int flashBorder = (flashrows * width);
    if (note < flashBorder)
    {
        setNote(note, ison, false);
    }
    else if (ison && note <= (height + 1) * width)
    {
        //a button was pressed, not released
        if (note >= height * width)
        {
            //stop buttons
            releaseGroup(note, false);
        }
        else
        {
            //other patterns
            boolean newstat = !allStatus[note];
            releaseGroup(note, false);
            setNote(note, newstat, false);
        }
    }
}

void APCMini::toggleFlashAll()
{
    allFlash = !allFlash;
    displaySideStatus();
    displayStatus();
}

void APCMini::setAllOff()
{
    for (byte note = 0; note < height * width; note++)
        setNote(note, false, true);
}

void APCMini::setNote(uint8_t note, bool ison, bool force)
{
    if (allStatus[note] == ison && !force)
        return;

    allStatus[note] = ison;

    displayButtonStatus(note);
}

void APCMini::releaseGroup(uint8_t note, bool recursiveCall)
{
    byte bottom = note % width;
    for (byte i = bottom; i < height * width; i += width)
        setNote(i, false, false);
}

bool APCMini::getStatus(uint8_t col, uint8_t row)
{
    if (col >= width || row >= height)
        return false;
    uint8_t note = (height - row - 1) * width + col;

    if (allFlashStatus[note]) //currently being flashed
        return true;

    if (allStatus[note])
        return true; //normally on

    if (patterns[note] > 0) //currently triggered by a pattern
    {
        uint8_t beat = (Tempo::GetBeatNumber() / 4) % 8;
        if (patterns[note] & (1 << beat))
            return true;
    }

    return false;
}

uint8_t APCMini::getFader(uint8_t col)
{
    if (col >= numfaders)
        return 0;
    return faders[col];
}

void APCMini::togglePatternMode()
{
    patternMode = !patternMode;
    displaySideStatus();
    displayBottomStatus();
    displayStatus();
}

void APCMini::switchPattern(uint8_t pat)
{
    if (pat >= width * height)
        return;

    uint8_t oldpat = currentlyEditingPattern;
    currentlyEditingPattern = pat;

    displayButtonStatus(oldpat);
    displayButtonStatus(currentlyEditingPattern);
    displayBottomStatus();
}

void APCMini::togglePatternBeat(uint8_t beat)
{
    if (beat >= 8)
        return;

    patterns[currentlyEditingPattern] ^= (1 << beat);

    displayBottomStatus();
}