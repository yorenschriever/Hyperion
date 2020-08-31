#pragma once
#include "pattern.h"

namespace Layered
{

class SinPattern : public LayeredPattern<Monochrome>
{
    int direction;

    Transition transition = Transition(
        400,none,0,
        400,none,0
    );

public:
    SinPattern(int direction=1){
        this->direction=direction;
    }

    inline void Calculate(Monochrome *pixels, int width, bool active) override
    {
        if (!transition.Calculate(active))
            return; //the fade out is done. we can skip calculating pattern data

        for (int index = 0; index < width; index++)
            pixels[index] += Monochrome((uint8_t)(127.f + 127.f * cos(float(index) / float(width) * (2*3.1415) - direction * millis() / 250.))) * transition.getValue(index,width);
    }

};

class SlowStrobePattern : public LayeredPattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool active) override
    {
        if (!active)
            return;

        for (int index = 0; index < width; index++)
            pixels[index] = millis() % 100 < 25 ? 255 : 0;
    }
};



class BlinderPattern : public LayeredPattern<Monochrome>
{
    public:
    BlinderPattern(FadeShape in=none, FadeShape out=none)
    {
        transition = Transition(
            100,in,200,
            600,out,500
        );
    } 
     
protected:
    Transition transition;

    inline void Calculate(Monochrome *pixels, int width, bool active) override
    {
        if (!transition.Calculate(active))
            return; //the fade out is done. we can skip calculating pattern data

        for (int index = 0; index < width; index++)
            pixels[index] += Monochrome(255) * transition.getValue(index,width);
    }
};



class BeatAllFadePattern : public LayeredPattern<Monochrome>
{
    TempoWatcher watcher = TempoWatcher();
    FadeDown fader = FadeDown(100);

    inline void Calculate(Monochrome *pixels, int width, bool active) override
    {
        if (!active && fader.getValue()==0)
            return;

        fader.duration = Midi::controllerValue(49)*2+50;

        if (watcher.Triggered())
            fader.reset();

        for (int index = 0; index < width; index++)
            pixels[index] += 255 * fader.getValue();
    }
};

class BeatShakePattern : public LayeredPattern<Monochrome>
{
    TempoWatcher watcher = TempoWatcher();
    FadeDown fader = FadeDown(150, WaitAtEnd);
    Permute perm = Permute(0);
    
    inline void Calculate(Monochrome *pixels, int width, bool active) override
    {
        if (!active && fader.getValue(width*10)==0)
            return; 

        perm.setSize(width);

        if (watcher.Triggered())
        {
            perm.permute();
            fader.reset();
        }

        for (int index = 0; index < width; index++)
            pixels[index] += 255 * fader.getValue(perm.at[index]*10);
    }
};

class BeatSingleFadePattern : public LayeredPattern<Monochrome>
{
    TempoWatcher watcher = TempoWatcher();
    FadeDown fader = FadeDown(200, WaitAtEnd);
    int current=0;

    inline void Calculate(Monochrome *pixels, int width, bool active) override
    {
        if (!active && fader.getValue()==0)
            return;

        if (watcher.Triggered())
        {
            current = random(width);
            fader.reset();
        }

        for (int index = 0; index < width; index++)
            pixels[index] += index==current ? 255 * fader.getValue() : 0;
    }
};

class BeatMultiFadePattern : public LayeredPattern<Monochrome>
{
    TempoWatcher watcher = TempoWatcher();
    FadeDown fader = FadeDown(200, WaitAtEnd);
    Permute perm = Permute(0);

    inline void Calculate(Monochrome *pixels, int width, bool active) override
    {
        if (!active && fader.getValue()==0)
            return;

        perm.setSize(width);

        if (watcher.Triggered())
        {
            perm.permute();
            fader.reset();
        }

        for (int index = 0; index < width; index++)
            pixels[index] += perm.at[index]<width/2 ? 255 * fader.getValue() : 0;
    }
};

class GlitchPattern : public LayeredPattern<Monochrome>
{
    Timeline timeline = Timeline(25);
    Permute perm = Permute(0);

    inline void Calculate(Monochrome *pixels, int width, bool active) override
    {
        if (!active)
            return;

        timeline.FrameStart();
        perm.setSize(width);
        //if (!timeline.Happened(0))
        //    return;

        if (timeline.Happened(0))
            perm.permute();

        for (int index = 0; index < width/2; index++)
            pixels[perm.at[index]] += 255;

        // if (timeline.Happened(25))
        //     for (int index = 0; index < width; index++)
        //         pixels[index] = 0;
    }
};

class OnPattern : public LayeredPattern<Monochrome>
{
    uint8_t intensity;

    Transition transition = Transition(
        400,none,0,
        400,none,0
    );

public:
    OnPattern(uint8_t intensity=1){
        this->intensity=intensity;
    }
    inline void Calculate(Monochrome *pixels, int width, bool active) override
    {
        if (!transition.Calculate(active))
            return; //the fade out is done. we can skip calculating pattern data

        Monochrome value = Monochrome(intensity) * transition.getValue();

        for (int index = 0; index < width; index++)
            pixels[index] += value;
    }
};

}