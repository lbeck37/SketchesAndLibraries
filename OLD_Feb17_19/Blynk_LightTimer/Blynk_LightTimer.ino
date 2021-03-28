static const char stSketchName[]  = "Blynk_LightTimer.ino";
static const char stFileDate[]    = "Dec 12, 2015D";
// 12/09/15 Add more virtual pins for Blynk LCD and Terminal.
// 12/09/15 Add more virtual pins to set relays for timers.
// 12/08/15 Copy from Blynk_Fireplace.ino
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

#define LOG0      lLineCount++ << " " << millis()

//Define Virtual and Analog pin names
#define DegF_VPin1          V1
#define DegF_VPin2          V2
#define DegF_VPin3          V3
#define RelayState_VPin5    V5
#define RelayState_VPin6    V6
#define LCD_Line1_VPin7     V7
#define LCD_Line1_VPin8     V8
#define Terminal_VPin9      V9
#define Button_V10          V10   //Relay 0
#define Timer_V11           V11   //Relay 0
#define Button_V20          V20   //Relay 1
#define Timer_V21           V21   //Relay 1

// Attach virtual serial terminal to Virtual Pin V1
WidgetTerminal      terminal(Terminal_VPin9);
WidgetLCD           LCDWidget(1);

static const bool   bRelaysInverted       = true;
static const int    sRelayOpen            = HIGH;
static const int    sRelayClosed          = LOW;
static const int    sNumRelays            = 2;
static const int    sRelayPin[]           = {4, 5};

static int          sRelayState[2];
static long         lLineCount        = 0;      //Serial Monitor uses for clarity.
static long         lLineCount2       = 0;      //For Blynk terminal window.
static long         lNumLoops         = 1;
static float        fDegF;


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

  /*//Test writing to LCD
  LCDWidget.clear();
  int sCharPos= 0;   //Position 0-15
  int sLineNum= 0;   //Line 0-1
  LCDWidget.print(0, 0, "Wow! :)");*/

  sSetupRelays();
  return;
} //setup


void loop()
{
  Blynk.run();
} //loop
  // This will print Blynk Software version to the Terminal Widget when
  // your hardware gets connected to Blynk Server


int sTerminalPrintVersion(){
  Serial << LOG0 << " sTerminalPrintVersion: Begin" << endl;
  terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  terminal.println("-------------");
  terminal.println("Type 'Marco' and get a reply, or type");
  terminal.println("anything else and get it printed back.");
  terminal.flush();
  return 1;
} //sTerminalPrintVersion


// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
//int sWriteTerminalLine(char *szString){
int sWriteTerminalLine(String szString){
  Serial << LOG0 << " sWriteTerminalLine: Received String= " <<  szString << endl;

  terminal.println(szString) ;

  // Ensure everything is sent
  terminal.flush();
  return 1;
} //sWriteTerminalLine


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


int sBlynkLog(String szString, int sValue){
  long lMillis= millis();
  long lCurrentLine= lLineCount2++;

  String szTermString= "";
  szTermString += lCurrentLine;
  szTermString += " ";
  szTermString += lMillis;
  szTermString += szString;
  szTermString +=  sValue;
  sWriteTerminalLine(szTermString);
  return 1;
} //sBlynkLog


//BLYNK_READ() functions are called by the Blynk app on the phone (at a 1 second interval)
//and returns the value or state of some variable.
BLYNK_READ(RelayState_VPin5){
  Blynk.virtualWrite(RelayState_VPin5, !sRelayState[0]);
} //BLYNK_READ(RelayState_VPin5)


BLYNK_READ(RelayState_VPin6){
  Blynk.virtualWrite(RelayState_VPin6, !sRelayState[1]);
} //BLYNK_READ(RelayState_VPin6)


//BLYNK_WRITE() functions are called by the Blynk app on the phone
//and pass a variable in the "param" object.
BLYNK_WRITE(Terminal_VPin9)
{
  Serial << LOG0 << " BLYNK_WRITE(Terminal_VPin9): Received Parameter= " <<  param.asStr() << endl;

  // if you type "Marco" into Terminal Widget - it will respond: "Polo:"
  if (String("Marco") == param.asStr()) {
      terminal.println("You said: 'Marco'") ;
      terminal.println("I said: 'Polo'") ;
  } else {

  // Send it back
  terminal.print("You said:");
  terminal.write(param.getBuffer(), param.getLength());
  terminal.println();
  }

  // Ensure everything is sent
  terminal.flush();
  return;
} //BLYNK_WRITE(Terminal_VPin9)


//Handler callback function called when Button set as a Switch is pressed.
//Light around button is lit when 1 is passed as parameter, unlit when 0 is passed.
//Opto-isolated relay is inverse logic, pulling input pin low cn relay.
//Relay #0 is connected to Blynk virtual pins 10, 11, 12
//Relay #1 is connected to Blynk virtual pins 20, 21, 22
BLYNK_WRITE(Button_V10){
  int sSetting= param.asInt();
  int sRelaySetting;
  Serial << LOG0 << " BLYNK_WRITE(Button_V10): Received Parameter= " << sSetting << endl;
  Serial << LOG0 << " ******* Blynk Button for Relay #0 ******" << endl;

  String szString= " Button_V10: ";
  sBlynkLog(szString, sSetting);

  //Test writing to LCD
  LCDWidget.clear();
  int sCharPos= 0;   //Position 0-15
  int sLineNum= 0;   //Line 0-1
  LCDWidget.print(0, 0, "Relay #0 set to: ");

  if (sSetting == 1){
    sRelaySetting= sRelayClosed;
  }
  else{
    sRelaySetting= sRelayOpen;
  }
  sSetRelay(0, sRelaySetting);    //Set relay #0
  return;
} //BLYNK_WRITE(Button_V10)


BLYNK_WRITE(Timer_V11){
  int sSetting= param.asInt();
  int sRelaySetting;
  Serial << LOG0 << " BLYNK_WRITE(Timer_V11): Received Parameter= " << sSetting << endl;
  Serial << LOG0 << " ******* Blynk Timer for Relay #0 ******" << endl;

  String szString= " Timer_V11: ";
  sBlynkLog(szString, sSetting);

  if (sSetting == 1){
    sRelaySetting= sRelayClosed;
    }
  else{
    sRelaySetting= sRelayOpen;
    }
  sSetRelay(0, sRelaySetting);    //Set relay #0
  return;
  } //BLYNK_WRITE(Timer_V11)


BLYNK_WRITE(Button_V20){
  int sSetting= param.asInt();
  int sRelaySetting;
  Serial << LOG0 << " BLYNK_WRITE(Button_V20): Received Parameter= " << sSetting << endl;
  Serial << LOG0 << " ******* Blynk Button for Relay #1 ******" << endl;

  String szString= " Button_V20: ";
  sBlynkLog(szString, sSetting);

  if (sSetting == 1){
    sRelaySetting= sRelayClosed;
  }
  else{
    sRelaySetting= sRelayOpen;
  }
  sSetRelay(1, sRelaySetting);    //Set relay #1
  return;
} //BLYNK_WRITE(Button_V20)


BLYNK_WRITE(Timer_V21){
  int sSetting= param.asInt();
  int sRelaySetting;
  Serial << LOG0 << " BLYNK_WRITE(Timer_V21): Received Parameter= " << sSetting << endl;
  Serial << LOG0 << " ******* Blynk Timer for Relay #1 ******" << endl;

  String szString= " Timer_V21: ";
  sBlynkLog(szString, sSetting);

  if (sSetting == 1){
    sRelaySetting= sRelayClosed;
  }
  else{
    sRelaySetting= sRelayOpen;
  }
  sSetRelay(1, sRelaySetting);    //Set relay #1
  return;
} //BLYNK_WRITE(Timer_V21)
//Last line.
