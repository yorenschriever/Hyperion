#pragma once
#include "pattern.h"

namespace LedStrip
{

    class BlackPattern : public LayeredPattern<RGBA>
    {

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            return;
        }
    };

    class TripleFadePattern : public LayeredPattern<RGBA>
    {
        FadeDown fade1 = FadeDown(100, WaitAtEnd);
        FadeDown fade2 = FadeDown(100, WaitAtEnd);
        FadeDown fade3 = FadeDown(100, WaitAtEnd);
        Timeline tl;
        TempoWatcher tw;

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            if (tw.Triggered() && Tempo::GetBeatNumber() % 2 == 0)
                tl.reset();
            tl.FrameStart();
            if (tl.Happened(0))
                fade1.reset();
            if (tl.Happened(150))
                fade2.reset();
            if (tl.Happened(300))
                fade3.reset();

            for (int index = 0; index < width; index++)
            {
                pixels[index] += // Params::getPrimaryColour();
                    Params::getPrimaryColour() * fade1.getValue(Transition::fromCenter(index, width, 300)) +
                    Params::getPrimaryColour() * fade2.getValue(Transition::fromCenter(index, width, 500)) +
                    Params::getSecondaryColour() * fade3.getValue(Transition::fromCenter(index, width, 600));
            }
        }
    };

    class HueGlowPattern : public LayeredPattern<RGBA>
    {
        // const int numSegments = 10;
        // Permute perm = Permute(numSegments);
        LFO<SinFast> lfo;

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            for (int index = 0; index < width; index++)
            {
                // int permutedQuantized = perm.at[index * numSegments / width] * width / numSegments;
                // RGBA colour = RGBA(RGB(Hue(200)))* lfo.getValue(float(index)/width,500); // ((float)permutedQuantized / width / 5 + 0.2);
                RGBA colour = Params::getPrimaryColour() * lfo.getValue(float(Transition::fromCenter(index, width, 1000)) / -1000, 500);
                pixels[index] += colour;
                // RGBA(127,255,0,fade1.getValue(fromCenter(index,width,300))*255) +
                // RGBA(127,127,0,fade2.getValue(fromCenter(index,width,500))*255) +
                // RGBA(127,255,0,fade3.getValue(fromCenter(index,width,600))*255);
            }
        }
    };

    class GlowPattern : public LayeredPattern<RGBA>
    {
        Permute perm;
        LFO<SinFast> lfo = LFO<SinFast>(10000);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            lfo.setPeriod ( 500 + 10000 * (1.0f - Params::getVelocity()));
            perm.setSize(width);

            for (int index = 0; index < width; index++)
                pixels[perm.at[index]] += Params::getPrimaryColour() * lfo.getValue(float(Transition::fromCenter(index, width, 1000)) / -1000);
        }
    };

    class MeteorPattern : public LayeredPattern<RGBA>
    {
        FadeDown fade1 = FadeDown(400, WaitAtEnd);
        FadeDown fade2 = FadeDown(400, WaitAtEnd);
        FadeDown fade3 = FadeDown(400, WaitAtEnd);
        Interval<PoissonInterval> sometimes1 = Interval<PoissonInterval>(700);
        Interval<PoissonInterval> sometimes2 = Interval<PoissonInterval>(700);
        Interval<PoissonInterval> sometimes3 = Interval<PoissonInterval>(1500);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            if (sometimes1.Happened() && fade1.getValue(300) == 0 && fade1.getValue(0) == 0)
                fade1.reset();
            if (sometimes2.Happened() && fade2.getValue(300) == 0 && fade2.getValue(0) == 0)
                fade2.reset();
            if (sometimes3.Happened() && fade3.getValue(300) == 0 && fade3.getValue(0) == 0)
                fade3.reset();

            for (int index = 0; index < width; index++)
            {
                pixels[index] +=
                    Params::getPrimaryColour() * fade1.getValue(Transition::fromRight(index, width, 300)) * (fade1.getValue()) +
                    Params::getSecondaryColour() * fade2.getValue(Transition::fromRight(index, width, 300)) * (fade2.getValue()) +
                    Params::getHighlightColour() * fade3.getValue(Transition::fromRight(index, width, 300)) * (fade3.getValue());
            }
        }
    };

    // class ClivePattern : public LayeredPattern<RGBA>
    // {
    //     const int avgInterval=500;
    //     const float precision = 1;
    //     LFO<SinFast> lfo;
    //     Permute perm;
    // public:
    //     inline void Calculate(RGBA *pixels, int width, bool active) override
    //     {
    //         if (!active)
    //             return;

    //         perm.setSize(width);

    //         for (int index = 0; index < width; index++) {
    //             int interval = avgInterval + perm.at[index]*(avgInterval*precision)/width;
    //             pixels[index] += RGBA(200,0,200, 255*lfo.getValue(0,interval));
    //         }
    //     }
    // };

    class ClivePattern : public LayeredPattern<RGBA>
    {
        int numSegments;
        int averagePeriod;
        float precision;
        LFO<SinFast> lfo;
        Permute perm;

    public:
        ClivePattern(int numSegments = 10, int averagePeriod = 1000, float precision = 1)
        {
            this->numSegments = std::max(numSegments, 1);
            this->averagePeriod = averagePeriod;
            this->precision = precision;
            perm = Permute(numSegments);
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            for (int index = 0; index < width; index++)
            {
                int permutedQuantized = perm.at[index * numSegments / width] * width / numSegments;
                int interval = averagePeriod + permutedQuantized * (averagePeriod * precision) / width;
                pixels[index] += Params::getSecondaryColour() * lfo.getValue(0, interval);
            }
        }
    };

    class GlitchPattern : public LayeredPattern<RGBA>
    {
        const int numSegments = 10;
        int numOn = numSegments / 4;
        Timeline timeline = Timeline(25);
        Permute perm = Permute(numSegments);
        Transition transition = Transition(
            200, Transition::none, 0,
            500, Transition::none, 0);
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            timeline.FrameStart();

            if (timeline.Happened(0))
                perm.permute();

            float val = timeline.GetTimelinePosition() < 25 ? transition.getValue() : 0;

            for (int index = 0; index < width; index++)
            {
                int segmentNumber = perm.at[index * numSegments / width];
                pixels[index] += Params::getHighlightColour() * (segmentNumber < numOn ? val : 0);
            }
        }
    };

    class SinStripPattern : public LayeredPattern<RGBA>
    {
        int index, numStrips;
        LFO<SinFast> lfo = LFO<SinFast>(5000);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        SinStripPattern(int index, int numStrips)
        {
            this->index = index;
            this->numStrips = numStrips;
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            RGBA col = Params::getPrimaryColour() * transition.getValue() * lfo.getValue((float)index / numStrips);

            for (int index = 0; index < width; index++)
                pixels[index] += col;
        }
    };

    template <class T>
    class LFOPattern : public LayeredPattern<RGBA>
    {
        int numWaves;
        LFO<T> lfo;
        LFO<Square> lfoColour;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        LFOPattern(int numWaves, int period = 5000)
        {
            this->numWaves = numWaves;
            this->lfo = LFO<T>(period);
            this->lfoColour = LFO<Square>(period/2);
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < width; index++)
            {
                float phase = (float)numWaves * index / width;
                RGBA colour = lfoColour.getValue(phase) ? Params::getSecondaryColour() : Params::getPrimaryColour();
                RGBA dimmedColour = colour * transition.getValue() * lfo.getValue(phase);
                pixels[index] += dimmedColour;
            }
        }
    };

    class GradientPattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < width; index++)
            {
                pixels[index] += (Params::getPrimaryColour() + Params::getSecondaryColour() * ((float)index / (width - 1))) * transition.getValue();
            }
        }
    };

    class BlinderPattern : public LayeredPattern<RGBA>
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

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index++)
                pixels[index] += Params::getPrimaryColour() * transition.getValue(index, width);
        }
    };

    class SlowStrobePattern : public LayeredPattern<RGBA>
    {
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            RGBA col = millis() % 100 < 25 ? Params::getPrimaryColour() : RGBA();

            for (int index = 0; index < width; index++)
                pixels[index] = col;
        }
    };

    class GlowPulsePattern : public LayeredPattern<RGBA>
    {
        Permute perm;
        LFO<SinFast> lfo = LFO<SinFast>(10000);
        Transition transition;

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            lfo.setPeriod( 500 + 10000 * (1.0f - Params::getVelocity()));
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                float val = 1.025 * lfo.getValue(float(Transition::fromCenter(index, width, 1000)) / -1000);
                if (val < 1.0)
                    pixels[perm.at[index]] = Params::getSecondaryColour() * lfo.getValue(float(Transition::fromCenter(index, width, 1000)) / -1000) * transition.getValue(index, width);
                else
                    pixels[perm.at[index]] = (Params::getSecondaryColour() + (Params::getHighlightColour() * (val - 1) / 0.025)) * transition.getValue(index, width);
            }
        }
    };

    class BeatSingleFadePattern : public LayeredPattern<RGBA>
    {
        TempoWatcher watcher = TempoWatcher();
        FadeDown fader = FadeDown(200, WaitAtEnd);
        int current = 0;
        int numSegments;

        public:
        BeatSingleFadePattern(int numSegments = 5)
        {
            this->numSegments = std::max(numSegments,1);
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active && fader.getValue() == 0)
            {
                watcher.Triggered();
                return;
            }

            if (active && watcher.Triggered())
            {
                current = random(numSegments);
                fader.reset();
            }

            int segmentWidth = width / numSegments;
            for (int index = 0; index < segmentWidth; index++)
            {
                int permutedQuantized = current * segmentWidth + index;
                pixels[permutedQuantized] += Params::getSecondaryColour() * fader.getValue();
            }
        }
    };

    class PalettePattern : public LayeredPattern<RGBA>
    {
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            RGBA cols[3] = {
                Params::getPrimaryColour(),
                Params::getSecondaryColour(),
                Params::getHighlightColour(),
            };

            for (int index = 0; index < width; index++)
                pixels[index] = cols[index % 3];
        }
    };

} // namespace LedStrip