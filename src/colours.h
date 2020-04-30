#pragma once

#include <inttypes.h>

class Colour {

};

class Monochrome;
class RGB ;
class RGBi;
class RGB12;

class Monochrome : Colour{
    public:
        //we need an empty constructor that creates a black colour
        Monochrome(){
            this->L=0;
        }

        Monochrome(uint8_t L){
            this->L=L;
        }

        inline void ApplyLut(uint16_t* lut[]){
          L=lut[0][L];
        }

        inline void dim(uint8_t value){
            L=(L*value) >> 8;
        }

        operator RGB();
        operator RGB12();
        operator RGBi();
  
        uint8_t L;
};

class RGBi : Colour{
    public:

        RGBi(){
            this->Ri=255;
            this->Gi=255;
            this->Bi=255;            
        }

        RGBi(uint8_t Ri, uint8_t Gi, uint8_t Bi){
            this->Ri=Ri;
            this->Gi=Gi;
            this->Bi=Bi;
        }

        inline void ApplyLut(uint16_t* lut[]){
          Ri=lut[0][Ri];
          Gi=lut[1][Gi];
          Bi=lut[2][Bi];
        }

        operator RGB();
        operator RGB12();
        operator Monochrome();

        uint8_t Ri,Gi, Bi;
};

class RGB : Colour{
    public:

        RGB(){
            this->R=0;
            this->G=0;
            this->B=0;            
        }

        RGB(uint8_t R, uint8_t G, uint8_t B){
            this->R=R;
            this->G=G;
            this->B=B;
        }

        inline void ApplyLut(uint16_t* lut[]){
          R=lut[0][R];
          G=lut[1][G];
          B=lut[2][B];
        }

        operator RGBi();
        operator RGB12();
        operator Monochrome();

        uint8_t R=0, G=0, B=0;
};

class RGB12 : Colour{
    public:

        RGB12(){
            this->R=0;
            this->G=0;
            this->B=0;
        }

        RGB12(uint16_t R, uint16_t G, uint16_t B){
            this->R=R;
            this->G=G;
            this->B=B;
        }

        inline void ApplyLut(uint16_t* lut[]){
          //i choose to scale down the values before i do a lookup in the lut.
          //This means that a 12bit lut also only has 256 entries
          //if there were inputs that provided actual 12 bit values then this
          //would mean the values get quantized. But as far as i can see that 
          //wont happen any time soon, and the only 12 bit values are upscaled
          //8bit values. Scaling them back down allows smaller luts, and thus
          //saves almost 4k in ram that would otherwise be used for nothing.
          R=lut[0][R>>4];
          G=lut[1][G>>4];
          B=lut[2][B>>4];
        }

        operator RGB();
        operator RGBi();
        operator Monochrome();

        uint16_t R, G, B;
};

Monochrome::operator RGB(){ return RGB(L,L,L); }
Monochrome::operator RGBi(){ return RGBi(L,L,L); }
Monochrome::operator RGB12(){ return RGB12(L<<4,L<<4,L<<4); }

RGB::operator Monochrome(){ return Monochrome((R+G+B)/3); }
RGB::operator RGBi(){ return RGBi(255-R,255-G,255-B); }
RGB::operator RGB12(){ return RGB12(R<<4,G<<4,B<<4); }

RGBi::operator Monochrome(){ return Monochrome((Ri+Gi+Bi)/3); }
RGBi::operator RGB(){ return RGB(255-Ri,255-Gi,255-Bi); }