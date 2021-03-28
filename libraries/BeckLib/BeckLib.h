//BeckLib.h, 5/19/19
#ifndef BECKLIB_H
#define BECKLIB_H
#include <BeckMiniLib.h>
#include <Streaming.h>
#include <Time.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <BlynkSimpleEsp8266.h>
//#include <Firebase.h>
//#include <BeckFirebase.h>
#include <Adafruit_ADS1015.h>   //Just for the defines

/*
#ifndef OTA_SERVER
  #define OTA_SERVER
#endif  //OTA_SERVER
*/

/*
#ifdef USE_U8GLIB
  #undef USE_U8GLIB
#endif  //USE_U8GLIB
*/

/*
#ifndef USE_U8GLIB
  #define USE_U8GLIB
#endif  //USE_U8GLIB
*/

/*
#ifdef ESP8266
	#ifndef OTA_SERVER
  	#define OTA_SERVER
	#endif  //OTA_SERVER
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  //#include <BlynkSimpleEsp8266.h>
  #ifdef OTA_SERVER
    #include <ESP8266WebServer.h>
    #include <ESP8266mDNS.h>
    #include <ESP8266HTTPUpdateServer.h>
    //extern ESP8266WebServer           oHttpServer;
    //extern ESP8266HTTPUpdateServer    oHttpUpdateServer;
  #endif  //OTA_SERVER
#endif  //ESP8266

#ifdef ESP32                    //FromBeckESP32WiFiExample.ino from Blynk Examples
  #define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
  #include <WiFi.h>
  #include <WiFiClient.h>
  //#include <BlynkSimpleEsp32.h>
#endif  //ESP32
*/

/*
//#define NO_DISPLAY
#ifndef NO_DISPLAY
	#ifdef USE_U8GLIB
		#include <U8glib.h>
	#else
		#include <dog_1701.h>
		#include <font_16x32nums.h>
		#include <font_6x8.h>
		#include <font_8x16.h>
		#include <font_8x8.h>
		#include <logo_BLH.h>
	#endif	//USE_U8GLIB
#endif	//NO_DISPLAY
*/

/* In minilib
#define INT8        int8_t
#define UINT8       uint8_t
#define INT16       int16_t
#define UINT16      uint16_t
#define INT32       int32_t
#define UINT32      uint32_t
#define INT64       int64_t
#define UINT64      uint64_t

#define LOG0        szLogLineHeader(++lLineCount)
#define BLog        LogJustToSerial
#define BLogS       LogJustToSerial

extern const char 	szVersionDate[];
*/

/*
extern const int    sFrontLights;
extern const int    sFireplace;
extern const int    sGarage;
extern const int    sGarageLocal;
extern const int    sHeater;
extern const int    sDevLocal;
extern const int    sDevRemote;
extern const int    sTankMonitor;
extern const int    sHotTub;
extern const int    sHotTubV2;
*/
/* In minilib
extern int					sProjectType_;

static const long   lSerialMonitorBaud    = 115200;
static const long   lMsecPerDay           = 86400000;
static const long   lMsecPerHour          =  3600000;
static const long   lMsecPerMin           =    60000;
static const long   lMsecPerSec           =     1000;

extern bool           bDebugLog;    //Used to limit number of printouts.
*/

//Function protos
#ifdef ESP8266
	void      SetupWiFi(const char* pcRouterName, const char* pcRouterPW);
	String    szWiFiStatus(wl_status_t status);
	void      LogESPValues();
#endif
#ifdef OTA_SERVER
  void      SetupHttpServer(const char* acHostname,
                            ESP8266WebServer&        oHttpServer,
                            ESP8266HTTPUpdateServer& oHttpUpdateServer);
  void      HandleHttpServer(ESP8266WebServer& oHttpServer);
  String    szIPaddress(IPAddress oIP);
#endif  //OTA_SERVER
/*
void      LogJustToSerial(String sLogline);
String    szLogLineHeader(long lLineCount);
String    szGetTime(long lMsec);
String    szAddZeros(int sValue, int sNumDigits);
*/


//Global variables
extern int32_t                  wHttpServerCount; //To allow logging every nth call at UPLOAD_FILE_WRITE

extern bool                     bWiFiOn;
extern bool                     bFirebaseOn;
extern bool                     bStartedOTA;

/*
#ifdef OTA_SERVER
  extern ESP8266WebServer         oHttpServer;
  extern ESP8266HTTPUpdateServer  oHttpUpdateServer;
#endif  //OTA_SERVER

#ifndef NO_DISPLAY
	#ifdef USE_U8GLIB
		extern U8GLIB_DOGS102           u8g;
	#else
		//Reference display object for the DOGS102-6 (102x64) display
		extern dog_1701 DOG;
	#endif	//USE_U8GLIB
#endif
*/

/* In minilib
extern long                     lLineCount;       //Serial Monitor uses for clarity.
extern String                   szLogLine;
  extern void LogToBoth(String szLogString);
  extern void LogToBoth(String szLogString, String szLogValue);
  extern void LogToBoth(String szLogString, int sLogValue);
  extern void LogToBoth(String szLogString, UINT32 uwLogValue);
  extern void LogToBoth(String szLogString, float fLogValue);
  extern void LogToBoth(String szLogString, double dLogValue);

  extern void LogToSerial(String szLogString);
  extern void LogToSerial(String szLogString, String szLogValue);
  extern void LogToSerial(String szLogString, int sLogValue);
  extern void LogToSerial(String szLogString, UINT32 uwLogValue);
  extern void LogToSerial(String szLogString, float fLogValue);
  extern void LogToSerial(String szLogString, double dLogValue);

extern const int       sUpButtonPin;
extern const int       sDownButtonPin;
extern const byte      cI2C_SDA_Pin;
extern const byte      cI2C_SCL_Pin;
extern const byte      cSPI_A0CmdData_Pin;
extern const byte      cSPI_MISO_Pin;
extern const byte      cSPI_MOSI_Pin;
extern const byte      cSPI_CLK_Pin;
extern const byte      cSPI_Select_Pin;
extern const int       sServoPin;
extern const byte      cBogusResetPin;
extern const byte      cHW_SPI;
*/

#endif   //BECKLIB_H
