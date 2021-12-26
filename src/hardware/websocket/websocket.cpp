#include "cert.h"
#include "private_key.h"
#include "websocket.h"
#include "debug.h"
using namespace httpsserver;

bool WebsocketContainer::initialized = false;

SSLCert* WebsocketContainer::cert; 
HTTPSServer* WebsocketContainer::secureServer;

bool WebsocketContainer::Initialize()
{
    if (initialized)
        return true;

    cert = new SSLCert(hyperion_crt_DER, hyperion_crt_DER_len,hyperion_key_DER, hyperion_key_DER_len);
    secureServer = new HTTPSServer(cert, 443, MAX_CLIENTS);
    
    ResourceNode * node404     = new ResourceNode("", "GET", &handle404);
    secureServer->setDefaultNode(node404);

    Debug.println("websocket init");

    initialized = true;
    return true;
}

void WebsocketContainer::handle404(HTTPRequest * req, HTTPResponse * res) {
  // Discard request body, if we received any
  // We do this, as this is the default node and may also server POST/PUT requests
  req->discardRequestBody();

  // Set the response status
  res->setStatusCode(200);
  res->setStatusText("Ok");

  // Set content type of the response
  res->setHeader("Content-Type", "text/html");

  // Write a tiny HTTP page
  res->println("<!DOCTYPE html>");
  res->println("<html>");
  res->println("<head><title>Websocket server</title></head>");
  res->println("<body><h1>Websocket server</h1><p>Websockets are running & certificate is accepted.</p></body>");
  res->println("</html>");
}

bool WebsocketContainer::RegisterSocket(WebsocketNode* socket)
{
    if (!Initialize())
        return false;
    //WebsocketNode * socketNode = new WebsocketNode(path, &WebsocketHandlerAdapter::create, path);
    //WebsocketNode * socketNode = new WebsocketNode(path, &socket->createConnection, path);
    secureServer->registerNode(socket);

    secureServer->start();
    return true;
}

void WebsocketContainer::Run(){
    if (initialized && secureServer!=NULL)
        secureServer->loop();
}

WebsocketHandler* WebsocketServer::createhandler(const void* arg) {
    WebsocketServer* server = (WebsocketServer*) arg;
    //Debug.println("Creating new websocket connection");
    RemoteWebsocketClient * handler = new RemoteWebsocketClient(server);
    server->registerClient(handler);
    return handler;
}