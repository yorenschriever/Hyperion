#include "apcmini.h"
#include "../midi/midi.h"

void APCMini::Initialize()
{
    Midi::Initialize();
    Midi::onNoteOn(handleNoteOn);
    Midi::onNoteOff(handleNoteOff);
    Midi::onControllerChange(handleControllerChange);
}

void APCMini::handleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
    handleNoteOn(channel, note, 0);
}

void APCMini::handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
    Midi::sendNoteOn(channel,note,velocity > 0 ? 100:0);
    Midi::waitTxDone();
}

void APCMini::handleControllerChange(uint8_t channel, uint8_t note, uint8_t velocity){

}