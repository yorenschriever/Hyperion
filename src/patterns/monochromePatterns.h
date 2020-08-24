#pragma once
#include "pattern.h"

class SinPattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = (uint8_t)(127.f + 127.f * cos(float(index) / float(width) * (2*3.1415) - millis() / 250.));
    }
};

class SawPattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = millis() / 50 + index * 2;
    }
};

class RandomPattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = random(0, 100) < 10 ? 255 : 0;
    }
};

class RandomPattern2 : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = random(0, 100) < 1 ? 255 : 0;
    }
};

class RandomFadePattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
        {
            uint8_t lastPixelvalue = pixels[index].L;
            if (random(0, 100) < 1)
                pixels[index] = 255;
            else
                pixels[index] = max(lastPixelvalue - 10, 0);
        }
    }
};

class MeteorPattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
        {
            int lastSelfvalue = pixels[index].L;
            int lastNeighbourvalue = pixels[(index + 1) % width].L;
            if (random(0, 300) < 1)
                pixels[index] = 255;
            else
                pixels[index] = max(max(lastSelfvalue - 25, lastNeighbourvalue - 10), 0);
        }
    }
};

class SlowStrobePattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = millis() % 100 < 25 ? 255 : 0;
    }
};

class FastStrobePattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = millis() % 50 < 25 ? 255 : 0;
    }
};


class TemperatureTestPattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = 255*Rotary::isButtonPressed();
    }
};

class BeatAllFadePattern : public Pattern<Monochrome>
{
    BPMWatcher watcher = BPMWatcher();
    Fade fader = Fade(Linear, 100, Down);

    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        fader.duration = Midi::controllerValue(49)*2+50;

        if (watcher.Triggered())
            fader.reset();

        for (int index = 0; index < width; index++)
            pixels[index] = 255 * fader.getValue();
    }
};

class BeatShakePattern : public Pattern<Monochrome>
{
    BPMWatcher watcher = BPMWatcher();
    Fade fader = Fade(Linear, 150, Down, WaitAtEnd);
    Permute perm = Permute(0);
    
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        perm.setSize(width);

        if (watcher.Triggered())
        {
            perm.permute();
            fader.reset();
        }

        for (int index = 0; index < width; index++)
            pixels[index] = 255 * fader.getValue(perm.at[index]*10);
    }
};

class BeatSingleFadePattern : public Pattern<Monochrome>
{
    BPMWatcher watcher = BPMWatcher();
    Fade fader = Fade(Linear, 200, Down, WaitAtEnd);
    int current=0;

    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        if (watcher.Triggered())
        {
            current = random(width);
            fader.reset();
        }

        for (int index = 0; index < width; index++)
            pixels[index] = index==current ? 255 * fader.getValue() : 0;
    }
};

class BeatMultiFadePattern : public Pattern<Monochrome>
{
    BPMWatcher watcher = BPMWatcher();
    Fade fader = Fade(Linear, 200, Down, WaitAtEnd);
    Permute perm = Permute(0);

    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        perm.setSize(width);

        if (watcher.Triggered())
        {
            perm.permute();
            fader.reset();
        }

        for (int index = 0; index < width; index++)
            pixels[index] = perm.at[index]<width/2 ? 255 * fader.getValue() : 0;
    }
};

class GlitchPattern : public Pattern<Monochrome>
{
    Timeline timeline = Timeline(50);

    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        timeline.FrameStart();
        //if (!timeline.Happened(0))
        //    return;

        if (timeline.Happened(0))
            for (int index = 0; index < width; index++)
                pixels[index] = random(2) ? 255 : 0;

        if (timeline.Happened(25))
            for (int index = 0; index < width; index++)
                pixels[index] = 0;
    }
};

class OnPattern : public Pattern<Monochrome>
{
    inline void Calculate(Monochrome *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = 255;
    }
};