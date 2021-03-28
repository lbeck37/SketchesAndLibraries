//static const String SketchName  = "BeckLib.cpp";
//static const String FileDate    = "Dec 16, 2017, Lenny-a";

/*
static const char SketchName[]  = "BeckLib.cpp";
static const char FileDate[]    = "Dec 16, 2017, Lenny-a";
*/

//BeckLib.cpp, April 14, 2017
//#define OTA_SERVER
#include <BeckLib.h>
#include <BeckBlynk.h>
//#define DEBUG_LOGGING
//#define NO_FIREBASE
//const char szVersionDate[]    = "May 4, 2017 -CC- Lenny";

//Global variables
long                  lLineCount      = 0;  //Serial Monitor uses for clarity.
int32_t                 wHttpServerCount  = 0;  //Log every nth call at UPLOAD_FILE_WRITE
String                  szLogLine;

bool                    bWiFiOn           = false;
bool                    bFirebaseOn       = false;
bool                    bStartedOTA       = false;

#ifdef OTA_SERVER
  ESP8266WebServer        oHttpServer(80);
  ESP8266HTTPUpdateServer   oHttpUpdateServer(true);
#endif  //OTA_SERVER

/*
const int    sUndefinedProject     = 0;
const int    sFrontLights          = 1;
const int    sFireplace            = 2;
const int    sGarage               = 3;
const int    sGarageLocal          = 4;
const int    sHeater               = 5;
const int    sDevLocal             = 6;
const int    sDevRemote            = 7;
const int    sTankMonitor          = 8;
const int    sHotTub          		 = 9;
const int    sHotTubV2          	 = 10;
*/
int					 sProjectType_;		//Different from local variable sProjectType in BeckBlynkESP.ino

  //Digital Pins
#ifdef ESP32
  //BlynkBeck uses pins 4, 5, 15, 16
  //static const int       sSelectButton;
  //static const int       sBacklightPin;
  const int       sUpButtonPin       =  0;
  const int       sDownButtonPin     =  2;
  const byte      cI2C_SDA_Pin       =  26;
  const byte      cI2C_SCL_Pin       =  27;
  const byte      cSPI_A0CmdData_Pin =  4;
  const byte      cSPI_MISO_Pin      = 12;
  const byte      cSPI_MOSI_Pin      = 13;
  const byte      cSPI_CLK_Pin       = 14;
  const byte      cSPI_Select_Pin    = 15;
  const int       sServoPin          = 16;
#else   //ESP32
  #ifdef ESP8266
    //BlynkBeck uses pins 4, 5, 15, 16
    //static const int       sSelectButton;
    //static const int       sBacklightPin;
    const int       sUpButtonPin     		=  0;
    const int       sDownButtonPin   		=  2;
    const byte      cI2C_SDA_Pin      	=  4;
    const byte      cI2C_SCL_Pin      	=  5;
    const byte      cSPI_A0CmdData_Pin  =  4;
    const byte      cSPI_MISO_Pin      	= 12;
    const byte      cSPI_MOSI_Pin      	= 13;
    const byte      cSPI_CLK_Pin       	= 14;
    const byte      cSPI_Select_Pin    	= 15;
    const int       sServoPin        		= 16;
    const byte      cBogusResetPin      =  5;
    const byte      cHW_SPI             =  0;      //This is what their demo used.
  #else
    //const int       sSelectButton         = A3;
    const int       sDownButton           = A2;
    const int       sUpButton             = A1;
    const int       sBacklightPin         =  6;
    const int       sServoPin             =  7;
    const byte      cSPICmdDataPin        =  9;
    const byte      cSPIChipSelectPin     = 10;
  #endif  //ESP8266
#endif  //ESP32

bool            bDebugLog= true;   //Used to limit number of printouts.

/*
//U8glibs constructor for DOGS102-6 (sometimes called 1701) display
//U8GLIB_DOGS102 u8g(13, 11, 10, 9, 8); // Pro Mini SPI: SCK = 13, MOSI = 11, CS = 10, A0 = 9
#ifndef NO_DISPLAY
	#ifdef USE_U8GLIB
		U8GLIB_DOGS102 u8g(cSPI_CLK_Pin, cSPI_MOSI_Pin, cSPI_Select_Pin, cSPI_A0CmdData_Pin);
	#else
		//Create display object fot the DOGS102-6 (102x64) display
		dog_1701 DOG;
	#endif	//USE_U8GLIB
#endif
*/


#ifdef ESP8266
void SetupWiFi(const char* pcRouterName, const char* pcRouterPW){
  wl_status_t eStatus;
  BLogS("SetupWiFi(): Setting WiFi mode to WIFI_AP_STA");
  WiFi.mode(WIFI_AP_STA);

  BLogS("SetupWiFi(): Call WiFi.begin(" + String(pcRouterName) + ", " + String(pcRouterPW) + ")");
  eStatus= WiFi.begin(pcRouterName, pcRouterPW);
  BLogS("SetupWiFi(): WiFi.begin() returned " + szWiFiStatus(eStatus) );

  BLogS("SetupWiFi(): Call WiFi.waitForConnectResult()");
  int wCount= 0;
  int wMaxTries= 3;
  while((WiFi.waitForConnectResult() != WL_CONNECTED) && (++wCount <= wMaxTries)){
    eStatus= WiFi.status();
    BLogS("SetupWiFi():W After waitForConnectResult(), WiFi.status= " + szWiFiStatus(WiFi.status()) );
    BLogS("SetupWiFi():W WiFi failed, retrying.");
    BLogS("SetupWiFi():W Call WiFi.begin(" + String(pcRouterName) + ", " + String(pcRouterPW) + ") for #" +
        (String)(wCount + 1) + " time");
    eStatus= WiFi.begin(pcRouterName, pcRouterPW);
    BLogS("SetupWiFi():W WiFi.begin() returned " + szWiFiStatus(eStatus) );
   }  //while
  //BLogS("SetupWiFi(): After WiFi.waitForConnectResult(): " + szWiFiStatus(WiFi.status()));

  eStatus= WiFi.status();
  BLogS("SetupWiFi(): After WiFi.waitForConnectResult(): WiFi status= " + szWiFiStatus(eStatus));
  if (eStatus == WL_CONNECTED) {
    bWiFiOn= true;
  } //if(eStatus==WL_CONNECTED)
  else {
    bWiFiOn= false;
  } //if(eStatus==WL_CONNECTED)else

  //BLogS("SetupWiFi(): My WiFi IP address= " + szIPaddress(WiFi.localIP()));
} //SetupWiFi


String szWiFiStatus(wl_status_t status) {
  //wl_status_t defined in wl_definitions.h
  switch(status) {
  case WL_IDLE_STATUS:
    return "WL_IDLE_STATUS";
  case WL_NO_SSID_AVAIL:
    return "WL_NO_SSID_AVAIL";
  case WL_SCAN_COMPLETED:
    return "WL_SCAN_COMPLETED";
  case WL_CONNECTED:
    return "WL_CONNECTED";
  case WL_CONNECT_FAILED:
    return "WL_CONNECT_FAILED";
  case WL_CONNECTION_LOST:
    return "WL_CONNECTION_LOST";
  case WL_DISCONNECTED:
    return "WL_DISCONNECTED";
  case WL_NO_SHIELD:
    return "WL_NO_SHIELD";
  default:
    return "BAD switch value";
  } //switch
} //GetWiFiStatusString


void LogESPValues() {
    //BLog("getVcc= " + (String)ESP.getVcc());
    BLog("getFreeHeap= " + (String)ESP.getFreeHeap());
    BLog("getChipId= " + (String)ESP.getChipId());
    BLog("getSdkVersion= " + (String)ESP.getSdkVersion());
    //BLog("getBootVersion= " + (String)ESP.getBootVersion());
    BLog("getCpuFreqMHz= " + (String)ESP.getCpuFreqMHz());
    //BLog("getFlashChipId= " + (String)ESP.getFlashChipId());
    //BLog("getFlashChipRealSize= " + (String)ESP.getFlashChipRealSize());
    BLog("getFlashChipSize= " + (String)ESP.getFlashChipSize());
    BLog("getFlashChipSpeed= " + (String)ESP.getFlashChipSpeed());
    //BLog("getFlashChipSizeByChipId= " + (String)ESP.getFlashChipSizeByChipId());
    //BLog("getFreeSketchSpace= " + (String)ESP.getFreeSketchSpace());
    //BLog("getSketchSize= " + (String)ESP.getSketchSize());
    return;
} //LogESPValues
#endif

#ifdef OTA_SERVER
void SetupHttpServer(const char* acHostname,
          ESP8266WebServer& oHttpServer,
          ESP8266HTTPUpdateServer& oHttpUpdateServer){
  BLog("SetupHttpServer(): Call MDNS.begin(" + String(acHostname) + ")");
  MDNS.begin(acHostname);

  BLog("SetupHttpServer(): Call oHttpUpdateServer.setup(&oHttpServer)");
  oHttpUpdateServer.setup(&oHttpServer);

  BLog("SetupHttpServer(): Call oHttpServer.begin())");
  oHttpServer.begin();

  BLog("SetupHttpServer(): Call MDNS.addService(http, tcp, 80)");
  MDNS.addService("http", "tcp", 80);

  BLog("SetupHttpServer(): HTTPUpdateServer ready!");
  if (bWiFiOn) {
    BLog("SetupHttpServer(): Open http://" + String(acHostname) + ".local/update to perform OTA Update.");
  }
  else {
    BLogS("SetupHttpServer(): WiFi is not on, unable to perform OTA Update.");
  }
} //SetupHttpServer


void HandleHttpServer(ESP8266WebServer& oHttpServer){
  oHttpServer.handleClient();
  delay(1);
} //HandleHttpServer


String szIPaddress(IPAddress oIP){
  String szDot= ".";
  String szOctetIP= oIP[0] + szDot +  oIP[1] + szDot  + oIP[2] + szDot +  oIP[3];
  return szOctetIP;
} //szIPaddress
#endif  //OTA_SERVER


/*
void LogJustToSerial(String sLogline){
  String sFullLogline=LOG0 + sLogline;
  Serial << sFullLogline << endl;
  return;
} //LogJustToSerial


String szLogLineHeader(long lLineCount){
  String szHeader= "";
  szHeader += lLineCount;
  szHeader += " ";
  szHeader += szGetTime(millis());
  return szHeader;
} //szLogLineHeader


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
  szString+= String(szAddZeros(sMsec, 3)) + " ";  //Trailing blank.
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


void LogToBoth(String szLogString, UINT32 uwLogValue){
  Serial << LOG0 << szLogString << " " << uwLogValue << endl;
  return;
} //LogToBoth:UINT32


void LogToBoth(String szLogString, float fLogValue){
  Serial << LOG0 << szLogString << " " << fLogValue << endl;
  BlynkLogLine(szLogString, fLogValue);
  return;
} //LogToBoth:float


void LogToBoth(String szLogString, double dLogValue){
  Serial << LOG0 << szLogString << " " << dLogValue << endl;
  BlynkLogLine(szLogString, dLogValue);
  return;
} //LogToBoth:double

//**********************************
void LogToSerial(String szLogString){
  Serial << LOG0 << szLogString << endl;
  return;
} //LogToSerial:empty


void LogToSerial(String szLogString, String szLogValue){
  Serial << LOG0 << szLogString << " " << szLogValue << endl;
  return;
} //LogToSerial:String


void LogToSerial(String szLogString, int sLogValue){
  Serial << LOG0 << szLogString << " " << sLogValue << endl;
  return;
} //LogToSerial:int


void LogToSerial(String szLogString, UINT32 uwLogValue){
  Serial << LOG0 << szLogString << " " << uwLogValue << endl;
  return;
} //LogToSerial:UINT32


void LogToSerial(String szLogString, float fLogValue){
  Serial << LOG0 << szLogString << " " << fLogValue << endl;
  return;
} //LogToSerial:float


void LogToSerial(String szLogString, double dLogValue){
  Serial << LOG0 << szLogString << " " << dLogValue << endl;
  return;
} //LogToSerial:double
*/
//Last line.
