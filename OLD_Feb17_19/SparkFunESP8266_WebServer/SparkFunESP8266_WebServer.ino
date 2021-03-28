/* SparkFun ESP8266 WebServer
  11/21/15- Get working with RealTerm for monitor
*/
//#include <Arduino.h>
#include <Streaming.h>
#include <ESP8266WiFi.h>

#define LOG0      lLineCount++ << " " << millis()
static long       lLineCount= 0;      //Serial Monitor uses for clarity.
static long       lNumLoops= 1;

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "sparkfun";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 5; // Thing's onboard, green LED
const int ANALOG_PIN = A0; // The only analog pin on the Thing
const int DIGITAL_PIN = 12; // Digital pin to be read

WiFiServer server(80);

void setup()
{
  initHardware();
  //Serial.println("setup(): Calling setupWiFi()");
  Serial << LOG0 << " setup(): Calling setupWiFi()" << endl;
  setupWiFi();
  //Serial.println("setup(): Calling server.begin()");
  Serial << LOG0 << " setup(): Calling server.begin()" << endl;
  server.begin();
} //setup


void initHardware()
{
  //Serial.begin(115200);
  Serial.begin(9600);
  //Serial.println("initHardware(): Initialized serial to 9600 baud");
  Serial << LOG0 << " initHardware(): Initialized serial to 9600 baud" << endl;
  //Serial << "Free Ram= " << freeRam() << endl;
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  // Don't need to set ANALOG_PIN as input,
  // that's all it can be.
} //initHardware


void setupWiFi()
{
  //Serial.println("setupWiFi(): Begin");
  Serial << LOG0 << " setupWiFi(): Begin" << endl;
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Thing " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
} //setupWiFi


void loop()
{
  if ((lNumLoops++ % 500000) == 1) {
    Serial << LOG0 << " loop(): Begin, lNumLoops= " << lNumLoops << endl;
  }
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  //Serial.println(req);
  Serial << LOG0 << "loop(): Received request |" << req << "|" << endl;
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.
  if (req.indexOf("/led/0") != -1)
    val = 0; // Will write LED low
  else if (req.indexOf("/led/1") != -1)
    val = 1; // Will write LED high
  else if (req.indexOf("/read") != -1)
    val = -2; // Will print pin reads
  // Otherwise request will be invalid. We'll say as much in HTML

  // Set GPIO5 according to the request
  if (val >= 0)
    digitalWrite(LED_PIN, val);

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "<BODY>\r\n";
  // If we're setting the LED, print out a message saying we did
  if (val >= 0)
  {
    s += "LED is now ";
    s += (val)?"on":"off";
  }
  else if (val == -2)
  #if 0
  { // If we're reading pins, print out those values:
    s += "Analog Pin = ";
    s += String(analogRead(ANALOG_PIN));
    s += "<br>"; // Go to the next line.
    s += "Digital Pin 12 = ";
    s += String(digitalRead(DIGITAL_PIN));
  }
  #else
  { // If we're reading pins, print out those values:
    s += "<H1>Hello from ESP8266<\H1\n";

    s += "<H1>\n";
    s += "Analog Pin = ";
    s += String(analogRead(ANALOG_PIN));
    s += "</H1>\n";
   //s += "<br>"; // Go to the next line.
    s += "<H1>Seconds since start= " + String(millis() / 1000) + "</H1\n";
  }
  #endif
  else
  {
    s += "Invalid Request.<br> Try /led/1, /led/0, or /read.";
  }
  s += "</BODY>\r\n";
  s += "</html>\n";

  //Serial.println("loop(): Send response to client.");
  Serial << LOG0 << " loop(): Send |" << s << "| to client" << endl;
  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
} //loop
//Last line.