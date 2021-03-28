const String SketchName  = "BeckESP_WebServer.ino";
const String FileDate    = "April 9, 2019u";
/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

#include <BeckAsyncWebServerLib.h>
#if ESP32
  #include "WiFi.h"
#else
  #include <ESP8266WiFi.h>
#endif
#include <Streaming.h>

const char* ssid = "Aspot24";
const char* password = "Qazqaz11";

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial << endl << "setup(): Begin " << SketchName << ", " << FileDate << endl;
  // Connect to Wi-Fi
  Serial << "setup(): Call WiFi.begin(" << ssid << ", " << password << ")" << endl;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  } //while
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  StartAsyncWebServer();
  return;
} //setup

void loop(){
  //Do nothing
  return;
} //loop
