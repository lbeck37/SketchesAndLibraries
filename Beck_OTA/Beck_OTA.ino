const char szSketchName[]  = "Beck_OTA.ino";
const char szFileDate[]    = "12/20/19a";	//From Beck_Biota.ino 12/20/19

#ifndef ESP8266
  #define ESP8266
#endif

/*
//#define DO_ALEXA              true
#define DO_ALEXA              false
#define DO_NTP                false
*/
#define DO_ACCESS_POINT       false
//#define DO_ASYNC_WEB_SERVER   false

//#include <BeckBiotaLib.h>
//#include <BeckMiniLib.h>
#include <BeckOTALib.h>
//#include <BeckSwitchLib.h>
//#include <BeckWebPages.h>
//#include <BeckWebServer.h>
//#include <BeckWiFiLib.h>
//#include <FirebaseArduino.h>
#ifdef ESP8266
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif    //ESP8266
#include <Streaming.h>
#include <WiFiClient.h>

#if DO_ACCESS_POINT
  #include <BeckAccessPointLib.h>
#endif

const char* 	WiFi_SSID 			= "Aspot24";
const char* 	WiFi_Password 		= "Qazqaz11";

const long    	lSerialMonitorBaud  = 115200;

void setup(){
  Serial.begin(lSerialMonitorBaud);
  delay(100);
  Serial << endl << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;

    WiFi.begin(WiFi_SSID, WiFi_Password);
    //Serial.print("Connecting to Wi-Fi");
    Serial << "Setup(): Connecting to WiFi" << endl;

    while (WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(300);
    }	//while

    Serial << endl << "Setup(:Connected with IP: " << WiFi.localIP() << endl;

    if (_bWiFiConnected){
      SetupOTAWebPages();
      //SetupTermoWebPage();
      //StartWebServer(_acHostname);
      #if DO_ACCESS_POINT
        SetupAccessPt(_acAccessPointSSID, _acAccessPointPW);
      #endif  //DO_ACCESS_POINT
    } //if(_bWiFiConnected)

    //ulLastTaskMsec= millis();
  Serial << "setup(): Done" << endl;
  return;
} //setup


void loop(){
  //ulLastTaskMsec= millis();
/*
  if (_bWiFiConnected){
    HandleWebServer();
    CheckTaskTime("loop(): HandleWebServer()");
  } //if(_bWiFiConnected)
*/
#if DO_ACCESS_POINT
  if (_bWiFiConnected){
    HandleSoftAPClient();       //Listen for HTTP requests from clients
    CheckTaskTime("loop(): HandleSoftAPClient()");
  } //if(_bWiFiConnected)
#endif  //DO_ACCESS_POINT
  if (!_bOTA_Started){
    //HandleSystem();
    //CheckTaskTime("loop(): HandleSystem()");
  } //if(!_bOTA_Started)
  else{
    Serial << "loop(): Check for update timeout" << endl;
    if (millis() > _ulUpdateTimeoutMsec) {
      _bOTA_Started = false;
      Serial << "loop(): Set bUpdating to " << _bOTA_Started << endl;
    } //if(millis()>ulUpdateTimeoutMsec)
  } //if(!_bOTA_Started)else
  return;
} //loop


//Last line.
