#pragma once
#include "pattern.h"

namespace MovingheadPatterns
{

    class TestPattern : public LayeredPattern<MovingHead>
    {

    LFO<SinFast> lfo = LFO<SinFast>(5000);
    LFO<SinFast> lfo2 = LFO<SinFast>(4000);

    public:
        inline void Calculate(MovingHead *pixels, int width, bool active) override
        {
            if (!active)
                return; 

            for (int index = 0; index < width; index++){
                pixels[index] = MovingHead(
                    Midi::controllerValue(51)*90/127,
                    Midi::controllerValue(52)*360/127,
                    //lfo.getValue()*255*255,
                    //lfo2.getValue()*255*255,
                    //RGBA(255,0,0,255)
                    //Hue(millis()/10)
                    Hue(Midi::controllerValue(48)*2), //*(float(Midi::controllerValue(49))/127,
                    Midi::controllerValue(50)*2

                    );
                //pixels[index].pan = lfo.getValue()*255;
                //pixels[index].colour = RGBWAU(255,0,0,0,0,0);
            }
        }
    };



    class WallPattern : public LayeredPattern<MovingHead>
    {
    Transition trans = Transition(200,400);
    LFO<SinFast> lfo = LFO<SinFast>(2000);
    public:
        inline void Calculate(MovingHead *pixels, int width, bool active) override
        {
            if (!trans.Calculate(active))
                return; 

            for (int index = 0; index < width; index++)
            {
                RGBA col = index%2==0? Params::getPrimaryColour() : Params::getHighlightColour();
                pixels[index] += MovingHead(0,-30,col*trans.getValue()*(0.25+0.75*lfo.getValue(float(index)/width)));
            }
        }
    };

    //TODO
    class WallBeatPattern : public LayeredPattern<MovingHead>
    {
    Transition trans = Transition(200,400);
    LFO<SinFast> lfo = LFO<SinFast>(2000);
    TempoWatcher tw;

    public:
        inline void Calculate(MovingHead *pixels, int width, bool active) override
        {
            if (!trans.Calculate(active))
                return; 

            for (int index = 0; index < width; index++)
            {
                RGBA col = index%2==0? Params::getPrimaryColour() : Params::getHighlightColour();
                pixels[index] += MovingHead(0,-30,col*trans.getValue()*(0.25+0.75*lfo.getValue()));
            }
        }
    };

    class Flash360Pattern : public LayeredPattern<MovingHead>
    {
    //Transition trans = Transition(200,400);
    LFO<SinFast> lfopan = LFO<SinFast>(10000);
    LFO<SinFast> lfotilt = LFO<SinFast>(8000);
    TempoWatcher tw;
    Permute perm;
    FadeDown fade = FadeDown(300);

    public:
        inline void Calculate(MovingHead *pixels, int width, bool active) override
        {
            perm.setSize(width);
            if (tw.Triggered() && active){
                perm.permute();
                fade.reset();
            }

            if (!active && fade.getValue()==0)
                return; 

            for (int index = 0; index < width; index++)
            {
                float pan = lfopan.getValue((float)index/width)*300-150;
                float tilt = lfotilt.getValue((float)index/width)*160-80;
                RGBA col = perm.at[index]==0? Params::getPrimaryColour() : RGBA(0,0,0,0);
                pixels[index] += MovingHead(pan,tilt,col*fade.getValue());
            }
        }
    };

    class WallDJPattern : public LayeredPattern<MovingHead>
    {
    LFO<SinFast> lfo = LFO<SinFast>(10000);
    Transition trans = Transition(200,400);
    public:
        inline void Calculate(MovingHead *pixels, int width, bool active) override
        {
            if (!trans.Calculate(active))
                return; 

            for (int index = 0; index < width; index++)
            {
                RGBA col = index%2==0? Params::getPrimaryColour() : Params::getHighlightColour();
                float tilt = index%2==0? -30:60;
                pixels[index] += MovingHead(0,tilt,col*lfo.getValue(float(index%2)/2)*trans.getValue());
            }
        }
    };

    class FrontPattern : public LayeredPattern<MovingHead>
    {
    Transition trans = Transition(200,400);
    LFO<SinFast> lfo = LFO<SinFast>(1000);
    public:
        inline void Calculate(MovingHead *pixels, int width, bool active) override
        {
            if (!trans.Calculate(active))
                return; 

            for (int index = 0; index < width; index++)
            {
                RGBA col = index%2==0? Params::getPrimaryColour() : Params::getHighlightColour();
                pixels[index] += MovingHead(0,60,col*lfo.getValue((float)index/width)*trans.getValue());
            }
        }
    };

    class SidesPattern : public LayeredPattern<MovingHead>
    {
    Transition trans = Transition(200,600);
    LFO<SinFast> lfo = LFO<SinFast>(5000);
    LFO<SinFast> lfotilt = LFO<SinFast>(4000);
    public:
        inline void Calculate(MovingHead *pixels, int width, bool active) override
        {
            if (!trans.Calculate(active))
                return; 

            for (int index = 0; index < width; index++)
            {
                RGBA col = index%2==0? Params::getPrimaryColour() : Params::getHighlightColour();
                pixels[index] += MovingHead((index-width/2)*90-40+40*lfotilt.getValue(0.25),20+40*lfotilt.getValue(),col * (0.1+0.9*lfo.getValue((float)index/width)) * trans.getValue());
            }
        }
    };

    class GlitchPattern : public LayeredPattern<MovingHead>
    {
    LFO<SinFast> lfopan = LFO<SinFast>(5000);
    LFO<SinFast> lfotilt = LFO<SinFast>(2000);
    public:
        inline void Calculate(MovingHead *pixels, int width, bool active) override
        {
            if (!active)
                return; 

            for (int index = 0; index < width; index++)
            {
                float pan = lfopan.getValue((float)index/width)*300-150;
                float tilt = lfotilt.getValue((float)index/width)*160-80;
                pixels[index] += MovingHead(pan,tilt,millis() % 100 < 25 ? Params::getHighlightColour() : RGBA(0,0,0,255), 0);
            }
        }
    };

    class UVPattern : public LayeredPattern<MovingHead>
    {

    public:
        inline void Calculate(MovingHead *pixels, int width, bool active) override
        {
            if (!active)
                return; 

            for (int index = 0; index < width; index++)
            {
                pixels[index] += MovingHead(0,-45,RGBA(0,0,0,255),millis() % 50 < 25 ? 255 : 0);
            }
        }
    };

}