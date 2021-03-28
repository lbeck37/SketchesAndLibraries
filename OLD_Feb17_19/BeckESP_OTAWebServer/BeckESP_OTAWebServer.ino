const char szSketchName[]  = "BeckESP_OTAWebServer.ino";
const char szFileDate[]    = "Lenny 1/25/19d";

#include <BeckMiniLib.h>
#include <BeckESP_OTAWebServerLib.h>
#include <Streaming.h>

static const char     szRouterName[]          = "Aspot24";
static const char     szRouterPW[]            = "Qazqaz11";
static const char     szOTAServerHostName[]   = "BeckOTA_012319a";

void setup(){
  Serial.begin(115200);
  //delay(100);
  //Serial.printf("\nBeckE32_OTAWebServer.ino 1/23/19b\nsetup(): Begin\n");
  Serial << endl << LOG0 << "setup(): Initialized serial to " << lSerialMonitorBaud << " baud" << endl;
  Serial << LOG0 << "setup(): Sketch: " << szSketchName << "/" << ", " << szFileDate << endl;

  Serial << LOG0 << ("setup(): Call SetupOTAServer()") << endl;
  SetupWiFi(szRouterName, szRouterPW);

  Serial << LOG0 << "setup(): Call SetupOTAServer()" << endl;
  SetupOTAServer(szOTAServerHostName);
  Serial << LOG0 << "setup(): SetupOTAServer() returned" << endl;
  return;
} //setup


void loop()
{
  HandleOTAServer();
  return;
} //loop
//Last line.
