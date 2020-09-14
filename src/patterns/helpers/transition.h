#pragma once
#include "fade.h"


using FadeShape = int(*)(int, int, int);

class Transition
{
    
public:
    //use this class to animate specific animations at the start and the end of 
    //a pattern.
    //mostly used to fade in/out layered patterns 
    Transition(
        unsigned int fadeInDuration,
        FadeShape fadeInShape,
        unsigned int fadeInShapeSize,
        unsigned int fadeOutDuration,
        FadeShape fadeOutShape,
        unsigned int fadeOutShapeSize
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

    Transition(
        unsigned int fadeInDuration=200,
        unsigned int fadeOutDuration=200
    )
    {
        fadein.duration = fadeInDuration;
        fadeout.duration = fadeOutDuration;
        this->fadeInShapeSize = 0;
        this->fadeOutShapeSize = 0;
        this->fadeInShape = none;
        this->fadeOutShape = none;
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

    float getValue(){
        return fadein.getValue() * (lastActive?1:fadeout.getValue());
    }

    float getValue(int index, int width){
        return fadein.getValue(fadeInShape(index,width,fadeInShapeSize)) * (lastActive?1:fadeout.getValue(fadeOutShape(index,width,fadeOutShapeSize)));
    }



    static int none(int index, int width, int delay){ return 0;}
    static int fromRight(int index, int width, int delay){ return delay * index / width;}
    static int fromLeft(int index, int width, int delay){return delay * (width-index) / width;}
    static int fromCenter(int index, int width, int delay){return (delay * std::abs(width - 2*index)) / width;}
    static int fromSides(int index, int width, int delay){return (delay * std::abs(2*index-width)) / width;}


private:
    Fade<Up> fadein= Fade<Up> (0);
    Fade<Down> fadeout= Fade<Down>(0);
    bool lastActive;

    int fadeInShapeSize;
    int fadeOutShapeSize;

    FadeShape fadeInShape=none;
    FadeShape fadeOutShape=none;
};