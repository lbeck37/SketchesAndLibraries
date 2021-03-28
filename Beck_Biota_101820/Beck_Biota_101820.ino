const char szSketchName[]  = "Beck_Biota.ino";
const char szFileDate[]    = "2/4/20n";

#ifndef ESP8266
  #define ESP8266
#endif

#define DO_ALEXA              true
#define DO_OTA                true
#define DO_ACCESS_POINT       false
#define DO_WEB_SERVER         false
#define DO_NTP                false
//#define DO_ASYNC_WEB_SERVER   false

#include <BeckBiotaLib.h>
#include <BeckMiniLib.h>
#include <BeckOTALib.h>
#include <BeckSwitchLib.h>
/*
#if DO_WEB_SERVER
  #include <BeckWebPages.h>
  #include <BeckWebServer.h>
#endif
*/
#include <BeckWiFiLib.h>
//#include <FirebaseArduino.h>
#include <Streaming.h>
#include <Time.h>
#include <WiFiClient.h>

/*
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
*/

#if DO_ALEXA
  #include <BeckAlexaLib.h>
#endif

#if DO_ACCESS_POINT
  #include <BeckAccessPointLib.h>
#endif

#if DO_NTP
  #include <BeckNTPLib.h>
#endif

/*
#define FIREBASE_HOST   "//test-70884.firebaseio.com"
#define FIREBASE_HOST   "https://test-70884.firebaseio.com"

#define FIREBASE_HOST   "https://test-70884.firebaseio.com/"
#define FIREBASE_AUTH   "AIzaSyD-Nm1dYBV6ehphAOQgkM5sz4oYLKF9ahg"
*/
#define FIREBASE_HOST   "https://thermo-2b830.firebaseio.com/"
#define FIREBASE_AUTH   "AIzaSyAkFumb-wjDUQ9HQjTOoHeXqTKztFSqf6o"

//static        ProjectType      eProjectType           = ePitchMeter;
static        ProjectType      eProjectType            = eThermoDev;
//static        ProjectType      eProjectType            = eFireplace;
//static        ProjectType      eProjectType            = eHeater;
//static        ProjectType      eProjectType            = eGarage;

static const  uint32_t    ulThermHandlerPeriodMsec    = 1 * lMsecPerSec; //mSec between running system handler
static        uint32_t    ulNextThermHandlerMsec      = 0;

//static const  uint32_t    ulMPU9150HandlerPeriodMsec  = 200;
static const  uint32_t    ulMPU9150HandlerPeriodMsec  = 0;
static const  uint32_t    ulMPU9150DisplayPeriodMsec  = 1000;
static        uint32_t    ulNextMPU9150DisplayMsec    = 0;
static        bool        bMPU9150_On;

static        int              _wBadCount             = 0;
static        int              _wGoodCount            = 0;

#if !DO_OTA
  bool            _bOTA_Started         = false;
  unsigned long   _ulUpdateTimeoutMsec  = millis();
#endif

//Protos
void HandleSystem();

void setup(){
  Serial.begin(lSerialMonitorBaud);
  delay(100);
  Serial << endl << LOG0 << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;

  _bSystemOk= SetupSystem(eProjectType);  //BeckBiotaib.cpp
  if(_bSystemOk){
    SetupWiFi();
    if (_bWiFiConnected){
#if DO_OTA
      SetupOTAWebPages();
#else
      Serial << LOG0 << "setup(): OTA is not enabled" << endl;
#endif
      //SetupTermoWebPage();

#if DO_WEB_SERVER
      StartWebServer(_acHostname);
#else
      Serial << LOG0 << "setup(): Web Server is not enabled" << endl;
#endif

#if DO_ACCESS_POINT
      SetupAccessPt(_acAccessPointSSID, _acAccessPointPW);
#else
      Serial << LOG0 << "setup(): Access Point is not enabled" << endl;
#endif  //DO_ACCESS_POINT

#if DO_ALEXA
      SetupAlexa(_acAlexaName);
#else
      Serial << LOG0 << "setup(): Alexa is not enabled" << endl;
#endif
    } //if(_bWiFiConnected)

/*
    Serial << LOG0 << "setup(): SetupSystem(): Call Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH)" << endl;
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
*/

    SetupI2C();
    if(eProjectType == ePitchMeter){
      bMPU9150_On= SetupMPU9150(szSketchName, szFileDate, ulMPU9150HandlerPeriodMsec);
    } //if(eProjectType==ePitchMeter)
#if DO_NTP
		if (_bWiFiConnected){
			SetupNTP();
		} //if(_bWiFiConnected)
#else
      Serial << LOG0 << "NTP is not enabled" << endl;
#endif
    SetupDisplay(_eProjectType);
    ClearDisplay();
    SetupSwitches();
    ulLastTaskMsec= millis();
  } //if(_bSystemOk)
  else{
    while(true){
      Serial << LOG0 << "setup(): SetupSystem(): Returned false" << endl;
      delay(10000); //10 sec
     }  //while(true)
  } //if(_bSystemOk)else
  Serial << LOG0 << "setup(): Done" << endl;
  return;
} //setup


void loop(){
  ulLastTaskMsec= millis();

#if DO_WEB_SERVER
  if (_bWiFiConnected){
    HandleWebServer();
    CheckTaskTime("loop(): HandleWebServer()");
  } //if(_bWiFiConnected)
#endif

  //TestFirefox();
  //CheckTaskTime("loop(): TestFirefox()");

#if DO_NTP
  if (_bWiFiConnected){
    HandleNTPUpdate();
    CheckTaskTime("loop(): HandleNTPUpdate()");
  } //if(_bWiFiConnected)
#endif
#if DO_ACCESS_POINT
  if (_bWiFiConnected){
    HandleSoftAPClient();       //Listen for HTTP requests from clients
    CheckTaskTime("loop(): HandleSoftAPClient()");
  } //if(_bWiFiConnected)
#endif  //DO_ACCESS_POINT
  if (!_bOTA_Started){
    HandleSystem();
    CheckTaskTime("loop(): HandleSystem()");
  } //if(!_bOTA_Started)
  else{
    Serial << LOG0 << "loop(): Check for update timeout" << endl;
    if (millis() > _ulUpdateTimeoutMsec) {
      _bOTA_Started = false;
      Serial << LOG0 << "loop(): Set bUpdating to " << _bOTA_Started << endl;
    } //if(millis()>ulUpdateTimeoutMsec)
  } //if(!_bOTA_Started)else
  return;
} //loop


void HandleSystem(){
#if DO_ALEXA
  if (_bWiFiConnected){
    HandleAlexa();
    CheckTaskTime("HandleAlexa");
    if(_bAlexaChanged){
      _bAlexaChanged= false;
      UpdateDisplay();
    } //if(bAlexaChanged)
  } //if(_bWiFiConnected)
  wAlexaHandleCount= 0;
#endif
  switch (_eProjectType){
    case eFireplace:
    case eHeater:
    case eGarage:
    case eThermoDev:
      if (millis() >= ulNextThermHandlerMsec){
        ulNextThermHandlerMsec= millis() + ulThermHandlerPeriodMsec;
        _wGoodCount= 0;
        _wBadCount= 0;
#if DO_ALEXA
        if (false && (wAlexaHandleCount < 1000)){
          //HandleAlexa() gets called ~8,000 times every 10 sec, except when it's 1 or 2
          LogToSerial("HandleSystem():HandleAlexa() Times called=", wAlexaHandleCount);
        } //if (wAlexaHandleCount<1000)
#endif
        //ulNextThermHandlerMsec= millis() + ulThermHandlerPeriodMsec;
        HandleThermostat();   //BeckThermoLib.cpp
        //HandleHeatSwitch();
        UpdateDisplay();
      } //if(millis()>=ulNextThermHandlerMsec)
     break;
    case ePitchMeter:
      if (bMPU9150_On){
        HandleMPU9150();
      } //if(bMPU9150_On)
      if (millis() >= ulNextMPU9150DisplayMsec){
        ulNextMPU9150DisplayMsec= millis() + ulMPU9150DisplayPeriodMsec;
        UpdateDisplay();
      } //if(millis()>=ulNextMPU9150DisplayMsec)
      break;
    case eFrontLights:
      //HandleFrontLights();
      break;
    default:
      Serial << LOG0 << "HandleSystem(): Bad switch, _eProjectType= " << _eProjectType << endl;
      break;
  } //switch
  return;
} //HandleSystem


/*
void TestFirefox(){
  unsigned long         ulTestFirefoxPeriodMsec = 5 * 1000;
  static unsigned long  ulNextTestFirefoxMsec   = millis() + ulTestFirefoxPeriodMsec;

  if (millis() >= ulNextTestFirefoxMsec){
    ulNextTestFirefoxMsec= millis() + ulTestFirefoxPeriodMsec;

    // set value
    float fValue= 42.0;
    //Serial << "TestFirefox(): Call Firebase.setFloat(\"number\", " << fValue << ")" << endl;
    //Firebase.setFloat("number", fValue);
    Serial << endl << LOG0 << "TestFirefox(): Call Firebase.setFloat(\"Setpoint\", " << fValue << ")" << endl;
    Firebase.setFloat("Setpoint", fValue);
    // handle error
    if (Firebase.failed()) {

      Serial.print("setting /Setpoint failed:");
      Serial.println(Firebase.error());

      //Serial << LOG0 << "TestFirefox(): Firebase.setFloat() failed, error= |" << Firebase.error() << "|" << endl;
      Serial << LOG0 << "TestFirefox(): Firebase.setFloat() failed, Call Firebase.error()" << endl;
      Firebase.error();
      return;
    } //if(Firebase.failed())
    delay(1000);


    // get value
    Serial << endl << LOG0 << "TestFirefox(): Call Firebase.getFloat(\"/DevBoard/DegF\")" << endl;
    float fDegF= Firebase.getFloat("/DevBoard/DegF");
    Serial << endl << LOG0 << "TestFirefox(): /DevBoard/DegF= " << fDegF << endl;

  } //if (millis()>=ulNextTestFirefoxMsec)
  return;
} //TestFirefox
*/
//Last line.
