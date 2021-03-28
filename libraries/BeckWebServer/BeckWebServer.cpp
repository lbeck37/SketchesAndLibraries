//BeckWebServer.cpp, 1/13/20a
#include <BeckWebServer.h>
#include <BeckMiniLib.h>
#include <WiFiClient.h>

#ifdef ESP8266
  #include <ESP8266WebServer.h>
  #include <ESP8266WiFi.h>
  ESP8266WebServer    oWebServer(80);    //Use normal port 80
#else   //ESP32
  #include <WebServer.h>
  #include <WiFi.h>
  WebServer           oWebServer(80);
#endif    //ESP8266

//Function protos
void SetupDefaultPages();
void HandleNotFound   ();

void StartWebServer(const char *acHostname){
  Serial << LOG0 << "StartWebServer(): acHostname= " << acHostname << endl;
  SetupDefaultPages();
  oWebServer.begin();
  return;
} //StartWebServer


void SetupDefaultPages(){
  Serial << LOG0 << "SetupDefaultPages(): Begin" << endl;
  oWebServer.onNotFound(HandleNotFound);
  return;
} //SetupDefaultPages


void HandleWebServer(){
  oWebServer.handleClient();
  delay(1);
  return;
} //HandleWebServer


void HandleNotFound(){
  Serial << LOG0 << "BeckWebServer.cpp: HandleNotFound(): Send 404: Not found" << endl;
  oWebServer.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
} //HandleNotFound
//Last line.
