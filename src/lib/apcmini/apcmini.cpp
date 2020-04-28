#include "apcmini.h"
#include "../midi/midi.h"

bool APCMini::allFlash=false;
bool APCMini::allStatus[width*height];
uint8_t APCMini::faders[numfaders];

void APCMini::Initialize()
{
    Midi::Initialize();
    Midi::onNoteOn(handleNoteOn);
    Midi::onNoteOff(handleNoteOff);
    Midi::onControllerChange(handleControllerChange);

    for (int i=0;i<numfaders; i++)
        faders[i]=127;
}

void APCMini::handleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
    if (channel != midichannel)
        return;
    handleKeyPress(note,false);
}

void APCMini::handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
    if (channel != midichannel)
        return;
    handleKeyPress(note,velocity>0);
}

void APCMini::handleControllerChange(uint8_t channel, uint8_t controller, uint8_t value){

    if (channel != midichannel)
        return;
    int fadernum = controller - firstfader;
    if (fadernum < 0 || fadernum >= numfaders)
        return;
    faders[fadernum] = value;
}


void APCMini::handleKeyPress(uint8_t note, bool ison)
{

    if (note == 82)
    {
        if (ison)
            toggleFlashAll();
        return;
    }

    int flashBorder = allFlash ? (height * width) : (flashrows * width);
    if (note < flashBorder){
        setNote(note,ison, false);
    } else if (ison && note <= (height+1)*width) { 
        //a button was pressed, not released 
        if (note >= height*width) 
        {
            //stop buttons
            releaseGroup(note,false);        
        } 
        else 
        {
            //other patterns
            boolean newstat = !allStatus[note];
            releaseGroup(note,false);
            setNote(note, newstat, false);
        }
    }
}

void APCMini::toggleFlashAll()
{
    allFlash = !allFlash;
    Midi::sendNoteOn(midichannel, 82,allFlash*100);
    Midi::waitTxDone(); 
    setAllOff();
}

void APCMini::setAllOff()
{
    for (byte note=0; note<height*width; note++)
        setNote(note,false,true);
}

void APCMini::setNote(uint8_t note, bool ison, bool force)
{
    if (allStatus[note] == ison && !force)
        return;

    allStatus[note] = ison;

    //set led on APC
    Midi::sendNoteOn(midichannel, note,ison*100);
    Midi::waitTxDone(); 
    //MIDI.sendNoteOn(note, ison*100, 1); 
    //delay(2); //must be 2, 1 doesnt work
}

void APCMini::releaseGroup(uint8_t note, bool recursiveCall)
{
    byte bottom = note%width;
    for (byte i=bottom; i < height*width; i+=width)
        setNote(i,false, false);
}

bool APCMini::getStatus(uint8_t col, uint8_t row)
{
    if (col >= width || row >= height) 
        return false;
    return allStatus[(height-row-1)*width+col];
}

uint8_t APCMini::getFader(uint8_t col)
{
    if (col >= numfaders)
        return 0;
    return faders[col];
}