const char szSketchName[]  = "BeckE32_BasicOTA.ino";
const char szFileDate[]    = "Apr 7, 2018, Lenny-d, Sloeber 4.2";
// "See https://diyprojects.io/arduinoota-esp32-wi-fi-ota-wireless-update-arduino-ide"
//#include <BeckLogLib.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Streaming.h>
//#include "HardwareSerial.h"

/*
const char* ssid = "..........";
const char* password = "..........";
*/
const char*       ssid      = "Aspot24";
const char*       password  = "Qazqaz11";
unsigned long     ulBaud    = 115200;
//int     sBaud    = 115200;

void setup() {
  //Serial.begin(ulBaud);
  Serial.begin(115200);
  Serial.println("Booting");
  Serial << endl << "setup(): Begin " << szSketchName << ", " << szFileDate << endl;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  } //while

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  //ArduinoOTA.setHostname("myesp32");
  ArduinoOTA.setHostname("ESP32-DevKitC");
  //ArduinoOTA.setHostname("SF-ESP32-Thing");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");
  //ArduinoOTA.setPassword("tuna37");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    Serial << endl << "ArduinoOTA.onStart(): Begin " << endl;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      Serial << "ArduinoOTA.onStart(): U_FLASH " << endl;
      type = "sketch";
    }
    else { // U_SPIFFS
      Serial << "ArduinoOTA.onStart(): U_SPIFFS " << endl;
      type = "filesystem";
    }
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial << "ArduinoOTA.onEnd(): Begin " << endl;
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("ArduinoOTA.onProgress(): %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("ArduinoOTA.onError(): Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  Serial << "setup(): Call ArduinoOTA.begin()" << endl;
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial << "setup(): After ArduinoOTA.begin(): " << szSketchName << ", " << szFileDate << endl;
  Serial << "setup(): WiFi.localIP()= " << WiFi.localIP() << endl;

  Serial << "setup(): End " << endl;
  return;
} //setup


void loop() {
  ArduinoOTA.handle();
  return;
} //loop
//Last line
