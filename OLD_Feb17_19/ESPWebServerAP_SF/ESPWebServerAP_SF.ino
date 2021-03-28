// 08/08/15 Port from Spark Fun AP Web Server example sketch
//#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <Streaming.h>
#include <ESP8266WiFi.h>                //Note "extern ESP8266WiFiClass WiFi;" reference at end

#define LOG0      lLineCount++ << " " << millis()
static long       lLineCount  = 0;      //Serial Monitor uses for clarity.

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "sparkfun";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN       = 5;        // Thing'szResponse onboard, green LED pulled low though 200 ohm
const int ANALOG_PIN    = A0;       // The only analog pin on the Thing
const int DIGITAL_PIN   = 12;       // Digital pin to be read

WiFiServer server(80);


void setup()
{
  Serial.begin(115200);
  Serial << LOG0 <<" setup(): Begin"<< endl;
  initHardware();
  setupWiFi();
  server.begin();
  return;
} //setup


void loop()
{
  // Check if a client has connected
  WiFiClient client= server.available();
  if (!client) {
    return;
  } //if(!client)

  Serial << LOG0 <<" loop(): server.available() returned a Client" << endl;
  // Read the first line of the request
  String szRequest = client.readStringUntil('\r');
  //Serial.println(szRequest);
  Serial << LOG0 <<" loop(): Request = |"<< szRequest <<"|"<< endl;
  client.flush();

  // Match the request
  int sOperation= -1; // We'll use 'sOperation' to keep track of both the
                // request type (read/set) and value if set.
  if (szRequest.indexOf("/led/0") != -1)
    sOperation= 0; // Will write LED low
  else if (szRequest.indexOf("/led/1") != -1)
    sOperation= 1; // Will write LED high
  else if (szRequest.indexOf("/read") != -1)
    sOperation= -2; // Will print pin reads, it'szResponse negative so digitalWrite(LED_PIN, -2) does nothing
  // Otherwise request will be invalid. We'll say as much in HTML

  // Set LED according to the request
  if (sOperation >= 0)
    digitalWrite(LED_PIN, sOperation);

  client.flush();

  // Prepare the response. Start with the common header:
  String szResponse = "HTTP/1.1 200 OK\r\n";
  szResponse += "Content-Type: text/html\r\n\r\n";
  szResponse += "<!DOCTYPE HTML>\r\n<html>\r\n";
  // If we're setting the LED, print out a message saying we did
  if (sOperation >= 0)
  {
    szResponse += "LED is now ";
    szResponse += (sOperation)?"on":"off";
  }
  else if (sOperation == -2)
  { // If we're reading pins, print out those values:
    szResponse += "Analog Pin = ";
    szResponse += String(analogRead(ANALOG_PIN));
    szResponse += "<br>"; // Go to the next line.
    szResponse += "Digital Pin 12 = ";
    szResponse += String(digitalRead(DIGITAL_PIN));
  }
  else
  {
    szResponse += "Invalid Request.<br> Try /led/1, /led/0, or /read.";
  }
  szResponse += "</html>\n";

  // Send the response to the client
  Serial << LOG0 <<" loop(): Send Response= |"<< szResponse <<"|"<< endl;
  client.print(szResponse);
  //delay(1);
  //Serial.println("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
  return;
} //loop


void setupWiFi()
{
  Serial << LOG0 << " setupWiFi(): Call WiFi.mode(WIFI_AP)" << endl;
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  /*String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Thing " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, AP_NameString.length() + 1, 0);

  for (int i=0; i < AP_NameString.length(); i++) {
     AP_NameChar[i]= AP_NameString.charAt(i);
   } //for
   AP_NameString.toCharArray(AP_NameChar, AP_NameString.length() + 1);
   WiFi.softAP(AP_NameChar, WiFiAPPSK);*/

   String NameString    = "Flojet";
   int  sNameBufferLen= NameString.length() + 1;
   //char acNameBuffer[NameString.length() + 1];
   char acNameBuffer[sNameBufferLen];
   char *pcNoPassword   = NULL;
   int  sChannel        = 2;
   //NameString(acNameBuffer, NameString.length() + 1);
   NameString.toCharArray(acNameBuffer, sNameBufferLen);

   Serial << LOG0 <<" setupWiFi(): Call WiFi.softAP("<< acNameBuffer <<", NULL, "<< sChannel <<")" << endl;
   WiFi.softAP(acNameBuffer, pcNoPassword, sChannel);

   return;
} //setupWiFi


void initHardware()
{
  Serial << LOG0 << " initHardware(): Begin" << endl;
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  // Don't need to set ANALOG_PIN as input,
  // that'szResponse all it can be.
} //initHardware
//Last line.