static const char szSketchName[]  = "BlynkAtoD.ino";
static const char szFileDate[]    = "September 19, 2016 D";
//10/14/16 I think this is a copy of BlynkBeck.ino that was incorrectly modified
//         Note that the switch in HandleSystem() only handles one case.
// 9/19/16 Copied from BeckBlynk and stripped down to FRONT_LIGHT prior to bringing in ADS1115
//Open issues:
//  - Switches 3 and 4 LEDs are not working.
//  - Add virtual pin and LED for turning on DEBUG.
//  - Get LEDs off at beginning.

//Uncomment out desired implementation.
#define FRONT_LIGHTS
//#define FIREPLACE
//#define GARAGE
//#define GARAGE_LOCAL    //Run off local Blynk server.
//#define HEATER
//#define DEV_LOCAL
//#define DEV_REMOTE

#include <Streaming.h>
#include <Time.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
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

#define Unassigned_V6     V6

#define Terminal_V7       V7
#define LCD_Line0_V8      V8
#define LCD_Line1_V9      V9

//Relay #1
#define Switch_1V10       V10
#define TimerA_1V11       V11
#define TimerB_1V12       V12
#define LED_1V13          V13

#define Unassigned_V14    V14
//Relay #2
#define Switch_2V15       V15
#define TimerA_2V16       V16
#define TimerB_2V17       V17
#define LED_2V18          V18

#define Unassigned_V19    V19
//Relay #3
#define Switch_3V20       V20
#define TimerA_3V21       V21
#define TimerB_3V22       V22
#define LED_3V23          V23

#define Unassigned_V24    V24
//Relay #4
#define Switch_4V25       V25
#define TimerA_4V26       V26
#define TimerB_4V27       V27
#define LED_4V28          V28

#define Unassigned_V29    V29
#define Unassigned_V30    V30
#define Unassigned_V31    V31

#define LOG0    szLogLineHeader(++lLineCount)

static const bool bSkipBlynk        	= false;
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
DallasTemperature	oSensors(&oOneWire);

//ESP8266WebServer  	oESP8266WebServer(80);

//UpdaterClass 		Update;	//Declaration at the end of cores\esp8266\Updater.h from BSP

const char*     acServerIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

static int          asSwitchState[]       = {0, 0, 0, 0, 0};
static int          asSwitchLastState[]   = {sNotInit, sNotInit, sNotInit, sNotInit, sNotInit};
static long         lLineCount            = 0;      //Serial Monitor uses for clarity.
//static long         lLineCount2           = 0;      //For Blynk terminal window.
//static long         lNumLoops             = 1;
static float          fLastDegF             = 37.37;  //Last temperature reading.
static int            sSetpointF            = 37;
static unsigned long  ulNextHandlerMsec     = 0;
static unsigned long  ulUpdateTimeoutMsec   = 0;
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
	//HandleHttpServer();
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
  //SetupServer();
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


void HandleSystem(){
  if (millis() >= ulNextHandlerMsec){
    String szLogString = "HandleSystem()";
    LogToBoth(szLogString);
    ulNextHandlerMsec= millis() + sSystemHandlerSpacing;
    switch (sProjectType){
      case sFrontLights:
        HandleFrontLights();
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
/*
  SetThermoState(sParam);
  HandleFurnaceSwitch();
*/

  //Send set point back to Value box set with PUSH from GetSetpointF_V3.
  SendIntToBlynk(GetSetpointF_V3, sSetpointF);
  //HandleBlynkLEDs();
  return;
} //BLYNK_WRITE(ThermoSwitch_V4)

//WidgetLED oLED0(ThermoLED_V5) is constructed earlier

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
//Last line.
