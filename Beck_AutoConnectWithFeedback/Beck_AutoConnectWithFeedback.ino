const char szSketchName[]  = "Beck_AutoConnectWithFeedback.ino";
const char szFileDate[]    = "1/13/20a";

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager
#include <Streaming.h>

/*
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}
*/
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial << "configModeCallback(): WiFi.softAPIP()= " << WiFi.softAPIP() << endl;
  //if you used auto generated SSID, print it
  Serial << "configModeCallback(): myWiFiManager->getConfigPortalSSID()= " <<
  		myWiFiManager->getConfigPortalSSID() << endl;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  Serial << endl << endl  << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager 	wifiManager;
  //reset settings - for testing
  if (true){
  	Serial << "setup(): Call resetSettings()" << endl;
  	wifiManager.resetSettings();
  }	//

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  Serial << "setup(): Call setAPCallback()" << endl;
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  Serial << "seup(): Call wifiManager.autoConnect() repeatably" << endl;
  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  Serial << "SetupWiFi():  SSID= " << WiFi.SSID() << ", IP address: " << WiFi.localIP() << endl;
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  return;
}	//setup

void loop() {
  // put your main code here, to run repeatedly:

}
