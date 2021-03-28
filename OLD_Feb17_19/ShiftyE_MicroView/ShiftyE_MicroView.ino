/* ShiftE_Calib.ino Arduino Sketch to run ShiftE derailer
 04/19/15- Implement Normal and Calibrate modes
 04/18/15- Implement Up/Down buttons using EasyButton library
 04/17/15- Clean up code in preparation for implementing 3-button interface
 04/13/15- Add servo with pot code
 04/11/15- Remove non-text items
 04/10/15 Beck- Port from Arduino 0022 to 1.6.3
*/

#undef USE_ST7565
#include <EasyButton.h>
#include <Streaming.h>
#ifdef USE_ST7565
  #include <ST7565.h>
#endif
#include <Servo.h>

//Here come the const's
//Hardcoded gear position, measured 97 to 49, first location is power-on position
static const int       asDefaultGearLocation[]= { 73, 97, 89, 81, 73, 65, 57, 49};
static const int       sNumGears             = 7;
static const boolean   bServoOn              = true;
static const boolean   bUsePot               = false;

static const int       sNormalMode           = 0;
static const int       sCalibMode            = 1;

static const int       sUp                   = 0;
static const int       sDown                 = 1;

static const int       sCenterDegrees        = 75;
static const int       sServoMsecWait        = 15;

static const int       sLcdContrast          = 0x18;
static const boolean   bButtonPullUp         = true;

   //Analog Pins
static const int       sPotPin               =  2;     // Analog pin for pot.

   //Digital Pins
//static const int       sDisplayBacklight     =  4;     // Turn LCD backlight on & off
static const int       sDisplayChipSelect    =  5;
static const int       sDisplayReset         =  6;
static const int       sDisplayDataSelect    =  7;
static const int       sDisplayClockOut      =  8 ;
static const int       sDisplayDataOut       =  9;
static const int       sUpButton             = 10;
static const int       sServoPin             = 11;
static const int       sDownButton           = 12;
static const int       sArduinoLED           = 13;    //Board LED connected to digital pin 13
//Constants used locally for state in sCheckButtons
static const int       sButtonOpen        = 0;
static const int       sButtonPressed     = 1;
static const int       sButtonClosed      = 2;
static const int       sButtonReleased    = 3;
static const int       sButtonHeld        = 4;

static const int       sMaxButtonPresses  = 10;
static const int       sHoldCode          = 99; //sButtonCount[] has this when button held.

static const int       sHoldDeltaPos      =  2; //Servo move size when button held.
static const int       sTrimDeltaPos      =  1; //Servo move size when button clicked.
//End of the const's

static int asGearLocation[sNumGears + 1];
static int sCurrentGear       = 4;

static int sCurrentMode       = sNormalMode;
static int sServoDegreesLast  = 0;

//Create servo object to control the servo
Servo myservo;

//Create EasyButton objects to handle button presses.
EasyButton UpButton  (sUpButton,   NULL, CALL_NONE, bButtonPullUp);
EasyButton DownButton(sDownButton, NULL, CALL_NONE, bButtonPullUp);


//Global state of buttons
static int sButtonCount[] = { 0, 0};
static int sDownButtonState  = 0;
static int sSelButtonState   = 0;

static int sLineCount= 0;     //Used in outputs to Serial Monitor for clarity.

#ifdef USE_ST7565
// ST7565 LCD 128x64 serial (SPI?) display
// Arduino pins used
// pin 9 - Serial data out (SID)
// pin 8 - Serial clock out (SCLK)
// pin 7 - Data/Command select (RS or A0)
// pin 6 - LCD reset (RST)
// pin 5 - LCD chip select (CS)
// pin 4 - Backlight On/Off, how does this work?

ST7565 glcd(sDisplayDataOut, sDisplayClockOut, sDisplayDataSelect,
            sDisplayReset  , sDisplayChipSelect);
static int sLineCount= 0;     //Used in outputs to Serial Monitor for clarity.

// a bitmap of a 16x16 Adafruit icon
static const unsigned char __attribute__ ((progmem)) logo16_glcd_bmp[]={
0x30, 0xf0, 0xf0, 0xf0, 0xf0, 0x30, 0xf8, 0xbe, 0x9f, 0xff, 0xf8, 0xc0, 0xc0, 0xc0, 0x80, 0x00,
0x20, 0x3c, 0x3f, 0x3f, 0x1f, 0x19, 0x1f, 0x7b, 0xfb, 0xfe, 0xfe, 0x07, 0x07, 0x07, 0x03, 0x00, };
#endif   //USE_ST7565

// The setup() method runs once, when the sketch starts
void setup()   {
  Serial.begin(9600);
  Serial << "Begin setup()" << endl;
  Serial << "Free Ram= " << freeRam() << endl;

  sFillGearLocations();

  if (bServoOn) {
    myservo.attach(sServoPin);
    //sMoveServo(sCenterDegrees);
    sMoveServo(asGearLocation[sCurrentGear]);
   }  //if(bServoOn)

  sShowStartScreens();

  //Dither the servo once so it's position shows on the LCD.
  sDitherServo(1, 1); // +/- 1 degree, once

  return;
}  //setup


void loop() {
  int    sPotVal;
  int    sServoDegrees;
  int    sServoPotDegrees;

  if (bUsePot) { //Reads value of pot (0 to 1023) and scale to servo (0 to 180).
    sPotVal = analogRead(sPotPin);
    sServoPotDegrees = map(sPotVal, 0, 1023, 0, 180);
    sServoDegrees= sServoPotDegrees;
  }  //if(bUsePot)
  else {
     sServoDegrees= sServoDegreesLast;
  }   //if(bUsePot)else

  sCheckButtons();

  sDisplayUpdate();

  //sHandleButtons();

  return;
}  //loop()


int sFillGearLocations(void) {
   for (int sGear=0; sGear <= sNumGears; sGear++) {
      asGearLocation[sGear]= asDefaultGearLocation[sGear];
   }  //for
   return 1;
}  //sFillGearLocations

#if 0
int sHandleButtons(void) {
   int          sButton;
   int          sGearChange;
   int          sNewGear;
   int          sTargetLocation;
   int          sTargetChange= 0;

  if (!bHandleBothHeld()) {
    switch(sCurrentMode) {
      case sNormalMode:
         sHandleNormalMode();
         break;   //sNormalMode
      case sCalibMode:
         sHandleCalibMode();
         break;   //sCalibMode
      default:
         Serial << "sHandleButtons(): Unexpected switch value." << endl;
         break;
     } //switch
   }  //if(!bHandleBothHeld())
   return 1;
}  //sHandleButtons


int sHandleNormalMode(void) {
   int          sButton;
   int          sGearChange;
   int          sNewGear;
   int          sTargetLocation;
   int          sTargetChange= 0;
   boolean      bReturn= false;

   if (!bReturn) {
     for (sButton= sUp; sButton <= sDown; sButton++) {
      if (sButton == sUp) {
         sGearChange= 1;
      }
      else {
         sGearChange= -1;
      }
      //Compute net gear change by handling at most one request from each button
      if (sButtonCount[sButton] > 0) {
         sTargetChange += sGearChange;
         Serial << sLineCount++ << " sHandleNormalMode(): Button" << sButton <<
            ", Count= " << sButtonCount[sButton] << ", sTargetChange= " << sTargetChange << endl;
         sButtonCount[sButton]--;
      }  //if((sButtonCount[sButton]!=sHoldCode)...
    } //for

    sNewGear= constrain(sCurrentGear + sTargetChange, 1, sNumGears);
    sCurrentGear= sNewGear;
    sTargetLocation= asGearLocation[sCurrentGear];

    //Make the actual shift
    sMoveServo(sTargetLocation);
   }  //if(!bReturn)
   return 1;
}  //sHandleNormalMode


int sHandleCalibMode(void) {
   int          sButton;
   int          sDirection;
   int          sTarget;

   for (sButton= sUp; sButton <= sDown; sButton++) {
      if (sButton == sUp) {
         sDirection= 1;
      }
      else {
         sDirection= -1;
      }
      if (sButtonCount[sButton] == sHoldCode) {
         sTarget= sServoDegreesLast + sDirection * sHoldDeltaPos;
         Serial << sLineCount++ << " sHandleCalibMode(): Button" << sButton;
         Serial << ", Count= " << sButtonCount[sButton] << ", Target= " << sTarget << endl;
         sMoveServo(sTarget);
      }  //if(sButtonCount[sButton]==sHoldCode)

      if ((sButtonCount[sButton] != sHoldCode) && (sButtonCount[sButton] > 0)) {
         sTarget= sServoDegreesLast + sDirection * sTrimDeltaPos;
         Serial << sLineCount++ << " sHandleCalibMode(): Button" << sButton;
         Serial << ", Count= " << sButtonCount[sButton] << ", Target= " << sTarget << endl;
         sMoveServo(sTarget);
         sButtonCount[sButton]--;
      }  //if((sButtonCount[sButton]!=sHoldCode)...
    } //for
 return 1;
}  //sHandleCalibMode


boolean bHandleBothHeld(void) {
   if ((sButtonCount[sUp] == sHoldCode) &&  (sButtonCount[sDown] == sHoldCode)) {
      Serial << sLineCount++ << " bHandleBothHeld(): Both buttons are being held." << endl;
      //Clear the button counts;
      sButtonCount[sUp]  = 0;
      sButtonCount[sDown]= 0;
      switch (sCurrentMode) {
         case sNormalMode:
            sCurrentMode= sCalibMode;
            break;
         case sCalibMode:
            sCurrentMode= sNormalMode;
            break;
         default:
            Serial << sLineCount++ << " bHandleBothHeld(): Bad switch :" << sCurrentMode << endl;
            break;
         }  //switch
      return true;
   }    //if((sButtonCount[sUp]==sHoldCode)...
   else {
      return false;
   }  //if((sButtonCount[sUp]==sHoldCode)...
}  //bHandleBothHeld
#endif

int sCheckButtons(void) {
/* Algorithm to determine when a button has been pressed or held.
 * Use IsRelease() to indicate the button has been pressed
 * Use IsHold() to indicate the button has been held down
 * States
 *    0 - Button open
 *    1 - Button pressed, IsPushed() returned true once
 *          Moves State to 2 when InPush() first returns true
 *    2 - InPush() returning true, button is down
 *    3 - IsRelease() returned true, button has been released
 *          Moves State to 0
 *    4 - IsHold() returned true, button  held past threshhold
 *          Moves State to 0 when IsRelease returns true.
 * sUpButtonCount will be:
 *    0   Nothing to be done
 *   99   Button was held and not yet released
 *   1-10 Number of button presses not yet handled, max 10
*/
  static int   sLocalButtonState[]  = {0, 0};
  //static int   sLocalDownButtonState= 0;
  boolean      bReturn              = false;
  int          sButton;

  UpButton.update();
  DownButton.update();

  //Run through the buttons, use short-circuiting to select
 for (sButton= sUp; sButton <= sDown; sButton++) {
  //Check for IsRelease for all buttons.
  if ( ((sButton == sUp)   && UpButton.IsRelease  ()) ||
       ((sButton == sDown) && DownButton.IsRelease()) ) {
    Serial << sLineCount++ << " sCheckButtons(): " << sButton << ".IsRelease" << endl;
    //Check to see if button is being held
    if (sLocalButtonState[sButton] != sButtonHeld) {
       if (sButtonCount[sButton] < sMaxButtonPresses) {
          //Increment the count of button presses to be handled.
          sButtonCount[sButton]++;
       } //if(sLocalUpButtonState!=sButtonHeld)
    }    //if(sLocalButtonState!=...
    else {
      //The button was being held down, change state to 0 to indicate done holding.
      sLocalButtonState[sButton]= sButtonOpen;
      sButtonCount[sButton]= 0;
    } //if(sLocalUpButtonState!=sButtonHeld)else
    bReturn= true;
  }   //if(((sButton==sUp)&&...

  //Check for IsHold for all buttons.
  if (!bReturn &&
       ((sButton == sUp)   && UpButton.IsHold  ()) ||
       ((sButton == sDown) && DownButton.IsHold()) ) {
  //if (!bReturn && UpButton.IsHold()) {
    Serial << sLineCount++ << " sCheckButtons(): " << sButton << ".IsHold" << endl;
    //Set state to indicate in hold.
    sLocalButtonState[sButton]= sButtonHeld;
    sButtonCount[sButton]= sHoldCode;
  }   //if(!bReturn&&UpButton.IsHold()
  bReturn= false;
 } //for (sButton= sUp...
  return 1;
}  //sCheckButtons


int sDisplayUpdate(void) {
   int sLine= 0;

   sDisplayServoLine (sLine++);
   sDisplayButtonLine(sLine++);
   sDisplayRefresh();

   return 1;
}  //sDisplayUpdate


#ifndef USE_ST7565
int sDisplayButtonLine(int sLine) {
  return 1;
}  //sDisplayButtonLine


int sDisplayServoLine(int sLine) {
  return 1;
}  //sDisplayServoLine


int sDisplayRefresh(void) {
   return 1;
}  //sDisplayRefresh


int sShowStartScreens(void) {
return 1;
}  //sShowStartScreens


#else
int sDisplayButtonLine(int sLine) {
  char            szUpButton[10];
  char            szDownButton[10];
  char            szSelButton[10];
  char            szLineBuffer[25];

  itoa(sButtonCount[sUp]  ,szUpButton  , 10);
  itoa(sButtonCount[sDown],szDownButton, 10);
  itoa(sSelButtonState,  szSelButton,  10);

  strcpy(szLineBuffer, "Up=");
  strcat(szLineBuffer, szUpButton);
  strcat(szLineBuffer, ", Down=");
  strcat(szLineBuffer, szDownButton);
  strcat(szLineBuffer, ", Sel=");
  strcat(szLineBuffer, szSelButton);

  glcd.drawstring(0, sLine, szLineBuffer);
  return 1;
}  //sDisplayButtonLine


int sDisplayServoLine(int sLine) {
  char   szServoDegrees[10];
  char   szLineBuffer[25];

  //Write to LCD
  itoa(sServoDegreesLast, szServoDegrees, 10);
  strcpy(szLineBuffer, "Servo = ");
  strcat(szLineBuffer, szServoDegrees);
  strcat(szLineBuffer, " degrees");
  glcd.drawstring(0, sLine, szLineBuffer);
  return 1;
}  //sDisplayServoLine


int sDisplayRefresh(void) {
   glcd.display();
   glcd.clear();
   return 1;
}  //sDisplayRefresh


int sShowStartScreens(void) {
  glcd.begin(sLcdContrast);
  glcd.display(); // show splashscreen
  delay(2000);
  glcd.clear();
return 1;
}  //sShowStartScreens
#endif  //USE_ST7565


int sMoveServo(int sServoDegrees) {
  if (sServoDegrees != sServoDegreesLast){
      Serial << "Servo = " << sServoDegrees << " degrees" << endl;
      sServoDegreesLast= sServoDegrees;
      sSetServoPosition(sServoDegrees);
  }  //if(sServoDegrees!=sServoDegreesLast)
  return 1;
} //sMoveServo


//sDitherServo() moves the servo back and forth and back to where it started.
int sDitherServo(int sDitherSize, int sNumTimes){
   int sStartPosition = sServoDegreesLast;
   int sFirstPosition = sStartPosition + sDitherSize;
   int sSecondPosition= sStartPosition - sDitherSize;
   int sCycle;

   for (sCycle= 0; sCycle < sNumTimes; sCycle++){
      sMoveServo(sFirstPosition);
      sMoveServo(sSecondPosition);
   }  //for(sCycle...
   sMoveServo(sStartPosition);
   return 1;
}  //sDitherServo


int sSetServoPosition(int sServoDegrees) {
   if (bServoOn) {
      myservo.write(sServoDegrees);
   }
   delay(sServoMsecWait);
   return 1;
}  //sSetServoPosition


//freeRam() returns the number of bytes currently free in RAM.
int freeRam(void)
{
  extern int  __bss_end;
  extern int  *__brkval;
  int free_memory;
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
  }
  return free_memory;
}  //freeRam
//Last line.
