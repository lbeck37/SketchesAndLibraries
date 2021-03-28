String acSketchName  = "Flojet.ino";
String acFileDate    = "January 27, 2017_HP7_A";
// Sketch to use relays 1 and 2 in parallel to power FloJet on and off
// 7/5/16 Use Maxim/Dallas 1-wire DS18B20 Temp sensor in in 1-wire configuration
// 7/4/16 Add (2) TMP36 temp sensors for motor case and outlet air.
// 8/31/15 Change pressure switch from a normally closed to a normally open switch.
// 8/29/15 Increase delay from 500 to 2000 msec after pump on to let pressure come up
// 7/26 Work on completing with fill valve on pin relay 3.
// 7/25 Changed to having Grey Drain and Black Flush cycles instead of just bPumpLoopRunning.
// 7/17B Remove SD code and simplify on Ace
// 7/16 Move development from Sammy XP to Ace W7 because Sammy was crashing Arduno IDE.
// 7/14/15 Remove check on motor voltage and add check on pressure switch.Ifdef SD card out.
#include <Arduino.h>
#include <Streaming.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//#define ONEWIRE_PIN    2

//#define USING_ESP8266
//#define USING_FLOWMETER

#define LOG0      lLineCount++ << " " << millis()
static long       lLineCount= 0;      //Serial Monitor uses for clarity.

#define MIN_SEC(Sec)    (Sec / 60) << ":" << (Sec % 60)
//#define CYCLE_SEC       Serial << ((sCycleSec()/60)) <<":"<< (sCycleSec() % 60)

//Pin refers to an Arduino GPIO pin number
//Relay can be 1,2,3,4 corresponding to Arduino pins 7,6,5,4 (pin 4 is also used by SD card shield)
#ifdef USING_ESP8266
static const int    sPumpPin		    = 0;
static const int    sOneWirePin     = 2;  //Dallas DS18B20 Temperature Sensor
static const int    sFillValveRelay	= 4;
static const int    sPressSwitchPin	= 5;
#ifdef USING_FLOWMETER
  static const int 	sFlowMeterPin  	= 16;      // Flow Meter Pin number, must support interrupt.
#endif  //USING_FLOWMETER

#else
static const int    sOneWirePin     = 2;  //Dallas DS18B20 Temperature Sensor
static const int    sPressSwitchPin	= 3;
static const int    sFillValveRelay = 3;
#ifdef USING_FLOWMETER
  static const int  sFlowMeterPin   = 2;      // Flow Meter Pin number, must support interrupt.
#endif  //USING_FLOWMETER
#endif


//Leonardo has INT0 on pin 3, Uno is on pin 2.
//Not sure if pinMode() or digitalWrite() needs to be performed as I got Leonardo
//going with pin 2 getting set up but sensor plugged into pin 3 for INT0

static const String szTempSensors[]       = {"Housing", "Exit Air"};
//Relay pin can be 1 to 4,no zero relay, pin 4 not available, conflicts with SD card.
static const int    asShieldRelay[]       = {0, 7, 6, 5, 4};
static const int    sFirstShieldPumpRelay	= 1;
static const int    sLastShieldPumpRelay	= 2;
static const int    sStatusSecs           = 2;
static const int    sTimeoutSecs          = 7 * 60;
static const int    sBlackFlushOnSecs     = 45;
static const int    sFinalFillSecs        = sBlackFlushOnSecs / 2;
static const long   lMsec                 = 1000;
static const long   lStatusMsec           = sStatusSecs  * lMsec;
static const long   lTimeoutMsec          = sTimeoutSecs * lMsec;
static const long   lPumpOnDelayMsec      = 1000;
static const int    sIdleCycle            = 0;
static const int    sGreyDrainCycle       = 1;
static const int    sBlackDrainCycle      = 2;
static const int    sBlackFillCycle       = 3;
static const int    sBlackIsDraining      = 1;
static const int    sBlackIsFilling       = 2;
static const float  fPulsesPerGal         = 1700.0;
static const float  fGpmLowerLimit        = 3.0;
static const boolean  bFlowSwitchIsNO     = true;
//End of const's

//Values changed when debug is on.
static int    sNumBlackFills              = 8;
static long   lBlackFlushOnMsec           = sBlackFlushOnSecs * lMsec;
static long   lFinalFillOnMsec            = sFinalFillSecs * lMsec;

static boolean    bDebug                  = false;
static long       lNextTimeoutMsec;          //Next time to toggle pump relay.
static long       lNextStatusMsec;          //Next time to print status on Serial Monitor.
static long       lStopBlackFillMsec;       //Time to stop filling black tank
static long       lCurrentMsec;
static long       lCycleStartMsec;
static int        sCurrentCycle;            //sIdleCycle, sGreyDrainCycle, sBlackDrainCycle
static int        sBlackDrainState;
static int        sBlackFillCount;

//Maxim/Dallas OneWire sensors
/* Set up a oneWire instance to communicate with any OneWire device*/
OneWire           oOneWire(sOneWirePin);

/* Tell Dallas Temperature Library to use oneWire Library */
DallasTemperature oSensors(&oOneWire);

volatile int  sFlowCount;

boolean           bPumpIsOn;                //Indicates current state of pump relays.

/*
When board powers up pump is tuned off and pump loop is not running.
Sketch uses Serial Monitor on a PC for output and input.
N keys may be pushed: G(rey), B(lack), F(ill), S(top), X(Debug)
G starts the Grey Drain Cycle.
  The pump ia turned on and when the pump runs dry it goes back to the Idle Cycle.
B starts the Black Drain Cycle.
  The pump is turned on and when the pump runs dry the water fill valve is turned on for
  one minute (~5 gal) and then off and the pump is run until again until dry. This is repeated
  N (7?) times.
S stops any cycle and goes into the Idle Cycle.
F starts the Black Fill Cycle which opens the black tank fill valve for 30 seconds to put
  ~2.5 gal back in the black tank. Use this when finished draining black and want to put some back.
D turns on Debug mode which allows testing without pressure switch being connected.
  Debug mode also changes lengths of fills and number.
Note: The Toggle command is not implemented any more.
In the Grey Drain and Black Flush cycles, a time limit for continuous running of the pump is set,
and when that time is exceeded the pump is shut off and we change to the Idle Cycle.
*/

void setup()  {
  Serial.begin(115200);
  sWaitForSerialMonitor();
  Serial << LOG0 << " setup(): Begin" << endl;
  Serial << LOG0 << "Sketch: " << acSketchName << ", " << acFileDate << endl;
  sSetupArduinoPins();
  sStopCycle();
  return;
} //setup


int sWaitForSerialMonitor() {
  //The following (for Leonardo only) waits for SerialMonitor to start on PC.
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  return 1;
}  //sWaitForSerialMonitor


void loop()  {
  lCurrentMsec= millis();
  sCheckForDryPump();
  sCheckForTimeout();
  sCheckCurrentCycle();
  sPrintStatus();
  sCheckKeyboard();
  return;
} //loop


int sSetupArduinoPins() {
  oSensors.begin();   //OneWire setup.
  sSetupPressureSwitch();
  sSetupPumpRelays();
  sSetupBlackFillValve();
#ifdef USING_FLOWMETER
  sSetupFlowMeter();
#endif  //USING_FLOWMETER
  return 1;
}  //sSetupArduinoPins


void vReadTempSensors() {
  float fDegrees[2];
  for (int sSensor= 0; sSensor < 2; sSensor++) {
    fDegrees[sSensor]= fGetDegF(sSensor);
  } //for(int sSensor= 0;...
  Serial << LOG0 << " vReadTempSensors(): " << szTempSensors[0] << "= " << fDegrees[0]
         << ", " << szTempSensors[1] << "= " << fDegrees[1] << endl;
} //vReadTempSensors


float fGetDegF(int sSensor){
  float fDegFReturn;
  oSensors.requestTemperatures(); // Send the command to get temperatures
  fDegFReturn= oSensors.getTempFByIndex(sSensor);
  return fDegFReturn;
}  //fGetDegF


#ifdef USING_FLOWMETER
void vIncrementFlowCount()                  // Interruot function
{
   sFlowCount++;
   return;
} //vIncrementFlowCount


boolean bWaterIsFlowing() {
  boolean bReturn= true;
  sFlowCount= 0;
  sei();                            // Enable interrupts
  delay(1000);                      //Count for one second.
  cli();                            //Turn off interrupts.

  float fGalPerMin= (sFlowCount * 60.0 / fPulsesPerGal);
  Serial << LOG0 << " loop(): fGalPerMin= "<< fGalPerMin << endl;
  if (fGalPerMin < fGpmLowerLimit) {
    bReturn= false;
  }
  return bReturn;
}  //bWaterIsFlowing


int sSetupFlowMeter() {
  pinMode(sFlowMeterPin, INPUT);
  digitalWrite(sFlowMeterPin, HIGH);              //Turn on internal pullup.
  attachInterrupt(0, vIncrementFlowCount, RISING); // Setup Interrupt
  return 1;
}  //sSetupFlowMeter
#endif  //USING_FLOWMETER


int sSetupPressureSwitch() {
  pinMode(sPressSwitchPin, INPUT);
  //Connect internal pull-up reistor.
  digitalWrite(sPressSwitchPin, HIGH);
  return 1;
}  //sSetupPressureSwitch


int sSetupPumpRelays() {
  Serial << LOG0 <<" sSetupPumpRelays(): Begin"<< endl;
#ifndef USING_ESP8266
  for (int sRelay= sFirstShieldPumpRelay; sRelay <= sLastShieldPumpRelay; sRelay++) {
    int sRelayDigitalPin= asShieldRelay[sRelay];
    Serial << LOG0 <<" sSetupPumpRelays(): Set relay #" << sRelay
           << " to pin " << sRelayDigitalPin << endl;
    pinMode(sRelayDigitalPin, OUTPUT);
  } //for
#else
  pinMode(sPumpPin, OUTPUT);
#endif
  return 1;
}  //sSetupPumpRelays


int sSetupBlackFillValve() {
  Serial << LOG0 <<" sSetupBlackFillValve(): Set Black Fill Valve to pin "
         << asShieldRelay[sFillValveRelay] << endl;
  pinMode(asShieldRelay[sFillValveRelay], OUTPUT);
  return 1;
}  //sSetupBlackFillValve


int sCycleSec() {
  int sReturn= (lCurrentMsec - lCycleStartMsec) / lMsec;
  return sReturn;
}  //sCycleSec


int sBlackFillCycleSecLeft() {
  int sReturn= (lStopBlackFillMsec - lCurrentMsec) / lMsec;
  return sReturn;
}  //sBlackFillCycleSecLeft


int sSetCycleStartMsec() {
  lCycleStartMsec= lCurrentMsec;
/*
  Serial << LOG0 <<" sSetCycleStartMsec(): Set  Cycle start to "
         << ((sCycleSec()/60)) <<":"<< (sCycleSec() % 60) << endl;
  Serial << LOG0 <<" sSetCycleStartMsec(): lCurrentMsec= "<< lCurrentMsec
         << ", lCycleStartMsec= " << lCycleStartMsec << endl;
  int sCycleSecReturns= sCycleSec();
  Serial << LOG0 <<" sSetCycleStartMsec(): sCycleSec() returns "<< sCycleSecReturns << endl;
*/
  Serial << LOG0 <<" sSetCycleStartMsec(): lCycleStartMsec= " << lCycleStartMsec << endl;
  return 1;
}  //sSetCycleStartMsec


int sTurnPumpOn(boolean bOn){
  int sValue;
  if (bOn) {
    bPumpIsOn= true;
    sValue= HIGH;
    lNextTimeoutMsec= lCurrentMsec + lTimeoutMsec;
    Serial << LOG0 <<" sTurnPumpOn(): Turning pump ON" << endl;
  }
  else {
    bPumpIsOn= false;
    sValue= LOW;
    Serial << LOG0 <<" sTurnPumpOn(): Turning pump OFF" << endl;
  }
#ifndef USING_ESP8266
  int sDigitalPin;
  for (int sRelay= sFirstShieldPumpRelay; sRelay <= sLastShieldPumpRelay; sRelay++) {
    sDigitalPin= asShieldRelay[sRelay];
    Serial << LOG0 <<" sTurnPumpOn(): Set pin "
           << sDigitalPin << " to " << sValue << endl;
    digitalWrite(sDigitalPin, sValue);    // NO3 and COM3 Connected
  } //for
#else
  digitalWrite(sPumpPin, sValue);
#endif

  //Give pressure time to come up.
  if (bPumpIsOn) {
    delay(lPumpOnDelayMsec);
  } //if(bPumpIsOn())
  return 1;
}  //sTurnPumpOn


int sOpenBlackFillValve(boolean bOn){
  //int sDigitalPin;
  int sValue;
  if (bOn) {
    sValue= HIGH;
    Serial << LOG0 <<" sOpenBlackFillValve(): Open valve" << endl;
  }
  else {
    sValue= LOW;
    Serial << LOG0 <<" sOpenBlackFillValve(): Close valve" << endl;
  }
  Serial << LOG0 <<" sOpenBlackFillValve(): Set pin "<< asShieldRelay[sFillValveRelay] <<" to "<< sValue << endl;
  digitalWrite(asShieldRelay[sFillValveRelay], sValue);
  return 1;
}  //sOpenBlackFillValve


int sCheckForDryPump() {
  if (bPumpIsOn && bPumpIsDry()) {
    sHandleDryPump();
  }
  return 1;
}  //sCheckForDryPump


int sCheckForTimeout() {
  if (bPumpIsOn && bTimeoutReached()) {
    sStopCycle();
  } //if
  return 1;
}  //sCheckForTimeout


int sCheckCurrentCycle() {
  switch (sCurrentCycle) {
    case sGreyDrainCycle:
      break;
    case sBlackDrainCycle:
      if ((sBlackDrainState == sBlackIsFilling) && bTimeToStopFillingBlack()) {
        sSwitchBlackToDraining();
      }
      break;
    case sBlackFillCycle:
#ifdef USING_FLOWMETER
      if (!bWaterIsFlowing()) {
        sStopCycle();
      } //if(!bWaterIsFlowing())
#endif //USING_FLOWMETER
      if (bTimeToStopFillingBlack()) {
        sStopCycle();
      } //if(bTimeToStopFillingBlack())

      break;
    case sIdleCycle:
      break;
    default:
      Serial << LOG0 << "sCheckCurrentCycle(): Bad case in switch()= "<< sCurrentCycle << endl;
      break;
  } //switch
  return 1;
}  //sCheckCurrentCycle


int sStartGreyDrainCycle() {
  //Returns 0 if pressure switch is not in closed position, unless debugging.
  Serial << LOG0 <<" sStartGreyDrainCycle(): Begin"<< endl;

  if (!bDebug && !bPressureSwitchIsOK()) {
    Serial << LOG0 <<" sStartGreyDrainCycle(): Pressure switch failed."<< endl;
    return 0;
  } //if(!bDebug&&!bPressureSwitchIsOK())

  sCurrentCycle= sGreyDrainCycle;
  sSetCycleStartMsec();
  sTurnPumpOn(true);
  return 1;
}  //sStartGreyDrainCycle


int sStartBlackDrainCycle() {
  //Returns 0 if pressure switch is not in closed position, unless debugging.
  Serial << LOG0 <<" sStartBlackDrainCycle(): Begin"<< endl;

  if (!bDebug && !bPressureSwitchIsOK()) {
    Serial << LOG0 <<" sStartBlackDrainCycle(): Pressure switch failed."<< endl;
    return 0;
  } //if(!bDebug&&!bPressureSwitchIsOK())

  sCurrentCycle= sBlackDrainCycle;
  sBlackDrainState= sBlackIsDraining;
  sSetCycleStartMsec();
  sBlackFillCount= 0;
  sTurnPumpOn(true);
  return 1;
}  //sStartBlackDrainCycle


int sSwitchBlackToFilling() {
  Serial << LOG0 <<" sSwitchBlackToFilling(): Begin"<< endl;
  //Returns 0 if number of desired fills was reached.
  int sReturn= 1;
  if (sBlackFillCount++ < sNumBlackFills) {
    Serial << LOG0 <<" sSwitchBlackToFilling(): Perform fill "<< sBlackFillCount
           <<" of "<< sNumBlackFills << endl;
    sBlackDrainState= sBlackIsFilling;
    lStopBlackFillMsec= lCurrentMsec + lBlackFlushOnMsec;
    sOpenBlackFillValve(true);
  } //if(sBlackFillCount++<sNumBlackFills)
  else {
    Serial << LOG0 <<" sSwitchBlackToFilling(): Total Fills reached: "<< sNumBlackFills << endl;
    sReturn= 0;
    } //if(sBlackFillCount++<sNumBlackFills)else
  return sReturn;
}  //sSwitchBlackToFilling


int sSwitchBlackToDraining() {
  Serial << LOG0 <<" sSwitchBlackToDraining(): Begin"<< endl;
  sBlackDrainState= sBlackIsDraining;
  sOpenBlackFillValve(false);
  sTurnPumpOn(true);
  sSetCycleStartMsec();
  return 1;
}  //sSwitchBlackToDraining


int sStartBlackFinalFillCycle() {
  Serial << LOG0 <<" sStartBlackFinalFillCycle(): Begin"<< endl;
  //Fill for 30 seconds for 2.5 gallons.
  sCurrentCycle= sBlackFillCycle;
  sSetCycleStartMsec();
  //lStopBlackFillMsec= lCurrentMsec + (l5GalOnMsec /2);
  lStopBlackFillMsec= lCurrentMsec + lFinalFillOnMsec;
  sOpenBlackFillValve(true);
  return 1;
}  //sStartBlackFinalFillCycle


int sStopCycle() {
  Serial << LOG0 <<" sStopCycle(): Begin"<< endl;
  sSetCycleStartMsec();
  sCurrentCycle= sIdleCycle;
  sOpenBlackFillValve(false);
  sTurnPumpOn(false);
  return 1;
}  //sStopCycle


boolean bPressureSwitchIsOK() {
  //Either verify the NC automotive oil pressure switch is connected and closed, or
  //verify the NO hot tub switch is not closed.
  boolean bReturn= true;
  int sSwitch= digitalRead(sPressSwitchPin);
  if (!bFlowSwitchIsNO && (sSwitch != LOW)) {
    Serial << LOG0 <<" bPressureSwitchIsOK(): ERROR: Switch is either not connected or open."<< endl;
    bReturn= false;
  } //if(!bFlowSwitchIsNO&&(sSwitch!=LOW))

  else if (bFlowSwitchIsNO && (sSwitch != HIGH)) {
    Serial << LOG0 <<" bPressureSwitchIsOK(): ERROR: Switch is closed w/o pump running."<< endl;
    bReturn= false;
  } //elseif(bFlowSwitchIsNO&&(sSwitch!=HIGH))
  return bReturn;
}  //bPressureSwitchIsOK


boolean bPumpIsDry() {
  boolean bReturn= false;
  int sSwitch= digitalRead(sPressSwitchPin);
  /*if (sSwitch == LOW) {
    Serial << LOG0 <<" bPumpIsDry(): Pressure is low"<< endl;
    bReturn= true;
  } //if(sSwitch==LOW)*/

  if ((bFlowSwitchIsNO && (sSwitch == HIGH)) || (!bFlowSwitchIsNO && (sSwitch == LOW))){
    Serial << LOG0 <<" bPumpIsDry(): Pressure is low"<< endl;
    bReturn= true;
  } //if(s(bFlowSwitchIsNO&&(sSwitch == HIGH)||...
  return bReturn;
}  //bPumpIsDry bFlowSwitchIsNO


int sHandleDryPump() {
  sTurnPumpOn(false);
  switch (sCurrentCycle) {
    case sGreyDrainCycle:
      sStopCycle();
      break;
    case sBlackDrainCycle:
      if (sBlackDrainState == sBlackIsDraining) {
        //Make sure pump was on for at least 3 seconds.
        //This is necessary for testing w/o pressure switch.
        if (sCycleSec() <= 3) {
          sStopCycle();
        } //if(sCycleSec()<=3)
        else {
          //sSwitchBlackToFilling() returns 0 when desired fills have happened.
          if (sSwitchBlackToFilling() == 0) {
            sStopCycle();
          } //if (sSwitchBlackToFilling() == 0)
        } //if(sCycleSec()<=3)else
      } //if(sBlackDrainState==sBlackIsDraining)
      break;
    case sIdleCycle:
    case sBlackFillCycle:
      //Do nothing.
      break;
    default:
      Serial << LOG0 << "sHandleDryPump(): Bad case in switch()= "<< sCurrentCycle << endl;
      break;
  } //switch
  return 1;
}  //sHandleDryPump


int sCheckKeyboard() {
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
      case 'g':
      case 'G':
        if (sCurrentCycle == sIdleCycle) {
          sStartGreyDrainCycle();
        }
        break;
      case 'b':
      case 'B':
        if (sCurrentCycle == sIdleCycle) {
          sStartBlackDrainCycle();
        }
        break;
      case 'f':
      case 'F':
        if (sCurrentCycle == sIdleCycle) {
          sStartBlackFinalFillCycle();
        }
        break;
      case 's':
      case 'S':
        sStopCycle();
        break;
      case 'd':
      case 'D':
        sToggleDebug();
        break;
      default:
        break;
    } //switch
  } //if(Serial.available()
  return 1;
}  //sCheckKeyboard


int sToggleDebug() {
  int sDebugRatio= 4;
  if (bDebug) {
    Serial << LOG0 <<" sToggleDebug(): Turning Debug OFF"<< endl;
    sNumBlackFills= sNumBlackFills * sDebugRatio;
    lBlackFlushOnMsec= lBlackFlushOnMsec * sDebugRatio;
    lFinalFillOnMsec= lFinalFillOnMsec * sDebugRatio;
  } //if(bDebug)
  else {
    Serial << LOG0 <<" sToggleDebug(): Turning Debug ON"<< endl;
    sNumBlackFills= sNumBlackFills / sDebugRatio;
    lBlackFlushOnMsec= lBlackFlushOnMsec / sDebugRatio;
    lFinalFillOnMsec= lFinalFillOnMsec / sDebugRatio;
  } //if(bDebug)else
  Serial << LOG0 <<" sToggleDebug(): Set l5GalOn Sec= "<< (lBlackFlushOnMsec/1000)
         <<", Final Fill Sec= "<< (lFinalFillOnMsec/1000)
         <<", sNumBlackFills= "<< sNumBlackFills << endl;
  bDebug= !bDebug;
  return 1;
}  //sToggleDebug


boolean bTimeoutReached() {
  boolean     bStopPump= false;
  if (lCurrentMsec > lNextTimeoutMsec) {
    Serial << LOG0 <<" bTimeoutReached(): Timeout reached, lNextTimeoutMsec= "
           << lNextTimeoutMsec << endl;
    bStopPump= true;
  } //if(lCurrentMsec>lNextTimeoutMsec)
  return bStopPump;
}  //bTimeoutReached


boolean bTimeToStopFillingBlack() {
  boolean     bStopFilling= false;
  if (lCurrentMsec > lStopBlackFillMsec) {
    Serial << LOG0 <<" bTimeToStopFillingBlack(): lStopBlackFillMsec= "<< lStopBlackFillMsec << endl;
    bStopFilling= true;
  } //if(lCurrentMsec>...
  return bStopFilling;
}  //bTimeToStopFillingBlack


int sPrintStatus() {
  //int sSecSinceStart;
  //static long lLastPrintedMsec= 0;
  if (lCurrentMsec >= lNextStatusMsec) {
    lNextStatusMsec= lCurrentMsec + lStatusMsec;
    if (bPumpIsOn) {
      Serial << LOG0 << " sPrintStatus(): Pump is ON, ";
    }
    else {
      Serial << LOG0 << " sPrintStatus(): Pump is OFF, ";
    }
    switch (sCurrentCycle) {
      case sIdleCycle:
        //sSecSinceStart= lCurrentMsec/1000;
        Serial <<"Cycle seconds= "<< ((sCycleSec()/60)) <<":"<< (sCycleSec() % 60)
               <<", Cycle= IDLE" << endl;
        break;
      case sGreyDrainCycle:
        Serial << "Cycle seconds= "<< ((sCycleSec()/60)) <<":"<< (sCycleSec() % 60)
          << " Cycle= GREY DRAIN" << endl;
        break;
      case sBlackDrainCycle:
        if (sBlackDrainState == sBlackIsDraining) {
          Serial <<"Cycle seconds= "<< ((sCycleSec()/60)) <<":"<< (sCycleSec() % 60)
                 << " Cycle= BLACK FLUSH DRAINING" << endl;
        }
        else {
          Serial << "Cycle "<< sBlackFillCount <<" of "<< sNumBlackFills <<", Cycle seconds= "<< ((sCycleSec()/60)) <<":"<< (sCycleSec() % 60)
               <<", to go= "<< ((sBlackFillCycleSecLeft()/60)) <<":"<< (sBlackFillCycleSecLeft() % 60)
               <<" seconds, Cycle= BLACK FLUSH FILLING" << endl;
        }
        break;
      case sBlackFillCycle:
        Serial <<"Cycle seconds= "<< ((sCycleSec()/60)) <<":"<< (sCycleSec() % 60)
               <<", to go= "<< ((sBlackFillCycleSecLeft()/60)) <<":"<< (sBlackFillCycleSecLeft() % 60)
               <<" seconds, Cycle= BLACK FINAL FILL" << endl;
        break;
      default:
        Serial << LOG0 << "loop(): Bad case in switch()= "<< sCurrentCycle << endl;
        break;
    } //switch
    vReadTempSensors();
  } //if(lCurrentMsec>=lNextStatusMsec)
  return 1;
}  //sPrintStatus
// Last line.
