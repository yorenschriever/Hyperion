#pragma once
#include "pattern.h"
#include "hardware/rotary/rotary.h"

class RainbowPattern : public Pattern<RGB>
{
    inline void Calculate(RGB *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = Hue(0xFF * index / width + (millis() / 10));
    }
};

//You can use this pattern to test the order of the colors in your ledstrip. It will flash Red, green, blue, off.
class ColourOrderPattern : public Pattern<RGB>
{
    inline void Calculate(RGB *pixels, int width, bool firstFrame) override
    {
        RGB col;
        int tick = millis() / 1000 % 4;
        if (tick == 0)
            col = RGB(255, 0, 0);
        if (tick == 1)
            col = RGB(0, 255, 0);
        if (tick == 2)
            col = RGB(0, 0, 255);
        if (tick == 3)
            col = RGB(0, 0, 0);

        for (int index = 0; index < width; index++)
            pixels[index] = col;
    }
};

///Demonstation how to mix colours. It displays a fade from red to green
class MixingPattern : public Pattern<RGB>
{
    inline void Calculate(RGB *pixels, int width, bool firstFrame) override
    {
        //create colors as RGBA objects, so we can use the Alpha channel to blend
        RGBA red = RGBA(255, 0, 0, 255);
        RGBA green = RGBA(0, 255, 0, 255);

        for (int index = 0; index < width; index++)
        {
            //There is a lot going on in the next line:
            //1. green is multiplied with a float, multiplying RGBA with float changes the alpha channel
            //2. the result is added to red. RGBA + RGBA will apply alpha blending to get a resulting RGBA colour
            //3. the RGBA colour is implicitly casted to an RGB colour, because pixels is RGB. The compiler will automatically do this for you
            pixels[index] = red + green * (float(index) / width);
        }
    }
};

///Demonstation how to mix colours. It displays a fade from red to green
//not finished yet. used as test pattern for glitches in the current state
class AnimatedMixingPattern : public Pattern<RGB>
{
    inline void Calculate(RGB *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
        {
            RGBA red = RGBA(255, 0, 0, index * 16);
            //RGBA green = RGBA(0,255,0, millis()/300);
            //RGBA blue  = RGBA(0,0,255, millis()/-210);
            pixels[index] = red; // + green + blue;
        }
    }
};

//Example pattern for LFO's
class LFOPattern : public Pattern<RGB>
{
    //Create a sinuosidal lfo with a period of 2500ms
    LFO<SinFast> lfo = LFO<SinFast>(2500);
    RGBA red = RGBA(255, 0, 0, 255);

    inline void Calculate(RGB *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            //get the value, but give an argument that shifts the phase a little bit depending on the position of the pixel.
            pixels[index] = red * lfo.getValue(float(index) / width);
    }
};

//example pattern for fades
class FadePattern : public Pattern<RGB>
{
    //create a random interval based on the poisson distribution with an average of 1000ms between each event
    Interval<ConstantInterval> sometimes = Interval<ConstantInterval>(1000);

    //create a fader that fades back in 250ms
    Fade fader = Fade(Linear, 250, Down);
    RGBA red = RGBA(255, 0, 0, 255);

    inline void Calculate(RGB *pixels, int width, bool firstFrame) override
    {
        if (sometimes.Happened())
            fader.reset();

        for (int index = 0; index < width; index++)
            //get the fader value, but add a pixel based start delay (in ms)
            pixels[index] = red * fader.getValue(250 + 250 * index / width);
    }
};

//example pattern for intervals
class PoissonPattern : public Pattern<RGB>
{
    //create a random interval based on the poisson distribution with an average of 1000ms between each event
    Interval<PoissonInterval> sometimes = Interval<PoissonInterval>(1000);
    Fade fader = Fade(Linear, 250, Down);
    RGBA red = RGBA(255, 0, 0, 255);

    inline void Calculate(RGB *pixels, int width, bool firstFrame) override
    {
        if (sometimes.Happened())
            fader.reset();

        for (int index = 0; index < width; index++)
            pixels[index] = red * fader.getValue();
    }
};

//example pattern for timelines, you provide a list of event adn the timeline will
//you you if the event happened in the time between the last frame and the current frame
class TimelinePattern : public Pattern<RGB>
{
    int ShaveAndAHaircut2Bits[8] = {0, 241, 319, 390, 461, 696, 1134, 1357};
    Timeline timeline = Timeline(3000);

    Fade fader = Fade(Linear, 100, Down);
    RGBA red = RGBA(255, 0, 0, 255);

    inline void Calculate(RGB *pixels, int width, bool firstFrame) override
    {
        timeline.FrameStart();
        for (int i = 0; i < 8; i++)
            if (timeline.Happened(ShaveAndAHaircut2Bits[i]))
                fader.reset();

        for (int index = 0; index < width; index++)
            pixels[index] = red * fader.getValue();
    }
};

//watcher is useful to convert a valuechange into an event. You can use it to watch a button press status,
//or the beat counter
class WatcherPattern : public Pattern<RGB>
{
    Watcher<bool> watcher = Watcher<bool>(Rotary::isButtonPressed, Rising);

    Fade fader = Fade(Linear, 300, Down);
    RGBA red = RGBA(255, 0, 0, 255);

    inline void Calculate(RGB *pixels, int width, bool firstFrame) override
    {
        if (watcher.Triggered())
            fader.reset();

        for (int index = 0; index < width; index++)
            pixels[index] = red * fader.getValue();
    }
};

//attach the wachter to the midi driver to check for button presses
class WatcherPattern2 : public Pattern<RGB>
{
    //use a lambda function to pass the note status of a specific note to the watcher
    Watcher<uint8_t> watcher = Watcher<uint8_t>([] () { return Midi::noteStatus(1); }, Rising);

    Fade fader = Fade(Linear, 300, Down);
    RGBA red = RGBA(255, 0, 0, 255);

    void Initialize(){
        Midi::Initialize();
    }

    inline void Calculate(RGB *pixels, int width, bool firstFrame) override
    {
        if (watcher.Triggered())
            fader.reset();

        for (int index = 0; index < width; index++)
            pixels[index] = static_cast<RGBA>(static_cast<RGB>(Hue(Midi::controllerValue(49)*2))) * fader.getValue();
    }
};


class HSLLFOPattern : public Pattern<RGB>
{
    LFO<SinFast> lfo = LFO<SinFast>(2500);

    inline void Calculate(RGB *pixels, int width, bool firstFrame) override
    {
        for (int index = 0; index < width; index++)
            pixels[index] = HSL(255,255,255*lfo.getValue());
    }
};