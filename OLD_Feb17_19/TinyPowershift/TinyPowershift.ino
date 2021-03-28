// 07/2/9/15 Port from Powershift.ino to display using I2C on TinyScreen on TinyDuino (Pro mini processor)

#include <Streaming.h>  //For some reason I can't include this from LBeck37.h
#include <LBeck37.h>
#include <SPI.h>
#include <EasyButton.h>
#include <Servo.h>
#include <Wire.h>
#include <TinyScreen.h>
//#include <U8glib.h>
#include <stdarg.h>

//Defines #if
#define LOG0              lLineCount++ << " " << millis()
#define UINT16             unsigned int

//Here come the const's
static const uint8_t   ucContrast           = 100;
static const boolean   bFlipDisplay         = false;
static const int       sSplashDelay         = 2000;     //mSec that Splash screen is on
static const int       sMainDelay           = 1000;      //for debug
static const int       sLineSpace           = 9;        //Line spacing in pixels
static const char      szSplashLine1[]      = {"Electic Shifting"};
static const char      szSplashLine2[]      = {"* Max Performance *"};
static const char      szSplashLine3[]      = {"*    Max Range    *"};
static const char      szSplashLine4[]      = {"PowerShift"};
static const char      szSplashLine5[]      = {"  by ShiftE"};
static const char      szServo[]            = {"Servo "};
//static const int  asDefaultGearLocation[]= {0, 150, 119, 92, 74, 64, 48, 17};
static const int asDefaultGearLocation[]= {0, 122, 101, 74, 68, 56, 35, 20};   //9-spd cogs 3-9
static const int       sServoMin             = 0;
static const int       sServoMax             = 180;
static const int       sServoMsecWait        = 15;
static const int       sNumGears             = 7;
static const boolean   bServoOn              = true;
static const int       sHoldDeltaPos         = 2; //Servo move size when button held.
static const int       sTrimDeltaPos         = 1; //Servo move size when button clicked.

static const int       sNormalMode           = 0;
static const int       sCalibMode            = 1;

static const int       sUp                   = 0;
static const int       sDown                 = 1;
static const int       sSelect               = 2;
static const int       sFirstButton          = sUp;
static const int       sLastButton           = sSelect;

static const boolean   bButtonPullUp         = true;

//Digital Pins
static const int       sSelectButton         = A3;
static const int       sDownButton           = A2;
static const int       sUpButton             = A1;
static const int       sBacklightPin         =  6;
static const int       sServoPin             =  7;
static const byte      cSPICmdDataPin        =  9;
static const byte      cSPIChipSelectPin     = 10;

//Gyro defines
static const int       sXAxis             = 0;
static const int       sYAxis             = 1;
static const int       sZAxis             = 2;
static const int       sNumAxis           = 3;

static const int       sAccel             = 0;
static const int       sRotation          = 1;
static const int       sTemperature       = 2;
static const int       sNumGyroTypes      = 3;

//Constants used locally for state in sCheckButtons
static const int       sButtonOpen        = 0;
static const int       sButtonPressed     = 1;
static const int       sButtonClosed      = 2;
static const int       sButtonReleased    = 3;
static const int       sButtonHeld        = 4;

//static const int       s6PixelChar        =  6;    //Use for spacing titles on the display.
static const int       sDefaultFont       =  0;
static const int       sMaxButtonPresses  = 10;

//String constant for printing
static const char      acXYZ[][2]          = {{'X', '\0'}, {'Y', '\0'}, {'Z', '\0'}};

static const unsigned long    ulModeSwitchTime  = 1000;  //Minimum msecs between mode changes
static const unsigned long    ulModeWaitTime    = 2000;  //Minimum msecs before mode is on
static const unsigned long    ulGyroReadTime    = 500;   //Gyro reads spaced by this.

//End of the const's
static long       lLineCount      = 0;      //Serial Monitor uses for clarity.

static int asGearLocation[sNumGears + 1];
static int sCurrentGear                   = 2;

static int        sCurrentMode      = sNormalMode;
static int        sServoPosLast     = 0;
static boolean    bTestDisplay      = true;

//*****Object creation*****
TinyScreen TinyDisplay = TinyScreen(0);

//Create servo object to control the servo
Servo myservo;

//Create EasyButton objects to handle button presses.
EasyButton UpButton     (sUpButton,     NULL, CALL_NONE, bButtonPullUp);
EasyButton DownButton   (sDownButton,   NULL, CALL_NONE, bButtonPullUp);
EasyButton SelectButton (sSelectButton, NULL, CALL_NONE, bButtonPullUp);

//Number of unhandled presses, up to sMaxButtonPresses
static int              sButtonCount[]       = { 0, 0, 0};
static int              sButtonCountLast[]   = { 0, 0, 0};
static boolean          abButtonBeingHeld[]  = { false, false, false};
static unsigned long    ulNextModeTime       = 0;  //msec when a mode switch can take place
static unsigned long    ulModeReadyTime      = 0;  //msec when button presses can be handled
//static int              sCurrentFont         = sFontNotSet;

//State of display items being changed and needing refresh.
static boolean     bButtonsChanged          = false;
static boolean     bGearChanged             = false;
static boolean     bServoChanged            = false;
static boolean     bModeChanged             = false;
//static int         sLC                      = 0;  //Serial Monitor Line Count, for clarity.

static char        szLineBuffer[25];   //DOGS102 line is 17 chars with 6x8 normal font.
static char        sz10CharString[10];

//TinyScreen stuff
#define BLACK           0x00
#define BLUE            0xE0
#define RED             0x03
#define GREEN           0x1C
#define DGREEN          0x0C
#define YELLOW          0x1F
#define WHITE           0xFF
#define ALPHA           0xFE
#define BROWN           0x32

uint8_t amtcolors=7;
uint8_t colors[]={BLACK,BLUE,RED,GREEN,WHITE,DGREEN,YELLOW};


// The Arduino setup() method runs once, when the sketch starts
void setup() {
   Serial.begin(9600);
   Serial << LOG0 <<" setup(): Begin July 29, 2015"<< endl;
   Serial << LOG0 << " Free Ram= " << freeRam() << endl;

   sSetupDisplay();
#ifdef DEBUG_ON
   sDrawStartScreen();
   sDrawMainScreen();
   delay(sMainDelay);
#else
   //sSetupGyro();
   sFillGearLocations();
   sSetupServo();
   //sSetupSmoothing();
   sDrawStartScreen();
   //sTestContrast();
#endif   //DEBUG_1

   //Dither the servo once so it's position shows on the LCD.
   sServoDither(1, 1); // +/- 1 degree, once
   return;
}  //setup


// The Arduino loop() method gets called over and over.
void loop() {
   if (bTestDisplay) {
      sExerciseDisplay();
   }  //if(bTestDisplay)
   else {
      sCheckButtons();
      //sGyroLoop();
      sDrawMainScreen();
      //sTestContrast();
      sHandleButtons();
   } //if(bTestDisplay)
   return;
}  //loop()


int sExerciseDisplay() {
  TinyDisplay.setFont(liberationSans_10ptFontInfo);
  TinyDisplay.fontColor(WHITE,BLACK);
  TinyDisplay.setBrightness(15);

  Serial << LOG0 << " sExerciseDisplay: Test Text" << endl;
  for(int i=5;i<20;i++){
    TinyDisplay.setCursor((3000-(i*5))%70,(i*14)%50);
    TinyDisplay.print("text");
    TinyDisplay.setCursor((i*7)%70,(6000-(i*6))%50);
    TinyDisplay.print("text");
    delay(150);
  }
  for(int i=0;i<5;i++){
    TinyDisplay.setCursor(0,i*12);
    TinyDisplay.print("FASTFASTFASTFAST");
  }
  delay(200);

  Serial << LOG0 << " sExerciseDisplay: Test Brightness" << endl;
  TinyDisplay.setCursor(15,20);
  TinyDisplay.fontColor(BLACK,WHITE);
  TinyDisplay.clearWindow(0,0,96,64);
  TinyDisplay.setBrightness(15);
  TinyDisplay.drawRect(0,0,96,64,1,WHITE);
  TinyDisplay.print("BRIGHT!");
  delay(500);
  TinyDisplay.fontColor(WHITE,BLACK);
  TinyDisplay.setCursor(30,20);
  TinyDisplay.clearWindow(0,0,96,64);
  TinyDisplay.setBrightness(0);
  TinyDisplay.print("DIM!");
  delay(500);
  TinyDisplay.setBrightness(5);
  delay(500);

   return 1;
}  //sExerciseDisplay


int sSetupDisplay() {
   Serial << LOG0 <<" sSetupDisplay(): Call begin() for Wire and TinyDisplay"<< endl;
   Wire.begin();
   TinyDisplay.begin();
   TinyDisplay.setFont(liberationSans_16ptFontInfo);
   TinyDisplay.fontColor(WHITE,BLACK);
   TinyDisplay.setCursor(0,0);
   TinyDisplay.print("Hello World");

   //TinyDisplay.clearWindow()
   return 1;
}  //sSetupDisplay


int sSetupServo() {
   Serial << LOG0 <<" sSetupServo(): Begin"<< endl;
   if (bServoOn) {
      myservo.attach(sServoPin);
      sServoMove(asGearLocation[sCurrentGear]);
   }  //if(bServoOn)
   return 1;
} //sSetupServo


int sDrawStartScreen(void) {
   delay(sSplashDelay);
   return 1;
}  //sDrawStartScreen


int sDrawMainScreen(void) {
   return 1;
}  //sDrawMainScreen


int sDisplayText(int sXpixel, int sYpixel, int sFont, const char *pcText) {
   //sSwitchFont(sFont);
   //u8g.drawStr( sXpixel, sYpixel, pcText);
   return 1;
}  //sDisplayText


int sFormatLine(const char *format, ...)
   {
   va_list ap;
   va_start(ap, format);
   vsnprintf(szLineBuffer, sizeof(szLineBuffer), format, ap);
   va_end(ap);
   return 1;
   }  //sFormatLine


int sDisplayMainObjects(void) {
   if (true || bScreenChanged()) {
      //sDisplayButtons();
      sDisplayCurrentGear();
      sDisplayServoPos();
      //sDisplayGyro();
      sDisplayOdometer();
   }  //if(bScreenChanged())
   return 1;
}  //sDisplayMainObjects


int sDisplaySplash(void) {
  /*
#if 0
   sDisplayText(0, sLPixel(1), sFontNormal, "PowerShift");
   sDisplayText(0, sLPixel(2), sFontNormal, "  by ShiftE");
   //Lines in normal font
   sDisplayText(20, sLPixel(5), sFontNormal, szSplashLine1);
   sDisplayText(0, sLPixel(6), sFontNormal, szSplashLine2);
   sDisplayText(0, sLPixel(7), sFontNormal, szSplashLine3);
#endif
   sDisplayText(10, 5, sFontBig, szSplashLine4);
   sDisplayText(10, 20, sFontBig, szSplashLine5);

   sDisplayText(8, 37, sFontNormal, szSplashLine1);
   sDisplayText(6, 46, sFontNormal, szSplashLine2);
   sDisplayText(6, 55, sFontNormal, szSplashLine3);
   */
   return 1;
}  //sDisplaySplash


int sDisplayCurrentGear() {
   //sint sGearFont  = sFontGearNum;
   int sLeftPixel = 4;
   int sTopPixel  = 6;
   if (sCurrentMode == sNormalMode) {
      itoa(sCurrentGear, sz10CharString, 10);
      //sDisplayText(sLeftPixel, sTopPixel, sGearFont, sz10CharString);
   }  //if (sCurrentMode..
   else {
      //We're in Calib mode so let's put a zero for the gear.
      //sDisplayText(sLeftPixel, sTopPixel, sGearFont, "0");
   }  //if (sCurrentMode..else

   return 1;
}  //sDisplayCurrentGear


int sDisplayServoPos() {
   strcpy(szLineBuffer, szServo);
   itoa(sServoPosLast, sz10CharString, 10);
   strcat(szLineBuffer, sz10CharString);
   //sDisplayText(50, sLPixel(1), sFontNormal, szLineBuffer);
   //sDisplayText(35, sLPixel(1), sFontBig, szLineBuffer);
   //sDisplayText(35, 4, sFontBig, szLineBuffer);
   return 1;
}  //sDisplayServoPos


int sDisplayOdometer() {
   return 1;
}  //sDisplayOdometer


int sDisplayButtons() {
   //Show 3 lines at right bottom for U d, D d, S d
   //String will be 3 char long => 18 pixels, start so 2 pixels remain on right
   strcpy(szLineBuffer, "U ");
   itoa(sButtonCount[sUp]  ,sz10CharString  , 10);
   strcat(szLineBuffer, sz10CharString);
   //sDisplayText(5, 82, sFontNormal, szLineBuffer);
   //sDisplayText(82, sLPixel(5), sFontNormal, szLineBuffer);

   strcpy(szLineBuffer, "D ");
   itoa(sButtonCount[sDown]  ,sz10CharString  , 10);
   strcat(szLineBuffer, sz10CharString);
   //sDisplayText(82, sLPixel(6), sFontNormal, szLineBuffer);

   strcpy(szLineBuffer, "S ");
   itoa(sButtonCount[sSelect]  ,sz10CharString  , 10);
   strcat(szLineBuffer, sz10CharString);
   //sDisplayText(82, sLPixel(7), sFontNormal, szLineBuffer);
   return 1;
}  //sDisplayButtons


int sLPixel(int sLineNumber) {
   //Converts line number to pixels at top
   int sPixel= constrain((((sLineNumber - 1) * sLineSpace) - 1), 0, 64);
   //Serial << sLC++ << " sLPixel(): Convert " << sLineNumber << " to " << sPixel << endl;
   return sPixel;
}  //sLPixel


int sFillGearLocations(void) {
   for (int sGear=0; sGear <= sNumGears; sGear++) {
      asGearLocation[sGear]= asDefaultGearLocation[sGear];
   }  //for
   return 1;
}  //sFillGearLocations


boolean bScreenChanged() {
   //Determine if something being displayed has changed & clear the flags.
#if USE_GYRO
   boolean bChanged= bGearChanged || bButtonsChanged || bServoChanged ||
                     bModeChanged || bGyroChanged;
   bGearChanged= bButtonsChanged= bServoChanged= bModeChanged= bGyroChanged= false;
#else
   boolean bChanged= bGearChanged || bButtonsChanged || bServoChanged || bModeChanged;
   bGearChanged= bButtonsChanged= bServoChanged= bModeChanged= false;
#endif  //USE_GYRO
   return bChanged;
}  //bScreenChanged


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
         Serial << sLC++ << " bHandleBothHeld(): Both buttons are being held." << endl;
         //Clear the button counts;
         sButtonCount[sUp]  = 0;
         sButtonCount[sDown]= 0;
         switch (sCurrentMode) {
            case sNormalMode:
               sCurrentMode= sCalibMode;
               bModeChanged= true;
               Serial << sLC++
                      << " bHandleBothHeld(): Switch from Normal to Calib mode." << endl;
               break;
            case sCalibMode:
               sCurrentMode= sNormalMode;
               bModeChanged= true;
               Serial << sLC++
                      << " bHandleBothHeld(): Switch from Calib to Normal mode." << endl;
               break;
            default:
               Serial << sLC++
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
   //boolean      bReturn= false;

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
      Serial << sLC++ << " sHandleNormalMode(): Button" << sButton << ", Count= "
             << sButtonCount[sButton] << ", sTargetChange= " << sTargetChange << endl;
      sButtonCount[sButton]--;
      bButtonsChanged= true;
      }  //if((sButtonCount[sButton]!=sHoldCode)...
   } //for

   sNewGear= constrain(sCurrentGear + sTargetChange, 1, sNumGears);
   if (sNewGear != sCurrentGear) {
      Serial <<sLC++<<" sHandleNormalMode(): Current gear= "<< sCurrentGear
             <<", New= "<< sNewGear<< endl;
      sCurrentGear= sNewGear;
      sTargetLocation= asGearLocation[sCurrentGear];

      //Make the actual shift
      sServoMove(sTargetLocation);
   }  //if(sNewGear!=sCurrentGear)
   return 1;
}  //sHandleNormalMode


int sHandleCalibMode(void) {
   int          sButton;
   int          sDirection;
   int          sTarget;

   for (sButton= sUp; sButton <= sDown; sButton++) {
      //The Up button lowers the servo position, gear 9 servo position< gear 1 position
      if (sButton == sUp) {
         sDirection= -1;
      }
      else {
         sDirection= 1;
      }
      //if (sButtonCount[sButton] == sHoldCode) {
      if (abButtonBeingHeld[sButton]) {
         sTarget= sServoPosLast + sDirection * sHoldDeltaPos;
         Serial << sLC++ << " sHandleCalibMode(): Button" << sButton
                << ", Count= " << sButtonCount[sButton] << ", Target= " << sTarget << endl;
         sServoMove(sTarget);
         bServoChanged= true;
      }  //if(sButtonCount[sButton]==sHoldCode)

      //if ((sButtonCount[sButton] != sHoldCode) && (sButtonCount[sButton] > 0)) {
      if (!abButtonBeingHeld[sButton] && (sButtonCount[sButton] > 0)) {
         sTarget= sServoPosLast + sDirection * sTrimDeltaPos;
         Serial << sLC++ << " sHandleCalibMode(): Button" << sButton
                << ", Count= " << sButtonCount[sButton] << ", Target= " << sTarget << endl;
         sServoMove(sTarget);
         bServoChanged= true;
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
         Serial << sLC++ << " sCheckButtons(): Button " << sButton
                << " was released." << endl;
         //Check to see if button is being held
         //if (sLocalButtonState[sButton] != sButtonHeld) {
         if ( !abButtonBeingHeld[sButton]) {
            if (sButtonCount[sButton] < sMaxButtonPresses) {
               //Increment the count of button presses to be handled.
               Serial << sLC++ << " sCheckButtons(): Button " << sButton
                      << " count incremented." << endl;
               sButtonCount[sButton]++;
               bButtonsChanged= true;
            } //if(sLocalUpButtonState!=sButtonHeld)
         }    //if(sLocalButtonState!=...
         else {
            //The button was being held down, update the state variable..
            Serial << sLC++ << " sCheckButtons(): Button " << sButton
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
         Serial << sLC++ << " sCheckButtons(): Button " << sButton
                << " being held." << endl;
         //Set state to indicate in hold.
         //sLocalButtonState[sButton]= sButtonHeld;
         //sButtonCount[sButton]= sHoldCode;
         abButtonBeingHeld[sButton] = true;
         sButtonCount[sButton]      = 0;  //Clear the count
         bButtonsChanged= true;
      }   //if(!bReturn&&UpButton.IsHold()
      bReturn= false;
   } //for (sButton= sUp...
  return 1;
}  //sCheckButtons


int sServoMove(int sServoPos) {
  if (sServoPos != sServoPosLast) {
      sServoPos= constrain(sServoPos, sServoMin, sServoMax);
      Serial << "sServoMove(): Move to " << sServoPos << endl;
      sServoPosLast= sServoPos;
      sServoSetPosition(sServoPos);
      bServoChanged= true;
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
   bServoChanged= true;
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

/*
//Move to bottom
#include <SPI.h>
#include <U8glib.h>
const int MPU= 0x68;  // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

#define APPLY_SMOOTHING    false
#if APPLY_SMOOTHING
  //#define FILTER_NAME        SMA
  //#define FILTER_FUNC        sma_filter
#endif
#define USE_GYRO        false

#if USE_DOGS102
//U8glibs constructor for DOGS102-6 (sometimes called 1701) display
U8GLIB_DOGS102 u8g(13, 11, 10, 9, 8);     // SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
#endif


//#define USE_DOGS102     false
#undef USE_DOGS102
//Constants for DOGS102 display.
static const int       sDisplayWidth        = 102;
static const int       sDisplayHeight       = 64;

static const int       sDisplayNormal       = 0xC0;
static const int       sDisplayFlipped      = 0xC8;

static const int       sBrightness100       = 255;
static const int       sBrightness75        = 192;
static const int       sBrightness50        = 128;
static const int       sBrightness25        =  64;
static const int       sBrightness0         =   0;
static const int       sDefaultBrightness   = sBrightness100;

static const int       sFontNotSet         =   0;
static const int       sFontNormal         =   1;
static const int       sFontBig         =   2;
static const int       sFontGearNum        =   3;
static const int       sFontSquare         =   4;

#if USE_GYRO
static const char      szAccel[]            = {"Accel"};
static const char      szGyro[]             = {"Gyro"};
#endif
#if APPLY_SMOOTHING
   static UINT16  *pusSmoothingMemory[sNumGyroTypes][sNumAxis];
#endif
#if USE_GYRO
  static int     asGyro             [sNumGyroTypes][sNumAxis];
#endif
#if USE_GYRO
static boolean     bGyroChanged             = false;
#endif

#if USE_GYRO
static unsigned long    ulNextGyroTime       = 0;  //msec when the gyro will be read
#endif

int sSetupDogsDisplay() {
   Serial << sLC++ <<"sSetupDogsDisplay(): Begin"<< endl;
   Serial << sLC++ <<"sSetupDogsDisplay(): Set Contrast to "<< ucContrast << endl;
   u8g.setContrast(ucContrast);
   sSwitchFont(sFontNormal);
   u8g.setColorIndex(1);
   if (bFlipDisplay) {
      u8g.setRot180();
   }  //if(bFlipDisplay)

   //Set backlight pin to be a PWM "analog" out pin.
   //Drive LED backlight through 15 ohm resistor.
   pinMode(sBacklightPin, OUTPUT);
   sDisplaySetBrightness(sDefaultBrightness);
   return 1;
}  //sSetupDogsDisplay


int sTestContrast() {
   for (int sContrast= 50; sContrast <= 125; sContrast += 25) {
      Serial << sLC++ <<"sTestContrast(): Contrast= "<< sContrast << endl;
      u8g.setContrast(sContrast);
      delay(1000);
   }  //for(int sContrast=0...
   return 1;
}  //sTestContrast


int sDisplayText(int sXpixel, int sYpixel, int sFont, const char *pcText) {
   sSwitchFont(sFont);
   u8g.drawStr( sXpixel, sYpixel, pcText);
   return 1;
}  //sDisplayText


int sSwitchFont(int sFont) {
   if (sFont != sCurrentFont) {
      switch (sFont) {
         case sFontNormal:
            u8g.setFont(u8g_font_5x7);
            break;
         case sFontBig:
            //u8g.setFont(u8g_font_fub11n);
            u8g.setFont(u8g_font_7x13B);
            break;
         case sFontGearNum:
            u8g.setFont(u8g_font_fub35n);
            break;
         case sFontSquare:
            break;
         default:
            Serial << "sSwitchFont(): Bad case in switch()= " << sFont << endl;
            break;
      }  //switch
      //Set the reference position for the font.
      u8g.setFontRefHeightExtendedText();
      u8g.setFontPosTop();
   }  //if(sFont!=sCurrentFont)
   return 1;
}  //sSwitchFont


int sDisplaySetBrightness(int sBrightness){
   analogWrite(sBacklightPin, sBrightness);
   return 1;
}  //sDisplaySetBrightness


int sDrawStartScreen(void) {
   //u8g.undoRotation();
   //u8g.setRot180();
   u8g.firstPage();
   do {
      sDisplaySplash();
   } while(u8g.nextPage());

   delay(sSplashDelay);
   return 1;
}  //sDrawStartScreen


int sDrawMainScreen(void) {
   //u8g.undoRotation();
   //u8g.setRot180();
   u8g.firstPage();
   do {
      sDisplayMainObjects();
   } while(u8g.nextPage());
   return 1;
}  //sDrawMainScreen
#endif  //USE_DOGS102


#if USE_GYRO
int sDisplayGyro() {
   int sXPixel   = 40;
   int sStartLine= 3;
   char szLocalBuffer[20];
   strcpy(szLineBuffer, szAccel);
   strcat(szLineBuffer, "  ");
   strcat(szLineBuffer, szGyro);
   sDisplayText(sXPixel, sLPixel(sStartLine++), sFontNormal, szLineBuffer);
   for (int sAxis= sXAxis; sAxis < sNumAxis; sAxis++) {
      sFormatLine("%5d %5d", sScaleGyro(asGyro[sAccel][sAxis]),
                             sScaleGyro(asGyro[sRotation][sAxis]));
      sDisplayText(sXPixel, sLPixel(sStartLine++), sFontNormal, szLineBuffer);
   }  //for sAxis
   return 1;
}  //sDisplayGyro


int sScaleGyro(int sNumber) {
   long wNumber= sNumber;
   wNumber= (wNumber * 2000) / 32767;
   int sScaleNumber= wNumber;
   return sScaleNumber;
}  //sScaleGyro


int sDegF(int sGyroReading) {
   float fGyroF= ((1.8 * sGyroReading) / 340.0) + 103.13;
   int sGyroF= fGyroF;
   return sGyroF;
}  //sDegF


int sSetupGyro() {
   Serial << sLC++ <<"sSetupGyro(): Begin"<< endl;
   //Set up the I2C bus.
   Wire.begin();
   Wire.beginTransmission(MPU);
   Wire.write(0x6B);  // PWR_MGMT_1 register
   Wire.write(0);     // set to zero (wakes up the MPU-6050)
   Wire.endTransmission(true);
   //Initialize the data array.
   for (int sDataType= sAccel; sDataType < sNumGyroTypes; sDataType++) {
      for (int sAxis= sXAxis; sAxis < sNumAxis; sAxis++) {
         asGyro[sDataType][sAxis]= 0;
      }  //for sDataType
   }  //for sAxis
   return 1;
}  //sSetupGyro


int sGyroLoop() {
   int      asGyroReading[sNumGyroTypes][sNumAxis];
   boolean  bApplySmoothing= APPLY_SMOOTHING;

   if (millis() > ulNextGyroTime) {
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
      Wire.endTransmission(false);
      Wire.requestFrom(MPU,14,true);  // request a total of 14 registers

      // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
      // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
      // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
      // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
      // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
      // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
      // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
      asGyroReading[sAccel][sXAxis]= Wire.read()<<8|Wire.read();
      asGyroReading[sAccel][sYAxis]= Wire.read()<<8|Wire.read();
      asGyroReading[sAccel][sZAxis]= Wire.read()<<8|Wire.read();

      asGyroReading[sTemperature][sXAxis]= Wire.read()<<8|Wire.read();

      asGyroReading[sRotation][sXAxis]=Wire.read()<<8|Wire.read();
      asGyroReading[sRotation][sYAxis]=Wire.read()<<8|Wire.read();
      asGyroReading[sRotation][sZAxis]=Wire.read()<<8|Wire.read();

      //Initialize missing temperature fields.
      for (int sAxis= sYAxis; sAxis < sNumAxis; sAxis++) {
         asGyroReading[sTemperature][sAxis]= 0;
      }  //for

      //Apply low-pass filter to data
      for (int sDataType= sAccel; sDataType < sNumGyroTypes; sDataType++) {
         for (int sAxis= sXAxis; sAxis < sNumAxis; sAxis++) {
#if APPLY_SMOOTHING
            asGyro[sDataType][sAxis]= FILTER_FUNC(asGyroReading[sDataType][sAxis],
                                                  pusSmoothingMemory[sDataType][sAxis]);
#else
            asGyro[sDataType][sAxis]= asGyroReading[sDataType][sAxis];
#endif  //APPLY_SMOOTHING
         }  //for sDataType
      }  //for sAxis
      bGyroChanged= true;
      ulNextGyroTime= millis() + ulGyroReadTime;
   }  //if (millis()>ulNextGyroTime)
   return 1;
}  //sGyroLoop


int sSetupSmoothing() {
   //Initialize memory for data smoothing and set data fields to zero.
   for (int sDataType= sAccel; sDataType < sNumGyroTypes; sDataType++) {
      for (int sAxis= sXAxis; sAxis < sNumAxis; sAxis++) {
#if APPLY_SMOOTHING
         pusSmoothingMemory[sDataType][sAxis]  = ms_init(FILTER_NAME);
#endif
         asGyro            [sDataType][sAxis]  = 0;
      }  //for sDataType
   }  //for sAxis
   return 1;
}  //sSetupSmoothing
#endif  //USE_GYRO
*/

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