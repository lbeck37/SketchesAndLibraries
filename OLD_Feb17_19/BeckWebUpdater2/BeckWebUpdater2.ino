static const char acSketchName[]  = "BeckWebUpdater2.ino";
static const char acFileDate[]    = "Apr 17, 2016A";
// 1/5/16 Get running on V64 eclipseArduino

#include <BeckLib.h>

//Global variables
long         lLineCount= 0;      //Serial Monitor uses for clarity.

//static const char   acRouterName[]        = "TrailheadBoise";
//static const char   acRouterPW[]          = "Trailhead2015";

static const char   acRouterName[]        = "Aspot24";
static const char   acRouterPW[]          = "Qazqaz11";
static char   		acHostname[]          = "esp39";

ESP8266WebServer    		oHttpServer(80);
ESP8266HTTPUpdateServer 	oHttpUpdateServer(true);

static const String 	acDatabaseURL		= "intense-fire-3958.firebaseio.com";
static const String 	acFirebaseSecret	= "LhXHxFsUn7SVYoRC82dKKSqqD67Ls9nfdtMBAWUe";
static const String 	acPushPath			= "/logs";
String	szLogLine;

void setup(void){
  Serial.begin(lSerialMonitorBaud);
  Serial << endl << LOG0 << " setup(): Initialized serial to " << lSerialMonitorBaud << " baud" << endl;
  Serial << LOG0 << " Sketch: " << acSketchName << ", " << acFileDate << endl;

  Serial << LOG0 << " Setting WiFi mode to WIFI_AP_STA" << endl;
  WiFi.mode(WIFI_AP_STA);

  Serial << LOG0 << " Call WiFi.begin(" << acRouterName << ", " << acRouterPW << ")" << endl;
  WiFi.begin(acRouterName, acRouterPW);

  Serial << LOG0 << " Call WiFi.waitForConnectResult()" << endl;
  while(WiFi.waitForConnectResult() != WL_CONNECTED){
	Serial << LOG0 << " WiFi failed, retrying." << endl;
	Serial << LOG0 << " Call WiFi.begin(" << acRouterName << ", " << acRouterPW << ")" << endl;
    WiFi.begin(acRouterName, acRouterPW);
   }

  szLogLine=  LOG0 + " *setup(): WifFi Connected, WiFi.status() returned WL_CONNECTED";
  LogToSerial(szLogLine);

  SetupHttpServer(acHostname, oHttpServer, oHttpUpdateServer);

  //Create Firebase client.
  Serial << LOG0 << " Create Firebase client" << endl;
  Serial << LOG0 << " Call Firebase('" << acDatabaseURL << "')" << endl;
  Serial << LOG0 << "     .auth('" << acFirebaseSecret << "')" << endl;
  Firebase oFBase = Firebase(acDatabaseURL).auth(acFirebaseSecret);
/*
  //Firebase oFBase = Firebase("intense-fire-3958.firebaseio.com").auth("LhXHxFsUn7SVYoRC82dKKSqqD67Ls9nfdtMBAWUe");
  Firebase oFBase = Firebase(acDatabaseURL);
  oFBase.auth(acFirebaseSecret);
*/

  //Serial << LOG0 << " Setup() done in version: " << acFileDate << endl;
  szLogLine= LOG0 + " Setup() done in version: " + acFileDate;
#if 1
  LogToSerial(szLogLine);
  Serial << LOG0 << " DEBUGGING: Skip call to LogToBoth()" << endl;
#else
  LogToBoth(oFBase, acPushPath, szLogLine);
#endif
  return;
}	//setup


void loop(void){
  HandleHttpServer(oHttpServer);
}	//loop
//Last line.