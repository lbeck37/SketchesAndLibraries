const char szSketchName[]  = "BeckE8266_GuideWebServerPOST.ino";
const char szFileDate[]    = "Lenny 1/12/19h";

#include <BeckE8266WiFiLib.h>
#include <Streaming.h>

static const char     szRouterName[]        = "Aspot24";
static const char     szRouterPW[]          = "Qazqaz11";
static const int      _wLedPin              = 2;
char                  _szDNSName[]          = "beckdev1";

void setup(void){
  Serial.begin(115200);
  delay(10);
  Serial << endl << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;
  pinMode(_wLedPin, OUTPUT);
  bSetupWiFi(szRouterName, szRouterPW);
  SetupAccessPoint();
  SetupWebServer(_oAccessPtIPAddress);
  SetupmDNS(_oStationIPAddress, _szDNSName);
  return;
} //setup


void loop(void){
  HandleSoftAPClient();       //Listen for HTTP requests from clients
  return;
} //loop

//Last line.
