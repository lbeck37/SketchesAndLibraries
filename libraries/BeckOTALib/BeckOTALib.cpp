//BeckOTALib.cpp, 1/11/20a
#include <BeckOTALib.h>
#include "BeckOTALibHTML.h"
#include <BeckMiniLib.h>
#include <BeckWebServer.h>
#include "ESP8266WiFi.h"

//These are set and used from Beck_Biota.ino
unsigned long       _ulUpdateTimeoutMsec   = 0;
bool                _bOTA_Started         = false;   //Turns off Blynk.

#ifdef ESP8266
  #ifndef UPDATE_SIZE_UNKNOWN
    #define UPDATE_SIZE_UNKNOWN 0xFFFFFFFF
  #endif
#endif    //ESP8266

void SetupOTAWebPages(){
  Serial << LOG0 << "SetupOTAWebPages(): Begin" << endl;

  Serial << LOG0 << "SetupOTAWebPages(): Call oWebServer.on() " << WiFi.localIP() << "/login" << endl;
  oWebServer.on("/login", HTTP_GET, []() {
  //oWebServer.on("/ota", HTTP_GET, []() {
    oWebServer.sendHeader("Connection", "close");
    oWebServer.send(200, "text/html", acOTA_LoginHTML);
  });

  Serial << LOG0 << "SetupOTAWebPages(): Call oWebServer.on() " << WiFi.localIP() << "/serverIndex" << endl;
  oWebServer.on("/serverIndex", HTTP_GET, []() {
    oWebServer.sendHeader("Connection", "close");
    oWebServer.send(200, "text/html", acOTA_ServerHTML);
  });
  oWebServer.on("/update", HTTP_POST, []() {
    oWebServer.sendHeader("Connection", "close");
    oWebServer.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = oWebServer.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      //flashing firmware to ESP
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
  return;
} //SetupOTAWebPages
//Last line.
