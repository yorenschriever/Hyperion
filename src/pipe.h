#pragma once

#include <inttypes.h>
#include "inputs/input.h"
#include "outputs/output.h"

using HandlerFunc = void (*)(uint8_t *data, int length, Output *out, uint16_t **lut);

const int MTU = 1500;
uint8_t data[MTU]; //i used to malloc this, but the MTU is 1500, so i just set it to the upped bound

class Pipe
{
public:
    Pipe(Input *in, Output *out, HandlerFunc converter = Pipe::transfer, uint16_t **lut = NULL)
    {
        this->in = in;
        this->out = out;
        this->converter = converter;
        this->lut = lut;
    }
    Output *out;
    Input *in;
    HandlerFunc converter;
    uint16_t **lut;

    inline void process()
    {
        //check if the output is done sending the previous message
        if (!out->Ready())
            return;

        //load the new frame from the input, or quit if none was available
        int datalength = in->loadData(data);
        if (datalength == 0)
            return;

        //pipe->handleData(data, datalength);
        this->converter(data, datalength, out, lut);

        out->Show();
    }

    void handleData(uint8_t *data, int length)
    {
        this->converter(data, length, out, lut);
    }

    //Basic transfer function without bells and whistles
    static void transfer(uint8_t *data, int length, Output *out, uint16_t **lut)
    {
        out->SetLength(length);
        //out->setPixel(data,0,length);
        out->SetData(data,length,0);
    }

    //Transfer function that can convert between colour representations and apply LUTs
    template <class T, class U>
    static void transfer(uint8_t *data, int length, Output *out, uint16_t **lut)
    {
        //tell the output the new length so it can allocate enough space for the data
        //we are going to send
        out->SetLength(length / sizeof(T) * sizeof(U));

        for (int i = 0; i < length / sizeof(T); i++)
        {
            //store the ith pixel in an object of type T (source datatype)
            T col = ((T *)data)[i];

            //this is where the actual conversion takes place. static cast will use
            //the cast operators in the Colour classes to convert between the
            //representations.
            U outcol = static_cast<U>(col); 

            //Apply lookup table to do gamma correction and other non linearities
            if (lut)
                outcol.ApplyLut(lut);

            //Pass the data to the output
            //TODO why provide the data pixel by pixel als have a function call overhead for each
            //pixel. cant we dump the entire byte array in one go?
            //out->setPixel((uint8_t *)&outcol,i,sizeof(U));
            out->SetData((uint8_t *)&outcol,sizeof(U),i*sizeof(U));
        }
    }
};
