static const char szSketchName[]  = "BlynkBeck.ino";
static const char szFileDate[]    = "November 28, 2016B Lenny";
// 9/16/16 Work on getting Garage to build and run.
// 1/06/16 Building from eclipseArduino
// 12/28/15 Change name from Blynk_Beck.ino, pin numbers for Blynk switches 3 and 4 and baud to 15200.
// 12/27/15 Add DEV_REMOTE.
// 12/26/15 Switch to C1200spot from dlinky.
// 12/24/15 Switch Garage to be local server, switch IPof local server.
// 12/21/15 Added Auth Token for HEATER project for testing.
// 12/18/15 Added Auth Token for DEV_LOCAL project for testing.
// 12/17/15 Add HandleSystem() to take care of relays also in loop().
// 12/16/15 Implement thermostat for GARAGE version.
// 12/15/15 Remove unused state virtual pins, implement thermostat for GARAGE version.
// 12/14/15 Rearrange virtual pins, build GARAGE version.
// 12/13/15 Merge in support for Fireplace.
// 12/12/15 Created from Blynk_LightTimer.ino
//Open issues:
//  - Switches 3 and 4 LEDs are not working.
//  - Add virtual pin and LED for turning on DEBUG.
//  - Get LEDs off at beginning.

//Uncomment out desired implementation.
//#define FRONT_LIGHTS
//#define FIREPLACE
//#define GARAGE
//#define GARAGE_LOCAL    //Run off local Blynk server.
//#define HEATER
//#define DEV_LOCAL
#define DEV_REMOTE

#define OTA_SERVER   false     //Skip running OTA server
#if 0
  #define SKIP_BLYNK      true
  #define DEBUG         true
  #define DEBUG_OTA   //Used to skip Blynk code while debugging OTA
#endif

#include <Streaming.h>
#include <Time.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#if OTA_SERVER
  #include <ESP8266WebServer.h>
  #include <ESP8266mDNS.h>
#endif
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONEWIRE_PIN       12

//Define Virtual Pin names
#define ReadF_V0          V0
#define ReadF_V1          V1
#define SetSetpointF_V2   V2
#define GetSetpointF_V3   V3
#define ThermoSwitch_V4   V4
#define ThermoLED_V5      V5

#define AtoD_1V6      V6

#define Terminal_V7       V7
#define LCD_Line0_V8      V8
#define LCD_Line1_V9      V9

//Relay #1
#define Switch_1V10       V10
#define TimerA_1V11       V11
#define TimerB_1V12       V12
#define LED_1V13          V13

#define AtoD_2V14     V14

//Relay #2
#define Switch_2V15       V15
#define TimerA_2V16       V16
#define TimerB_2V17       V17
#define LED_2V18          V18

#define AtoD_3V19     V19

//Relay #3
#define Switch_3V20       V20
#define TimerA_3V21       V21
#define TimerB_3V22       V22
#define LED_3V23          V23

#define AtoD_4V24     V24

//Relay #4
#define Switch_4V25       V25
#define TimerA_4V26       V26
#define TimerB_4V27       V27
#define LED_4V28          V28

#define Unassigned_V29    V29
#define Unassigned_V30    V30
#define Unassigned_V31    V31

#define LOG0    szLogLineHeader(++lLineCount)

#ifdef SKIP_BLYNK
  static const bool bSkipBlynk          = true;
#else
  static const bool bSkipBlynk          = false;
#endif
static const int    sSwitchOpen           = 0;
static const int    sSwitchClosed         = 1;
static const int    sOff                  = 0;
static const int    sOn                   = 1;
static const int    sNotInit              = -3737;
static const int    sNumSwitches          = 4;
//static const int    sMaxNumSwitches       = 4;
static const int    sThermoDummySwitch    = 0;  //Thermostat Blynk LED lives at unused switch #0.
static const int    asSwitchPin[]         = {-1, 4, 5, 15, 16};    //0 is not a switch, switches are at 1,2,3,4
static const bool   abSwitchInverted[]    = {0, true, true, true, true};  //Opto-isolated relays close when pulled low.
//(3) types of sketches are supported: front lights, fireplace and garage
static const int    sFrontLights          = 1;
static const int    sFireplace            = 2;
static const int    sGarage               = 3;
static const int    sGarageLocal          = 4;
static const int    sHeater               = 5;
static const int    sDevLocal             = 6;
static const int    sDevRemote            = 7;
static const int    sOneWirePin           = ONEWIRE_PIN;  //Dallas DS18B20 Temperature Sensor
//static const int    sMaxFDelta            = 2;  //Amount room temp can rise above setpoint.

static const long   lSerialMonitorBaud    = 115200;
static const long   lMsecPerDay           = 86400000;
static const long   lMsecPerHour          =  3600000;
static const long   lMsecPerMin           =    60000;
static const long   lMsecPerSec           =     1000;

static const int    sFurnaceSwitchNum     = 2;      //Was 1, switch number that turns furnace on and off.
static const long   sThermoTimesInRow     = 3;      //Max times temp is outside range before switch
static const float  fMaxHeatRangeF        = 2.00;   //Temp above setpoint before heat is turned off

//static const char   szRouterName[]        = "Aspot24";
//static const char   szRouterName[]        = "HP7spot";
static const char   szRouterName[]        = "LenSpot";
//static const char   szRouterName[]        = "P291spot";
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

WidgetTerminal      oTerminal(Terminal_V7);
WidgetLCD           LCDWidget(1);

//LED for thermostat state has no actual switch but it will live as unused switch #0.
WidgetLED           oLED0(ThermoLED_V5);
WidgetLED           oLED1(LED_1V13);
WidgetLED           oLED2(LED_2V18);
WidgetLED           oLED3(LED_3V23);
WidgetLED           oLED4(LED_4V28);

//Maxim/Dallas OneWire sensors
/* Set up a oneWire instance to communicate with any OneWire device*/
OneWire         oOneWire(sOneWirePin);

/* Tell Dallas Temperature Library to use oneWire Library */
DallasTemperature oSensors(&oOneWire);

#if OTA_SERVER
  ESP8266WebServer    oESP8266WebServer(80);
#endif

//UpdaterClass    Update; //Declaration at the end of cores\esp8266\Updater.h from BSP

const char*     acServerIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

static int          asSwitchState[]       = {0, 0, 0, 0, 0};
static int          asSwitchLastState[]   = {sNotInit, sNotInit, sNotInit, sNotInit, sNotInit};
static long         lLineCount            = 0;      //Serial Monitor uses for clarity.
//static long         lLineCount2           = 0;      //For Blynk terminal window.
//static long         lNumLoops             = 1;
static float          fLastDegF             = 37.37;  //Last temperature reading.
static int            sSetpointF            = 37;
static int            sThermoTimesCount     = 0;      //Number of times temperature out of range
static unsigned long  ulNextHandlerMsec     = 0;
static unsigned long  ulUpdateTimeoutMsec   = 0;
static bool           bThermoOn             = true;   //Whether thermostat is running.
static bool           bFurnaceOn            = false;  //If switch is on to turn on furnace.
static float        fThermoOffDegF        = sSetpointF + fMaxHeatRangeF;
static long         sSystemHandlerSpacing; //Number of mSec between running system handlers
static bool         bDebugLog             = true;   //Used to limit number of printouts.
static bool         bUpdating             = false;   //Turns off Blynk.

void setup()
{
  sSetupTime();
  Serial.begin(lSerialMonitorBaud);
  Serial << endl << LOG0 << " setup(): Initialized serial to " << lSerialMonitorBaud << " baud" << endl;
  Serial << LOG0 << " setup(): Sketch: " << szSketchName << "/" << szProjectType << ", " << szFileDate << endl;

  //Wire.begin();
  SetupWiFi();
  SetupSwitches();
  SetupSystem();
  return;
} //setup


void loop() {
//#ifndef SKIP_SERVER
#if OTA_SERVER
  HandleHttpServer();
#endif
  if (!bSkipBlynk) {
    if (!bUpdating) {
      Blynk.run();
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


void SetupWiFi(){
  WiFi.mode(WIFI_AP_STA);
  Serial << LOG0 << " SetupServer(): Call WiFi.begin("<< szRouterName << ", " << szRouterPW << ")" << endl;
  WiFi.begin(szRouterName, szRouterPW);
  //wl_status_t eWiFiStatus= WiFi.waitForConnectResult();
  wl_status_t eWiFiStatus= (wl_status_t)WiFi.waitForConnectResult();
  if(eWiFiStatus == WL_CONNECTED) {
    Serial << LOG0 << " SetupServer(): WiFi.waitForConnectResult() returned " << szWiFiStatus(eWiFiStatus) << endl;
    #if OTA_SERVER
      SetupServer();
    #endif
  } //if(eWiFiStatus==WL_CONNECTED)
  else {
    //Serial << LOG0 << " SetupServer(): ERROR: WiFi.waitForConnectResult() returned " << ucWiFiStatus << endl;
    Serial << LOG0 << " SetupServer(): ERROR: WiFi.waitForConnectResult() returned " << szWiFiStatus(eWiFiStatus) << endl;
  } //if(eWiFiStatus==WL_CONNECTED)else

  switch (sProjectType){
    case sGarageLocal:
    case sDevLocal:
      Serial << LOG0 << " setup(): Call Blynk.config(" << acBlynkAuthToken << ", IPAddress(192,168,15,191))" << endl;
      Blynk.config(acBlynkAuthToken, IPAddress(192,168,15,191));
      break;
    default:
      Serial << LOG0 << " SetupWiFi(): Call Blynk.config(" << acBlynkAuthToken << ")" << endl;
      Blynk.config(acBlynkAuthToken);
      break;
  } //switch
  Serial << LOG0 << " SetupWiFi(): Blynk.config() returned" << endl;
  return;
} //SetupWiFi


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


#if OTA_SERVER
void SetupServer(void) {
    MDNS.begin(acHostname);
    oESP8266WebServer.on("/", HTTP_GET, [](){
      oESP8266WebServer.sendHeader("Connection", "close");
      oESP8266WebServer.sendHeader("Access-Control-Allow-Origin", "*");
      oESP8266WebServer.send(200, "text/html", acServerIndex);
    });
    oESP8266WebServer.on("/update", HTTP_POST, []() {
      oESP8266WebServer.sendHeader("Connection", "close");
      oESP8266WebServer.sendHeader("Access-Control-Allow-Origin", "*");
      oESP8266WebServer.send(200, "text/plain", (Update.hasError()) ? "Update Failed!" : "Update Successful!");
      ESP.restart();
    },[](){
      HandleUpdate();
    });
    oESP8266WebServer.begin();
    MDNS.addService("http", "tcp", 80);
    Serial << LOG0 << " SetupServer(): Open http://" << acHostname << ".local to perform an OTA update" << endl;
//#endif
  return;
} //SetupServer


void HandleUpdate() {
  //upload() returns oHttpServer._currentUpload which is an HTTPUpload struct
  HTTPUpload& stHTTPUpload = oESP8266WebServer.upload();
  if (stHTTPUpload.status == UPLOAD_FILE_START) {
    PauseBlynk();
    Serial.setDebugOutput(true);
    WiFiUDP::stopAll();
    uint32_t ulMaxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    Serial << LOG0 << " HandleUpdate(): Update status     = UPLOAD_FILE_START" << endl;
    Serial << LOG0 << " HandleUpdate(): ulMaxSketchSpace  = " << ulMaxSketchSpace << endl;
    Serial << LOG0 << " HandleUpdate(): Update filename   = " << stHTTPUpload.filename << endl;
    Serial << LOG0 << " HandleUpdate(): Update name       = " << stHTTPUpload.name << endl;
    Serial << LOG0 << " HandleUpdate(): Update type       = " << stHTTPUpload.type << endl;
    Serial << LOG0 << " HandleUpdate(): Update totalSize  = " << stHTTPUpload.totalSize << endl;
    Serial << LOG0 << " HandleUpdate(): Update currentSize= " << stHTTPUpload.currentSize << endl;
    if (!Update.begin(ulMaxSketchSpace)) { //start with max available size
      Update.printError(Serial);
    } //if(!Update.begin(maxSketchSpace))
  } //if(WiFi.waitForConnectResult()==WL_CONNECTED)
  else if (stHTTPUpload.status == UPLOAD_FILE_WRITE) {
    //Serial << LOG0 << " Handle /update HTTP_POST: UPLOAD_FILE_WRITE, upload.currentSize= " << stHTTPUpload.currentSize << endl;
    if (Update.write(stHTTPUpload.buf, stHTTPUpload.currentSize)
        != stHTTPUpload.currentSize) {
      Update.printError(Serial);
    } //if(Update.write(upload.buf, upload.currentSize) != upload.currentSize)
  } //else if(upload.status==UPLOAD_FILE_WRITE)
  else if (stHTTPUpload.status == UPLOAD_FILE_END) {
    Serial << LOG0 << " HandleUpdate(): Update status     = UPLOAD_FILE_END" << endl;
    Serial << LOG0 << " HandleUpdate(): Update filename   = " << stHTTPUpload.filename << endl;
    Serial << LOG0 << " HandleUpdate(): Update name       = " << stHTTPUpload.name << endl;
    Serial << LOG0 << " HandleUpdate(): Update type       = " << stHTTPUpload.type << endl;
    Serial << LOG0 << " HandleUpdate(): Update totalSize  = " << stHTTPUpload.totalSize << endl;
    Serial << LOG0 << " HandleUpdate(): Update currentSize= " << stHTTPUpload.currentSize << endl;
    HandleFileEnd(stHTTPUpload);
#if false
    if (Update.end(true)) { //true to set the size to the current progress
      //Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      Serial << LOG0
          << " HandleUpdate(): UPLOAD_FILE_END (rebooting?), upload.totalSize= "
          << stHTTPUpload.totalSize << endl;
    } //if(Update.end(true))
    else {
      Update.printError(Serial);
    } //if(Update.end(true))else
    Serial.setDebugOutput(false);
#endif
  } //else if(upload.status==UPLOAD_FILE_END)
  yield();
  return;
} //HandleUpdate


void HandleFileEnd(HTTPUpload& stHTTPUploadLocal) {
  if (Update.end(true)) { //true to set the size to the current progress
    Serial << LOG0 << " HandleFileEnd(): UPLOAD_FILE_END (rebooting?), upload.totalSize= " << stHTTPUploadLocal.totalSize << endl;
  } //if(Update.end(true))
  else {
    Update.printError(Serial);
  } //if(Update.end(true))else
  Serial.setDebugOutput(false);
  return;
} //HandleFileEnd


void PauseBlynk() {
    bUpdating= true;
    Serial << LOG0 << " PauseBlynk(): Set bUpdating to " << bUpdating << endl;
    ulUpdateTimeoutMsec= millis() + 20000;
  return;
} //PauseBlynk
#endif


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
  String szLogString = "SetupSwitches()";
  LogToBoth(szLogString);
  for (int sSwitch= 1; sSwitch <= sNumSwitches; sSwitch++){
    pinMode(asSwitchPin[sSwitch], OUTPUT);
    SetSwitch(sSwitch, sSwitchOpen);
  } //for
  return;
} //SetupSwitches


//#ifndef SKIP_SERVER
#if OTA_SERVER
void HandleHttpServer(void){
  oESP8266WebServer.handleClient();
  delay(1);
  return;
} //HandleHttpServer
#endif

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
        HandleDevelopment();
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


void HandleDevelopment(){
  String szLogString = "HandleDevelopment()";
  LogToBoth(szLogString);
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


void DebugHandleBlynkLEDs(){
  String szLogString= "S Now Last: ";
  int sLastSwitch= 4;
  for (int sSwitch= 0; sSwitch <= sLastSwitch; sSwitch++){
    szLogString += sSwitch;
    szLogString += " ";
    szLogString += asSwitchState[sSwitch];
    szLogString += " ";
    szLogString += asSwitchLastState[sSwitch];
    if (sSwitch != sLastSwitch){
      szLogString += ", ";
    } //if(sSwitch!=sLastSwitch)
  } //for
  LogToBoth(szLogString);
  return;
} //DebugHandleBlynkLEDs


void HandleBlynkLEDs(){
  String szLogString = "HandleBlynkLEDs()";
  LogToBoth(szLogString);
  //DebugHandleBlynkLEDs();
  //Only send data back to Blynk if state of LED has changed.
  //static int asSwitchLastState[]= {sNotInit, sNotInit, sNotInit, sNotInit, sNotInit};
  bDebugLog= bDebug;  //If bDebug is OfF then we turn logging off for this routine, resetting at end.
  for (int sSwitch= 0; sSwitch <= sNumSwitches; sSwitch++){
    if (true || (asSwitchState[sSwitch] != asSwitchLastState[sSwitch])){
      asSwitchLastState[sSwitch]= asSwitchState[sSwitch];
      switch (sSwitch){
        case 0:
          if (asSwitchState[sSwitch]){
            szLogString = "Switch ON LED";
            oLED0.on();
          } //if(asSwitchState[sSwitch])
          else{
            szLogString = "Switch OFF LED";
            oLED0.off();
          } //if(asSwitchState[sSwitch])else
          break;
        case 1:
          if (asSwitchState[sSwitch]){
            szLogString = "Switch ON LED";
            oLED1.on();
          } //if(asSwitchState[sSwitch])
          else{
            szLogString = "Switch ON LED";
            oLED1.off();
          } //if(asSwitchState[sSwitch])else
          break;
        case 2:
          if (asSwitchState[sSwitch]){
            szLogString = "Switch ON LED";
            oLED2.on();
          } //if(asSwitchState[sSwitch])
          else{
            szLogString = "Switch OFF LED";
            oLED2.off();
          } //if(asSwitchState[sSwitch])else
          break;
        case 3:
          if (asSwitchState[sSwitch]){
            szLogString = "Switch ON LED";
            oLED3.on();
          } //if(asSwitchState[sSwitch])
          else{
            szLogString = "Switch OFF LED";
            oLED3.off();
          } //if(asSwitchState[sSwitch])else
          break;
        case 4:
          if (asSwitchState[sSwitch]){
            szLogString = "Switch ON LED";
            oLED4.on();
          } //if(asSwitchState[sSwitch])
          else{
            szLogString = "Switch OFF LED";
            oLED4.off();
          } //if(asSwitchState[sSwitch])else
          break;
        default:
          break;
      } //switch
      //LogToBoth(szLogString, sSwitch);
    } //if(asSwitchState[sSwitch]!=asSwitchLastState[sSwitch])
  } //for
  bDebugLog= true;
  return;
} //HandleBlynkLEDs


void HandleFurnaceSwitch(){
  String szLogString = "HandleFurnaceSwitch(): bFurnaceOn";
  LogToBoth(szLogString, bFurnaceOn);
  //Serial << LOG0 << "HandleFurnaceSwitch(): bThermoOn, bFurnaceOn " << bThermoOn << ", " << bFurnaceOn << endl;
  //Make sure  switch state represents bHeatOn correctly.
  if (bFurnaceOn){
    //Serial << LOG0 << "HandleFurnaceSwitch(): Set asSwitchState[sFurnaceSwitchNum] to sOn" << endl;
    asSwitchState[sFurnaceSwitchNum]= sOn;
  } //if(bFurnaceOn)
  else{
    //Serial << LOG0 << "HandleFurnaceSwitch(): Set asSwitchState[sFurnaceSwitchNum] to sOff" << endl;
    asSwitchState[sFurnaceSwitchNum]= sOff;
  } //if(bFurnaceOn)else
  SetSwitch(sFurnaceSwitchNum, asSwitchState[sFurnaceSwitchNum]);
  return;
} //HandleFurnaceSwitch


void TurnFurnaceOn(bool bTurnOn){
  if (bTurnOn){
    String szLogString= "TurnFurnaceOn(): Furnace turned ON";
    LogToBoth(szLogString);
    bFurnaceOn= true;
    SetFurnaceSwitch(sSwitchClosed);
    sThermoTimesCount= 0;
  } //if(bTurnOn)
  else{
    String szLogString= "TurnFurnaceOn(): Furnace turned OFF";
    LogToBoth(szLogString);
    bFurnaceOn= false;
    SetFurnaceSwitch(sSwitchOpen);
    sThermoTimesCount= 0;
  } //if(bTurnOn)else
  return;
} //TurnFurnaceOn


void SetThermoState(int sSwitchState){
  asSwitchState[sThermoDummySwitch]= sSwitchState;
  if (sSwitchState == sOn){
    bThermoOn= true;
  } //if(sState==sOn)
  else{
    bThermoOn= false;
    bFurnaceOn= false;
    sThermoTimesCount= 0;
    SetFurnaceSwitch(sSwitchOpen);
  } //if(sState==sOn)else
  return;
} //SetThermoState


void SetFurnaceSwitch(int sSwitchState){
  SetSwitch(sFurnaceSwitchNum, sSwitchState);
  return;
} //SetFurnaceSwitch


void SetSwitch(int sSwitch, int sSwitchState){
  int sSwitchPin= asSwitchPin[sSwitch];
  bool bPinSetting;
  asSwitchState[sSwitch]= sSwitchState;
  bDebugLog= bDebug;
  if (abSwitchInverted[sSwitch]){
    bPinSetting= !sSwitchState;
  } //if(abSwitchInverted[sSwitch])
  else{
    bPinSetting= sSwitchState;
  } //if(abSwitchInverted[sSwitch])else
  //Serial << LOG0 << "SetSwitch(): sSwitch, sSwitchState, sSwitchPin, bPinSetting " << sSwitch << ", " << sSwitchState << ", " << sSwitchPin << ", " << bPinSetting <<  endl;
  //Serial << LOG0 << "SetSwitch(): sSwitch, sSwitchState, sSwitchPin, bPinSetting" <<  endl;
  String szLogString= "SetSwitch:  ";
  szLogString += sSwitch;
  szLogString += ",";
  szLogString += sSwitchState;
  szLogString += ",";
  szLogString += sSwitchPin;
  szLogString += ",";
  szLogString += bPinSetting;
  //LogToBoth(szLogString);

  //Some switch positions don't have pins, just Blynk LEDs.
  if (sSwitchPin >= 0){
    digitalWrite(sSwitchPin, bPinSetting);
    asSwitchState[sSwitch]= sSwitchState;
  } //if(sSwitchPin>=0)
  bDebugLog= true;
  //HandleBlynkLEDs();
  return;
} //SetSwitch


String szGetTime(long lMsec){
  String  szString;

  int sDays    =    lMsec                                               / lMsecPerDay ;
  int sHours   =   (lMsec % lMsecPerDay)                                / lMsecPerHour;
  int sMinutes =  ((lMsec % lMsecPerDay) % lMsecPerHour)                / lMsecPerMin ;
  int sSeconds = (((lMsec % lMsecPerDay) % lMsecPerHour) % lMsecPerMin) / lMsecPerSec;
  int sMsec    =    lMsec % lMsecPerSec;
  szString = String(sDays) + ":";
  szString+= String(szAddZeros(sHours, 2)) + ":";
  szString+= String(szAddZeros(sMinutes, 2)) + ":";
  szString+= String(szAddZeros(sSeconds, 2)) + ".";
  szString+= String(szAddZeros(sMsec, 3)) + " ";     //Send with trailing blank to seperate from next field.
  return szString;
} //szGetTime


//szAddLeadingZeros() adds 1 or 2 zeros (depending on sNumDigits being 3 or not).
String szAddZeros(int sValue, int sNumDigits){
  String szReturn;
  if ((sNumDigits == 3) && (sValue < 100)){
    szReturn= "0";
  } //if((sNumDigits==3)&&(sValue<100)
  if (sValue < 10){
    szReturn += "0";
  } //if(lValue<10)
  szReturn += String(sValue);
  return szReturn;
} //szAddZeros

/*
int sTerminalPrintVersion(){
  Serial << LOG0 << " sTerminalPrintVersion: Begin" << endl;
  oTerminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  oTerminal.println("-------------");
  oTerminal.println("Type 'Marco' and get a reply, or type");
  oTerminal.println("anything else and get it printed back.");
  oTerminal.flush();
  return 1;
} //sTerminalPrintVersion
*/

// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
//int WriteTerminalLine(char *szString){
void WriteTerminalLine(String szString){
  if (bDebugLog){
    oTerminal.println(szString) ;
    oTerminal.flush();          // Ensure everything is sent
  } //if(bDebugLog)
  return;
} //WriteTerminalLine


void WriteTerminalString(String szString){
  if (bDebugLog){
    oTerminal.print(szString);
    oTerminal.print(" ");       //Send training space
    oTerminal.flush();          // Ensure everything is sent
  } //if(bDebugLog)
  return;
} //WriteTerminalString


String szLogLineHeader(long lLineCount){
  String szHeader= "";
  szHeader += lLineCount;
  szHeader += " ";
  //szTermString += szTime;
  szHeader += szGetTime(millis());
  //szHeader += " ";
  return szHeader;
} //szLogLineHeader


//LogToBoth() and BlynkLogLine()have multiple versions
//depending on there being a 2nd variable and its type.
void LogToBoth(String szLogString){
  Serial << LOG0 << szLogString << endl;
  BlynkLogLine(szLogString);
  return;
} //LogToBoth:empty


void LogToBoth(String szLogString, String szLogValue){
  Serial << LOG0 << szLogString << " " << szLogValue << endl;
  BlynkLogLine(szLogString, szLogValue);
  return;
} //LogToBoth:String


void LogToBoth(String szLogString, int sLogValue){
  Serial << LOG0 << szLogString << " " << sLogValue << endl;
  BlynkLogLine(szLogString, sLogValue);
  return;
} //LogToBoth:int


void LogToBoth(String szLogString, float fLogValue){
  Serial << LOG0 << szLogString << " " << fLogValue << endl;
  BlynkLogLine(szLogString, fLogValue);
  return;
} //LogToBoth:float


void BlynkLogLine(String szString){
  String szTermString= szLogLineHeader(lLineCount);
  szTermString += szString;
  WriteTerminalLine(szTermString);
  return;
} //BlynkLogLine:empty


void BlynkLogLine(String szString, String szLogValue){
  String szTermString= szLogLineHeader(lLineCount);
  szTermString += szString;
  szTermString +=  " ";
  szTermString +=  szLogValue;
  WriteTerminalLine(szTermString);
  return;
} //BlynkLogLine:String


void BlynkLogLine(String szString, int sValue){
  String szTermString= szLogLineHeader(lLineCount);
  szTermString += szString;
  szTermString +=  " ";
  szTermString +=  sValue;
  WriteTerminalLine(szTermString);
  return;
} //BlynkLogLine:int


void BlynkLogLine(String szString, float fValue){
  String szTermString= szLogLineHeader(lLineCount);
  szTermString += szString;
  szTermString +=  fValue;
  WriteTerminalLine(szTermString);
  return;
} //BlynkLogLine:float


float fGetDegF(bool bTakeReading){
  float fDegFReturn= 37.37;   //Value used for default in testing w/o reading sensor. fLastDegF
  if (bTakeReading){
    oSensors.requestTemperatures(); // Send the command to get temperatures
    fDegFReturn= oSensors.getTempFByIndex(0);
    fLastDegF= fDegFReturn;
  } //if(bTakeReading)
  else{
    fDegFReturn= fLastDegF;
  } //if(bTakeReading)else
  return fDegFReturn;
}  //fGetDegF


float fRound(float fNum){
  oSensors.requestTemperatures(); // Send the command to get temperatures
  float fRounded= floor(fNum + 0.5);
  return fRounded;
}  //fRound


void SendIntToBlynk(int sVirtualPin, int sValue){
  String szString= " SendIntToBlynk: ";
  BlynkLogLine(szString, sValue);
  Blynk.virtualWrite(sVirtualPin, sValue);
  return;
} //SendIntToBlynk


//BLYNK_READ() functions are called by the Blynk app on the phone (at a 1 second interval)
//and returns the value or state of some variable.
//BLYNK_WRITE() functions are called by the Blynk app on the phone
//and pass a variable in the "param" object.
BLYNK_READ(ReadF_V0){
  bool bTakeReading= true;
  float fDegF= fGetDegF(bTakeReading);
  String szLogString= "Read ReadF_V0 ";
  LogToBoth(szLogString, fDegF);

  //Blynk.virtualWrite(ReadF_V0, fRound(fDegF));
  Blynk.virtualWrite(ReadF_V0, fDegF);
} //BLYNK_READ(ReadF_V0)


BLYNK_READ(ReadF_V1){
  bool bTakeReading= false;
  float fDegF= fGetDegF(bTakeReading);
  String szLogString= "Read ReadF_V1 ";
  LogToBoth(szLogString, fDegF);

  Blynk.virtualWrite(ReadF_V1, fDegF);
} //BLYNK_READ(ReadF_V1)


BLYNK_WRITE(SetSetpointF_V2){
  int sSetpointParam= param.asInt();
  sSetpointF= sSetpointParam;
  fThermoOffDegF= sSetpointF + fMaxHeatRangeF;
  String szLogString= "SetSetpointF_V2 ";
  LogToBoth(szLogString, sSetpointF);

  //Send set point back to Value box set with PUSH from GetSetpointF_V3.
  SendIntToBlynk(GetSetpointF_V3, sSetpointF);
  return;
} //BLYNK_WRITE(Switch_2V15)


BLYNK_READ(GetSetpointF_V3){
  int sReturnF= sSetpointF;
  String szLogString= "GetSetpointF_V3 ";
  LogToBoth(szLogString, sSetpointF);

  Blynk.virtualWrite(GetSetpointF_V3, sReturnF);
} //BLYNK_READ(GetSetpointF_V3)


BLYNK_WRITE(ThermoSwitch_V4){
  //Turn thermostat on and off.
  int sParam= param.asInt();
  String szLogString= "ThermoSwitch_V4 ";
  LogToBoth(szLogString, sParam);
  SetThermoState(sParam);
  HandleFurnaceSwitch();

  //Send set point back to Value box set with PUSH from GetSetpointF_V3.
  SendIntToBlynk(GetSetpointF_V3, sSetpointF);
  //HandleBlynkLEDs();
  return;
} //BLYNK_WRITE(ThermoSwitch_V4)

//WidgetLED oLED0(ThermoLED_V5) is constructed earlier

BLYNK_READ(AtoD_1V6){
  static float fVolts= 1.0;
  fVolts= fVolts + 0.001;
  String szLogString= "Read AtoD_1V6 ";
  LogToBoth(szLogString, fVolts);
  Blynk.virtualWrite(AtoD_1V6, fVolts);
} //BLYNK_READ(AtoD_1V6)


//Handler callback function called when Button set as a Switch is pressed.
//Light around button is lit when 1 is passed as parameter, unlit when 0 is passed.
//Opto-isolated relay is inverse logic, pulling input pin low cn relay.
//Relay #0 is connected to Blynk virtual pins 10, 11, 12
//Relay #1 is connected to Blynk virtual pins 20, 21, 22
BLYNK_WRITE(Switch_1V10){
  //Virtual pin 10, 15, 20 and 25 control Switches 1, 2, 3 and 4.
  int sSwitchNumber= 1;
  int sSwitchSetting;
  int sSetting= param.asInt();

  String szLogString= "Set Switch_1V10 ";
  szLogString += sSetting;
  LogToBoth(szLogString);
/*
  //Test writing to LCD
  LCDWidget.clear();
  int sCharPos= 0;   //Position 0-15
  int sLineNum= 0;   //Line 0-1
  LCDWidget.print(0, 0, "Relay #0 set to: ");
*/
  if (sSetting == 1){
    sSwitchSetting= sSwitchClosed;
  }
  else{
    sSwitchSetting= sSwitchOpen;
  }
  SetSwitch(sSwitchNumber, sSwitchSetting);
  return;
} //BLYNK_WRITE(Switch_1V10)


BLYNK_WRITE(TimerA_1V11){
  int sSwitchNumber= 1;
  int sSwitchSetting;
  int sSetting= param.asInt();
  String szLogString= "Set TimerA_1V11 ";
  szLogString += sSetting;
  LogToBoth(szLogString);

  if (sSetting == 1){
    sSwitchSetting= sSwitchClosed;
    }
  else{
    sSwitchSetting= sSwitchOpen;
    }
  SetSwitch(sSwitchNumber, sSwitchSetting);
  return;
  } //BLYNK_WRITE(TimerA_1V11)


BLYNK_WRITE(TimerB_1V12){
  int sSwitchNumber= 1;
  int sSwitchSetting;
  int sSetting= param.asInt();
  String szLogString= "Set TimerB_1V12 ";
  szLogString += sSetting;
  LogToBoth(szLogString);

  if (sSetting == 1){
    sSwitchSetting= sSwitchClosed;
    }
  else{
    sSwitchSetting= sSwitchOpen;
    }
  SetSwitch(sSwitchNumber, sSwitchSetting);
  return;
  } //BLYNK_WRITE(TimerB_1V12)


//WidgetLED oLED1(LED_1V13) is constructed earlier


BLYNK_READ(AtoD_2V14){
  static float fVolts= 2.0;
  fVolts= fVolts + 0.001;
  String szLogString= "Read AtoD_2V14 ";
  LogToBoth(szLogString, fVolts);
  Blynk.virtualWrite(AtoD_2V14, fVolts);
} //BLYNK_READ(AtoD_2V14)


BLYNK_WRITE(Switch_2V15){
  int sSwitchNumber= 2;
  int sSwitchSetting;
  int sSetting= param.asInt();
  String szLogString= "Set Switch2V15 ";
  szLogString += sSetting;
  LogToBoth(szLogString);

  if (sSetting == 1){
    sSwitchSetting= sSwitchClosed;
  }
  else{
    sSwitchSetting= sSwitchOpen;
  }
  SetSwitch(sSwitchNumber, sSwitchSetting);
  return;
} //BLYNK_WRITE(Switch_2V15)


BLYNK_WRITE(TimerA_2V16){
  int sSwitchNumber= 2;
  int sSwitchSetting;
  int sSetting= param.asInt();
  String szLogString= "Set TimerA_2V16 ";
  szLogString += sSetting;
  LogToBoth(szLogString);

  if (sSetting == 1){
    sSwitchSetting= sSwitchClosed;
  }
  else{
    sSwitchSetting= sSwitchOpen;
  }
  SetSwitch(sSwitchNumber, sSwitchSetting);
  return;
} //BLYNK_WRITE(TimerA_2V16)


BLYNK_WRITE(TimerB_2V17){
  int sSwitchNumber= 2;
  int sSwitchSetting;
  int sSetting= param.asInt();
  String szLogString= "Set TimerB_2V17 ";
  szLogString += sSetting;
  LogToBoth(szLogString);
  if (sSetting == 1){
    sSwitchSetting= sSwitchClosed;
  }
  else{
    sSwitchSetting= sSwitchOpen;
  }
  SetSwitch(sSwitchNumber, sSwitchSetting);
  return;
} //BLYNK_WRITE(TimerB_2V17)


//WidgetLED oLED1(LED_2V18) is constructed earlier


BLYNK_READ(AtoD_3V19){
  static float fVolts= 3.0;
  fVolts= fVolts + 0.001;
  String szLogString= "Read AtoD_3V19 ";
  LogToBoth(szLogString, fVolts);
  Blynk.virtualWrite(AtoD_3V19, fVolts);
} //BLYNK_READ(AtoD_3V19)


BLYNK_WRITE(Switch_3V20){
  int sSwitchNumber= 3;
  int sSwitchSetting;
  int sSetting= param.asInt();
  String szLogString= "Set Switch_3V20 ";
  szLogString += sSetting;
  LogToBoth(szLogString);
  if (sSetting == 1){
    sSwitchSetting= sSwitchClosed;
  }
  else{
    sSwitchSetting= sSwitchOpen;
  }
  SetSwitch(sSwitchNumber, sSwitchSetting);
  return;
} //BLYNK_WRITE(Switch_3V20)


BLYNK_WRITE(TimerA_3V21){
  int sSwitchNumber= 3;
  int sSwitchSetting;
  int sSetting= param.asInt();
  String szLogString= "Set TimerA_3V21 ";
  szLogString += sSetting;
  LogToBoth(szLogString);
  if (sSetting == 1){
    sSwitchSetting= sSwitchClosed;
  }
  else{
    sSwitchSetting= sSwitchOpen;
  }
  SetSwitch(sSwitchNumber, sSwitchSetting);
  return;
} //BLYNK_WRITE(TimerA_3V21)


BLYNK_WRITE(TimerB_3V22){
  int sSwitchNumber= 3;
  int sSwitchSetting;
  int sSetting= param.asInt();
  String szLogString= "Set TimerB_3V22 ";
  szLogString += sSetting;
  LogToBoth(szLogString);

  if (sSetting == 1){
    sSwitchSetting= sSwitchClosed;
  }
  else{
    sSwitchSetting= sSwitchOpen;
  }
  SetSwitch(sSwitchNumber, sSwitchSetting);
  return;
} //BLYNK_WRITE(TimerB_3V22)


//WidgetLED oLED1(LED_3V23) is constructed earlier


BLYNK_READ(AtoD_4V24){
  static float fVolts= 4.0;
  fVolts= fVolts + 0.001;
  String szLogString= "Read AtoD_4V24 ";
  LogToBoth(szLogString, fVolts);
  Blynk.virtualWrite(AtoD_4V24, fVolts);
} //BLYNK_READ(AtoD_4V24)


BLYNK_WRITE(Switch_4V25){
  int sSwitchNumber= 4;
  int sSwitchSetting;
  int sSetting= param.asInt();
  String szLogString= "Set Switch_4V25 ";
  szLogString += sSetting;
  LogToBoth(szLogString);
  if (sSetting == 1){
    sSwitchSetting= sSwitchClosed;
  }
  else{
    sSwitchSetting= sSwitchOpen;
  }
  SetSwitch(sSwitchNumber, sSwitchSetting);
  return;
} //BLYNK_WRITE(Switch_4V25)


BLYNK_WRITE(TimerA_4V26){
  int sSwitchNumber= 4;
  int sSwitchSetting;
  int sSetting= param.asInt();
  String szLogString= "Set TimerA_4V26 ";
  szLogString += sSetting;
  LogToBoth(szLogString);
  if (sSetting == 1){
    sSwitchSetting= sSwitchClosed;
  }
  else{
    sSwitchSetting= sSwitchOpen;
  }
  SetSwitch(sSwitchNumber, sSwitchSetting);
  return;
} //BLYNK_WRITE(TimerA_4V26)


BLYNK_WRITE(TimerB_4V27){
  int sSwitchNumber= 4;
  int sSwitchSetting;
  int sSetting= param.asInt();
  String szLogString= "Set TimerB_4V27 ";
  szLogString += sSetting;
  LogToBoth(szLogString);

  if (sSetting == 1){
    sSwitchSetting= sSwitchClosed;
  }
  else{
    sSwitchSetting= sSwitchOpen;
  }
  SetSwitch(sSwitchNumber, sSwitchSetting);
  return;
} //BLYNK_WRITE(TimerB_4V27)


//WidgetLED oLED1(LED_4V28) is constructed earlier

/*
BLYNK_WRITE(Terminal_V7)
{
  Serial << LOG0 << " BLYNK_WRITE(Terminal_V7): Received Parameter= " <<  param.asStr() << endl;
  // if you type "Marco" into Terminal Widget - it will respond: "Polo:"
  if (String("Marco") == param.asStr()) {
      oTerminal.println("You said: 'Marco'") ;
      oTerminal.println("I said: 'Polo'") ;
  } else {

  // Send it back
  oTerminal.print("You said:");
  oTerminal.write(param.getBuffer(), param.getLength());
  oTerminal.println();
  }

  // Ensure everything is sent
  oTerminal.flush();
  return;
} //BLYNK_WRITE(Terminal_V7)
*/
//Last line.
