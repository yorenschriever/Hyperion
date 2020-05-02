#pragma once

#include <inttypes.h>

class Input
{

public:
    //Implement this method to initialize your input. 
    //do not put code in the constructur, put it here
    virtual void begin();

    //Implement this function to provide data to the pipe
    //When this function is called you should put copy the
    //most recent pixel data into the dataPtr and return
    //the number of byte you have written. Ideally you should 
    //have a buffer ready to be copied, as every execution in 
    //this function slows down the main loop.
    virtual int loadData(uint8_t *dataPtr);

    //Some functions to keep track of performance.
    //During loadData you can update usedframecount and missedframecount
    //to keep track of the number of frames that that we used by the pipe,
    //and the ones that your input received, but were already 
    //replaced before loadData was called
    uint8_t getUsedFramecount() { return usedframecount; }
    uint8_t getMissedFrameCount() { return missedframecount; }
    uint8_t getTotalFrameCount() { return missedframecount + usedframecount; }
    void resetFrameCount()
    {
        usedframecount = 0;
        missedframecount = 0;
    }
protected:
    int usedframecount = 0;
    int missedframecount = 0;
};
