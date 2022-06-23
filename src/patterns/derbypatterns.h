#pragma once
#include "pattern.h"
#include "colours.h"

namespace DerbyPatterns
{

    class OnPattern : public LayeredPattern<Derby>
    {
        uint8_t intensity;

        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

    public:
        OnPattern(uint8_t intensity = 200)
        {
            this->intensity = intensity;
        }
        inline void Calculate(Derby *pixels, int width, bool active) override
        {
            if (!active)
                return;

            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            Derby value = Derby(intensity * transition.getValue(), Params::getPrimaryColour(), 0);

            for (int index = 0; index < width; index++)
                pixels[index] = value;
        }
    };

    class StrobePattern : public LayeredPattern<Derby>
    {
    public:
        uint8_t intensity;
        StrobePattern(uint8_t intensity = 255)
        {
            this->intensity = intensity;
        }
        inline void Calculate(Derby *pixels, int width, bool active) override
        {
            if (active)
                for (int index = 0; index < width; index++)
                    pixels[index] = Derby(intensity, Params::getPrimaryColour(), 0);
        }
    };

    class MovePattern : public LayeredPattern<Derby>
    {
        LFO<SinFast> lfo = LFO<SinFast>();

    public:
        inline void Calculate(Derby *pixels, int width, bool active) override
        {
            if (active)
                for (int index = 0; index < width; index++)
                    pixels[index] = Derby(200, Params::getPrimaryColour(), lfo.getValue() * 255);
        }
    };

    class BeatMovePattern : public LayeredPattern<Derby>
    {
    public:
        inline void Calculate(Derby *pixels, int width, bool active) override
        {
            if (active)
                for (int index = 0; index < width; index++)
                    pixels[index] = Derby(200, Params::getPrimaryColour(), Tempo::GetProgress(4)*255);
        }
    };


    class RandomPositionPattern : public LayeredPattern<Derby>
    {
    public:
        inline void Calculate(Derby *pixels, int width, bool active) override
        {
            if (!active)
                return;
            for (int index = 0; index < width; index++)
                pixels[index] = Derby(200, Params::getPrimaryColour(), stableRandom(255));
        }
    };

    class BeatFadePattern : public LayeredPattern<Derby>
    {
    protected:
        TempoWatcher watcher = TempoWatcher();
        FadeDown fader = FadeDown(100);

    public:
        BeatFadePattern(int fadeout = 100)
        {
            fader.duration = fadeout;
        }

        inline void Calculate(Derby *pixels, int width, bool active) override
        {
            if (!active && fader.getValue() == 0)
            {
                watcher.Triggered();
                return;
            }

            if (active && watcher.Triggered())
                fader.reset();

            for (int index = 0; index < width; index++)
                pixels[index] = Derby(200 * fader.getValue(), Params::getPrimaryColour(), 0);
        }
    };
}