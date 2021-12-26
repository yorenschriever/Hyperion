#pragma once
#include "input.h"
#include "hardware/websocket/websocket.h"

/*

WebsocketInput works, but is not really stable yet:

1. When combined with neoPixelsOutput, sometimes not all pixel data will be sent.
I'm not sure where this bug originates, but i expect some concurrency issue

2. the messagebuffer (that is enabled with sock->setBufferSize(1)) is not very 
efficient with memory. it allocates/frees for every message. 

3. Only 2 concurrent connections are established. Chromes connection limit for
the same server is 6, so i dont know why this happens

*/
class WebsocketInput : public Input
{

public:
    WebsocketInput(const char *path)
    {
        this->path = path;
    }

    virtual void begin()
    {
        sock = new WebsocketServer(path);
        sock->setBufferSize(1);
    }

    virtual int loadData(uint8_t *dataPtr)
    {
        WebsocketContainer::Run();

        int cb, cbr;
        bool gotFrame = false;
        if ((cbr = sock->readMessage(dataPtr)))
        {
            // Debug.printf("got frame %d\n",cbr);

            // Debug.printf("data = ",cbr);
            // for (int i=0; i < 50; i++)
            //    Debug.printf("%x %x %x, ",dataPtr[i*3+0],dataPtr[i*3+1],dataPtr[i*3+2]);
            // Debug.println("");

            //vTaskDelay(50);

            gotFrame = true;
            cb = cbr;
            missedframecount++;
            sock->sendAll("OK");
        }

        if (!gotFrame)
            return 0;

        usedframecount++;
        missedframecount--;
        Debug.printf("returning %d\n",cb);
        return cb;
    }

private:
    const char *path;
    WebsocketServer *sock;
};