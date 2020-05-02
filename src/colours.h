#pragma once

#include <inttypes.h>
#include "debug.h"

class Colour
{
};

class Monochrome;
class RGB;
class GRB;
class Monochrome12;
class RGB12;

class Monochrome : Colour
{
public:
    //we need an empty constructor that creates a black colour
    Monochrome()
    {
        this->L = 0;
    }

    Monochrome(uint8_t L)
    {
        this->L = L;
    }

    inline void ApplyLut(uint16_t *lut[])
    {
        L = lut[0][L];
    }

    inline void dim(uint8_t value)
    {
        L = (L * value) >> 8;
    }

    operator RGB();
    operator GRB();
    operator RGB12();
    operator Monochrome12();

    uint8_t L;
};

class RGB : Colour
{
public:
    RGB()
    {
        this->R = 0;
        this->G = 0;
        this->B = 0;
    }

    RGB(uint8_t R, uint8_t G, uint8_t B)
    {
        this->R = R;
        this->G = G;
        this->B = B;
    }

    inline void ApplyLut(uint16_t *lut[])
    {
        R = lut[0][R];
        G = lut[1][G];
        B = lut[2][B];
    }

    inline void dim(uint8_t value)
    {
        R = (R * value) >> 8;
        G = (G * value) >> 8;
        B = (B * value) >> 8;
    }

    operator RGB12();
    operator GRB();
    operator Monochrome();
    operator Monochrome12();

    uint8_t R, G, B;
};

class GRB : Colour
{
public:
    GRB()
    {
        this->G = 0;
        this->R = 0;
        this->B = 0;
    }

    GRB(uint8_t G, uint8_t R, uint8_t B)
    {
        this->G = G;
        this->R = R;
        this->B = B;
    }

    inline void ApplyLut(uint16_t *lut[])
    {
        G = lut[1][G];
        R = lut[0][R];
        B = lut[2][B];
    }

    inline void dim(uint8_t value)
    {
        G = (G * value) >> 8;
        R = (R * value) >> 8;
        B = (B * value) >> 8;
    }

    operator RGB12();
    operator RGB();
    operator Monochrome();
    operator Monochrome12();

    uint8_t G, R, B;
};

class RGB12 : Colour
{
public:
    RGB12()
    {
        this->R = 0;
        this->G = 0;
        this->B = 0;
    }

    RGB12(uint16_t R, uint16_t G, uint16_t B)
    {
        this->R = R;
        this->G = G;
        this->B = B;
    }

    inline void ApplyLut(uint16_t *lut[])
    {
        //i choose to scale down the values before i do a lookup in the lut.
        //This means that a 12bit lut also only has 256 entries
        //if there were inputs that provided actual 12 bit values then this
        //would mean the values get quantized. But as far as i can see that
        //wont happen any time soon, and the only 12 bit values are upscaled
        //8bit values. Scaling them back down allows smaller luts, and thus
        //saves almost 4k in ram that would otherwise be used for nothing.
        R = lut[0][R >> 4];
        G = lut[1][G >> 4];
        B = lut[2][B >> 4];
    }

    inline void dim(uint8_t value)
    {
        R = (R * value) >> 8;
        G = (G * value) >> 8;
        B = (B * value) >> 8;
    }

    operator RGB();
    operator GRB();
    operator Monochrome();
    operator Monochrome12();

    uint16_t R, G, B;
};

class Monochrome12 : Colour
{
public:
    Monochrome12()
    {
        this->L = 0;
    }

    Monochrome12(uint16_t L)
    {
        this->L = L;
    }

    inline void ApplyLut(uint16_t *lut[])
    {
        //i choose to scale down the values before i do a lookup in the lut.
        //This means that a 12bit lut also only has 256 entries
        //if there were inputs that provided actual 12 bit values then this
        //would mean the values get quantized. But as far as i can see that
        //wont happen any time soon, and the only 12 bit values are upscaled
        //8bit values. Scaling them back down allows smaller luts, and thus
        //saves almost 4k in ram that would otherwise be used for nothing.
        L = lut[0][L >> 4];
    }

    inline void dim(uint8_t value)
    {
        L = (L * value) >> 8;
    }

    operator RGB();
    operator GRB();
    operator RGB12();
    operator Monochrome();

    uint16_t L;
};

inline Monochrome::operator RGB() { return RGB(L, L, L); }
inline Monochrome::operator RGB12() { return RGB12(L << 4, L << 4, L << 4); }
inline Monochrome::operator Monochrome12() { return Monochrome12(L << 4); }
inline Monochrome::operator GRB() { return GRB(L, L, L); }

inline RGB::operator Monochrome() { return Monochrome((R + G + B) / 3); }
inline RGB::operator RGB12() { return RGB12(R << 4, G << 4, B << 4); }
inline RGB::operator Monochrome12() { return Monochrome12(((R + G + B) << 4) / 3); }
inline RGB::operator GRB() { return GRB(G,R,B); }

inline RGB12::operator Monochrome() { return Monochrome((R + G + B) / 3); }
inline RGB12::operator RGB() { return RGB12(R >> 4, G >> 4, B >> 4); }
inline RGB12::operator Monochrome12() { return Monochrome12((R + G + B) / 3); }
inline RGB12::operator GRB() { return GRB(G >> 4, R >> 4, B >> 4); }

inline GRB::operator Monochrome() { return Monochrome((R + G + B) / 3); }
inline GRB::operator RGB12() { return RGB12(R << 4, G << 4, B << 4); }
inline GRB::operator Monochrome12() { return Monochrome12(((R + G + B) << 4) / 3); }
inline GRB::operator RGB() { return RGB(R,G,B); }