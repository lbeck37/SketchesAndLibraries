static const char szSketchName[]  = "BeckESP8266_WebUpdater.ino";
static const char szFileDate[]    = "Oct 14, 2017, Lenny-b";
#include <BeckLogLib.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

/*
const char* host = "esp8266-webupdate";
const char* ssid = "........";
const char* password = "........";
*/
const char* host = "BeckESP8266_WebUpdater";
const char* ssid = "Aspot24";
const char* password = "Qazqaz11";

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

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

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("\nHTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
  Serial << "IP address: " << WiFi.localIP() << endl;
  Serial << "setup(): End " << endl;
  return;
}	//setup


void loop(void){
  httpServer.handleClient();
  return;
}	//loop
//Last line
