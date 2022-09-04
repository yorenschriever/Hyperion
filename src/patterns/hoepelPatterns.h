#pragma once
#include "pattern.h"
#include "helpers/pixelMap.h"
#include "helpers/fade.h"
#include "helpers/watcher.h"
#include <vector>
#include <math.h>

namespace Hoepels
{

    class MonochromePattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

        void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            RGBA col = Params::getPrimaryColour() * transition.getValue();
            for (int i = 0; i < width; i++)
            {
                pixels[i] = col;
            }
        }
    };

    class StereochromePattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

        void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                RGBA col = ((hoepel % 2 == 0) ? Params::getPrimaryColour() : Params::getSecondaryColour()) * transition.getValue();
                for (int led = 0; led < 50; led++)
                {
                    pixels[hoepel * 50 + led] = col;
                }
            }
        }
    };

    class GradientPattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

        void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                for (int led = 0; led < 50; led++)
                {
                    RGBA col = Params::getPrimaryColour() + (Params::getSecondaryColour() * abs(float(led-25)/25));
                    pixels[hoepel * 50 + led] = col * transition.getValue();
                }
            }
        }
    };

    class Rainbow : public LayeredPattern<RGBA>
    {
        LFO<SawDown> lfo;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

        void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(Params::getVelocity(10000, 500));

            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                for (int led = 0; led < 50; led++)
                {
                    RGBA col = Hue(led*255/50 + lfo.getValue()*255);
                    pixels[hoepel * 50 + led] = col * transition.getValue();
                }
            }
        }
    };

    class Rainbow2 : public LayeredPattern<RGBA>
    {
        LFO<SawDown> lfo;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

        void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(Params::getVelocity(10000, 500));

            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                for (int led = 0; led < 50; led++)
                {
                    int hoepel2 = hoepel / 2;
                    RGBA col = Hue(hoepel2*255/5 + lfo.getValue()*255);
                    pixels[hoepel * 50 + led] = col * transition.getValue();
                }
            }
        }
    };

    class Rainbow3 : public LayeredPattern<RGBA>
    {
        LFO<SawDown> lfo;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        const int hoepelmapping[10] = {0, 7,
                                       2, 9,
                                       4, 1,
                                       6, 3, 
                                       8, 5};

        void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(Params::getVelocity(10000, 500));

            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                for (int led = 0; led < 50; led++)
                {
                    RGBA col = Hue(hoepelmapping[hoepel]*255/10 + lfo.getValue()*255);
                    pixels[hoepel * 50 + led] = col * transition.getValue();
                }
            }
        }
    };

    class HoepelsBase : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        TempoWatcher tempo;
        FadeDown fade[10] = {
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd)};
        int count = 0;

        void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            if (tempo.Triggered())
            {
                count = (count + 1) % 10;
                fade[count].reset();
            }

            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                // RGBA col = Params::getPrimaryColour() * fade[hoepel].getValue() * transition.getValue();
                for (int led = 0; led < 50; led++)
                {
                    RGBA col = Params::getHighlightColour() * fade[hoepel].getValue(led * 50) * transition.getValue();
                    pixels[hoepel * 50 + led] = col;
                }
            }
        }
    };

    class HoepelsTransition : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        TempoWatcher tempo;
        Transition chase[10] = {
            Transition(1050, Transition::fromSides, 500, 1050, Transition::fromCenter, 1000),
            Transition(1050, Transition::fromSides, 500, 1050, Transition::fromCenter, 1000),
            Transition(1050, Transition::fromSides, 500, 1050, Transition::fromCenter, 1000),
            Transition(1050, Transition::fromSides, 500, 1050, Transition::fromCenter, 1000),
            Transition(1050, Transition::fromSides, 500, 1050, Transition::fromCenter, 1000),
            Transition(1050, Transition::fromSides, 500, 1050, Transition::fromCenter, 1000),
            Transition(1050, Transition::fromSides, 500, 1050, Transition::fromCenter, 1000),
            Transition(1050, Transition::fromSides, 500, 1050, Transition::fromCenter, 1000),
            Transition(1050, Transition::fromSides, 500, 1050, Transition::fromCenter, 1000),
            Transition(1050, Transition::fromSides, 500, 1050, Transition::fromCenter, 1000)};
        int count = 0;
        Permute perm = Permute(10);

        void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < 10; i++)
            {
                chase[i].Calculate(Tempo::GetBeatNumber() % 10 == perm.at[i]);
            }

            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                for (int led = 0; led < 50; led++)
                {
                    RGBA col = Params::getPrimaryColour() * chase[hoepel].getValue(led, 50) * transition.getValue();
                    pixels[hoepel * 50 + led] = col;
                }
            }
        }
    };

    class SinStripPattern : public LayeredPattern<RGBA>
    {
        const int numStrips = 10;
        LFO<SinFast> lfo = LFO<SinFast>(5000);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            //lfo.setPeriod(Params::getVelocity(10000, 500));
            RGBA col = Params::getSecondaryColour() * transition.getValue();

            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                for (int led = 0; led < 50; led++){
                    pixels[hoepel * 50 + led] = col * lfo.getValue((float)led / 50 + (float)hoepel/20);
                }
            }

        }
    };

    class SinStripPattern2 : public LayeredPattern<RGBA>
    {
        const int numStrips = 10;
        LFO<SinFast> lfo = LFO<SinFast>(5000);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            //lfo.setPeriod(Params::getVelocity(10000, 500));
            RGBA col = Params::getHighlightColour() * transition.getValue();

            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                float dir = hoepel % 2 ==0 ? 1 : -2;
                //float frq = hoepel % 2 ==0 ? 1 : 2;
                for (int led = 0; led < 50; led++){
                    pixels[hoepel * 50 + led] = col * lfo.getValue(dir * (float)led / 50 + (float)hoepel/20);
                }
            }

        }
    };

    class FirePattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        TempoWatcher tempo;
        Transition chase[10] = {
            Transition(150, Transition::fromCenter, 150, 150, Transition::fromCenter, 150),
            Transition(150, Transition::fromCenter, 150, 150, Transition::fromCenter, 150),
            Transition(150, Transition::fromCenter, 150, 150, Transition::fromCenter, 150),
            Transition(150, Transition::fromCenter, 150, 150, Transition::fromCenter, 150),
            Transition(150, Transition::fromCenter, 150, 150, Transition::fromCenter, 150),
            Transition(150, Transition::fromCenter, 150, 150, Transition::fromCenter, 150),
            Transition(150, Transition::fromCenter, 150, 150, Transition::fromCenter, 150),
            Transition(150, Transition::fromCenter, 150, 150, Transition::fromCenter, 150),
            Transition(150, Transition::fromCenter, 150, 150, Transition::fromCenter, 150),
            Transition(150, Transition::fromCenter, 150, 150, Transition::fromCenter, 150)};
        int count = 0;
        Permute perm = Permute(10);
        Timeline timeline = Timeline();

        void Calculate(RGBA *pixels, int width, bool active) override
        {
            timeline.FrameStart();

            if (!transition.Calculate(active))
                return;

            if (tempo.Triggered())
                timeline.reset();

            for (int i = 0; i < 10; i++)
            {
                chase[i].Calculate(timeline.GetTimelinePosition() < i * 50);
            }

            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                for (int led = 0; led < 50; led++)
                {
                    RGBA col = Params::getSecondaryColour() * chase[hoepel].getValue(led, 50) * transition.getValue();
                    pixels[hoepel * 50 + led] = col;
                }
            }
        }
    };

    class FireworkPattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        TempoWatcher tempo;
        FadeDown fade[2] = {FadeDown(400, WaitAtEnd),FadeDown(400, WaitAtEnd)};
        Permute perm[2] = {Permute(10),Permute(10)};

        void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            if (tempo.Triggered())
            {
                int which = abs(Tempo::GetBeatNumber())%2;
                fade[which].reset();
                perm[which].permute();
            }

            RGBA baseCol = Params::getHighlightColour() * transition.getValue();
            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                for (int led = 0; led < 50; led++)
                {
                    float fadeValue = 0;
                    for (int i=0;i<2;i++){
                        int rotled = (perm[i].at[hoepel]*5 + led)%50;
                        int mirrorled = abs(rotled - 25);
                        fadeValue += fade[i].getValue(mirrorled * mirrorled / 0.5);                        
                    }
                    RGBA col = baseCol * fadeValue;
                    pixels[hoepel * 50 + led] = col;
                }
            }
        }
    };

    class SinPattern : public LayeredPattern<RGBA>
    {
        const int numStrips = 10;
        LFO<SinFast> lfo = LFO<SinFast>(5000);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(Params::getVelocity(5000, 500));
            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                RGBA col = Params::getPrimaryColour() * lfo.getValue((float)(hoepel)/10) * transition.getValue();
                for (int led = 0; led < 50; led++)
                    pixels[hoepel * 50 + led] = col;
            }
        }
    };


    class ChasePattern : public LayeredPattern<RGBA>
    {
        const int numStrips = 10;
        LFO<SawDown> lfo = LFO<SawDown>(5000);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(Params::getVelocity(5000, 500));
            RGBA col = Params::getHighlightColour() * transition.getValue();

            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                float dir = 1; //hoepel % 2 ==0 ? 1 : -1;
                for (int led = 0; led < 50; led++){
                    pixels[hoepel * 50 + led] = col * lfo.getValue(dir * (float)led / 50);
                }
            }

        }
    };

    class AntichasePattern : public LayeredPattern<RGBA>
    {
        const int numStrips = 10;
        LFO<SawDown> lfo = LFO<SawDown>(5000);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(Params::getVelocity(5000, 500));
            RGBA col = Params::getSecondaryColour() * transition.getValue();

            for (int hoepel = 0; hoepel < 10; hoepel++)
            {
                float dir = hoepel % 2 ==0 ? 1 : -1;
                for (int led = 0; led < 50; led++){
                    pixels[hoepel * 50 + led] = col * lfo.getValue(dir * (float)led / 50);
                }
            }

        }
    };

    template <class T>
    class ClivePattern : public LayeredPattern<RGBA>
    {
        int numSegments;
        int averagePeriod;
        float precision;
        LFO<T> lfo;
        Permute perm;
        int color;

    public:
        ClivePattern(int color = 0, int numSegments = 10, int averagePeriod = 1000, float precision = 1, float pulsewidth = 1)
        {
            this->numSegments = std::max(numSegments, 1);
            this->averagePeriod = averagePeriod;
            this->precision = precision;
            this->perm = Permute(numSegments);
            this->lfo.setPulseWidth(pulsewidth);
            this->color = color;
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            auto col = Params::getPrimaryColour();
            if (color==1) col = Params::getSecondaryColour();
            if (color==2) col = Params::getHighlightColour();

            for (int index = 0; index < width; index++)
            {
                int permutedQuantized = perm.at[index * numSegments / width] * width / numSegments;
                int interval = averagePeriod + permutedQuantized * (averagePeriod * precision) / width;
                pixels[index] += col * lfo.getValue(0, interval);
            }
        }
    };

    class SquareGlitchPattern : public LayeredPattern<RGBA>
    {
        Timeline timeline = Timeline(10);
        Permute perm = Permute(100);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            perm.permute();

            for (int index = 0; index < width; index++)
            {
                int quantized = index / 10;
                if (perm.at[quantized] > 25)
                    continue;
                pixels[index] += Params::getSecondaryColour() * transition.getValue();
            }
        }
    };

    class OnbeatFadeAllPattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade =  FadeDown(1400, WaitAtEnd);
        TempoWatcher tempo = TempoWatcher();

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active)) return;
            if (tempo.Triggered()) fade.reset();

            RGBA col = Params::getPrimaryColour() * fade.getValue() * transition.getValue();
            for (int i = 0; i < width; i++)
                pixels[i] += col;
        }
    };

    class OnbeatFadePattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade[5] = {
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd)};
        TempoWatcher tempo = TempoWatcher();
        int pos = 0;

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            if (tempo.Triggered())
            {
                pos = (pos + 1 + stableRandom(4)) % 5;
                fade[pos].reset();
            }

            for (int i = 0; i < 5; i++)
            {
                RGBA col = Params::getSecondaryColour() * fade[i].getValue() * transition.getValue();
                for (int j = 0; j < 100; j++)
                    pixels[i*100+j] += col;
            }
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

}