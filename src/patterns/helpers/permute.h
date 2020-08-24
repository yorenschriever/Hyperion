
#pragma once

#include <Arduino.h>
#include <stdint.h>

//Permute can be used to get a random list of numers in random order
class Permute
{

private:
    int size;

public:
    int* at=0;

    Permute(int size)
    {
        setSize(size);
    }

    void setSize(int size){
        if (size == this->size)
            return;
        if (at)
            delete at;
        at = new int[size];
        this->size=size;
        order();
        permute();
    }

    void permute()
    {
        unsigned i;
        for (i = 0; i <= size-2; i++) {
            unsigned j = i+random(size-i); /* A random integer such that i ≤ j < n */
            /* Swap the randomly picked element with permutation[i] */
            int temp  = at[i];
            at[i]=at[j];
            at[j]=temp;
        }
    }

    void order()
    {
        for (int i =0;i<size;i++)
            at[i]=i;
    }
};