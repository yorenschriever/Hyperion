#pragma once

#include <inttypes.h>
#include <math.h>

//LUTs, or look up tables, can help you tweak the colours, and make them look better.
//For example luts can help you with gamma correction, colour correction, and solve
//other non linearities in the lamps response.
class LUT 
{
public:

    int Dimension;
    uint16_t** luts;
};