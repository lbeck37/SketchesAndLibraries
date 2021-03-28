//BeckAngularTestPages.cpp, 5/5/19c
#include <BeckAngularTestPages.h>
#include "BeckAngularTestPagesHTML.h"
#include <BeckMiniLib.h>
#include <BeckWebServer.h>
#include <ArduinoJson.h>

double _dLastDegF      = 70.37;
double _dSetpointF     = 71.00;
double _dMaxHeatRangeF = 00.10;
double _dThermoOffDegF = _dSetpointF + _dMaxHeatRangeF;

//Function protos
void HandleTestRoot   ();
void HandleLoginRoot  ();
void HandleTestFunc   ();
void HandleLoginFunc  ();

// Enough space for:
// + 1 object with 6 members
const int wJsonCapacity = JSON_OBJECT_SIZE(6);

//For GET
StaticJsonDocument<wJsonCapacity>     oGetJsonDoc;
StaticJsonDocument<wJsonCapacity>     oPostJsonDoc;
char      szJsonText[128];


void SetupAngularTestPages(){
  Serial << LOG0 << "SetupAngularTestPages(): Begin" << endl;

  Serial << LOG0 << "SetupAngularTestPages(): Set up handlers" << endl;
  oWebServer.on("/ajs3", HTTP_GET, [](){
    Serial << LOG0 << "SetupAngularTestPages(): Got a GET on /ajs" << endl;
    oWebServer.sendHeader("Connection", "close");
    oWebServer.send(200, "text/html", acAngularTestPagesHTML);
  });
  return;
} //SetupAngularTestPages


void SetupFormTestPages(void){
  oWebServer.on("/"            , HTTP_GET,   HandleTestRoot);
  oWebServer.on("/login"       , HTTP_GET,   HandleLoginRoot);
  oWebServer.on("/handlelogin" , HTTP_POST,  HandleLoginFunc);
  oWebServer.on("/handletest"  , HTTP_POST,  HandleTestFunc);

  oWebServer.begin();                            // Actually start the server
  Serial << endl << LOG0 << "setup(): ESP8266WebServer started" << endl;
  return;
} //SetupFormTestPages


void HandleTestRoot() {
  oWebServer.send(200, "text/html", "<form action=\"/handletest\" method=\"POST\"><input type=\"text\" name=\"word1\" placeholder=\"Word #1\"></br><input type=\"text\" name=\"word2\" placeholder=\"Word #2\"></br><input type=\"submit\" value=\"Next\"></form><p>Enter words for test</p>");
  return;
} //HandleTestRoot


void HandleLoginRoot() {                          // When URI / is requested, send a web page with a button to toggle the LED
  //oWebServer.send(200, "text/html", "<form action=\"/login\" method=\"POST\"><input type=\"text\" name=\"username\" placeholder=\"Username\"></br><input type=\"password\" name=\"password\" placeholder=\"Password\"></br><input type=\"submit\" value=\"Login\"></form><p>Try 'dude' and '111' ...</p>");
  oWebServer.send(200, "text/html", "<form action=\"/handlelogin\" method=\"POST\"><input type=\"text\" name=\"username\" placeholder=\"Username\"></br><input type=\"password\" name=\"password\" placeholder=\"Password\"></br><input type=\"submit\" value=\"Login\"></form><p>Try 'dude' and '111' ...</p>");
  return;
} //HandleLoginRoot


void HandleTestFunc() {
  Serial << endl << LOG0 << "HandleTestFunc(): Received word1= " << oWebServer.arg("word1") <<
      ", password= " << oWebServer.arg("word2") << endl;

  oWebServer.send(200, "text/html", "<h1>Thanks for entering <i>" + oWebServer.arg("word1") + "</i> <i>" + oWebServer.arg("word2") + "</i>, You Rock!</h1><p>Please proceed to the next fork in the road and take it. <i>YB</i></p>");
  return;
} //HandleTestFunc


void HandleLoginFunc() {                         // If a POST request is made to URI /login
  if( ! oWebServer.hasArg("username") || ! oWebServer.hasArg("password")
      || oWebServer.arg("username") == NULL || oWebServer.arg("password") == NULL) { // If the POST request doesn't have username and password data
    oWebServer.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }

  if(oWebServer.arg("username") == "dude" && oWebServer.arg("password") == "111") { // If both the username and the password are correct
    oWebServer.send(200, "text/html", "<h1>Welcome, " + oWebServer.arg("username") + "!</h1><p>Login successful</p>");
  } else {                                                                              // Username and password don't match
    oWebServer.send(401, "text/plain", "401: Unauthorized");
  }
  return;
} //HandleLoginFunc

/* Actual original HTML text
<form action="/login" method="POST">
    <input type="text" name="username" placeholder="Username"></br>
    <input type="password" name="password" placeholder="Password"></br>
    <input type="submit" value="Login">
</form>
<p>
    Try 'dude' and '111' ...
</p>
*/

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
} //CallBackFunc GetThermoData


String szLastDegF() {
  return(String(_dLastDegF, 2));
} //szLastDegF


void ChangeDataValues() {
  Serial << LOG0 << "ChangeDataValues(): Begin" << endl;
  static const double dChange= 0.01;

  _dLastDegF       += dChange;
  _dSetpointF      += dChange;
  _dMaxHeatRangeF  += (dChange / 10.0);
  _dThermoOffDegF   = _dSetpointF + _dMaxHeatRangeF;
  return;
}

void HandleThermoDataGet() {
  ChangeDataValues();
  //Add args to Json Doc
  oGetJsonDoc["dLastDegF"]        = _dLastDegF;
  oGetJsonDoc["dSetpointF"]       = _dSetpointF;
  oGetJsonDoc["dMaxHeatRangeF"]   = _dMaxHeatRangeF;

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

  _dLastDegF      = oPostJsonDoc["dLastDegF"];
  _dSetpointF     = oPostJsonDoc["dSetpointF"];
  _dMaxHeatRangeF = oPostJsonDoc["dMaxHeatRangeF"];

  _dThermoOffDegF= _dSetpointF + _dMaxHeatRangeF;
  //Serial << LOG0 << "HandleThermoDataPost(): _dLastDegF, _dSetpointF, _dMaxHeatRangeF, _dThermoOffDegF: " << endl;
  //Serial << LOG0 << "HandleThermoDataPost(): " << _dLastDegF << ", " << _dSetpointF << ", " << _dMaxHeatRangeF << ", " << _dThermoOffDegF << endl;
  return;
} //HandleThermoDataPost


String szDummyDegF() {
  static float    fCurrent= 0.37;
  float fReturn= fCurrent;
  fCurrent += 1.00;
  return(String(fReturn, 2));
} //szDummyDegF


float fDummyDegF() {
  static float    fCurrent= 0.37;
  float fReturn= fCurrent;
  fCurrent += 1.00;
  return(fReturn);
} //fDummyDegF


String szSetPointDegF() {
  return(String(_dSetpointF, 2));
} //szSetPointDegF


String szTermoOffDegF() {
  return(String(_dThermoOffDegF, 2));
} //szTermoOffDegF


void SetupTermostatTestPages(){
  Serial << LOG0 << "SetupTermostatTestPages(): Set up handlers" << endl;
  oWebServer.on("/Thermostat", HTTP_GET, [](){
    Serial << LOG0 << "SetupTermostatTestPages(): Got a GET on /Thermostat, sending acThermostatTestPagesHTML[]" << endl;
    oWebServer.sendHeader("Connection", "close");
    oWebServer.send(200, "text/html", acThermostatTestPagesHTML);
    return;
  });

  oWebServer.on("/ThermoGet", HTTP_GET, [](){
    Serial << LOG0 << "SetupTermostatTestPages(): Got a GET on /ThermoGet" << endl;
    HandleThermoDataGet();
  });

  oWebServer.on("/ThermoPost", HTTP_POST, [](){
    Serial << LOG0 << "SetupTermostatTestPages(): Got a POST on /ThermoPost" << endl;
    HandleThermoDataPost();
    return;
  });

  oWebServer.on("/LastDegF", HTTP_GET, [](){
    Serial << LOG0 << "SetupTermostatTestPages(): Got a GET on /LastDegF" << endl;
    //oWebServer.send(200, "text/plain", szLastDegF().c_str());
    oWebServer.send(200, "text/plain", szDummyDegF().c_str());
    //oWebServer.send(200, "text/plain", fDummyDegF());
    return;
  });

  oWebServer.on("/SetPointDegF", HTTP_GET, [](){
    oWebServer.send(200, "text/plain", szSetPointDegF().c_str());
    return;
  });

  oWebServer.on("/TermoOffDegF", HTTP_GET, [](){
    oWebServer.send(200, "text/plain", szTermoOffDegF().c_str());
    return;
  });

  return;
} //SetupTermostatTestPages
//Last line.
