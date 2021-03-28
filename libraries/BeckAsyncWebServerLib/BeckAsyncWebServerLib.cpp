//BeckAsyncWebServerLib.cpp, 4/16/19a
#include <BeckAsyncWebServerLib.h>
#include <BeckMiniLib.h>
#include <BeckThermoLib.h>
#include "ESPAsyncWebServer.h"
#include <Streaming.h>
#ifdef ESP8266
  #include <ESP8266mDNS.h>
#else   //ESP32
  #include <ESPmDNS.h>
#endif    //ESP8266

#include "BeckAsyncWebServerHTML.h"
/*
const char index_html[] PROGMEM = R"rawliteral(
  #include "BeckAsyncWebServerHTML.h"
  )rawliteral";
*/


AsyncWebServer oAsyncWebServer(80);

String CallBackFunc(const String& var){
  if(var == "TEMPERATURE"){
    return "99.99";
  }
  else if(var == "SET_POINT"){
    return "99.99";
  }
  else if(var == "THERMO_OFF"){
    return "99.99";
  }
  return String();
} //CallBackFunc


String szLastDegF() {
  return(String(_fLastDegF, 2));
} //szLastDegF


String szSetPointDegF() {
  return(String(_fSetpointF, 2));
} //szSetPointDegF


String szTermoOffDegF() {
  return(String(_fThermoOffDegF, 2));
} //szTermoOffDegF


void StartAsyncWebServer(const char *acHostname){
  Serial << LOG0 << "StartAsyncWebServer(): Begin" << endl;
  //Use MDNS for host name resolution
  Serial << LOG0 << "StartAsyncWebServer(): Start mDNS for " << acHostname << endl;
  if (!MDNS.begin(acHostname)) {
    Serial << LOG0 << "StartAsyncWebServer(): Error setting up MDNS responder" << endl;
    while (1) {
      delay(1000);
    } //while
  } //if (!MDNS.begin(acHostname))

  Serial << LOG0 << "StartAsyncWebServer(): Set up handlers" << endl;
  oAsyncWebServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, CallBackFunc);
  });

  oAsyncWebServer.on("/LastDegF", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", szLastDegF().c_str());
  });

  oAsyncWebServer.on("/SetPointDegF", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", szSetPointDegF().c_str());
  });

  oAsyncWebServer.on("/TermoOffDegF", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", szTermoOffDegF().c_str());
  });

  // Start server
  Serial << LOG0 << "StartAsyncWebServer(): Call oAsyncWebServer.begin()" << endl;
  oAsyncWebServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial << LOG0 << "StartAsyncWebServer(): Open http://" << acHostname << " to access webpage" << endl;
  Serial << LOG0 << "StartAsyncWebServer(): Open http://" << WiFi.localIP() << " to access webpage" << endl;
  return;
} //StartAsyncWebServer
//Last line
