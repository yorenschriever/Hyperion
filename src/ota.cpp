
#include <Arduino.h>
#include <WebServer.h>
//#include "ESP32AsyncWebServer.h"
#include <ESPmDNS.h>
#include <Update.h>
#include "ota.h"

WebServer otaserver(81);
//
//volatile boolean otaActive = false;
//
//boolean getOtaActive()
//{
//  return otaActive;
//}

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
  otaserver.on("/update", HTTP_POST, []() {
    otaserver.sendHeader("Connection", "close");
    otaserver.send(200, "text/plain", (Update.hasError()) ? "UPDATE FAILED\n" : "Update Successful\n");
    delay(1000);
    ESP.restart();
  }, []() {
//    otaActive = true;
//    
//    static boolean stopped = false;
//    if (!stopped){
//      stopAllSockets();
//      stopped=true;
//    }
    
    //Serial.println("update");
    HTTPUpload& upload = otaserver.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      static int decimate=0;
      if (decimate++%20==0) {
        Serial.printf("progress: %d\n",upload.totalSize);
        Serial.println(otaserver.header("Content-Length"));
      }
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  otaserver.begin();
}

void handleOta()
{
  otaserver.handleClient();
}
