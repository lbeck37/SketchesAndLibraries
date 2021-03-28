const char szSketchName[]  = "BeckE8266_TimezoneExample.ino";
const char szFileDate[]    = "Lenny 12/08/18m";
/**
   Arduino ESP8266 UDP NTP Client
   v. 1.1
   Copyright (C) 2017 Robert Ulbricht
   http://www.arduinoslovakia.eu
   Get the time from a Network Time Protocol (NTP) time server.
   Convert time to few localtimes.

   Libraries:
   TimeLib: https://github.com/PaulStoffregen/Time
   Version: 1.5 or higher
   Timezone: https://github.com/JChristensen/Timezone
   Version: 1.0 or higher
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <Timezone.h>

// Safe stored password
// http://www.arduinoslovakia.eu/blog/2017/6/vlozenie-definicie-makra-do-programu-v-arduine?lang=en
#if defined(_SSID)
const char* ssid = _SSID;
const char* pass = _PWD;
#else
char ssid[] = "Aspot24";  //  your network SSID (name)
char pass[] = "Qazqaz11";       // your network password
#endif

//Unix time starts on Jan 1 1970. In seconds, that's 2,208,988,800
const time_t lSeventyYears = 2208988800UL;

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

//Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
TimeChangeRule CET  = {"CET" , Last, Sun, Oct, 3,  60};     //Central European Standard Time
Timezone CE(CEST, CET);

//Australia Eastern Time Zone (Sydney, Melbourne)
TimeChangeRule aEDT = {"AEDT", First, Sun, Oct, 2, 660};    //UTC + 11 hours
TimeChangeRule aEST = {"AEST", First, Sun, Apr, 3, 600};    //UTC + 10 hours
Timezone ausET(aEDT, aEST);

//US Eastern Time Zone (New York, Detroit)
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  //Eastern Daylight Time = UTC - 4 hours
TimeChangeRule usEST = {"EST", First , Sun, Nov, 2, -300};   //Eastern Standard Time = UTC - 5 hours
Timezone usET(usEDT, usEST);

//US Mountain Time Zone (Boise)
TimeChangeRule oMDT_Rule = {"MDT", Second, Sun, Mar, 2, -360};  //Eastern Daylight Time = UTC - 6 hours
TimeChangeRule oMST_Rule = {"MST", First , Sun, Nov, 2, -420};   //Eastern Standard Time = UTC - 7 hours
Timezone oMT_Timezone(oMDT_Rule, oMST_Rule);


void setup(){
  Serial.begin(115200);
  delay(500);
  Serial.printf("\nsetup(): Sketch: %s, %s\n", szSketchName, szFileDate);
  //Serial.println();

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
}	//setup


void loop(){
  //get a server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP);

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);

  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
  }
  else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):

    //unsigned long secsSince1900 = highWord << 16 | lowWord;
    time_t lSecsSince1900 = highWord << 16 | lowWord;

    //Subtract seventy years:
    time_t 	lUTC_Secs= (lSecsSince1900 - lSeventyYears);

    TimeChangeRule *pTimeChangeRule;

    printTime(lUTC_Secs, "UTC", "Universal Coordinated Time");
    time_t	lBoiseTime= oMT_Timezone.toLocal (lUTC_Secs, &pTimeChangeRule);
    printTime(lBoiseTime, pTimeChangeRule->abbrev, "Boise");

    printTime(CE.toLocal   (lUTC_Secs, &pTimeChangeRule), pTimeChangeRule->abbrev, "Bratislava");
    printTime(usET.toLocal (lUTC_Secs, &pTimeChangeRule), pTimeChangeRule->abbrev, "New York");
    printTime(ausET.toLocal(lUTC_Secs, &pTimeChangeRule), pTimeChangeRule->abbrev, "Sydney");
    Serial.println("");
  }
  // wait ten seconds before asking for the time again
  delay(10000);
}	//loop


// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address){
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

//Function to print time with time zone
void printTime(time_t t, char *tz, char *loc)
{
  sPrintI00(hour(t));
  sPrintDigits(minute(t));
  sPrintDigits(second(t));
  Serial.print(' ');
  Serial.print(dayShortStr(weekday(t)));
  Serial.print(' ');
  Serial.print(monthShortStr( month(t) ));
  Serial.print(' ');
  sPrintI00(day(t));
  //Serial.print(', ');	//I guess a single quote is not the same as a double (' != ")
  Serial.print(", ");

  Serial.print(year(t));
  Serial.print(' ');
  Serial.print(tz);
  Serial.print(' ');
  Serial.print(loc);
  Serial.println();
}

//Print an integer in "00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintI00(int val)
{
  if (val < 10) Serial.print('0');
  Serial.print(val, DEC);
  return;
}

//Print an integer in ":00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintDigits(int val)
{
  Serial.print(':');
  if (val < 10) Serial.print('0');
  Serial.print(val, DEC);
}
