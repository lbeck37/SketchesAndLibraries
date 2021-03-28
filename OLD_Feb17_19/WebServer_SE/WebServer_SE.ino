// 08/04/15 Ported to ESP8266
/*--------------------------------------------------------------
  Program:      eth_websrv_page
  References:   - WebServer example by David A. Mellis and
                  modified by Tom Igoe
  Date:         7 January 2013
  Author:       W.A. Smith, http://startingelectronics.org
--------------------------------------------------------------*/
#include <Arduino.h>
#include <Streaming.h>
#include <ESP8266WiFi.h>                //Note "extern ESP8266WiFiClass WiFi;" reference at end

#define LOG0      lLineCount++ << " " << millis()
static long       lLineCount  = 0;      //Serial Monitor uses for clarity.

//const char     *szSSID                 = "P291spot";
const char     *szSSID                 = "Linky24";
//const char     *szSSID                 = "dlinky";

const char     *szPassword             = "Qazqaz11";
const int      sD0LedPin                  = 0;
const int      sLedPin                    = 2;
const int      sSwitchPin              = 4;

//Create an instance of the server and specify the port to listen on as an argument
WiFiServer server(80);

String szHTTP_Request;          // stores the HTTP request
boolean bLedOn = false;   // state of LED, off by default

const static long    lD0LedStageMsec[] = { 1000,  250,   250,  250,   250,  250}; //Msec that LED is on or off
const static boolean  bD0LedStageIsOn[] = {false, true, false, true, false, true};
const static int        sD0LedLastStage   = 6;
static boolean          bD0LedIsOn           = false;
static long             lD0LedNextMsec;          //Next time to change stage of LED
static int              sD0CurrentStage;            //0 to sD0LedLastStage
static long             lCurrentMsec;

void setup()
{
   sSetupWiFi();
   //sSetupD0Led();
  pinMode(sLedPin, OUTPUT);       // LED on pin sLedPin
   return;
}  //setup


int sSetupWiFi() {
  Serial.begin(115200);
  Serial << LOG0 << " sSetupWiFi(): Begin" << endl;
  delay(10);

  //Connect to WiFi network
  Serial << LOG0 << " sSetupWiFi(): Connecting to "<< szSSID << endl;
  WiFi.begin(szSSID, szPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial << endl << LOG0 <<" sSetupWiFi(): WiFi connected"<< endl;

  //Start the server
  server.begin();
  Serial << LOG0 <<" sSetupWiFi(): Server started, my IP is  "<< WiFi.localIP() << endl;
   return 1;
}  //sSetupWiFi


int sSetupSwitch() {
  Serial << LOG0 << " sSetupSwitch(): Begin" << endl;
  Serial << LOG0 << " sSetupSwitch(): Set Switch pin "<< sSwitchPin <<" to INPUT"<< endl;
   pinMode(sSwitchPin, INPUT);

   //Connect internal pull-up reistor.
  Serial << LOG0 << " sSetupSwitch(): internal pullup on Switch pin "<< sSwitchPin << endl;
  digitalWrite(sSwitchPin, HIGH);
   return 1;
}  //sSetupSwitch


int sSetupD0Led() {
  Serial << LOG0 << " sSetupD0Led(): Begin" << endl;
   lCurrentMsec= millis();
   pinMode(sD0LedPin, OUTPUT);       // LED on pin sLedPin
   sD0CurrentStage= 0;
   lD0LedNextMsec= lCurrentMsec + lD0LedStageMsec[sD0CurrentStage];
  Serial << LOG0 << " sSetupD0Led(): Set lD0LedNextMsec to " << lD0LedNextMsec << endl;
  Serial << LOG0 << " sSetupD0Led(): Set call sTurnOnD0Led() with "
         << bD0LedStageIsOn[sD0CurrentStage] << endl;
   sTurnOnD0Led(bD0LedStageIsOn[sD0CurrentStage]);
   return 1;
}  //sSetupD0Led


int sTurnOnD0Led(boolean bOn) {
   if (bOn) {
     digitalWrite(sLedPin, HIGH);
   }
   else {
     digitalWrite(sLedPin, LOW);
   }
   return 1;
}  //sTurnOnD0Led


int sCycleD0Led() {
   lCurrentMsec= millis();
   if (lCurrentMsec > lD0LedNextMsec) {
      if (sD0CurrentStage < sD0LedLastStage) {
         sD0CurrentStage++;
      }
      else {
      Serial << LOG0 <<" sCycleD0Led(): Set D0 LED to stage Zero"<< endl;
         sD0CurrentStage= 0;
      }
      lD0LedNextMsec= lCurrentMsec + lD0LedStageMsec[sD0CurrentStage];
      sTurnOnD0Led(bD0LedStageIsOn[sD0CurrentStage]);
   }
   return 1;
}  //sCycleD0Led


void loop()
{
    WiFiClient client = server.available();  // try to get client
      //sCycleD0Led();
    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        Serial << LOG0 <<" loop(): Got a Client"<< endl;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                szHTTP_Request += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    // send web page
                    client.println("<!DOCTYPE html>");
                    client.println("<html>");
                    client.println("<head>");
                    client.println("<title>Arduino LED Control</title>");
                    client.println("<meta http-equiv=\"refresh\" content=\"1\">");
                    client.println("</head>");
                    client.println("<body>");
                    client.println("<h1>Switch</h1>");
                    client.println("<p>State of switch is:</p>");
                    GetSwitchState(client);

                    client.println("<h1>FloJet LED</h1>");
                    client.println("<p>Click to switch LED on Arduino D2 on and off.</p>");
                    client.println("<form method=\"get\">");
                    ProcessCheckbox(client);
                    client.println("</form>");
                    client.println("</body>");
                    client.println("</html>");
                    //Serial.print(szHTTP_Request);
                    Serial << LOG0 <<"loop(): HTTP Request= |"<< szHTTP_Request <<"|"<< endl;
                    szHTTP_Request = "";    // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                }
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}  //loop


// switch LED and send back HTML for LED checkbox
void ProcessCheckbox(WiFiClient cl)
{
  if (szHTTP_Request.indexOf("LED2=2") > -1) {  // see if checkbox was clicked
    // the checkbox was clicked, toggle the LED
    Serial << LOG0 <<" ProcessCheckbox(): Checkbox was clicked"<< endl;
    if (bLedOn) {
      Serial << LOG0 <<" ProcessCheckbox(): Set bLedOn to FALSE"<< endl;
      bLedOn = false;
    }  //if(bLedOn)
    else {
      Serial << LOG0 <<" ProcessCheckbox(): Set bLedOn to TRUE"<< endl;
      bLedOn = true;
    }  //if(bLedOn)else
  }  //if(szHTTP_Request.indexOf(...

  if (bLedOn) {    // switch LED on
    Serial << LOG0 <<" ProcessCheckbox(): Turn external LED ON"<< endl;
    digitalWrite(sLedPin, HIGH);
    // checkbox is checked
    cl.println("<input type=\"checkbox\" name=\"LED2\" value=\"2\" \
    onclick=\"submit();\" checked>LED2");
  }  //if(bLedOn)
  else {              // switch LED off
    Serial << LOG0 <<" ProcessCheckbox(): Turn external LED OFF"<< endl;
    digitalWrite(sLedPin, LOW);
    // checkbox is unchecked
    cl.println("<input type=\"checkbox\" name=\"LED2\" value=\"2\" \
    onclick=\"submit();\">LED2");
  }  //if(bLedOn)else
  return;
}  //ProcessCheckbox


void GetSwitchState(WiFiClient cl) {
  if (digitalRead(sSwitchPin)) {
    Serial << LOG0 <<" GetSwitchState(): Switch is ON"<< endl;
    cl.println("<p>ON</p>");
  }
  else {
    Serial << LOG0 <<" GetSwitchState(): Switch is OFF"<< endl;
    cl.println("<p>OFF</p>");
  }
  return;
}  //GetSwitchState
//Last line