#include <inttypes.h>
#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <WebsocketHandler.hpp>
#include <deque>
#include "debug.h"

#define MAX_CLIENTS 10

using namespace httpsserver;

class WebsocketServer;
class RemoteWebsocketClient;

//mother class, contains all websocketservers
class WebsocketContainer
{
public:
    static bool Initialize();
    static bool RegisterSocket(WebsocketNode *socket);
    static void Run();

private:
    static bool initialized;
    static SSLCert *cert;
    static HTTPSServer *secureServer;
    static void handle404(HTTPRequest *req, HTTPResponse *res);
};

class WebsocketServer
{
public:
    using WebsocketMessageHandler = void (*)(RemoteWebsocketClient *client, WebsocketServer *server, std::string);

    WebsocketServer(const char *path)
    {
        WebsocketContainer::Initialize();
        serverNode = new WebsocketNode(path, &createhandler, this);
        WebsocketContainer::RegisterSocket(serverNode);
    }

    void send(RemoteWebsocketClient *client, std::string msg)
    {
        ((WebsocketHandler *)client)->send(msg, WebsocketHandler::SEND_TYPE_TEXT);
    }

    void sendOther(RemoteWebsocketClient *exclude, std::string msg)
    {
        for (auto client : clients)
            if (client != exclude)
                send(client, msg);
    }

    void sendAll(std::string msg)
    {
        for (auto client : clients)
            send(client, msg);
    }

    int connectionCount()
    {
        return clients.size();
    }

    void onMessage(WebsocketMessageHandler handler)
    {
        _handler = handler;
    }

    int readMessage(uint8_t *buffer)
    {
        if (bufferedMessages.empty())
            return 0;

        std::string *msg = bufferedMessages.front();
        int len = msg->length();
        memcpy(buffer, msg->data(), len);
        delete msg;
        bufferedMessages.pop_front();
        return len;
    }

    void setBufferSize(int size)
    {
        maxBufferSize = size;
    }

private:
    static WebsocketHandler *createhandler(const void *arg);
    std::vector<RemoteWebsocketClient *> clients;
    WebsocketNode *serverNode;
    WebsocketMessageHandler _handler = NULL;

    std::deque<std::string *> bufferedMessages = std::deque<std::string *>();
    int maxBufferSize = 0;

    void registerClient(RemoteWebsocketClient *client)
    {
        clients.push_back(client);
    }

    void unregisterClient(RemoteWebsocketClient *client)
    {
        clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    }

    void bufferMessage(std::string message)
    {
        if (maxBufferSize == 0)
            return;

        while (bufferedMessages.size() > maxBufferSize)
        {
            delete bufferedMessages.front();
            bufferedMessages.pop_front();
        }

        std::string * heapstr = new std::string(message);
        bufferedMessages.push_back(heapstr);
    }

    friend class RemoteWebsocketClient;
};

class RemoteWebsocketClient : public WebsocketHandler
{
public:
    RemoteWebsocketClient(WebsocketServer *server)
    {
        _server = server;
    }

    // This method is called when a message arrives
    void onMessage(WebsocketInputStreambuf *input)
    {
        if (_server->_handler == NULL && _server->maxBufferSize == 0)
            return;

        std::ostringstream ss;
        std::string msg;
        ss << input;
        msg = ss.str();

        if (_server->maxBufferSize != 0)
            _server->bufferMessage(msg);

        if (_server->_handler != NULL)
            _server->_handler(this, _server, msg);
    }

    // Handler function on connection close
    void onClose()
    {
        _server->unregisterClient(this);
    }

private:
    WebsocketServer *_server;
};