// Sketch to use relays 1 and 2 in parallel to power FloJet on and off
// 7/17B Remove SD code and simplify on Ace
// 7/16 Move development from Sammy XP to Ace W7 because Sammy was crashing Arduno IDE.
// 7/14/15 Remove check on motor voltage and add check on pressure switch.Ifdef SD card out.
#include <Arduino.h>
#include <Streaming.h>

#define LOG0 lLineCount++ << " " << millis()

static const int    asRelay[]       = {0, 7, 6, 5, 4};  //Relay can be 1 to 4,no zero relay
static const int    sPressurePin    = 3;
static const int    sFirstRelay     = 1;
static const int    sLastRelay      = 2;
static const int    sPumpOnSecs     = 360;
static const int    sPumpOffSecs    = 600;
static const long   lMsec           = 1000;
static const long   lPumpOnMillis   = sPumpOnSecs  * lMsec;
static const long   lPumpOffMillis  = sPumpOffSecs * lMsec;

static long       lLineCount      = 0;      //Serial Monitor uses for clarity.
static int        sLastToggleSecsLeft;
static long       lNextToggleMsec;          //Next time to toggle pump relay.
static long       lCurrentMsec;

boolean           bPumpIsOn;                //Indicates current state of relays.
boolean           bPumpLoopRunning;         //loop() checks this
boolean           bPumpJustToggled;         //For logging.

/*
When board powers up pump is tuned off and pump loop is not running.
Sketch uses Serial Monitor on a PC for output and input.
Three keys may be pushed: R(un), S(top), T(oggle)
Pressing R starts the pump, sets a time for toggling it off and starts the pump loop running.
Pressing S stops the pump and turns off the pump loop.
Pressing T toggles the pump on or off. If the pump loop is running it sets the toggle time
and keeps the loop running. It does not start the loop if it's not on.
*/

void setup()  {
  Serial.begin(9600);
  sWaitForSerialMonitor();
  Serial << LOG0 << " setup(): Begin" << endl;
  sSetupPressureSwitch();
  sSetupPumpRelays();
  sClearPumpLoop();
  return;
} //setup


int sWaitForSerialMonitor(){
  //The following (for Leonardo only) waits for SerialMonitor to start on PC.
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  return 1;
}  //sWaitForSerialMonitor


void loop()  {
	if (bPumpIsOn && bPumpIsDry()) {
		sTogglePump();
	}
	if (bPumpLoopRunning && bTimeToTogglePump()) {
    sTogglePump();
  } //if(bPumpLoopRunning&&...

  sPrintStatus();
  sCheckKeyboard();
  return;
} //loop


int sClearPumpLoop(){
  lNextToggleMsec   = 0;
  bPumpIsOn         = false;
  bPumpLoopRunning  = false;
  bPumpJustToggled  = true;
  return 1;
}  //sClearPumpLoop


int sStartPumpLoop(){
	sClearPumpLoop();
	bPumpLoopRunning= true;
	sTurnPumpOn(true);
	return 1;
}  //sStartPumpLoop


int sStopPumpLoop(){
	bPumpLoopRunning= false;
	sTurnPumpOn(false);
	return 1;
}  //sStopPumpLoop


int sSetupPressureSwitch(){
  pinMode(sPressurePin, INPUT);
  //Connect internal pull-up reistor.
  digitalWrite(sPressurePin, HIGH);
  return 1;
}  //sSetupPressureSwitch


boolean bPumpIsDry(){
  boolean bReturn= false;
  long lNowMsec= millis();
  int sSwitch= digitalRead(sPressurePin);
  if (sSwitch == LOW) {
    Serial << LOG0 <<" bPumpIsDry(): Pressure is low"<< endl;
		bReturn= true;
  } //if(sSwitch==LOW)
  return bReturn;
}  //bPumpIsDry


int sCheckKeyboard(){
  if (Serial.available()) {
    char cChar= Serial.read();
    int sChar= cChar;
    if (sChar != 13) {
			Serial << LOG0 <<" sCheckKeyboard(): Character read= "<< cChar <<", "<< sChar << endl;
		}
		else {
			Serial << LOG0 <<" sCheckKeyboard(): Character read= CR" << endl;
		}
    switch (cChar) {
      case 'r':
      case 'R':
        sStartPumpLoop();
        break;
      case 's':
      case 'S':
        sStopPumpLoop();
        break;
      case 't':
      case 'T':
        sTogglePump();
        break;
      default:
        break;
    } //switch
  } //if(Serial.available()
  return 1;
}  //sCheckKeyboard


boolean bTimeToTogglePump(){
  static int  sLastToggleSecsLeft = 0;
  boolean     bTogglePump;
  //lCurrentMsec= millis();
  if (lCurrentMsec > lNextToggleMsec) {
    Serial << LOG0 << " bTimeToTogglePump(): lNextToggleMsec= "
           << lNextToggleMsec << endl;
    Serial << LOG0 <<" bTimeToTogglePump(): Setting bTogglePump to TRUE"<< endl;
    bTogglePump= true;
  } //if(millis()>lNextToggleMsec)
  else {
    bTogglePump= false;
  } //else
  return bTogglePump;
}  //bTimeToTogglePump


int sPrintStatus(){
  //Print once a second.
  static long lLastPrintedMsec= 0;
  lCurrentMsec= millis();
  if ( ((lCurrentMsec - lLastPrintedMsec)/1000) > 0) {
    lLastPrintedMsec= lCurrentMsec;
    if (bPumpIsOn) {
      Serial << LOG0 << " sPrintStatus(): Pump is ON, ";
    }
    else {
      Serial << LOG0 << " sPrintStatus(): Pump is OFF, ";
    }
    if (bPumpLoopRunning) {
      int lSecToToggle= (lNextToggleMsec-lCurrentMsec)/1000;
      Serial <<"Seconds until pump toggle= "<< (lSecToToggle / 60) <<":"<< (lSecToToggle % 60) << endl;
    } //if(bPumpLoopRunning)
    else {
      int lSecSinceStart= lCurrentMsec/1000;
      Serial <<"Seconds since start= "<< (lSecSinceStart / 60) <<":"<< (lSecSinceStart % 60) << endl;
    } //if(bPumpLoopRunning)else
  } //if(((lCurrentMsec...
  return 1;
}  //sPrintStatus


int sTogglePump(){
  if (bPumpIsOn) {
    sTurnPumpOn(false);
  }
  else {
    sTurnPumpOn(true);
  } //if(bPumpIsOn)else
  Serial << LOG0 << " sTogglePump(): Pump toggled, set next done to "
         << lNextToggleMsec << endl;
  bPumpJustToggled= true;
  return 1;
}  //sTogglePump


int sTurnPumpOn(boolean bOn){
  int sDigitalPin;
  int sValue;
  if (bOn) {
    bPumpIsOn= true;
    sValue= HIGH;
    //Serial << LOG0 <<" sTurnPumpOn(): DEBUG ONLY setting sValue= LOW" << endl;
    //sValue= LOW;
  	lNextToggleMsec= millis() + lPumpOnMillis;
    Serial << LOG0 <<" sTurnPumpOn(): Turning pump ON" << endl;
  }
  else {
    bPumpIsOn= false;
    sValue= LOW;
  	lNextToggleMsec= millis() + lPumpOffMillis;
    Serial << LOG0 <<" sTurnPumpOn(): Turning pump OFF" << endl;
  }
  for (int sRelay= sFirstRelay; sRelay <= sLastRelay; sRelay++) {
    sDigitalPin= asRelay[sRelay];
    Serial << LOG0 <<" sTurnPumpOn(): Set pin "
           << sDigitalPin << " to " << sValue << endl;
    digitalWrite(sDigitalPin, sValue);    // NO3 and COM3 Connected
  } //for
  //Give pressure time to come up.
  if (bPumpIsOn) {
		delay(500);
	}	//if(bPumpIsOn())
  return 1;
}  //sTurnPumpOn


int sSetupPumpRelays(){
  Serial << LOG0 <<" sSetupPumpRelays(): Begin"<< endl;
  for (int sRelay= sFirstRelay; sRelay <= sLastRelay; sRelay++) {
    int sRelayDigitalPin= asRelay[sRelay];
    Serial << LOG0 <<" sSetupPumpRelays(): Set relay #" << sRelay
           << " to pin " << sRelayDigitalPin << endl;
    pinMode(sRelayDigitalPin, OUTPUT);
  } //for
  return 1;
}  //sSetupPumpRelays
// Last line.
