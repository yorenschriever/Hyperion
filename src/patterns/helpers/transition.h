#pragma once
#include "fade.h"

int none(int index, int width, int delay){ return 0;}
int fromRight(int index, int width, int delay){ return delay * index / width;}
int fromLeft(int index, int width, int delay){return delay * (width-index) / width;}
int fromCenter(int index, int width, int delay){return (delay * std::abs(width - 2*index)) / width;}
int fromSides(int index, int width, int delay){return (delay * std::abs(2*index-width)) / width;}

using FadeShape = int(*)(int, int, int);

class Transition
{
    
public:
    //use this class to animate specific animations at the start and the end of 
    //a pattern.
    //mostly used to fade in/out layered patterns 
    Transition(
        unsigned int fadeInDuration=200,
        FadeShape fadeInShape=none,
        unsigned int fadeInShapeSize=0,
        unsigned int fadeOutDuration=200,
        FadeShape fadeOutShape=none,
        unsigned int fadeOutShapeSize=0
    )
    {
        fadein.duration = fadeInDuration;
        fadeout.duration = fadeOutDuration;
        this->fadeInShapeSize = fadeInShapeSize;
        this->fadeOutShapeSize = fadeOutShapeSize;
        this->fadeInShape = fadeInShape;
        this->fadeOutShape = fadeOutShape;
        lastActive=false;
    }

    bool Calculate(bool active){
        if (active && !lastActive)
            fadein.reset();
        if (!active && lastActive)
            fadeout.reset();
        lastActive = active;

        //let the pattern know if getValue is going to return anything other than 0,
        //if not, the pattern can early exit pattern calculation
        return active || fadeout.getValue(fadeOutShapeSize) > 0;
    }

    float getValue(int index, int width){
        return fadein.getValue(fadeInShape(index,width,fadeInShapeSize)) * (lastActive?1:fadeout.getValue(fadeOutShape(index,width,fadeOutShapeSize)));
    }

private:
    Fade<Up> fadein= Fade<Up> (0);
    Fade<Down> fadeout= Fade<Down>(0);
    bool lastActive;

    int fadeInShapeSize;
    int fadeOutShapeSize;

    FadeShape fadeInShape=none;
    FadeShape fadeOutShape=none;
};