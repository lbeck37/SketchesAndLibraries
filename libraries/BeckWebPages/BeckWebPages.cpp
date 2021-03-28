//BeckWebPages.cpp, 8/11/19a
#include <BeckWebPages.h>
//#include "BeckWebPagesHTML.h"
#include "BeckThermoFirebaseHTML.h"
#include <BeckMiniLib.h>
#include <BeckThermoLib.h>
#include <BeckWebServer.h>
#include <ArduinoJson.h>

// Enough space for:
// + 1 object with 6 members
const int wJsonCapacity = JSON_OBJECT_SIZE(6);

//For GET
StaticJsonDocument<wJsonCapacity>     oGetJsonDoc;
StaticJsonDocument<wJsonCapacity>     oPostJsonDoc;
char      szJsonText[128];


void HandleThermoDataGet() {
  //Add args to Json Doc
  oGetJsonDoc["dLastDegF"]        = _fLastDegF;
  oGetJsonDoc["dSetpointF"]       = _fSetpointF;
  oGetJsonDoc["dMaxHeatRangeF"]   = _fMaxHeatRangeF;

  Serial << LOG0 << "HandleThermoDataGet(): Call serializeJson()" << endl;
  serializeJson(oGetJsonDoc, szJsonText);
  Serial << LOG0 << "HandleThermoDataGet(): Sending " << szJsonText << endl;

  oWebServer.send(200, "text/plain", szJsonText);
  return;
} //HandleThermoDataGet


void HandleThermoDataPost() {
  const String szPlain= oWebServer.arg("plain");
  Serial << LOG0 << "HandleThermoDataPost(): Received " << szPlain << endl;
  DeserializationError oDeserialError= deserializeJson(oPostJsonDoc, szPlain);
  if(oDeserialError != DeserializationError::Ok){
    Serial << LOG0 << "HandleThermoDataPost(): ERROR: oDeserialError is NOT DeserializationError::Ok " << endl;
  }
  else{
    oWebServer.send(200, "text/plain", "200: Good POST");
  }

/*
  _fLastDegF      = oPostJsonDoc["dLastDegF"];
  _fSetpointF     = oPostJsonDoc["dSetpointF"];
  _fMaxHeatRangeF = oPostJsonDoc["dMaxHeatRangeF"];
*/

  _fThermoOffDegF= _fSetpointF + _fMaxHeatRangeF;
  //Serial << LOG0 << "HandleThermoDataPost(): _fLastDegF, _fSetpointF, _fMaxHeatRangeF, _fThermoOffDegF: " << endl;
  //Serial << LOG0 << "HandleThermoDataPost(): " << _fLastDegF << ", " << _fSetpointF << ", " << _fMaxHeatRangeF << ", " << _fThermoOffDegF << endl;
  return;
} //HandleThermoDataPost


void SetupTermoWebPage(){
  Serial << LOG0 << "SetupTermoWebPage(): Set up handlers" << endl;
  oWebServer.on("/Thermostat", HTTP_GET, [](){
    Serial << LOG0 << "SetupTermoWebPage(): Got a GET on /Thermostat, sending web page" << endl;
    oWebServer.sendHeader("Connection", "close");
    //oWebServer.send(200, "text/html", acTestPageHTML);
    //oWebServer.send(200, "text/html", acThermoWebPageHTML);
    oWebServer.send(200, "text/html", acThermoFirebaseHTML);
    return;
  });

  oWebServer.on("/ThermoGet", HTTP_GET, [](){
    Serial << LOG0 << "SetupTermoWebPage(): Got a GET on /ThermoGet" << endl;
    HandleThermoDataGet();
  });

  oWebServer.on("/ThermoPost", HTTP_POST, [](){
    Serial << LOG0 << "SetupTermoWebPage(): Got a POST on /ThermoPost" << endl;
    HandleThermoDataPost();
    return;
  });

  oWebServer.on("/LastDegF", HTTP_GET, [](){
    Serial << LOG0 << "SetupTermoWebPage(): Got a GET on /LastDegF" << endl;
    //oWebServer.send(200, "text/plain", szLastDegF().c_str());
    //oWebServer.send(200, "text/plain", szDummyDegF().c_str());
    //oWebServer.send(200, "text/plain", fDummyDegF());
    return;
  });

  return;
} //SetupTermoWebPage
//Last line.
