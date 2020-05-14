#pragma once

#include <inttypes.h>

//Derive form this class when you are going to write new inputs. You should at least implement
//loaddata(), and you can implement begin(), if there is some setup to do. Putting the
//initialization code in begin() instead of the constrcutor makes you code easier to debug.
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
    int getUsedFramecount() { return usedframecount; }
    int getMissedFrameCount() { return missedframecount; }
    int getTotalFrameCount() { return missedframecount + usedframecount; }
    void resetFrameCount()
    {
        usedframecount = 0;
        missedframecount = 0;
    }

protected:
    int volatile usedframecount = 0;
    int volatile missedframecount = 0;
};
