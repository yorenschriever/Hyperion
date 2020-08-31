#pragma once

#include <Arduino.h>
#include <inttypes.h>
#include "pattern.h"
#include "../colours.h"
#include "helpers/lfo.h"
#include "helpers/fade.h"
#include "helpers/interval.h"
#include "helpers/timeline.h"
#include "helpers/watcher.h"
#include "helpers/permute.h"
#include "helpers/transition.h"

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

//layered patterns work slightly different: they are executed regardless if
//they are active. The framebuffer that they receive is already filled
//with data from other patterns, and they have to mix their own layer into it.
//This method allows fade-outs and blending.
template<class T_COLOUR>
class LayeredPattern : public Pattern<T_COLOUR> {
public:
    virtual void Calculate(T_COLOUR* frameBuffer, int width, bool active);
};