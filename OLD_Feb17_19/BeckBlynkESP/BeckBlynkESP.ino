static const char szSketchName[]  = "BeckBlynkESP.ino";

//Uncomment out desired implementation.
//#define FRONT_LIGHTS
//#define FIREPLACE
//#define GARAGE
//#define GARAGE_LOCAL    //Run off local Blynk server.
//#define HEATER
//#define DEV_LOCAL
//#define DEV_REMOTE
//#define TANK_MONITOR
//#define HOT_TUB
#define HOT_TUB_V2

//#include <string>
//#include <iostream>

#include <BeckLib.h>
#include <BeckBlynk.h>
#include <BeckControlLib.h>
#include <BeckI2C.h>
#include <BeckOneWire.h>
#include <BeckAtoD.h>
#include <BeckGyro.h>
#include <BeckTanks.h>
#include <BeckDisplay.h>

#define OTA_SERVER   false     //Skip running OTA server

#define LOG0    szLogLineHeader(++lLineCount)

const UINT8 ucOneWirePin_           = 2;  //Was 12, Dallas DS18B20 Temperature Sensor
VirtualPin_t	eWaterTemp						= eVP45;
const bool 	bGyroOn                  = true;

#ifdef SKIP_BLYNK
  static const bool bSkipBlynk       = true;
#else
  static const bool bSkipBlynk       = false;
#endif

static const int    sFrontLights          = 1;
static const int    sFireplace            = 2;
static const int    sGarage               = 3;
static const int    sGarageLocal          = 4;
static const int    sHeater               = 5;
static const int    sDevLocal             = 6;
static const int    sDevRemote            = 7;
static const int    sTankMonitor          = 8;
static const int    sHotTub          			= 9;
static const int    sHotTubV2          		= 10;

/*
static const char   szRouterName[]        = "P291spot";
static const char   szRouterPW[]          = "Wsxwsx22";

static const char   szRouterName[]        = "TPspot";
static const char   szRouterName[]        = "Library";
static const char   szRouterPW[]          = "";
*/
static const char   szRouterName[]        = "Aspot24";
static const char   szRouterPW[]          = "Qazqaz11";

static const char   acHostname[]          = "esp37";

#ifdef DEBUG
  static const bool       bDebug                = true;    //Used to select places to disable bDebugLog.
#else
  static const bool       bDebug                = false;   //Used to select places to disable bDebugLog.
#endif

//To get Blynk Auth Token from the Blynk App, go to the Project Settings (nut icon).
#ifdef FRONT_LIGHTS
  char acBlynkAuthToken[] = "37a58cc7a39045a59bca1fb1281880a2";     //Light Timer Blynk token
  static const char szProjectType[]    = "FRONT_LIGHTS";
  static int sProjectType= sFrontLights;
#endif
#ifdef FIREPLACE
  char acBlynkAuthToken[] = "35131c5204f34f8e93b574436df46397";
  static const char szProjectType[]    = "FIREPLACE";
  static int sProjectType= sFireplace;
#endif
#ifdef GARAGE
  char acBlynkAuthToken[] = "5e9c5f0ae3f8467597983a6fa9d11101";
  static const char szProjectType[]    = "GARAGE";
  static int sProjectType= sGarage;
#endif
#ifdef GARAGE_LOCAL
  char acBlynkAuthToken[] = "7917cbe7f4614ba19b366a172e629683";
  static const char szProjectType[]    = "GARAGE_LOCAL";
  static int sProjectType= sGarageLocal;
#endif
#ifdef HEATER
  char acBlynkAuthToken[] = "8fe963d2af4e48b5bfb358d91aad583e";
  static const char szProjectType[]    = "HEATER";
  static int sProjectType= sHeater;
#endif
#ifdef DEV_LOCAL
  //static const char acBlynkAuthToken[]  = "55bce1afbf894b3bb67b7ea34f29d45a";
  static const char acBlynkAuthToken[]    = "9fc34bc2cbb34ddf8d392f7c562fb52e";   //Local server
  static const char szProjectType[]     = "DEV_LOCAL";
  static const int  sProjectType        = sDevLocal;
#endif
#ifdef DEV_REMOTE
  static const char acBlynkAuthToken[]  = "55bce1afbf894b3bb67b7ea34f29d45a";
  static const char szProjectType[]     = "DEV_REMOTE";
  static const int  sProjectType        = sDevRemote;
#endif
#ifdef TANK_MONITOR
  static const char acBlynkAuthToken[]  = "a74c478f829945c2bb5eb9bb88c9f406";
  static const char szProjectType[]     = "TANK_MONITOR";
  static const int  sProjectType        = sTankMonitor;
#endif
#ifdef HOT_TUB
  static const char acBlynkAuthToken[]  = "a1ac9db4af734950bafa6d072b0e7ca8";
  static const char szProjectType[]     = "HOT_TUB";
  static const int  sProjectType        = sHotTub;
#endif
#ifdef HOT_TUB_V2
  static const char acBlynkAuthToken[]  = "aeeef3b69d9d477ba6edf91c8e42fe72";
  static const char szProjectType[]     = "HOT_TUB_V2";
  static const int  sProjectType        = sHotTubV2;
#endif

static unsigned long	ulNextHandlerMsec     = 0;
static unsigned long  ulUpdateTimeoutMsec   = 0;
static long           lSystemHandlerSpacing = 10 * lMsecPerSec;  //mSec between running system handlers

static bool           bUpdating             = false;    //Turns off Blynk.

BeckDisplay*    pBeckDisplay;
BeckI2C*        pBeckI2C;
BeckAtoD*       pBeckAtoD;
BeckGyro*       pBeckGyro;
BeckBlynk*      pBeckBlynk;
BeckTanks*      pBeckTanks;
BeckOneWire*		pBeckOneWire;

//Functions
void setup()
{
  sSetupTime();
  Serial.begin(lSerialMonitorBaud);
  Serial << endl << LOG0 << "setup(): Initialized serial to " << lSerialMonitorBaud << " baud" << endl;
  Serial << LOG0 << " setup(): Sketch: " << szSketchName << "/" << szProjectType << ", " << szVersionDate << endl;

  SetupWiFi(szRouterName, szRouterPW);
  SetupDevices();
  SetupSwitches();
  SetupSystem();
  return;
} //setup


void loop() {
	//First check for overheat.
  switch (sProjectType){
    case sHotTub:
    case sHotTubV2:
    	if(!bOverheatFailure_){
        bCheckOverheat(false);
    	}	//if(!bOverheatFailure_)
      break;
    default:
      break;
  } //switch
#if OTA_SERVER
  HandleHttpServer();
#endif
  if (!bSkipBlynk) {
    if (!bUpdating) {
      pBeckBlynk->Run();
      HandleSystem();
    } //if(!bUpdating)
    else {
      Serial << LOG0 << " loop(): Check for update timeout, bSkipBlynk= " << bSkipBlynk << endl;
      if (millis() > ulUpdateTimeoutMsec) {
        bUpdating = false;
        Serial << LOG0 << " loop(): Set bUpdating to " << bUpdating
            << endl;
      } //if(millis()>ulUpdateTimeoutMsec)
    } //if(!bUpdating)else
  } //if(!bSkipBlynk)
} //loop


void SetupDevices() {
  String szLogString = "BeckBlynkESP.ino::SetupDevices(): Begin";
  LogToSerial(szLogString);

  pBeckI2C    = new BeckI2C(0);
  //SetupDisplay();

  pBeckDisplay    = new BeckDisplay();
  pBeckAtoD       = new BeckAtoD		(pBeckI2C, eADS1115);
  pBeckBlynk      = new BeckBlynk		(acBlynkAuthToken, pBeckAtoD);
  pBeckTanks      = new BeckTanks		(pBeckAtoD, pBeckDisplay);
  pBeckOneWire		= new BeckOneWire	(ucOneWirePin_);

  if (false && pBeckI2C->bDevicePresent(eGyro)) {
    pBeckGyro= new BeckGyro(pBeckI2C);
  }//if(pBeckI2C->bDevicePresent(eGyro))

  pBeckDisplay->Setup();

  switch (sProjectType){
    case sHotTub:
    case sHotTubV2:
    	sProjectType_= 10;
    	sSetpointF_= 75;
    	break;
    case sGarage:
    case sGarageLocal:
    	sProjectType_= 3;
    	sSetpointF_= 37;
    	break;
    default:
    	sProjectType_= 0;
      break;
  } //switch

 return;
} //SetupDevices


/*
void SetupDisplay(){
  String szLogString = "BeckBlynkESP.ino::SetupDisplay(): Begin";
  LogToSerial(szLogString);

  oDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // Clear the buffer.
  oDisplay.clearDisplay();
  return;
} //SetupDisplay
*/


int sSetupTime(){
  //setTime(0,0,0, 0,0,0);  //hr, min, sec, day, month, year
  return 1;
} //sSetupTime


void SetupSystem(){
  String szLogString = "SetupSystem()";
  LogToBoth(szLogString);
  //sSystemHandlerSpacing = 10 * lMsecPerSec;
  switch (sProjectType){
  case sHotTub:
  case sHotTubV2:
		SetupHotTub();
    break;
	default:
		break;
  } //switch
  //HandleBlynkLEDs();
  return;
} //SetupSystem


void SetupSwitches(){
  String szLogString = "SetupSwitches(): Begin";
  LogToBoth(szLogString);
  //pBeckI2C->TestI2C();

  for (int sSwitch= 1; sSwitch <= sNumSwitches_; sSwitch++){
		String szLogString = "  pinMode(,OUTPUT) for pin= ";
		LogToBoth(szLogString, asSwitchPin[sSwitch]);
    pinMode(asSwitchPin[sSwitch], OUTPUT);
    SetSwitch(sSwitch, sSwitchOpen);
  } //for

  pinMode(sFlowSensorPin_, INPUT);

  //pBeckI2C->TestI2C();
  //szLogString = "SetupSwitches(): End";
  //LogToBoth(szLogString);
  return;
} //SetupSwitches


void HandleSystem(){
  if (millis() >= ulNextHandlerMsec){
    ulNextHandlerMsec= millis() + lSystemHandlerSpacing;
    switch (sProjectType){
      case sFrontLights:
        HandleFrontLights();
        break;
      case sFireplace:
        HandleFireplace();
        break;
      case sHotTub:
      case sHotTubV2:
      	HandleHotTub();
        //HandleBlynkLEDs();
        //HandleHeatSwitch();			//See BeckControlLib.cpp
        break;
      case sGarage:
      case sGarageLocal:
        HandleThermostat();
        //HandleBlynkLEDs();
        //HandleHeatSwitch();			//See BeckControlLib.cpp
        break;
      case sHeater:
        HandleHeater();
        break;
      case sDevLocal:
      case sDevRemote:
        HandleTankMonitor();
        break;
      case sTankMonitor:
        HandleTankMonitor();
        break;
      default:
        String szLogString= "HandleSystem:Bad switch";
        LogToBoth(szLogString, sProjectType);
        break;
    } //switch
    HandleBlynkLEDs();
  } //if(millis()>=ulNextHandlerMsec)
  return;
} //HandleSystem


void HandleHotTub(){
  String szLogString = "HandleHotTub()";
  LogToBoth(szLogString);
  //CheckFlowSensor();
  HandleThermostat();
  HandleHeatSwitch();			//See BeckControlLib.cpp
  return;
} //HandleHotTub


void HandleTankMonitor(){
  String szLogString = "HandleTankMonitor()";
  LogToBoth(szLogString);
  pBeckTanks->Read();
  pBeckTanks->UpdateDisplay();
  return;
} //HandleTankMonitor


void HandleDevelopment(){
  String szLogString = "HandleDevelopment()";
  LogToBoth(szLogString);
  if(bGyroOn) {
    pBeckGyro->Read();
  }
  return;
} //HandleDevelopment


void HandleHeater(){
  String szLogString = "HandleHeater()";
  LogToBoth(szLogString);
  return;
} //HandleHeater


void HandleFrontLights(){
  String szLogString = "HandleFrontLights()";
  LogToBoth(szLogString);
  return;
} //HandleFrontLights


void HandleFireplace(){
  String szLogString = "HandleFireplace()";
  LogToBoth(szLogString);
  return;
} //HandleFireplace


void HandleThermostat(){
  String szLogString = "HandleThermostat()";
  LogToBoth(szLogString);
  //Only do anything if the thermostat is turned on.
  if (bThermoOn_){
    float fDegF= pBeckOneWire->fGetDegF(eWaterTemp);
    float fRoundDegF= fRound(fDegF);
    fThermoOffDegF_= sSetpointF_ + fMaxHeatRangeF;
    //DebugHandleThermostat(fDegF, fRoundDegF);
    if (bHeatOn_){
    	bCheckFlowSensor(true);
      if (fRoundDegF >= fThermoOffDegF_){
				szLogString= "HandleThermostat(): fRoundDegF >= fThermoOffDegF";
				LogToBoth(szLogString);
		    szLogString= "HandleThermostat(): *** sThermoTimesCount_=";
		    LogToBoth(szLogString, sThermoTimesCount_);
        if (++sThermoTimesCount_ >= sThermoTimesInRow){
          TurnHeatOn(false);
        } //if(sThermoTimesCount>=sThermoTimesInRow)
      } //if(fRoundDegF>=fThermoOffDegF)
      else{
        sThermoTimesCount_= 0;
      } //if(fRoundDegF>=fThermoOffDegF)else
    } //if(bHeatOn)
    else{
      if (fRoundDegF <= sSetpointF_){
				szLogString= "HandleThermostat(): *** sThermoTimesCount_=";
				LogToBoth(szLogString, sThermoTimesCount_);
        if (++sThermoTimesCount_ >= sThermoTimesInRow){
          TurnHeatOn(true);
        } //if(sThermoTimesCount>=sThermoTimesInRow)
      } //if(fRoundDegF<sSetpointF)
      else{
        sThermoTimesCount_= 0;
      } //if(fRoundDegF<sSetpointF)else
    } //if(bHeatOn)else
  } //if(bThermoOn)
  else{
    //LogToBoth(szLogString);
    szLogString= " bThermoOn is false";
    LogToBoth(szLogString);
  }	//if(bThermoOn)else
  return;
} //HandleThermostat


void DebugHandleThermostat(float fDegF, float fRoundDegF){
  //String szLogString2= " ";
  String szLogString = "DebugHandleThermostat()";
  LogToBoth(szLogString);
  szLogString= "  DegF=";
  LogToBoth(szLogString, fDegF);
  szLogString= "  fRoundDegF=";
  LogToBoth(szLogString, fRoundDegF);
  szLogString= "  sSetpointF_=";
  LogToBoth(szLogString, sSetpointF_);
  szLogString= "  fThermoOffDegF=";
  LogToBoth(szLogString, fThermoOffDegF_);
  szLogString= "  bHeatOn_=";
  LogToBoth(szLogString, bHeatOn_);
  szLogString= "  sThermoTimesCount_=";
  LogToBoth(szLogString, sThermoTimesCount_);
  return;
} //DebugHandleThermostat


float fRound(float fNum){
  oSensors.requestTemperatures(); // Send the command to get temperatures
  float fRounded= floor(fNum + 0.5);
  return fRounded;
}  //fRound
//Last line.
