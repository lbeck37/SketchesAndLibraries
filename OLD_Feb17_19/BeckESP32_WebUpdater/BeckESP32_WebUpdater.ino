static const char szSketchName[]  = "BeckESP32_WebUpdater.ino";
static const char szFileDate[]    = "Oct 15, 2017, Lenny-c";
#include <BeckLogLib.h>

/*
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ESP32_HTTPUpdateServer.h>

const char* host = "BeckESP32_WebUpdater";
const char* ssid = "Aspot24";
const char* password = "Qazqaz11";

//ESP8266WebServer 		ESP_WebServer(80);
WebServer 		ESP_WebServer(80);

ESP32_HTTPUpdateServer httpUpdater;

void setup(void){

  Serial.begin(115200);
  Serial.println();
  Serial << endl << "setup(): Begin " << szSketchName << ", " << szFileDate << endl;
  //Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while(WiFi.waitForConnectResult() != WL_CONNECTED){
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }

  MDNS.begin(host);

  httpUpdater.setup(&ESP_WebServer);
  ESP_WebServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("\nHTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
  Serial << "IP address: " << WiFi.localIP() << endl;
  Serial << "setup(): End " << endl;
  return;
}	//setup


void loop(void){
  ESP_WebServer.handleClient();
  return;
}	//loop
//Last line
