const char szSketchName[]  = "BeckE8266_AccessPoint.ino";
const char szFileDate[]    = "Lenny 1/16/19g";

#include <BeckMiniLib.h>
#include <BeckWiFiLib.h>
#include <BeckE8266OTALib.h>
#include <BeckE8266AccessPointLib.h>
#include <BeckE8266NTPLib.h>

static const char     szRouterName[]        = "Aspot24";
static const char     szRouterPW[]          = "Qazqaz11";
static const char     acHostname[]          = "BeckThermoDev";

//char                  _szDNSName[]          = "beckdev1";
unsigned long         _ulNextHandlerMsec    = 0;
unsigned long         _ulHandlerSpacing     = 10 * 1000;
int                   _wCount               = 1;

void setup(void){
  Serial.begin(115200);
  delay(10);
  Serial << endl << LOG0 <<"setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;
  //pinMode(_wLedPin, OUTPUT);
  SetupWiFi(szRouterName, szRouterPW);
  SetupOTAServer(acHostname);
  SetupAccessPoint();
  SetupWebServer(_oAccessPtIPAddress);
  SetupNTP();
  //SetupmDNS(_oStationIPAddress, _szDNSName);
  return;
} //setup


void loop(void){
  HandleOTAServer();
  HandleSoftAPClient();       //Listen for HTTP requests from clients

  if (millis() >= _ulNextHandlerMsec){
    _ulNextHandlerMsec= millis() + _ulHandlerSpacing;
    Serial << LOG0 << "loop(): Corrected: " << szFormatDateString() << ", "
        << szFormatTimeString() << endl;
  } //f (millis()>=_ulNextHandlerMsec)
  return;
} //loop
//Last line.
