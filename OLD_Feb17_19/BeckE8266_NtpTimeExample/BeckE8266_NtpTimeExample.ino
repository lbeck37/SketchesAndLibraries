const char szSketchName[]  = "BeckE8266_NtpTimeExample.ino";
const char szFileDate[]    = "Lenny 1/27/19a";
/*
 Name:    NtpClient.ino
 Created: 20/08/2016
 Author:  gmag11@gmail.com
 Editor:  http://www.visualmicro.com
*/
#include <BeckMiniLib.h>
#include <BeckWiFiLib.h>
#include <BeckE8266OTALib.h>
#include <BeckE8266AccessPointLib.h>
//#include <BeckE8266NTPLib.h>
#include <BeckNTPLib.h>

static const char   szRouterName[]      = "Aspot24";
static const char   szRouterPW[]        = "Qazqaz11";

static const char   acHostname[]			  = "BeckThermoDev";
static const int		wLoopMilliDelay		  = 5100;

void setup(){
  Serial.begin(115200);
  Serial << endl << LOG0 << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;

  SetupWiFi(szRouterName, szRouterPW);
  SetupOTAServer(acHostname);
  SetupAccessPoint();
  SetupWebServer(_oAccessPtIPAddress);
  SetupNTP();

  return;
} //setup


void loop(){
  static unsigned long ulLastMilli = 0;

	HandleOTAServer();
  HandleSoftAPClient();       //Listen for HTTP requests from clients

  if ((millis() - ulLastMilli) > wLoopMilliDelay) {
    ulLastMilli = millis();
    Serial << LOG0 << "loop(): Corrected: " << szFormatDateString() << ", "
    		<< szFormatTimeString() << endl;
  }	//if((millis()-ulLastMilli)>sLoopMilliDelay)
  return;
}	//loop
//Last line.
