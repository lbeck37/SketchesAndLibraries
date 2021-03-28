const char szSketchName[]  = "Beck_Stub.ino";
const char szFileDate[]    = "1/4/20j";

#ifndef ESP8266
  #define ESP8266
#endif

#define DO_ACCESS_POINT       false

#include <ESP8266WiFi.h>
#include <Streaming.h>

#if DO_ACCESS_POINT
  #include <BeckAccessPointLib_Simple.h>
  const char*   _acAccessPointSSID    = "StubSpot";
  const char*   _acAccessPointPW      = "Qazqaz11";
#endif

const long    	lSerialMonitorBaud  = 115200;

const char* 	  WiFi_SSID 			   	= "Aspot24";
const char* 	  WiFi_Password 		  = "Qazqaz11";

bool			  _bWiFiConnected;

void setup(){
  Serial.begin(lSerialMonitorBaud);
  delay(100);
  Serial << endl  << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;

  Serial << "Setup(): Connecting to WiFi" << endl;
  WiFi.begin(WiFi_SSID, WiFi_Password);

  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }	//while
  Serial << endl;

  if (WiFi.status() == WL_CONNECTED) {
    _bWiFiConnected= true;
    Serial << "setup(): Connected with IP: " << WiFi.localIP() << endl;
  }
  else {
    _bWiFiConnected= false;
    Serial << "setup(): WiFi did not connect" << endl;
  }


#if DO_ACCESS_POINT
  SetupAccessPt(_acAccessPointSSID, _acAccessPointPW);
#endif  //DO_ACCESS_POINT

  Serial << "setup(): Done" << endl;
  return;
} //setup


void loop(){
#if DO_ACCESS_POINT
  if (_bWiFiConnected){
    HandleSoftAPClient();       //Listen for HTTP requests from clients
  } //if(_bWiFiConnected)
  delay(250);
#else
  Serial << "loop(): Do nothing" << endl;
  delay(5000);
#endif  //DO_ACCESS_POINT
  return;
} //loop
//Last line.
