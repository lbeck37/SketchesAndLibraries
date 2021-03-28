//BeckWiFiLib.cpp, 10/18/20a, was 2/4/20b
#include <BeckWiFiLib.h>
#include <BeckLogLib.h>
#include <BeckMiniLib.h>
#include <Streaming.h>
#include <ESP8266WiFiMulti.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
//#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager

bool          _bWiFiConnected;

#define USE_WIFI_MANAGER		false

//Protos
#if USE_WIFI_MANAGER
	#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager
	bool			_bResetSettings		= false;
	bool 			bRunWiFiManager();
	//void 			configModeCallback 		(WiFiManager *myWiFiManager);
#else
	bool 			bSetupMulti();
#endif


void SetupWiFi(){
#if USE_WIFI_MANAGER

  Serial << LOG0 << "SetupWiFi(): Call bRunWiFiManager()" << endl;
  _bWiFiConnected= bRunWiFiManager();

  if (!_bWiFiConnected){
    Serial << LOG0 << "SetupWiFi(): bRunWiFiManager() returned false, WiFi failed to connect." << endl;
  } //if(!_bWiFiConnected)
#else
  Serial << LOG0 << "SetupWiFi(): Not using WiFi Manager" << endl;
  Serial << LOG0 << "SetupWiFi(): Call bSetupMulti()" << endl;
  _bWiFiConnected= bSetupMulti();

  if (!_bWiFiConnected){
    Serial << LOG0 << "SetupWiFi(): bSetupMulti() returned false, WiFi failed to connect." << endl;
  } //if(!_bWiFiConnected)
#endif

  if (_bWiFiConnected){
    Serial << LOG0 << "SetupWiFi():  SSID= " << WiFi.SSID() << ", IP address: " << WiFi.localIP() << endl;
  } //if(_bWiFiConnected)

  return;
} //SetupWiFi


#if USE_WIFI_MANAGER
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial << "configModeCallback(): WiFi.softAPIP()= " << WiFi.softAPIP() << endl;

  //if you used auto generated SSID, print it
  Serial << "configModeCallback(): myWiFiManager->getConfigPortalSSID()= " <<
  		myWiFiManager->getConfigPortalSSID() << endl;

  return;
}	//configModeCallback


bool bRunWiFiManager(){
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
	bool					bOk					= true;
  WiFiManager 	wifiManager;
  if (_bResetSettings){
    //reset settings - for testing
  	Serial << LOG0 << "bRunWiFiManager(): Call resetSettings(), slowly brings up web page" << endl;
  	wifiManager.resetSettings();
  }	//if(_bResetSettings)
  else {
  	Serial << LOG0 << "bRunWiFiManager(): Pull SSID & PW from EEPROM, resetSettings() not called." << endl;
  }	//if(_bResetSettings)else

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  Serial << LOG0 << "bRunWiFiManager(): Call setAPCallback()" << endl;
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  Serial << LOG0 << "bRunWiFiManager(): Call wifiManager.autoConnect()" << endl;
  if(!wifiManager.autoConnect()) {
    //Serial.println("failed to connect and hit timeout");
    Serial << LOG0 << "bRunWiFiManager(): Failed to connect and hit timeout" << endl;
    Serial << LOG0 << "bRunWiFiManager(): Call ESP.reset()" << endl;
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    Serial << LOG0 << "bRunWiFiManager(): Back from ESP.reset(), set bOk to false" << endl;
    bOk= false;
    delay(1000);
  }
  Serial << LOG0 << "bRunWiFiManager(): returning " << bOk << endl;
	return true;
}	//bRunWiFiManager

#else

bool bSetupMulti(){
  //#include <ESP8266WiFiMulti.h>
  #ifdef ESP8266
    #include <ESP8266WiFiMulti.h>
    ESP8266WiFiMulti    oWiFiMulti;
  #else
    #include <WiFiMulti.h>
    WiFiMulti           oWiFiMulti;
  #endif    //ESP8266

  //These names shouldn't have leading "_" since they are not global anymore.
  const int     _wSSIDNumChar   = 32;
  const int     _wPWNumChar     = 65;
  const int     _wNumRouters    =  3;
  char          _acRouterNames     [_wNumRouters][_wSSIDNumChar] = {"Aspot24" , "Lspot"   , "Cspot"};
  char          _acRouterPWs       [_wNumRouters][_wPWNumChar]   = {"Qazqaz11", "Qazqaz11", "Qazqaz11"};

  bool					bOk							= true;
  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);

  //Create router list for oWiFiMulti
  for (int wRouterNum= 0; !_bWiFiConnected && (wRouterNum < _wNumRouters); wRouterNum++){
    oWiFiMulti.addAP(_acRouterNames[wRouterNum], _acRouterPWs[wRouterNum]);   //Add Wi-Fi networks to connect to
  } //for(int wRouterNum=0;...

  //_bWiFiConnected= false;
  int wNotConnectCount    = 0;
  int wMaxNotConnectCount   = 100;  //10 sec at 250 msec delay
  bool  bWiFiMultiConnected	= true;

  Serial << LOG0 << "SetupMulti(): Call oWiFiMulti.run() ";
  while (oWiFiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    //Serial << ".";
    Serial << (wNotConnectCount % 10);
    if (++wNotConnectCount > wMaxNotConnectCount){
    	bWiFiMultiConnected= false;
      Serial << endl << LOG0 << "SetupWifi(): oWiFiMulti.run() didn't find router" << endl;
      break;
    } //if(++wNotConnectCount>wMaxNotConnectCount)
  } //while
  Serial << endl;

  if (bWiFiMultiConnected) {
    Serial << LOG0 << "bSetupMulti(): Success:  SSID= " << WiFi.SSID() << ", IP address: " << WiFi.localIP() << endl;
  	bOk= true;
  } //if(!bWiFiMultiNoConnect)
  else{
  	Serial << LOG0 << "bSetupMulti(): Failed to find an access point from its list." << endl;
  	bOk= false;
  //_bWiFiConnected= false;
  } //if(!bWiFiMultiNoConnect)else

  return bOk;
} //bSetupMulti
#endif


String szWiFiStatus(wl_status_t eWiFiStatus){
  String szStatus;
  switch (eWiFiStatus){
  case WL_IDLE_STATUS:
    szStatus= "WL_IDLE_STATUS";
    break;
  case WL_NO_SSID_AVAIL:
    szStatus= "WL_NO_SSID_AVAIL";
    break;
  case WL_SCAN_COMPLETED:
    szStatus= "WL_SCAN_COMPLETED";
    break;
  case WL_CONNECTED:
    szStatus= "WL_CONNECTED";
    break;
  case WL_CONNECT_FAILED:
    szStatus= "WL_CONNECT_FAILED";
    break;
  case WL_CONNECTION_LOST:
    szStatus= "WL_CONNECTION_LOST";
    break;
  case WL_DISCONNECTED:
    szStatus= "WL_DISCONNECTED";
    break;
  case WL_NO_SHIELD:
    szStatus= "WL_NO_SHIELD";
    break;
  default:
    szStatus= "default";
    break;
  } //switch
  return szStatus;
} //szWiFiStatus
//Last line.
