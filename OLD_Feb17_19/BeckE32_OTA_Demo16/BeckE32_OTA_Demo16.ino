const String SketchName  = "Beck32_OTA_Demo16.ino";
const String FileDate    = "Apr 8, 2018, Lenny-d";
//Execute the following command line to perform update.
//"python .\espota.py -i 192.168.0.38 -I 192.168.0.37 -p 3232 -P3232 -f BeckE32_OTA_Demo16.bin"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <Streaming.h>
/*
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
*/

/* fill your ssid and password here */
const char* ssid = "Aspot24";
const char* password = "Qazqaz11";

long lastMsg = 0;
int flag = false;

/* LED is pin GIO12 */
int led = 12;

void setup() {

  Serial.begin(115200);
  Serial << endl << "setup(): Begin " << SketchName << ", " << FileDate << endl;
  /* connect to wifi */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  /* Wait for connection */
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  /* set LED as output */
  pinMode(led, OUTPUT);

  /* create a connection at port 3232 */
  ArduinoOTA.setPort(3232);
  /* we use mDNS instead of IP of ESP32 directly */
  ArduinoOTA.setHostname("esp32");

  /* we set password for updating */
  //ArduinoOTA.setPassword("iotsharing");

  /* this callback function will be invoked when updating start */
  ArduinoOTA.onStart([]() {
    Serial.println("Start updating");
  });
  /* this callback function will be invoked when updating end */
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd updating");
  });
  /* this callback function will be invoked when a number of chunks of software was flashed
  so we can use it to calculate the progress of flashing */
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  /* this callback function will be invoked when updating error */
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  /* start updating */
  ArduinoOTA.begin();
  Serial.print("ESP IP address: ");
  Serial.println(WiFi.localIP());
} //setup


void loop() {
  /* this function will handle incoming chunk of SW, flash and respond sender */
  ArduinoOTA.handle();

  long now = millis();
  static int sCount= 0;
  if (now - lastMsg > 5000) {
    lastMsg = now;
    Serial << "loop(): Count= " << sCount++ << endl;
  } //if(now-lastMsg>5000)
} //loop
