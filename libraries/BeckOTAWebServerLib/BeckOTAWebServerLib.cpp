//BeckOTAWebServerLib.cpp, 4/10/19b
#include <BeckOTAWebServerLib.h>
#include "BeckOTAWebServerPages.h"
#include <BeckAsyncWebServerLib.h>
#include <BeckLogLib.h>
#include <BeckMiniLib.h>
#include <ArduinoOTA.h>
#include <WiFiClient.h>
#ifdef ESP8266
  #include <ESP8266WebServer.h>
  #include <Updater.h>
#else   //ESP32   //Not tested
  #include <WebServer.h>
  #include <Update.h>
#endif    //ESP8266

//const char*         acServerIndex         = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
unsigned long       _ulUpdateTimeoutMsec   = 0;
bool                _bOTA_Started         = false;   //Turns off Blynk.

#ifdef ESP8266
  ESP8266WebServer    oOTAWebServer(81);    //OTA uses port 81, ie: 192.168.0.169:81/login
#else   //ESP32
  WebServer           oOTAWebServer(81);
#endif    //ESP8266

#ifdef ESP8266
  #ifndef UPDATE_SIZE_UNKNOWN
    #define UPDATE_SIZE_UNKNOWN 0xFFFFFFFF
  #endif
#endif    //ESP8266

void StartOTAWebServer(void){
  Serial << LOG0 << "StartOTAWebServer(): Begin" << endl;
  oOTAWebServer.on("/login", HTTP_GET, []() {
    oOTAWebServer.sendHeader("Connection", "close");
    oOTAWebServer.send(200, "text/html", loginIndex);
  });
  oOTAWebServer.on("/serverIndex", HTTP_GET, []() {
    oOTAWebServer.sendHeader("Connection", "close");
    oOTAWebServer.send(200, "text/html", serverIndex);
  });
  oOTAWebServer.on("/update", HTTP_POST, []() {
    oOTAWebServer.sendHeader("Connection", "close");
    oOTAWebServer.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = oOTAWebServer.upload();
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
  oOTAWebServer.begin();
  //MDNS.addService("http", "tcp", 80);
  //Serial << LOG0 << "StartOTAWebServer(): Open http://" << acHostname << ":81/login to perform an OTA update" << endl;
  Serial << LOG0 << "StartOTAWebServer(): Open http://" << WiFi.localIP() << ":81/login to perform an OTA update" << endl;
  return;
} //StartOTAWebServer


void HandleOTAServer(void){
  oOTAWebServer.handleClient();
  delay(1);
  return;
} //HandleOTAServer
//Last line.
