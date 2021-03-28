// 6/28/15ab From Seeed Relay Wiki
// Sketch to exercise all (4) relays on Seeed Relay shield.

#include <Arduino.h>
#include <Streaming.h>

static const int asRelay[]  = {0, 7, 5, 6, 4};  //Relay can be 1 to 4,no zero relay
static const int sNumRelays= 4;

static int       sLineCount= 0;     //Used in outputs to Serial Monitor for clarity.

//int MotorControl = 5;    // Digital Arduino Pin used to control the motor


void setup()  {
  Serial.begin(9600);
  Serial << sLineCount++ << " setup(): Begin" << endl;

  sSetupRelays();

  return;
} //setup


// the loop routine runs over and over again forever:
void loop()  {
  Serial << sLineCount++ << " loop(): Begin" << endl;
  sCycleRelays();
  return;
} //loop


int sSetupRelays(){
  Serial << sLineCount++ <<" sSetupRelays(): Begin"<< endl;
  for (int sRelay= 1; sRelay <= sNumRelays; sRelay++) {
    int sRelayDigitalPin= asRelay[sRelay];
    Serial << sLineCount++ <<" sSetupRelays(): Set relay #" << sRelay
           << " to pin " << sRelayDigitalPin << endl;
    pinMode(sRelayDigitalPin, OUTPUT);
  } //for
  return 1;
}  //sSetupRelays


int sCycleRelays(){
  int sDigitalPin;

  Serial << sLineCount++ <<" sCycleRelays(): Begin"<< endl;
  for (int sRelay= 1; sRelay <= sNumRelays; sRelay++) {
    sDigitalPin= asRelay[sRelay];
    sCycleOneRelay(sDigitalPin);
  } //for

  return 1;
}  //sCycleRelays


int sCycleOneRelay(int sDigitalPin){
  int sOnMsec   = 250;
  int sOffMsec  = 0;

  Serial << sLineCount++ <<" sCycleOneRelay(): Begin"<< endl;

  Serial << sLineCount++ <<" sCycleOneRelay(): Turn ON pin "<< sDigitalPin << endl;
  digitalWrite(sDigitalPin, HIGH);    // NO3 and COM3 Connected
  delay(sOnMsec);

  Serial << sLineCount++ <<" sCycleOneRelay(): Turn OFF pin "<< sDigitalPin << endl;
  digitalWrite(sDigitalPin, LOW);     // NO3 and COM3 Disconnected
  delay(sOffMsec);
  return 1;
}  //sCycleOneRelay

// Last line.