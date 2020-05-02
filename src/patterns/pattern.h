#pragma once

#include <Arduino.h>
#include <inttypes.h>
#include "pattern.h"
#include "../colours.h"

template<class T>
using Pattern = T (*)(int index, int width, T* lastValue);
