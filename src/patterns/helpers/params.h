#pragma once
#include "colours.h"

class Params
{
public:
    
    static RGBA getPrimaryColour(){ return primaryColour; }
    static RGBA getSecondaryColour(){ return secondaryColour; }
    static RGBA getHighlightColour(){ return highlightColour; }
    static float getVelocity(){ return velocity; }
    static float getIntesity(){ return intesity; }
    static float getVariant(){ return variant; }

    static RGBA primaryColour;
    static RGBA secondaryColour;
    static RGBA highlightColour;
    static float velocity;
    static float intesity;
    static float variant;
};

RGBA Params::primaryColour = RGB(255, 0, 0);
RGBA Params::secondaryColour = RGB(0, 255, 0);
RGBA Params::highlightColour = RGB(0, 0, 255);
float Params::velocity = 0.5;
float Params::intesity = 0.5;
float Params::variant = 0.5;
