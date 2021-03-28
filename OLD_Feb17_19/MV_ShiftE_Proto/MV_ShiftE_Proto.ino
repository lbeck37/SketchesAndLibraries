/* ShiftE_Calib.ino Arduino Sketch to run ShiftE derailer
 05/09/15- Change Gear locations for 9-spd cassette using cogs 3 to 9
 04/26/15B- Update Servo Xmin and max to allow calib on 29er
 04/26/15- Switch to using uSec for servo and moved servo to digital pin 6 was 3.
 04/25/15- Update to work with MicroView
 04/19/15- Implement Normal and Calibrate modes
 04/18/15- Implement Up/Down buttons using EasyButton library
 04/17/15- Clean up code in preparation for implementing 3-button interface
 04/13/15- Add servo with pot code
 04/11/15- Remove non-text items
 04/10/15 Beck- Port from Arduino 0022 to 1.6.3
*/

#include<MicroView.h>
#include <EasyButton.h>
#include <Streaming.h>
#include <Servo.h>

//Here come the const's
//Hardcoded gear position, measured 97 to 49 degrees, first location is power-on position
//static const int     asDefaultGearLocation[] = {1406, 1539, 1494, 1450, 1406, 1361, 1317, 1272};//uSec
//static const int       asDefaultGearLocation[]= {73, 97, 89, 81, 73, 65, 57, 49};   //Degrees
//static const int       asDefaultGearLocation[]= {66, 120, 91, 80, 66, 54, 34, 2};   //Degrees
//static const int       asDefaultGearLocation[]= {0, 130, 94, 80, 62, 42, 28, 0};   //Degrees
//static const int       asDefaultGearLocation[]= {0, 150, 119, 92, 74, 64, 48, 17};   //Degrees
static const int       asDefaultGearLocation[]= {0, 122, 101, 74, 68, 56, 35, 20};   //9-spd  using cogs 3-9
static const int       sServoMin             = 0;
static const int       sServoMax             = 180;
static const int       sServoMsecWait        = 15;
static const int       sNumGears             = 7;
static const boolean   bServoOn              = true;
static const int       sHoldDeltaPos         = 2;          //Servo move size when button held.
static const int       sTrimDeltaPos         = 1;          //Servo move size when button clicked.

static const int       sNormalMode           = 0;
static const int       sCalibMode            = 1;

static const int       sUp                   = 0;
static const int       sDown                 = 1;
static const int       sSelect               = 2;
static const int       sFirstButton          = sUp;
static const int       sLastButton           = sSelect;


static const int       sDisplayWidth         = LCDWIDTH;
static const int       sDisplayHeight        = LCDHEIGHT;

static const boolean   bButtonPullUp         = true;

//Digital Pins
static const int       sSelectButton         = A3;    //MV pin  4
static const int       sDownButton           = A2;    //MV pin  5
static const int       sUpButton             = A1;    //MV pin  6
static const int       sServoPin             =  6;    //MV pin 14

//Constants used locally for state in sCheckButtons
static const int       sButtonOpen        = 0;
static const int       sButtonPressed     = 1;
static const int       sButtonClosed      = 2;
static const int       sButtonReleased    = 3;
static const int       sButtonHeld        = 4;

static const int       sDefaultFont       =  0;
static const int       sMaxButtonPresses  = 10;

static const unsigned long    ulModeSwitchTime  = 1000;  //Minimum msecs between mode changes
static const unsigned long    ulModeWaitTime    = 2000;  //Minimum msecs before mode is on
//End of the const's

static int asGearLocation[sNumGears + 1];
static int sCurrentGear                   = 2;

static int sCurrentMode                   = sNormalMode;
static int sServoPosLast                  = 0;

//Create servo object to control the servo
Servo myservo;

//Create EasyButton objects to handle button presses.
EasyButton UpButton     (sUpButton,     NULL, CALL_NONE, bButtonPullUp);
EasyButton DownButton   (sDownButton,   NULL, CALL_NONE, bButtonPullUp);
EasyButton SelectButton (sSelectButton, NULL, CALL_NONE, bButtonPullUp);


//Number of unhandled presses, up to sMaxButtonPresses
static int              sButtonCount[]       = { 0, 0, 0};
static boolean          abButtonBeingHeld[]  = { false, false, false};
static unsigned long    ulNextModeTime       = 0;  //msec when a mode switch can take place
static unsigned long    ulModeReadyTime      = 0;  //msec when button presses can be handled

static int sLineCount= 0;     //Used in outputs to Serial Monitor for clarity.

//Create some things for working with MicroView display
static int         sFontNumber;
static int         sFontHeight;
static int         sFontWidth;
static int         sLineNumber;
static int         sCursorX;
static int         sCursorY;
static char        szShortString[10];
static char        szValue[10];
static char        szLineBuffer[25];
static boolean     bClearPage             = false;       //Flag to use calling sDisplayText()
static boolean     bDoDisplay             = false;       //Flag to use calling sDisplayText()

// The Arduino setup() method runs once, when the sketch starts
void setup()   {
  Serial.begin(9600);
  Serial << "Begin setup()" << endl;
  Serial << "Free Ram= " << freeRam() << endl;

  sFillGearLocations();

  sServoInit();

  sShowStartScreens();

  //Dither the servo once so it's position shows on the LCD.
  sServoDither(1, 1); // +/- 1 degree, once

  return;
}  //setup


// The Arduino loop() method gets called over and over.
void loop() {
  sCheckButtons();
  sDisplayUpdate();
  sHandleButtons();
  return;
}  //loop()


int sDisplayText(int sCursorX, int sCursorY, char *pcText) {
   uView.setCursor(sCursorX, sCursorY);
   uView.print(pcText);
   return 1;
}  //sDisplayText


int sDisplayBegin() {
   uView.begin();          // start MicroView
   return 1;
}  //sDisplayBegin


int sDisplayClearBuffer() {
   uView.clear(PAGE);
   return 1;
}  //sDisplayClearBuffer


int sDisplayRefresh() {
   uView.display();
   return 1;
}  //sDisplayRefresh


int sSetFont(int sFontType) {
   uView.setFontType(sFontType);

   sFontNumber= sFontType;
   sFontHeight= uView.getFontHeight();
   sFontWidth = uView.getFontWidth();

   return 1;
}  //sSetFont


int sShowStartScreens(void) {
   char        szWelcomeText[40];
   int         sLineNumber;   //Top line is zero

   sDisplayBegin();
   sDisplayClearBuffer();

   sSetFont(sDefaultFont);

   sLineNumber = 0;
   sCursorX    = 0;
   sCursorY    = sLineNumber * sFontHeight;
   sDisplayText(sCursorX, sCursorY, "PowerShift");

   sLineNumber = 2;
   sCursorX    = 0 * sFontWidth;
   sCursorY    = sLineNumber * sFontHeight;
   sDisplayText(sCursorX, sCursorY, "by");

   sLineNumber = 4;
   sCursorX    = 0 * sFontWidth;
   sCursorY    = sLineNumber * sFontHeight;
   sDisplayText(sCursorX, sCursorY, "ShiftE");

   sDisplayRefresh();

   delay(3000);
   return 1;
}  //sShowStartScreens


int sDisplayUpdate(void) {
   sDisplayClearBuffer();
   sSetFont(0);
   sDisplayButtons();
   sDisplayServoPos();
   sDisplayCurrentMode();
   sDisplayCurrentGear();
   sDisplayRefresh();
   return 1;
}  //sDisplayUpdate


int sDisplayButtons() {
   //Format is UDS 0 0 0 for Up, Down, Select count
   strcpy(szLineBuffer, "UDS ");

   itoa(sButtonCount[sUp]  ,szShortString  , 10);
   strcat(szLineBuffer, szShortString);
   strcat(szLineBuffer, " ");

   itoa(sButtonCount[sDown]  ,szShortString  , 10);
   strcat(szLineBuffer, szShortString);
   strcat(szLineBuffer, " ");

   itoa(sButtonCount[sSelect]  ,szShortString  , 10);
   strcat(szLineBuffer, szShortString);
   strcat(szLineBuffer, " ");

   sLineNumber = 0;
   sCursorX    = 0;
   sCursorY    = sLineNumber * sFontHeight;
   sDisplayText(sCursorX, sCursorY, szLineBuffer);

   return 1;
}  //sDisplayButtons


int sDisplayCurrentGear() {
   if (sCurrentMode == sNormalMode) {
      //Serial << "sDisplayCurrentGear(): Mode= " << sCurrentMode << endl;
      itoa(sCurrentGear, szShortString, 10);
      sSetFont(2);
      //sSetFont(3);
      sCursorX    = (sDisplayWidth - sFontWidth) / 2;
      sCursorY    = sDisplayHeight - sFontHeight;
      sDisplayText(sCursorX, sCursorY, szShortString);
   }  //if (sCurrentMode..

   return 1;
}  //sDisplayCurrentGear


int sDisplayServoPos() {
   itoa(sServoPosLast, szShortString, 10);
   strcpy(szLineBuffer, "Servo ");
   strcat(szLineBuffer, szShortString);
   sLineNumber = 2;
   sCursorX    = 0;
   sCursorY    = sLineNumber * sFontHeight;
   sDisplayText(sCursorX, sCursorY, szLineBuffer);
   return 1;
}  //sDisplayServoPos


int sDisplayCurrentMode() {
   switch(sCurrentMode) {
      case sNormalMode:
         strcpy(szLineBuffer, "Norm  Mode");
         break;   //sNormalMode
      case sCalibMode:
         strcpy(szLineBuffer, "Calib Mode");
         break;   //sCalibMode
      default:
         strcpy(szLineBuffer, "Funky Mode");
         Serial << "sDisplayCurrentMode(): Unexpected switch value." << endl;
         break;
   } //switch
   sLineNumber = 1;
   sCursorX    = 0;
   sCursorY    = sLineNumber * sFontHeight;
   sDisplayText(sCursorX, sCursorY, szLineBuffer);

   return 1;
}  //sDisplayCurrentMode


int sFillGearLocations(void) {
   for (int sGear=0; sGear <= sNumGears; sGear++) {
      asGearLocation[sGear]= asDefaultGearLocation[sGear];
   }  //for
   return 1;
}  //sFillGearLocations


int sHandleButtons(void) {
   int          sButton;
   int          sGearChange;
   int          sNewGear;
   int          sTargetLocation;
   int          sTargetChange= 0;

  if (!bHandleBothHeld()) {
     if (millis() > ulModeReadyTime) {
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
     }   //if(millis()...
   }  //if(!bHandleBothHeld())
   return 1;
}  //sHandleButtons


boolean bHandleBothHeld(void) {
   if (abButtonBeingHeld[sUp] &&  abButtonBeingHeld[sDown]) {
      if (millis() > ulNextModeTime) {
         Serial << sLineCount++ << " bHandleBothHeld(): Both buttons are being held." << endl;
         //Clear the button counts;
         sButtonCount[sUp]  = 0;
         sButtonCount[sDown]= 0;
         switch (sCurrentMode) {
            case sNormalMode:
               sCurrentMode= sCalibMode;
               Serial << sLineCount++
                      << " bHandleBothHeld(): Switch from Normal to Calib mode." << endl;
               break;
            case sCalibMode:
               sCurrentMode= sNormalMode;
               Serial << sLineCount++
                      << " bHandleBothHeld(): Switch from Calib to Normal mode." << endl;
               break;
            default:
               Serial << sLineCount++
                      << " bHandleBothHeld(): Bad switch :" << sCurrentMode << endl;
               break;
         }  //switch
         //Create the guard times between mode switches or button handling
         ulNextModeTime= millis() + ulModeSwitchTime;
         ulModeReadyTime= millis() + ulModeWaitTime;
      }  //if(millis()...
      return true;
   }    //if((sButtonCount[sUp]==sHoldCode)...
   else {
      return false;
   }  //if((sButtonCount[sUp]==sHoldCode)...
}  //bHandleBothHeld


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
         Serial << sLineCount++ << " sHandleNormalMode(): Button" << sButton << ", Count= "
                << sButtonCount[sButton] << ", sTargetChange= " << sTargetChange << endl;
         sButtonCount[sButton]--;
      }  //if((sButtonCount[sButton]!=sHoldCode)...
    } //for

    sNewGear= constrain(sCurrentGear + sTargetChange, 1, sNumGears);
    sCurrentGear= sNewGear;
    sTargetLocation= asGearLocation[sCurrentGear];

    //Make the actual shift
    sServoMove(sTargetLocation);
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
      //if (sButtonCount[sButton] == sHoldCode) {
      if (abButtonBeingHeld[sButton]) {
         sTarget= sServoPosLast + sDirection * sHoldDeltaPos;
         Serial << sLineCount++ << " sHandleCalibMode(): Button" << sButton
                << ", Count= " << sButtonCount[sButton] << ", Target= " << sTarget << endl;
         sServoMove(sTarget);
      }  //if(sButtonCount[sButton]==sHoldCode)

      //if ((sButtonCount[sButton] != sHoldCode) && (sButtonCount[sButton] > 0)) {
      if (!abButtonBeingHeld[sButton] && (sButtonCount[sButton] > 0)) {
         sTarget= sServoPosLast + sDirection * sTrimDeltaPos;
         Serial << sLineCount++ << " sHandleCalibMode(): Button" << sButton
                << ", Count= " << sButtonCount[sButton] << ", Target= " << sTarget << endl;
         sServoMove(sTarget);
         sButtonCount[sButton]--;
      }  //if((sButtonCount[sButton]!=sHoldCode)...
    } //for
 return 1;
}  //sHandleCalibMode


int sCheckButtons(void) {
/* Algorithm to determine when a button has been pressed or held.
 * Use IsRelease() to indicate the button has been pressed
 * Use IsHold() to indicate the button has been held down
 * States (Initial idea, not sure how close method is to this 4/25/15)
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
  //static int   sLocalButtonState[]  = {0, 0};
  boolean      bReturn              = false;
  int          sButton;

  UpButton.update();
  DownButton.update();
  SelectButton.update();

   //Run through the buttons, use short-circuiting to select
   for (sButton= sUp; sButton <= sDown; sButton++) {
      //Check for IsRelease for all buttons.
      if ( ((sButton == sUp)   && UpButton.IsRelease  ()) ||
      ((sButton == sDown) && DownButton.IsRelease()) ) {
         Serial << sLineCount++ << " sCheckButtons(): Button " << sButton
                << " was released." << endl;
         //Check to see if button is being held
         //if (sLocalButtonState[sButton] != sButtonHeld) {
         if ( !abButtonBeingHeld[sButton]) {
            if (sButtonCount[sButton] < sMaxButtonPresses) {
               //Increment the count of button presses to be handled.
               Serial << sLineCount++ << " sCheckButtons(): Button " << sButton
                      << " count incremented." << endl;
               sButtonCount[sButton]++;
            } //if(sLocalUpButtonState!=sButtonHeld)
         }    //if(sLocalButtonState!=...
         else {
            //The button was being held down, update the state variable..
            Serial << sLineCount++ << " sCheckButtons(): Button " << sButton
                   << " done being held." << endl;
            //sLocalButtonState[sButton]= sButtonOpen;
            abButtonBeingHeld[sButton]= false;
         } //if(sLocalUpButtonState!=sButtonHeld)else
         bReturn= true;
      }   //if(((sButton==sUp)&&...

      //Check IsHold for up and down buttons.
      if (!bReturn &&
      ((sButton == sUp)   && UpButton.IsHold  ()) ||
      ((sButton == sDown) && DownButton.IsHold()) ) {
         Serial << sLineCount++ << " sCheckButtons(): Button " << sButton
                << " being held." << endl;
         //Set state to indicate in hold.
         //sLocalButtonState[sButton]= sButtonHeld;
         //sButtonCount[sButton]= sHoldCode;
         abButtonBeingHeld[sButton] = true;
         sButtonCount[sButton]      = 0;  //Clear the count
      }   //if(!bReturn&&UpButton.IsHold()
      bReturn= false;
   } //for (sButton= sUp...
  return 1;
}  //sCheckButtons


int sServoInit() {
   if (bServoOn) {
      myservo.attach(sServoPin);
      sServoMove(asGearLocation[sCurrentGear]);
   }  //if(bServoOn)
   return 1;
} //sServoInit

int sServoMove(int sServoPos) {
  if (sServoPos != sServoPosLast) {
      sServoPos= constrain(sServoPos, sServoMin, sServoMax);
      Serial << "sServoMove(): Move to " << sServoPos << endl;
      sServoPosLast= sServoPos;
      sServoSetPosition(sServoPos);
  }  //if(sServoPos...
  return 1;
} //sServoMove


//sServoDither() moves the servo back and forth and back to where it started.
int sServoDither(int sDitherSize, int sNumTimes){
   int sStartPosition = sServoPosLast;
   int sFirstPosition = sStartPosition + sDitherSize;
   int sSecondPosition= sStartPosition - sDitherSize;
   int sCycle;

   for (sCycle= 0; sCycle < sNumTimes; sCycle++){
      sServoMove(sFirstPosition);
      sServoMove(sSecondPosition);
   }  //for(sCycle...
   sServoMove(sStartPosition);
   return 1;
}  //sServoDither


int sServoSetPosition(int sServoPos) {
   //Note that values of sServoPos less than 200 are considered degrees and
   //values greater are uSec pulse width by myservo.write().
   if (bServoOn) {
      myservo.write(sServoPos);
   }
   delay(sServoMsecWait);
   return 1;
}  //sServoSetPosition


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