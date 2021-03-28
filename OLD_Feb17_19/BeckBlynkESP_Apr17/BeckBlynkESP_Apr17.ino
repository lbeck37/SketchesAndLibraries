static const char szSketchName[]  = "BeckBlynkESP_Apr17.ino";
//static const char szFileDate[]    = "Feb 26, 2017 -G- Lenny";
static const char szFileDate[]    = "Apr 1, 2017 -A- Lenny";
//Uncomment out desired implementation.
//#define FRONT_LIGHTS
//#define FIREPLACE
#define GARAGE
//#define GARAGE_LOCAL    //Run off local Blynk server.
//#define HEATER
//#define DEV_LOCAL
//#define DEV_REMOTE
//#define TANK_MONITOR

#include <BeckLib.h>
#include <BeckBlynk.h>
#include <BeckControlLib.h>
#include <BeckI2C.h>
#include <BeckAtoD.h>
#include <BeckGyro.h>
#include <BeckTanks.h>
#include <BeckDisplay.h>

#define OTA_SERVER   false     //Skip running OTA server

#define LOG0    szLogLineHeader(++lLineCount)

const bool bGyroOn                  = true;

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

static const char   szRouterName[]        = "P291spot";
static const char   szRouterPW[]          = "Wsxwsx22";

//static const char   szRouterName[]        = "Aspot24";
//static const char   szRouterName[]        = "TPspot";
//static const char   szRouterPW[]          = "Qazqaz11";

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

static unsigned long  ulNextHandlerMsec     = 0;
static unsigned long  ulUpdateTimeoutMsec   = 0;
static long           sSystemHandlerSpacing;    //Number of mSec between running system handlers
static bool           bUpdating             = false;    //Turns off Blynk.

//BeckI2C     oBeckI2C    (0);
//BeckAtoD    oBeckAtoD   (&oBeckI2C, eADS1115);
//BeckBlynk   oBeckBlynk  (acBlynkAuthToken, &oBeckAtoD);
//BeckGyro    oBeckGyro   (&oBeckI2C, bGyroOn);

BeckDisplay*    pBeckDisplay;
BeckI2C*        pBeckI2C;
BeckAtoD*       pBeckAtoD;
BeckGyro*       pBeckGyro;
BeckBlynk*      pBeckBlynk;
BeckTanks*      pBeckTanks;

//Functions
void setup()
{
  sSetupTime();
  Serial.begin(lSerialMonitorBaud);
  Serial << endl << LOG0 << "setup(): Initialized serial to " << lSerialMonitorBaud << " baud" << endl;
  Serial << LOG0 << " setup(): Sketch: " << szSketchName << "/" << szProjectType << ", " << szFileDate << endl;

  SetupWiFi(szRouterName, szRouterPW);
  SetupDevices();
  SetupSwitches();
  SetupSystem();
  return;
} //setup


void loop() {
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
  //szLogString = "BeckBlynkESP.ino::SetupDevices(): pBeckI2C=";
  //LogToSerial(szLogString, (UINT32)pBeckI2C);

  //SetupDisplay();

  pBeckDisplay    = new BeckDisplay();
  pBeckAtoD       = new BeckAtoD(pBeckI2C, eADS1115);
  pBeckBlynk      = new BeckBlynk(acBlynkAuthToken, pBeckAtoD);
  pBeckTanks      = new BeckTanks(pBeckAtoD, pBeckDisplay);

  if (false && pBeckI2C->bDevicePresent(eGyro)) {
    pBeckGyro= new BeckGyro(pBeckI2C);
  }//if(pBeckI2C->bDevicePresent(eGyro))

  pBeckDisplay->Setup();

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
  switch (sProjectType){
  case sDevRemote:
  case sDevLocal:
      sSystemHandlerSpacing = 10 * lMsecPerSec;
      break;
    default:
      sSystemHandlerSpacing = 10 * lMsecPerSec;
      break;
  } //switch
  //HandleBlynkLEDs();
  return;
} //SetupSystem


void SetupSwitches(){
  String szLogString = "SetupSwitches(): Begin";
  LogToBoth(szLogString);
  pBeckI2C->TestI2C();

  for (int sSwitch= 1; sSwitch <= sNumSwitches; sSwitch++){
    pinMode(asSwitchPin[sSwitch], OUTPUT);
    SetSwitch(sSwitch, sSwitchOpen);
  } //for

  pBeckI2C->TestI2C();
  //szLogString = "SetupSwitches(): End";
  //LogToBoth(szLogString);
  return;
} //SetupSwitches


void HandleSystem(){
  if (millis() >= ulNextHandlerMsec){
    String szLogString = "HandleSystem()";
    LogToBoth(szLogString);
    ulNextHandlerMsec= millis() + sSystemHandlerSpacing;
    switch (sProjectType){
      case sFrontLights:
        HandleFrontLights();
        break;
      case sFireplace:
        HandleFireplace();
        break;
      case sGarage:
      case sGarageLocal:
        HandleThermostat();
        //HandleBlynkLEDs();
        HandleFurnaceSwitch();
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
  if (bThermoOn){
    float fDegF= fGetDegF(true);
    float fRoundDegF= fRound(fDegF);
    DebugHandleThermostat(fDegF);
    if (bFurnaceOn){
      if (fRoundDegF >= fThermoOffDegF){
        if (++sThermoTimesCount >= sThermoTimesInRow){
          TurnFurnaceOn(false);
        } //if(sThermoTimesCount>=sThermoTimesInRow)
      } //if(fRoundDegF>=fThermoOffDegF)
      else{
        sThermoTimesCount= 0;
      } //if(fRoundDegF>=fThermoOffDegF)else
    } //if(bFurnaceOn)
    else{
      if (fRoundDegF <= sSetpointF){
        if (++sThermoTimesCount >= sThermoTimesInRow){
          TurnFurnaceOn(true);
        } //if(sThermoTimesCount>=sThermoTimesInRow)
      } //if(fRoundDegF<sSetpointF)
      else{
        sThermoTimesCount= 0;
      } //if(fRoundDegF<sSetpointF)else
    } //if(bFurnaceOn)else
  } //if(bThermoOn)
  else{
    LogToBoth(szLogString);
    szLogString= " bThermoOn is false";
    LogToBoth(szLogString);
  }
  //} //if(millis()>=ulNextHandlerMsec)
  return;
} //HandleThermostat


void DebugHandleThermostat(float fDegF){
  //String szLogString2= " ";
  String szLogString = "HandleThermostat";
  LogToBoth(szLogString);
  szLogString= " DegF=";
  LogToBoth(szLogString, fDegF);
  szLogString= " SetpointF=";
  LogToBoth(szLogString, sSetpointF);
  szLogString= " OffDegF=";
  LogToBoth(szLogString, fThermoOffDegF);
  szLogString= " bFurnaceOn=";
  LogToBoth(szLogString, bFurnaceOn);
  szLogString= " OnCount=";
  LogToBoth(szLogString, sThermoTimesCount);
  return;
} //DebugHandleThermostat


float fRound(float fNum){
  oSensors.requestTemperatures(); // Send the command to get temperatures
  float fRounded= floor(fNum + 0.5);
  return fRounded;
}  //fRound
//Last line.
