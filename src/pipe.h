#pragma once

#include <inttypes.h>
#include "inputs/input.h"
#include "outputs/output.h"
#include "lut.h"

using HandlerFunc = int (*)(uint8_t *data, int length, Output *out, LUT *lut);

const int MTU = 3100;
uint8_t data[MTU]; //i used to malloc this, but the MTU is 1500, so i just set it to the upped bound

//Pipes are object that tie everything together: a pipe has an input and an output.
//The input privodes the data and the output sends it out to the lamps.
//Additionally you can provide a custom transfer function. This is the function that will copy the data
//from input to output. One that is often used is the static function 
//Pipe::transfer<SourceColour,TargetColour>. This function will convert the data from source format 
//to target format. eg. from RGB to RGBW or from RGB to Monochrome. The pipe also stores a reference
//to the lut that that the transfer function can apply. 
class Pipe
{
public:
    Pipe(Input *in, Output *out, HandlerFunc converter = Pipe::transfer, LUT *lut = NULL)
    {
        this->in = in;
        this->out = out;
        this->converter = converter;
        this->lut = lut;
    }
    Output *out;
    Input *in;
    HandlerFunc converter;
    LUT *lut;

    inline void process()
    {
        //check if the output is done sending the previous message
        if (!out->Ready())
            return;

        //load the new frame from the input, or quit if none was available
        //TODO: either pass the MTU size here so the input knows when to stop copying,
        //or better: ask for the size and realloc more space if needed.
        int datalength = in->loadData(data);
        if (datalength == 0)
            return;

        numPixels = this->converter(data, datalength, out, lut);

        out->Show();
    }

    //Basic transfer function without bells and whistles
    static int transfer(uint8_t *data, int length, Output *out, LUT *lut)
    {
        out->SetLength(length);
        out->SetData(data, length, 0);
        return length; //unkown number of pixels, assume 1 pixel per byte
    }

    //Transfer function that can convert between colour representations and apply LUTs
    //This function has been thoroughly optimized, because this is the workhorse of the
    //device. When making changes be absolutely sure that you keep performance in mind.
    //See docs/speed and call stack for steps taken to optimize
    template <class T_SOURCECOLOUR, class T_TARGETCOLOUR>
    static int transfer(uint8_t *data, int length, Output *out, LUT *lut)
    {
        //tell the output the new length so it can allocate enough space for the data
        //we are going to send
        //todo this function should return the actual length it has allocated, so we can use tht later
        out->SetLength(length / sizeof(T_SOURCECOLOUR) * sizeof(T_TARGETCOLOUR));

        int numPixels = length / sizeof(T_SOURCECOLOUR);
        for (int i = 0; i < numPixels; i++)
        {
            //store the ith pixel in an object of type T (source datatype)
            T_SOURCECOLOUR col = ((T_SOURCECOLOUR *)data)[i];

            //this is where the actual conversion takes place. static cast will use
            //the cast operators in the Colour classes to convert between the
            //representations.
            T_TARGETCOLOUR outcol = static_cast<T_TARGETCOLOUR>(col);

            //Apply lookup table to do gamma correction and other non linearities
            if (lut)
                outcol.ApplyLut(lut);

            //Pass the data to the output
            //TODO why provide the data pixel by pixel als have a function call overhead for each
            //pixel. cant we dump the entire byte array in one go?
            //What about a function that passes us a datapointer, and we place the items directly into it
            //this saves a function call for each pixel, and an additional memcpy.
            out->SetData((uint8_t *)&outcol, sizeof(T_TARGETCOLOUR), i * sizeof(T_TARGETCOLOUR));
        }
        return numPixels;
    }

    int getNumPixels()
    {
        return numPixels;
    }

private:
    int numPixels; //keep track of the number of pixels is for stats only.
};
