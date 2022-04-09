#pragma once

#include <inttypes.h>
#include <vector>

struct PixelPosition
{
    float x;
    float y;
};

typedef std::vector<PixelPosition> PixelMap;