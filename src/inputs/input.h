#pragma once

#include <inttypes.h>

class Input 
{

  public:

    uint8_t getUsedFramecount() { return usedframecount; }
    uint8_t getMissedFrameCount() { return missedframecount; }
    uint8_t getTotalFrameCount() { return missedframecount + usedframecount; }
    void resetFrameCount() {
        usedframecount = 0;
        missedframecount = 0;
    }

    virtual void begin();
    virtual int loadData(uint8_t* dataPtr);

  protected:
    
    int usedframecount=0;
    int missedframecount=0;
};
