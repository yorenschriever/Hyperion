#pragma once

#include <Arduino.h>
#include <inttypes.h>
#include "pattern.h"
#include "../colours.h"
#include "lfo.h"
#include "fade.h"

template<class T_COLOUR>
class Pattern {

public:

    //override this method if you want to complex initialization behaviour.
    //do not put such behaviour in the constructor
    virtual void Initialize(){}

    //framebuffer = the buffer to fill. contains data form the last frame that you can modify
    //width = number of pixels
    //fistFrame is true if on the first render after the pattern was selected
    virtual void Calculate(T_COLOUR* frameBuffer, int width, bool firstFrame);

};