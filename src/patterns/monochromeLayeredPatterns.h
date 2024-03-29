#pragma once
#include "pattern.h"

namespace Layered
{

    class EmptyPattern : public LayeredPattern<Monochrome>
    {
    public:
        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
        }
    };

    // for debugging
    class SimplePattern : public LayeredPattern<Monochrome>
    {
    public:
        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!active)
                return;

            for (int index = 0; index < width; index++)
                pixels[index] += Monochrome(127);
        }
    };

    class SinPattern : public LayeredPattern<Monochrome>
    {
        int direction;
        float phase;

        LFO<SinFast> lfo;
        Transition transition = Transition(
            1000, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        SinPattern(int direction = 4, float phase = 0)
        {
            this->direction = direction;
            this->phase = phase;
        }

        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index++)
                pixels[index] += Monochrome(255 * lfo.getValue(float(index) / width, direction * 250)) * transition.getValue(index, width);
        }
    };

    class GlowPattern : public LayeredPattern<Monochrome>
    {
        int direction;
        Permute perm1 = Permute(0);
        Permute perm2 = Permute(0);
        Transition transition = Transition(
            1000, Transition::none, 0,
            2000, Transition::none, 0);

    public:
        GlowPattern(int direction = 1)
        {
            this->direction = direction;
        }

        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            perm1.setSize(width);
            perm2.setSize(width);

            for (int index = 0; index < width; index++)
            {
                float period1 = float(perm1.at[index]) / float(width) * (2 * 3.1415) * 14.7 + 5.5;
                float period2 = float(perm2.at[index]) / float(width) * (2 * 3.1415) * 15.3 + 3.5;
                float combined = std::max(cos(period1 * float(millis()) / (20000.f * direction)), cos(period2 * float(millis()) / (20000.f * direction)));

                pixels[index] += Monochrome((uint8_t)(150.f + 105.f * combined)) * transition.getValue(index, width);
            }
        }
    };

    class FastStrobePattern : public LayeredPattern<Monochrome>
    {
        Transition transition = Transition(
            1000, Transition::none, 0,
            0, Transition::none, 0);

        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            Monochrome value = Monochrome(millis() % 50 < 25 ? 255 : 0) * transition.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] = pixels[index] * (1. - transition.getValue()) + value;
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
        BlinderPattern(FadeShape in = Transition::none, FadeShape out = Transition::none, uint8_t intensity = 255, int fadein = 200, int fadeout = 600)
        {
            transition = Transition(
                fadein, in, 200,
                fadeout, out, 500);
            this->intensity = intensity;
        }

    protected:
        Transition transition;
        uint8_t intensity;

        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index++)
                pixels[index] += Monochrome(intensity) * transition.getValue(index, width);
        }
    };

    class BeatAllFadePattern : public LayeredPattern<Monochrome>
    {
    public:
        BeatAllFadePattern(int fadeout = 100)
        {
            fader.duration = fadeout;
        }

    protected:
        TempoWatcher watcher = TempoWatcher();
        FadeDown fader = FadeDown(100);

        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!active && fader.getValue() == 0)
            {
                watcher.Triggered();
                return;
            }

            // fader.duration = Midi::controllerValue(49)*2+50;

            if (active && watcher.Triggered())
                fader.reset();

            for (int index = 0; index < width; index++)
                pixels[index] += 255 * fader.getValue();
        }
    };

    class BeatShakePattern : public LayeredPattern<Monochrome>
    {
        TempoWatcher watcher = TempoWatcher();
        FadeDown fader = FadeDown(300, WaitAtEnd);
        Permute perm = Permute(0);

        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!active && fader.getValue(width * 10) == 0)
            {
                watcher.Triggered();
                return;
            }

            perm.setSize(width);

            if (active && watcher.Triggered())
            {
                perm.permute();
                fader.reset();
            }

            for (int index = 0; index < width; index++)
                pixels[index] += 255 * fader.getValue(perm.at[index] * 10);
        }
    };

    class BeatSingleFadePattern : public LayeredPattern<Monochrome>
    {
        TempoWatcher watcher = TempoWatcher();
        FadeDown fader = FadeDown(200, WaitAtEnd);
        int current = 0;

        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!active && fader.getValue() == 0)
            {
                watcher.Triggered();
                return;
            }

            if (active && watcher.Triggered())
            {
                current = random(width);
                fader.reset();
            }

            for (int index = 0; index < width; index++)
                pixels[index] += index == current ? 255 * fader.getValue() : 0;
        }
    };

    class BeatMultiFadePattern : public LayeredPattern<Monochrome>
    {
    public:
        BeatMultiFadePattern(int duration = 200)
        {
            fader.duration = duration;
        }

        TempoWatcher watcher = TempoWatcher();
        FadeDown fader = FadeDown(200, WaitAtEnd);
        Permute perm = Permute(0);

        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!active && fader.getValue() == 0)
            {
                watcher.Triggered();
                return;
            }

            perm.setSize(width);

            if (active && watcher.Triggered())
            {
                perm.permute();
                fader.reset();
            }

            for (int index = 0; index < width; index++)
                pixels[index] += perm.at[index] < width / 2 ? 255 * fader.getValue() : 0;
        }
    };

    class GlitchPattern : public LayeredPattern<Monochrome>
    {
        Timeline timeline = Timeline(50);
        Permute perm = Permute(0);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            timeline.FrameStart();
            perm.setSize(width);

            if (timeline.Happened(0))
                perm.permute();

            uint8_t val = timeline.GetTimelinePosition() < 25 ? 255 * transition.getValue() : 0;

            for (int index = 0; index < width / 2; index++)
                pixels[perm.at[index]] += Monochrome(val);
        }
    };

    class OnPattern : public LayeredPattern<Monochrome>
    {
        uint8_t intensity;

        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

    public:
        OnPattern(uint8_t intensity = 100)
        {
            this->intensity = intensity;
        }
        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!active)
                return;

            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            Monochrome value = Monochrome(intensity) * transition.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] += value;
        }
    };

    class ChaseFromCenterPattern : public LayeredPattern<Monochrome>
    {
    public:
        ChaseFromCenterPattern(int duration = 200)
        {
            fader.duration = duration;
        }

        FadeDown fader = FadeDown(400, WaitAtEnd);
        Watcher<int> watcher = Watcher<int>(
            []()
            { return Tempo::GetBeatNumber() / 2; },
            Changing);

        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!active && fader.getValue() == 0)
            {
                watcher.Triggered();
                return;
            }

            if (active && watcher.Triggered())
                fader.reset();

            for (int index = 0; index < width; index++)
                pixels[index] += 255 * fader.getValue(Transition::fromCenter(index, width, 400));
        }
    };

    template <class T>
    class LFOPattern : public LayeredPattern<Monochrome>
    {
        int numWaves;
        LFO<T> lfo;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        LFOPattern(int numWaves, int period = 5000, float pulsewidth = 0.5)
        {
            this->numWaves = numWaves;
            this->lfo = LFO<T>(period);
            this->lfo.setPulseWidth(pulsewidth);
        }

        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < width; index++)
            {
                pixels[index] += 255 * transition.getValue() * lfo.getValue((float)numWaves * index / width);
            }
        }
    };

    class BeatStepPattern : public LayeredPattern<Monochrome>
    {
        int steps;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        BeatStepPattern(int steps = 2)
        {
            this->steps = steps;
        }

        inline void Calculate(Monochrome *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < width; index++)
            {
                if (index * steps == (Tempo::GetBeatNumber()) % (width * steps))
                    pixels[index] += 255 * transition.getValue();
            }
        }
    };

}