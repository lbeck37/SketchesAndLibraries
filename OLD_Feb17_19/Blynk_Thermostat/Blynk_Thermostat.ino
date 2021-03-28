static const char stSketchName[]  = "Blynk_Thermostat.ino";
static const char stFileDate[]    = "Dec 9, 2015";
// 12/09/15 Copy from Blynk_LightTimer.ino
#include <Streaming.h>
#define BLYNK_PRINT Serial    // Comment out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
//Get Blynk Auth Token from the Blynk App, go to the Project Settings (nut icon).
//char acBlynkAuthToken[] = "55bce1afbf894b3bb67b7ea34f29d45a";   //LEDonD4 Blynk token
//char acBlynkAuthToken[] = "0192b13c767c49e6a7fc8f3bbc5c8f7f";   //Fireplace Blynk token
char acBlynkAuthToken[] = "37a58cc7a39045a59bca1fb1281880a2";     //Light Timer Blynk token

float     fDegF;
#define LOG0      lLineCount++ << " " << millis()
//Define Virtual and Analog pin names
#define DegF_VPin1          V1
#define DegF_VPin2          V2
#define DegF_VPin3          V3
#define RelayState_VPin5    V5
#define RelayState_VPin6    V6
#define Relay_VPin10        V10   //Relay 0
#define Relay_VPin11        V11   //Relay 0
#define Relay_VPin12        V12   //Relay 0
#define Relay_VPin20        V20   //Relay 1
#define Relay_VPin21        V21   //Relay 1
#define Relay_VPin22        V22   //Relay 1

static const bool   bRelaysInverted       = true;
static const int    sRelayOpen            = HIGH;
static const int    sRelayClosed          = LOW;
static const int    sNumRelays            = 2;
static const int    sRelayPin[]           = {4, 5};

static int          sRelayState[2];
static long         lLineCount        = 0;      //Serial Monitor uses for clarity. sRelayOpen
static long         lNumLoops         = 1;

void setup()
{
  Serial.begin(9600);
  Serial << endl << LOG0 << " setup(): Initialized serial to 9600 baud" << endl;
  Serial << LOG0 << " setup(): Sketch: " << stSketchName << ", " << stFileDate << endl;
  Wire.begin();

  Serial << LOG0 << " setup(): Call Blynk.begin(acBlynkAuthToken, dlinky, Qazqaz11)" << endl;
  Blynk.begin(acBlynkAuthToken, "dlinky", "Qazqaz11");
  Serial << LOG0 << " setup(): Back from Blynk.begin()" << endl;

  Serial << LOG0 << " setup(): sRelayPin[0]= " << sRelayPin[0] << ", sRelayPin[1]= " << sRelayPin[1] << endl;
  sSetupRelays();
  return;
} //setup


void loop()
{
  Blynk.run();
} //loop


int sSetupRelays(){
  Serial << LOG0 << " sSetupRelays: Call pinMode() & sSetRelay() to enable OUTPUT on both pins" << endl;
  for (int sRelay= 0; sRelay < sNumRelays; sRelay++){
    pinMode(sRelayPin[sRelay], OUTPUT);
    sSetRelay(sRelay, sRelayOpen);
  } //for
  return 1;
} //sSetupRelays


int sSetRelay(int sRelay, int sRelaySetting){
  //Serial << LOG0 << " sSetRelay: sRelay= " << sRelay << endl;
  Serial << LOG0 << " sSetRelay: Call digitalWrite to set relay on Pin " << sRelayPin[sRelay] << " to " << sRelaySetting << endl;
  digitalWrite(sRelayPin[sRelay], sRelaySetting);
  sRelayState[sRelay]= sRelaySetting;
  return 1;
} //sSetRelay


//Handler callback function called when Button set as a Switch is pressed.
//Light around button is lit when 1 is passed as parameter, unlit when 0 is passed.
//Opto-isolated relay is inverse logic, pulling input pin low cn relay.
//Relay #0 is connected to Blynk virtual pins 10, 11, 12
//Relay #1 is connected to Blynk virtual pins 20, 21, 22
BLYNK_WRITE(Relay_VPin10){
  int sBlynkButtonSetting= param.asInt();
  int sRelaySetting;
  Serial << LOG0 << " BLYNK_WRITE(Relay_VPin10): Received Parameter= " << sBlynkButtonSetting << endl;
  Serial << LOG0 << " ******* Blynk Button for Relay #0 ******" << endl;
  if (sBlynkButtonSetting == 1){
    sRelaySetting= sRelayClosed;
  }
  else{
    sRelaySetting= sRelayOpen;
  }
  sSetRelay(0, sRelaySetting);    //Set relay #0
  return;
} //BLYNK_WRITE(Relay_VPin10)


BLYNK_WRITE(Relay_VPin11){
  int sBlynkButtonSetting= param.asInt();
  int sRelaySetting;
  Serial << LOG0 << " BLYNK_WRITE(Relay_VPin11): Received Parameter= " << sBlynkButtonSetting << endl;
  Serial << LOG0 << " ******* Blynk Timer for Relay #0 ******" << endl;
  if (sBlynkButtonSetting == 1){
    sRelaySetting= sRelayClosed;
  }
  else{
    sRelaySetting= sRelayOpen;
  }
  sSetRelay(0, sRelaySetting);    //Set relay #0
  return;
} //BLYNK_WRITE(Relay_VPin11)


BLYNK_WRITE(Relay_VPin12){
  int sBlynkButtonSetting= param.asInt();
  int sRelaySetting;
  Serial << LOG0 << " BLYNK_WRITE(Relay_VPin12): Received Parameter= " << sBlynkButtonSetting << endl;
  if (sBlynkButtonSetting == 1){
    sRelaySetting= sRelayClosed;
  }
  else{
    sRelaySetting= sRelayOpen;
  }
  sSetRelay(0, sRelaySetting);    //Set relay #0
  return;
} //BLYNK_WRITE(Relay_VPin12)


BLYNK_WRITE(Relay_VPin20){
  int sBlynkButtonSetting= param.asInt();
  int sRelaySetting;
  Serial << LOG0 << " BLYNK_WRITE(Relay_VPin20): Received Parameter= " << sBlynkButtonSetting << endl;
  Serial << LOG0 << " ******* Blynk Button for Relay #1 ******" << endl;
  if (sBlynkButtonSetting == 1){
    sRelaySetting= sRelayClosed;
  }
  else{
    sRelaySetting= sRelayOpen;
  }
  sSetRelay(1, sRelaySetting);    //Set relay #1
  return;
} //BLYNK_WRITE(Relay_VPin20)


BLYNK_WRITE(Relay_VPin21){
  int sBlynkButtonSetting= param.asInt();
  int sRelaySetting;
  Serial << LOG0 << " BLYNK_WRITE(Relay_VPin21): Received Parameter= " << sBlynkButtonSetting << endl;
  Serial << LOG0 << " ******* Blynk Timer for Relay #1 ******" << endl;
  if (sBlynkButtonSetting == 1){
    sRelaySetting= sRelayClosed;
  }
  else{
    sRelaySetting= sRelayOpen;
  }
  sSetRelay(1, sRelaySetting);    //Set relay #1
  return;
} //BLYNK_WRITE(Relay_VPin21)


BLYNK_WRITE(Relay_VPin22){
  int sBlynkButtonSetting= param.asInt();
  int sRelaySetting;
  Serial << LOG0 << " BLYNK_WRITE(Relay_VPin22): Received Parameter= " << sBlynkButtonSetting << endl;
  if (sBlynkButtonSetting == 1){
    sRelaySetting= sRelayClosed;
  }
  else{
    sRelaySetting= sRelayOpen;
  }
  sSetRelay(1, sRelaySetting);    //Set relay #1
  return;
} //BLYNK_WRITE(Relay_VPin22)


BLYNK_READ(RelayState_VPin5){
  Blynk.virtualWrite(RelayState_VPin5, !sRelayState[0]);
} //BLYNK_READ(RelayState_VPin5)


BLYNK_READ(RelayState_VPin6){
  Blynk.virtualWrite(RelayState_VPin6, !sRelayState[1]);
} //BLYNK_READ(RelayState_VPin6)
//Last line.
