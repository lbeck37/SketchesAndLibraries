const char szSketchName[]  = "BeckE8266_GuideWi_Fi_mDNS.ino";
const char szFileDate[]    = "Lenny 1/6/19g";

#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266mDNS.h>        // Include the mDNS library
//#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi SSID library
#include <Streaming.h>

static const char     szRouterName[]        = "Aspot24";
static const char     szRouterPW[]          = "Qazqaz11";
static const char     szDNSName[]          	= "beckdev1";

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial << endl << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;
  Serial << "setup(): Connecting to " << szRouterName << " using " << szRouterPW << endl;

  WiFi.begin(szRouterName, szRouterPW);             // Connect to the network
  Serial << "setup(): Connecting .";

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(250);
    Serial << ".";
  }	//while(WiFi.status()!=WL_CONNECTED)

  Serial << endl << "setup(): Connected to " << WiFi.SSID() << ", IP address: " << WiFi.localIP() << endl;

  if (!MDNS.begin(szDNSName)) {             // Start the mDNS responder for szDNSName.local
    Serial << "setup(): Error setting up MDNS responder for " << szDNSName << endl;
  }	//if(!MDNS.begin(szDNSName))
  else {
    Serial << "setup(): mDNS responder started for " << szDNSName << endl;
    //MDNS.addService("http", "tcp", 80);
    Serial << "setup(): Access this device using " << WiFi.localIP() << " or " << szDNSName << ".local" << endl;
  }	//if(!MDNS.begin(szDNSName))else

  Serial << "setup(): Done." << endl;
}	//setup

void loop() { }
//Last line.
