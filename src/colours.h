#pragma once

#include <inttypes.h>
#include "debug.h"
#include "luts/lut.h"

#define CONV8TO12 4096/255
#define CONV12TO8 255/4096

//Colours currently fulfill 2 purposes:
//1. act as a struct to store colur data. This can be used by the 
//transfer function to convert different colour spaces.
//2. as a method to create patterns: layer colours on top of eachother
//I have chosen to combine both things into a single set of classes.
//Colours should support operators to convert themselves to other other
//colours. This will be used by then transfer function when casting one
//colour to another
//They need to have a default constructor that initializes to colour 
//to black. And also a constructor to provide values for all their channels.
//the method dim() is used to to decrease the bightness, for example
//by the faders of the apcmini.
//RGBA is the most advanced colour, it supports alpha, colour blending, 
//colour addition etc. I use RGBA a lot to create contents for patterns
//and then cast to RGB to send it out to the leds. 
class Colour
{
};

class Monochrome;
class RGB;
class GRBW;
class GRB;
class Monochrome12;
class RGB12;
class RGBA;

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

    inline void ApplyLut(LUT *lut)
    {
        L = lut->luts[0][L];
    }

    inline void dim(uint8_t value)
    {
        L = (L * value) >> 8;
    }

    operator RGB();
    operator GRB();
    operator RGB12();
    operator Monochrome12();

    Monochrome operator+ (Monochrome other){
        //layering is implemented as HTP
        return Monochrome(std::max(L,other.L));
    }

    Monochrome& operator+= (const Monochrome& other){
        //layering is implemented as HTP
        L = std::max(L,other.L);
        return *this;
    }

    Monochrome operator* (float scale){
        return Monochrome(constrain(L*scale,0,0xFF));
    }
    Monochrome operator/ (float scale){
        return Monochrome(constrain(L/scale,0,0xFF));
    }

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

    inline void ApplyLut(LUT *lut)
    {
        R = lut->luts[0 % lut->Dimension][R];
        G = lut->luts[1 % lut->Dimension][G];
        B = lut->luts[2 % lut->Dimension][B];
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
    operator RGBA();
    operator GRBW();

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

    inline void ApplyLut(LUT *lut)
    {
        G = lut->luts[1 % lut->Dimension][G];
        R = lut->luts[0 % lut->Dimension][R];
        B = lut->luts[2 % lut->Dimension][B];
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

    inline void ApplyLut(LUT *lut)
    {
        //i choose to scale down the values before i do a lookup in the lut.
        //This means that a 12bit lut also only has 256 entries
        //if there were inputs that provided actual 12 bit values then this
        //would mean the values get quantized. But as far as i can see that
        //wont happen any time soon, and the only 12 bit values are upscaled
        //8bit values. Scaling them back down allows smaller luts, and thus
        //saves almost 4k in ram that would otherwise be used for nothing.
        R = lut->luts[0 % lut->Dimension][R * CONV12TO8];
        G = lut->luts[1 % lut->Dimension][G * CONV12TO8];
        B = lut->luts[2 % lut->Dimension][B * CONV12TO8];
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

class GRBW : Colour
{
public:
    GRBW()
    {
        this->R = 0;
        this->G = 0;
        this->B = 0;
        this->W = 0; 
    }

    GRBW(uint8_t G, uint8_t R, uint8_t B, uint8_t W)
    {
        this->R = R;
        this->G = G;
        this->B = B;
        this->W = W;
    }

    inline void ApplyLut(LUT *lut)
    {
        R = lut->luts[0 % lut->Dimension][R];
        G = lut->luts[1 % lut->Dimension][G];
        B = lut->luts[2 % lut->Dimension][B];
        W = lut->luts[3 % lut->Dimension][W];
    }

    inline void dim(uint8_t value)
    {
        R = (R * value) >> 8;
        G = (G * value) >> 8;
        B = (B * value) >> 8;
        W = (W * value) >> 8;
    }

    uint8_t G, R, B, W;
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

    inline void ApplyLut(LUT *lut)
    {
        //i choose to scale down the values before i do a lookup in the lut.
        //This means that a 12bit lut also only has 256 entries
        //if there were inputs that provided actual 12 bit values then this
        //would mean the values get quantized. But as far as i can see that
        //wont happen any time soon, and the only 12 bit values are upscaled
        //8bit values. Scaling them back down allows smaller luts, and thus
        //saves almost 4k in ram that would otherwise be used for nothing.
        L = lut->luts[0][L * CONV12TO8];
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

class HSL : Colour
{
public:
    HSL()
    {
        this->H = 0;
        this->S = 0;
        this->L = 0;
    }

    HSL(uint8_t H, uint8_t S, uint8_t L)
    {
        this->H = H;
        this->S = S;
        this->L = L;
    }

    // inline void ApplyLut(LUT* lut)
    // {
    //     R = lut->luts[0%lut->Dimension][R];
    //     G = lut->luts[1%lut->Dimension][G];
    //     B = lut->luts[2%lut->Dimension][B];
    // }

    inline void dim(uint8_t value)
    {
        //FIXME this is not the the correct way to dim HSL, 
        //because for example whites will dim via a fully saturated colour now 
        //the should become less white
        L = (L * value) >> 8;
    }

    operator RGB();
    operator Monochrome();

    uint8_t H, S, L;
};

class Hue : Colour
{
public:
    Hue()
    {
        this->H = 0;
    }

    Hue(uint8_t H)
    {
        this->H = H;
    }

    operator RGB();

    uint8_t H;
};

class HSV : Colour
{
public:
    HSV()
    {
        this->H = 0;
        this->S = 0;
        this->V = 0;
    }

    HSV(uint8_t H, uint8_t S, uint8_t V)
    {
        this->H = H;
        this->S = S;
        this->V = V;
    }

    // inline void ApplyLut(LUT* lut)
    // {
    //     R = lut->luts[0%lut->Dimension][R];
    //     G = lut->luts[1%lut->Dimension][G];
    //     B = lut->luts[2%lut->Dimension][B];
    // }

    inline void dim(uint8_t value)
    {
        //is the the correct way to dim HSL?
        V = (V * value) >> 8;
    }

    operator RGB();
    operator Monochrome();

    uint8_t H, S, V;
};

class RGBA : Colour
{
public:
    RGBA()
    {
        this->R = 0;
        this->G = 0;
        this->B = 0;
        this->A = 0;
    }

    RGBA(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
    {
        this->R = R;
        this->G = G;
        this->B = B;
        this->A = A;
    }

    inline void dim(uint8_t value)
    {
        R = (R * value) >> 8;
        G = (G * value) >> 8;
        B = (B * value) >> 8;
    }

    operator RGB() { return RGB(R*A>>8,G*A>>8,B*A>>8); }

    //overload some operators to quickly apply some blend modes
    //https://en.wikipedia.org/wiki/Blend_modes#Normal_blend_mode

    //overload to + operator, so you can use color1 + colour2 to stack 2 colors on top of eachother
    //normal blend mode
    //https://en.wikipedia.org/wiki/Alpha_compositing
    RGBA operator+ (RGBA other){
        //TODO, this is not the method described by wikipedia
        //that one is the one i tried below.
        return RGBA(
            (this->R * other.A + other.R * (0xFF - other.A)) >> 8,
            (this->G * other.A + other.G * (0xFF - other.A)) >> 8,
            (this->B * other.A + other.B * (0xFF - other.A)) >> 8,
            this->A);

        //term in wiki => term here
        //src => other
        //dst => this
        int outA = other.A + (this->A * (0xFF - other.A) >> 8);
        if (outA == 0)
            return RGBA(0,0,0,0); 

        return RGBA(
            (((other.R << 16) + (this->R * this->A * (255-other.A))) / outA) >> 8,
            (((other.G << 16) + (this->G * this->A * (255-other.A))) / outA) >> 8,
            (((other.B << 16) + (this->B * this->A * (255-other.A))) / outA) >> 8,
            outA);
    }

    //TODO multiply blend mode
    // RGBA operator* (RGBA* other){
    //     return RGBA(
    //         ?,
    //         ?,
    //         ?,
    //         this->A);
    // }
    RGBA operator* (float scale){
        return RGBA(R,G,B,constrain(A*scale,0,0xFF));
    }
    RGBA operator/ (float scale){
        return RGBA(R,G,B,constrain(A/scale,0,0xFF));
    }

    uint8_t R, G, B, A;
};



inline Monochrome::operator RGB() { return RGB(L, L, L); }
inline Monochrome::operator RGB12() { return RGB12(L * CONV8TO12, L * CONV8TO12, L * CONV8TO12); }
inline Monochrome::operator Monochrome12() { return Monochrome12(L * CONV8TO12); }
inline Monochrome::operator GRB() { return GRB(L, L, L); }

inline Monochrome12::operator Monochrome() { return Monochrome(L * CONV12TO8); }

inline RGB::operator Monochrome() { return Monochrome((R + G + B) / 3); }
inline RGB::operator RGB12() { return RGB12(R * CONV8TO12, G * CONV8TO12, B * CONV8TO12); }
inline RGB::operator Monochrome12() { return Monochrome12(((R + G + B) * CONV8TO12) / 3); }
inline RGB::operator GRB() { return GRB(G, R, B); }
inline RGB::operator RGBA() { return RGBA(R, G, B, 255); }
inline RGB::operator GRBW() { 
    uint8_t W = min(min(R,G),B);
    return GRBW(G-W, R-W, B-W, W); 
}


inline RGB12::operator Monochrome() { return Monochrome((R + G + B) * CONV12TO8 / 3); }
inline RGB12::operator RGB() { return RGB12(R * CONV12TO8, G * CONV12TO8, B * CONV12TO8); }
inline RGB12::operator Monochrome12() { return Monochrome12((R + G + B) / 3); }
inline RGB12::operator GRB() { return GRB(G * CONV12TO8, R * CONV12TO8, B * CONV12TO8); }

inline GRB::operator Monochrome() { return Monochrome((R + G + B) / 3); }
inline GRB::operator RGB12() { return RGB12(R * CONV8TO12, G * CONV8TO12, B * CONV8TO12); }
inline GRB::operator Monochrome12() { return Monochrome12(((R + G + B) * CONV8TO12) / 3); }
inline GRB::operator RGB() { return RGB(R, G, B); }

inline HSL::operator Monochrome() { return Monochrome(L); }
inline HSV::operator Monochrome() { return Monochrome(V); } //This is not really consitent with the conversion in the line above

inline float hueToRGB(float p, float q, float t){
    if(t < 0) t += 1;
    if(t > 1) t -= 1;
    if(t < 1.f/6) return p + (q - p) * 6 * t;
    if(t < 1.f/2) return q;
    if(t < 2.f/3) return p + (q - p) * (2.f/3 - t) * 6;
    return p;
}

inline HSL::operator RGB()
{
    float r, g, b, h, s, l;

    //Scale each value to 0.0 - 1.0, from type int:
    h = (float)H / 255.0;  //Hue is represented as a range of 360 degrees, mapped on a value range from 0-255
    s = (float)S / 255.0;  //Saturation 
    l = (float)L / 255.0;  //Lightness
    
    if (s == 0)         //Saturation of 0 means a shade of grey
    {
    	r = g = b = l;
    }
    else                //
    {
        float q = l < 0.5 ? l * (1. + s) : l + s - l * s;
        float p = 2. * l - q;
        r = hueToRGB(p, q, h + 1./3);
        g = hueToRGB(p, q, h);
        b = hueToRGB(p, q, h - 1./3);
    }

    return RGB(r*255,g*255,b*255);
}

inline Hue::operator RGB()
{
    int WheelPos = 255 - H;
    if(WheelPos < 85)
    {
        return RGB(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else if(WheelPos < 170)
    {
        WheelPos -= 85;
        return RGB(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    else
    {
        WheelPos -= 170;
        return RGB(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
}


inline HSV::operator RGB()
{
    int H = this->H*360./255.;
    double S = (double)this->S/255.;
    double V = (double)this->V/255.;
  
    double C = S * V;
    double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
    double m = V - C;
    double Rs, Gs, Bs;

	if(H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;	
	}
	else if(H >= 60 && H < 120) {	
		Rs = X;
		Gs = C;
		Bs = 0;	
	}
	else if(H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;	
	}
	else if(H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;	
	}
	else if(H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;	
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;	
	}

    return RGB((Rs + m) * 255,(Gs + m) * 255,(Bs + m) * 255);
}