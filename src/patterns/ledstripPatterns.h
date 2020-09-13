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
        Timeline tl = Timeline(1000);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            tl.FrameStart();
            if (tl.Happened(0))
                fade1.reset();
            if (tl.Happened(150))
                fade2.reset();
            if (tl.Happened(300))
                fade3.reset();

            for (int index = 0; index < width; index++)
            {
                pixels[index] +=
                    RGBA(127, 255, 0, fade1.getValue(fromCenter(index, width, 300)) * 255) +
                    RGBA(127, 127, 0, fade2.getValue(fromCenter(index, width, 500)) * 255) +
                    RGBA(127, 255, 0, fade3.getValue(fromCenter(index, width, 600)) * 255);
            }
        }
    };

    class HueGlowPattern : public LayeredPattern<RGBA>
    {
        //const int numSegments = 10;
        //Permute perm = Permute(numSegments);
        LFO<SinFast> lfo;

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            for (int index = 0; index < width; index++)
            {
                //int permutedQuantized = perm.at[index * numSegments / width] * width / numSegments;
                //RGBA colour = RGBA(RGB(Hue(200)))* lfo.getValue(float(index)/width,500); // ((float)permutedQuantized / width / 5 + 0.2);
                RGBA colour = RGBA(RGB(Hue(200)))* lfo.getValue(float(fromCenter(index,width,1000))/-1000,500);
                pixels[index] += colour;
                // RGBA(127,255,0,fade1.getValue(fromCenter(index,width,300))*255) +
                // RGBA(127,127,0,fade2.getValue(fromCenter(index,width,500))*255) +
                // RGBA(127,255,0,fade3.getValue(fromCenter(index,width,600))*255);
            }
        }
    };

    class MeteorPattern : public LayeredPattern<RGBA>
    {
        FadeDown fade1 = FadeDown(400, WaitAtEnd);
        FadeDown fade2 = FadeDown(400, WaitAtEnd);
        FadeDown fade3 = FadeDown(400, WaitAtEnd);
        Interval<PoissonInterval> sometimes1 = Interval<PoissonInterval>(1000);
        Interval<PoissonInterval> sometimes2 = Interval<PoissonInterval>(1000);
        Interval<PoissonInterval> sometimes3 = Interval<PoissonInterval>(1000);
        //Timeline tl = Timeline(1000);
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
                    RGBA(127, 255, 0,
                         fade1.getValue(fromRight(index, width, 300)) * (fade1.getValue()) * 255) +
                    RGBA(255, 127, 0,
                         fade2.getValue(fromRight(index, width, 300)) * (fade2.getValue()) * 255) +
                    RGBA(200, 200, 0,
                         fade3.getValue(fromRight(index, width, 300)) * (fade3.getValue()) * 255);
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
        const int avgInterval = 1000;
        const float precision = 1;
        const int numSegments = 10;
        LFO<SinFast> lfo;
        Permute perm = Permute(numSegments);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            for (int index = 0; index < width; index++)
            {
                int permutedQuantized = perm.at[index * numSegments / width] * width / numSegments;
                int interval = avgInterval + permutedQuantized * (avgInterval * precision) / width;
                pixels[index] += RGBA(200, 0, 200, 255 * lfo.getValue(0, interval));
            }
        }
    };

    class GlitchPattern : public LayeredPattern<RGBA>
    {
        const int numSegments = 10;
        int numOn = numSegments / 4;
        Timeline timeline = Timeline(50);
        Permute perm = Permute(numSegments);
        Transition transition = Transition(
            200, none, 0,
            500, none, 0);
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            timeline.FrameStart();

            if (timeline.Happened(0))
                perm.permute();

            uint8_t val = timeline.GetTimelinePosition() < 25 ? 255 * transition.getValue() : 0;

            for (int index = 0; index < width; index++)
            {
                int segmentNumber = perm.at[index * numSegments / width];
                pixels[index] += RGBA(255, 255, 255, segmentNumber < numOn ? val : 0);
            }
        }
    };

    class SinStripPattern : public LayeredPattern<RGBA>
    {
        int index, numStrips;
        LFO<SinFast> lfo = LFO<SinFast>(5000);
        Transition transition = Transition(
            200, none, 0,
            1000, none, 0);
    public:
        SinStripPattern(int index, int numStrips){
            this->index=index;
            this->numStrips=numStrips;
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            RGBA col = RGBA(255,100,0,transition.getValue() * lfo.getValue((float)index/numStrips) * 255);

            for (int index = 0; index < width; index++)
                pixels[index] += col;
        }
    };

    class GradientPattern : public LayeredPattern<RGBA>
    {
        RGBA col1 = RGBA(255,255,255,50);
        RGBA col2 = RGBA(255,127,0,255);
        Transition transition = Transition(
            200, none, 0,
            1000, none, 0);
    public:

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < width; index++){
                pixels[index] += (col2 + col1*((float)index/(width-1)) ) * transition.getValue();
            }
        }
    };

} // namespace LedStrip