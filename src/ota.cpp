
#include <Arduino.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "ota.h"
#include "debug.h"

WebServer otaserver(81);

void setupOta()
{
    MDNS.begin("Hyperion");

    otaserver.on("/", HTTP_GET, []() {
        otaserver.sendHeader("Connection", "close");
        otaserver.send(200, "text/html", "Hi");
    });
    //  otaserver.on("/serverIndex", HTTP_GET, []() {
    //    otaserver.sendHeader("Connection", "close");
    //    otaserver.send(200, "text/html", "Hello");
    //  });
    /*handling uploading firmware file */
    otaserver.on(
        "/update", HTTP_POST, []() {
    otaserver.sendHeader("Connection", "close");
    otaserver.send(200, "text/plain", (Update.hasError()) ? "UPDATE FAILED\n" : "Update Successful\n");
    delay(1000);
    ESP.restart(); }, []() {
//    otaActive = true;
//    
//    static boolean stopped = false;
//    if (!stopped){
//      stopAllSockets();
//      stopped=true;
//    }
    
    //Debug.println("update");
    HTTPUpload& upload = otaserver.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Debug.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        //Update.printError(Debug);
        //TODO
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      static int decimate=0;
      if (decimate++%20==0) {
        Debug.printf("progress: %d\n",upload.totalSize);
        Debug.println(otaserver.header("Content-Length"));
      }
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        //Update.printError(Debug);
        //TODO
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Debug.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        //Update.printError(Debug);
        //TODO
      }
    } });
    otaserver.begin();
}

void handleOta()
{
    otaserver.handleClient();
}
