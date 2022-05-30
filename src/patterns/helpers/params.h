#pragma once
#include "colours.h"

class Params
{
public:
    static RGBA getPrimaryColour() { return primaryColour; }
    static RGBA getSecondaryColour() { return secondaryColour; }
    static RGBA getHighlightColour() { return highlightColour; }
    static float getVelocity(float start = 0, float end = 1) { return convert(start, end, velocity); }
    static float getintensity(float start = 0, float end = 1) { return convert(start, end, intensity); }
    static float getVariant(float start = 0, float end = 1) { return convert(start, end, variant); }

    // static float getVelocityInv(float start = 0, float end = 1) { return convert(start, end, velocity); }
    // static float getintensityInv(float start = 0, float end = 1) { return convert(start, end, intensity); }
    // static float getVariantInv(float start = 0, float end = 1) { return convert(start, end, variant); }

    static RGBA primaryColour;
    static RGBA secondaryColour;
    static RGBA highlightColour;
    static float velocity;
    static float intensity;
    static float variant;

private:
    static float convert(float start, float end, float value)
    {
        float distance = end - start;
        return start + value * distance;
    }
};

RGBA Params::primaryColour = RGB(255, 0, 0);
RGBA Params::secondaryColour = RGB(0, 255, 0);
RGBA Params::highlightColour = RGB(0, 0, 255);
float Params::velocity = 0.5;
float Params::intensity = 0.5;
float Params::variant = 0.5;
