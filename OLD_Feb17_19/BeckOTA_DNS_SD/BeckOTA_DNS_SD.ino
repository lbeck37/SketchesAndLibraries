static const char szSketchName[]  = "BeckOTA_DNS_SD.ino";
static const char szFileDate[]    = "Dec 29, 2015";
// 12/27/15 Beck from DNS_SD_Arduino_OTA.ino example

#include <Streaming.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#define LOG0    szLogLineHeader(++lLineCount)

const char* host = "esp8266-ota";
const char* ssid = "C1200spot";
const char* pass = "Qazqaz11";
const uint16_t aport = 8266;

WiFiServer  oTelnetServer(aport);
WiFiClient  oTelnetClient;
WiFiUDP     oOTA_WiFiUDP;

static const long   lSerialMonitorBaud    = 115200;
static const long   lMsecPerDay           = 86400000;
static const long   lMsecPerHour          =  3600000;
static const long   lMsecPerMin           =    60000;
static const long   lMsecPerSec           =     1000;
static long         lLineCount            = 0;      //Serial Monitor uses for clarity.


void setup(){
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Arduino oOTA_WiFiUDP Test");

  Serial.printf("Sketch sSize: %u\n", ESP.getSketchSize());
  Serial.printf("Free sSize: %u\n", ESP.getFreeSketchSpace());

  Serial << endl << LOG0 << " setup(): Initialized serial to " << lSerialMonitorBaud << " baud" << endl;
  Serial << LOG0 << " setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;

  WiFi.begin(ssid, pass);
  if(WiFi.waitForConnectResult() == WL_CONNECTED){
    MDNS.begin(host);
    MDNS.addService("arduino", "tcp", aport);
    oOTA_WiFiUDP.begin(aport);
    oTelnetServer.begin();
    oTelnetServer.setNoDelay(true);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } //if(WiFi.waitForConnectResult()==WL_CONNECTED)
} //setup


void loop() {
  //oOTA_WiFiUDP Sketch
  if (oOTA_WiFiUDP.parsePacket()){
    IPAddress oRemoteIPAddress = oOTA_WiFiUDP.remoteIP();
    int sCmd  = oOTA_WiFiUDP.parseInt();
    int sPort = oOTA_WiFiUDP.parseInt();
    int sSize   = oOTA_WiFiUDP.parseInt();

    Serial.print("Update Start: ip:");
    Serial.print(oRemoteIPAddress);
    Serial.printf(", sPort:%d, sSize:%d\n", sPort, sSize);
    uint32_t usStartTime = millis();

    WiFiUDP::stopAll();

    if(!Update.begin(sSize)){
      Serial.println("Update Begin Error");
      return;
    } //if(!Update.begin(sSize))

    WiFiClient oWiFiClient;
    if (oWiFiClient.connect(oRemoteIPAddress, sPort)){
      uint32_t sWritten;
      while(!Update.isFinished()){
        sWritten= Update.write(oWiFiClient);
        if(sWritten > 0){
          oWiFiClient.print(sWritten, DEC);
        } //if(sWritten > 0)
      } //while
      Serial.setDebugOutput(false);

      if(Update.end()){
        oWiFiClient.println("OK");
        Serial.printf("Update Success: %u\nRebooting...\n", millis() - usStartTime);
        ESP.restart();
      }
      else {
        Update.printError(oWiFiClient);
        Update.printError(Serial);
      }
    } //if(oWiFiClient.connect(oRemoteIPAddress,sPort))
    else{
      Serial.printf("Connect Failed: %u\n", millis() - usStartTime);
    } //if(oWiFiClient.connect(oRemoteIPAddress,sPort))else
  } //if(oOTA_WiFiUDP.parsePacket())

  //IDE Monitor (connected to Serial)
  if (oTelnetServer.hasClient()){
    if (!oTelnetClient || !oTelnetClient.connected()){
      if(oTelnetClient){
        oTelnetClient.stop();
      } //if(oTelnetClient)
      oTelnetClient= oTelnetServer.available();
    } //if(!oTelnetClient||!oTelnetClient.connected())
    else{
      WiFiClient toKill= oTelnetServer.available();
      toKill.stop();
    } //if(!oTelnetClient||!oTelnetClient.connected())else
  } //if(oTelnetServer.hasClient())

  if (oTelnetClient && oTelnetClient.connected() && oTelnetClient.available()){
    while(oTelnetClient.available())
      Serial.write(oTelnetClient.read());
  } //if(oTelnetClient&&oTelnetClient.connected()&&oTelnetClient.available())

  if (Serial.available()){
    size_t    sLength    = Serial.available();
    uint8_t   *pucBuffer = (uint8_t *)malloc(sLength);
    Serial.readBytes(pucBuffer, sLength);
    if (oTelnetClient && oTelnetClient.connected()){
      oTelnetClient.write((uint8_t *)pucBuffer, sLength);
      yield();
    } //if(oTelnetClient&&oTelnetClient.connected())
    free(pucBuffer);
  } //if(Serial.available())
  delay(1);
} //loop


String szLogLineHeader(long lLineCount){
  String szHeader= "";
  szHeader += lLineCount;
  szHeader += " ";
  //szTermString += szTime;
  szHeader += szGetTime(millis());
  //szHeader += " ";
  return szHeader;
} //szLogLineHeader


String szGetTime(long lMsec){
  String  szString;

  int sDays    =    lMsec                                               / lMsecPerDay ;
  int sHours   =   (lMsec % lMsecPerDay)                                / lMsecPerHour;
  int sMinutes =  ((lMsec % lMsecPerDay) % lMsecPerHour)                / lMsecPerMin ;
  int sSeconds = (((lMsec % lMsecPerDay) % lMsecPerHour) % lMsecPerMin) / lMsecPerSec;
  int sMsec    =    lMsec % lMsecPerSec;
  szString = String(sDays) + ":";
  szString+= String(szAddZeros(sHours, 2)) + ":";
  szString+= String(szAddZeros(sMinutes, 2)) + ":";
  szString+= String(szAddZeros(sSeconds, 2)) + ".";
  szString+= String(szAddZeros(sMsec, 3)) + " ";     //Send with trailing blank to seperate from next field.
  return szString;
} //szGetTime


//szAddLeadingZeros() adds 1 or 2 zeros (depending on sNumDigits being 3 or not).
String szAddZeros(int sValue, int sNumDigits){
  String szReturn;
  if ((sNumDigits == 3) && (sValue < 100)){
    szReturn= "0";
  } //if((sNumDigits==3)&&(sValue<100)
  if (sValue < 10){
    szReturn += "0";
  } //if(lValue<10)
  szReturn += String(sValue);
  return szReturn;
} //szAddZeros
//Last line.