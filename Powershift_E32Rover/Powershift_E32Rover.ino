//#include <Arduino.h>
//#include <BeckLogLib.h>
#include <BeckMiniLib.h>
#include <BeckMPU6050_Gyro.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <WROVER_KIT_LCD.h>
#include <esp_wp1.h>
#include <esp_wp2.h>
#include <Wire.h>
#include <EasyButton.h>
#include <ESP32_Servo.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansOblique18pt7b.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

#define min(X, Y)     (((X) < (Y)) ? (X) : (Y))
#define PAUSE_DELAY   delay(2000)
#define RADIX_10      10
#define DO_BUTTONS    true
#define USE_DOGS102		false

//Here come the const's
static const String SketchName  = "Powershift_E32Rover.ino";
static const String FileDate    = "July 15, 2018, Lenny-a";
//static const int asDefaultGearLocation[]= {0, 122, 101, 74, 68, 56, 35, 20};   //9-spd cogs 3-9
//static const int asDefaultGearLocation[]= {0, 152, 137, 122, 101, 74, 68, 56, 35, 27, 20};  //10-speed estimates
//static const int asDefaultGearLocation[]= {0, 168, 135, 113, 101, 79, 66, 58, 41, 29, 14};  //10-speed
static const int asDefaultGearLocation[]= {0, 135, 113, 101, 81, 68, 58, 41, 29, 14}; //10-speed. cogs 2-10
static const int       sServoMin             = 0;
static const int       sServoMax             = 180;
static const int       sServoMsecWait        = 15;
static const int       sNumGears             = 9;
static const bool      bServoOn              = true;
static const int       sHoldDeltaPos         = 2; //Servo move size when button held.
static const int       sTrimDeltaPos         = 1; //Servo move size when button clicked.

static const int       sNormalMode           = 0;
static const int       sCalibMode            = 1;

static const int       sUp                   = 0;
static const int       sDown                 = 1;
//static const int       sSelect               = 2;
//static const int       sFirstButton          = sUp;
//static const int       sLastButton           = sDown;

static const boolean   bButtonPullUp         = true;

//Digital Pins
#if 1   //ESP32
//static const int       sServoPin             = 12;
static const int       sUpButton             =  2;
static const int       sDownButton           =  4;
static const int       sI2C_SDA              = 26;
static const int       sI2C_SCL              = 27;
#endif

#if 0   //Pro Mini
//static const int       sSelectButton         = A3;
static const int       sDownButton           = A2;
static const int       sUpButton             = A1;
static const int       sServoPin             =  7;
static const byte      cSPICmdDataPin        =  9;
static const byte      cSPIChipSelectPin     = 10;
#endif

#if 0   //NodeMCU
//static const int       sServoPin             =  9;    //Pin 9, SD2
//static const int       sServoPin             =  4;    //Pin 4, D2, I2C SDA
//static const int       sServoPin             =  1;    //Pin 1, D10, TXD0
static const int       sServoPin             =  3;    //Pin 3, D9, RXD0
static const int       sDownButton           =  2;    //Pin 2, D4
static const int       sUpButton             =  0;    //Pin 0, D3
static const byte      cSPICmdDataPin        = 16;    //Pin 16, D0
//static const byte      cSPICmdDataPin        =  5;    //Pin 5, D1, ILI9341 display article
static const byte      cSPIChipSelectPin     = 15;    //Pin 15, D8
//static const byte      cSPIChipSelectPin     =  4;    //Pin 4, D2, ILI9341 display article
static const byte      cSPI_MOSI_Pin         = 13;    //Pin 13, D7
static const byte      cSPI_CLK_Pin          = 14;    //Pin 14, D5
#endif

static const int       sAccel             = 0;
//static const int       sRotation          = 1;
//static const int       sTemperature       = 2;
static const int       sNumGyroTypes      = 3;

//Constants used locally for state in sCheckButtons
/*
static const int       sButtonOpen        = 0;
static const int       sButtonPressed     = 1;
static const int       sButtonClosed      = 2;
static const int       sButtonReleased    = 3;
static const int       sButtonHeld        = 4;

static const int       sDefaultFont       =  0;
*/
static const int       sMaxButtonPresses  = 10;

static const unsigned long    ulModeSwitchTime  = 1000;  //Minimum msecs between mode changes
static const unsigned long    ulModeWaitTime    = 2000;  //Minimum msecs before mode is on
//static const unsigned long    ulReadTimeSpacing    = 500;   //Gyro and ADC reads spaced by this.
static const unsigned long    ulReadTimeSpacing    = 2000;   //Gyro and ADC reads spaced by this.

#if USE_DOGS102
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

static const int       sFontNormal          =   1;     //6x8
static const int       sFontBig             =   2;     //8x16
static const int       sFontBigNum          =   3;     //16x32nums
static const int       sFontSquare          =   4;     //8x8

static const int       sFontSize1           =   1;
static const int       sFontSize2           =   2;
static const int       sFontSize3           =   3;
static const int       sFontSize4           =   4;
static const int       sFontSize5           =   5;
#endif	//USE_DOGS102

//static const byte      cBogusResetPin       = 4;
//static const byte      cHW_SPI              = 0;      //This is what their demo used.
static const uint16_t  usBackgroundColor    = WROVER_BLACK;
static const UINT16    usBoostTop           = 90;
static const UINT16    usAccelMotorTop      = 135;
static const UINT16    usMotorLeft          = 130;
static const INT16     sPCF8591             = 0x48;  // I2C address of the PCF8591 A/D DAC
//End of the const's

enum AnalogIndex {
  eBatteryVolts= 0,
  eThermistor,
  eBatteryAmps,
  eThumbThrottle,
  eLastAnalogIndex
};

static byte acRawAnalogValue[eLastAnalogIndex];

static int asGearLocation[sNumGears + 1];
static int sCurrentGear                   = 2;

static int asGyro[sNumGyroTypes][sNumAxis];

static int sCurrentMode                   = sNormalMode;
static int sServoPosLast                  = 0;

WROVER_KIT_LCD    RoverLCD;

//Create servo object to control the servo
Servo             myservo;

#if DO_BUTTONS
//Create EasyButton objects to handle button presses.
EasyButton UpButton     (sUpButton,     NULL, CALL_NONE, bButtonPullUp);
EasyButton DownButton   (sDownButton,   NULL, CALL_NONE, bButtonPullUp);
//EasyButton SelectButton (sSelectButton, NULL, CALL_NONE, bButtonPullUp);
#endif  //DO_BUTTONS

//Number of unhandled presses, up to sMaxButtonPresses
static int              sButtonCount[]       = { 0, 0, 0};
static boolean          abButtonBeingHeld[]  = { false, false, false};
static unsigned long    ulNextModeTime       = 0;  //msec when a mode switch can take place
static unsigned long    ulModeReadyTime      = 0;  //msec when button presses can be handled
static unsigned long    ulNextReadTime       = 0;  //msec when the gyro will be read

//State of display items being changed and needing refresh.
static boolean     bButtonsChanged          = true;
static boolean     bGearChanged             = true;
static boolean     bServoChanged            = true;
static boolean     bModeChanged             = true;
static boolean     bGyroChanged             = true;
static boolean     bPitchChanged            = true;
static boolean     bBoostChanged            = true;
static boolean     bMotorChanged            = true;
static boolean     bMPHChanged              = true;

static int         sLineCount= 0;     //Used in outputs to Serial Monitor for clarity.

static char       szTempBuffer[100];   //DOGS102 line is 17 chars with 6x8 normal font.
static char       sz100CharString[101];

//wMPU6050 acceleration and pitch things
//const double      dGConvert         = 16384.0;  //wMPU6050 16-bit +/- 2G Full scale
const double      dGConvert         = 16383.0;  // 12/06/17 I'm seeing values up to -16,580 with a vertical orientation, 0x3FFF = 16,383
const double      dRadsToDeg        = 180.0/PI;
double            adGvalueXYZ[3];
double            dRollDeg;
double            dPitchDeg;
double            dPitchPercent   = 0.0;
double            dBoostWatts     = 0.0;
double            dMPH            = 0.0;
double            dMotorVolts     = 0.0;
double            dMotorAmps      = 0.0;
double            dMotorWatts     = 0.0;

void(* ResetESP32)(void)= 0;        //Hopefully system crashes and reset when this is called.

// The Arduino setup() method runs once, when the sketch starts
void setup()   {
  Serial.begin(115200);
  Serial << endl << "setup(): Begin " << SketchName << ", " << FileDate << endl;
  Serial << "setup(): Call Wire.begin(sI2C_SDA, sI2C_SCL) " << sI2C_SDA << ", " << sI2C_SCL << endl;
  Wire.begin(sI2C_SDA, sI2C_SCL);
  FillGearLocations();
  SetupGyro();
  //SetupPCF8591();
  ServoInit();
  DisplayBegin();
  bButtonsChanged= true;  //Make the display show up during debugging.
  return;
}  //setup


// The Arduino loop() method gets called over and over.
void loop() {
  CheckButtons();
  if (millis() > ulNextReadTime) {
  	ReadAtoD();
    ReadAccel();
    ulNextReadTime= millis() + ulReadTimeSpacing;
  } //if (millis()>ulNextGyroTime)
  DisplayUpdate();
  HandleButtons();
  CheckKeyboard();
  return;
}  //loop()


void ServoInit() {
  Serial << "ServoInit(): Check bServoOn" << endl;
  if (bServoOn) {
    Serial << "ServoInit(): Call myservo.attach() for pin " << sServoPin << endl;
    myservo.attach(sServoPin);
  }  //if(bServoOn)
  Serial << "ServoInit(): Call sServoMove()" << endl;
  ServoMove(asGearLocation[sCurrentGear]);
  return;
} //ServoInit


void ServoMove(int sServoPos) {
  if (sServoPos != sServoPosLast) {
      sServoPos= constrain(sServoPos, sServoMin, sServoMax);
      sServoPosLast= sServoPos;
      ServoSetPosition(sServoPos);
      bServoChanged= true;
  }  //if(sServoPos...
  return;
} //ServoMove


//ServoDither() moves the servo back and forth and back to where it started.
void ServoDither(int sDitherSize, int sNumTimes){
   int sStartPosition = sServoPosLast;
   int sFirstPosition = sStartPosition + sDitherSize;
   int sSecondPosition= sStartPosition - sDitherSize;
   int sCycle;

   for (sCycle= 0; sCycle < sNumTimes; sCycle++){
      ServoMove(sFirstPosition);
      ServoMove(sSecondPosition);
   }  //for(sCycle...
   ServoMove(sStartPosition);
   bServoChanged= true;
   return;
}  //ServoDither


void ServoSetPosition(int sServoPos) {
   //Note that values of sServoPos less than 200 are considered degrees and
   //values greater are uSec pulse width by myservo.write().
   if (bServoOn) {
      myservo.write(sServoPos);
   }
   else {
     Serial << "sServoSetPosition():bServoOn is false, skip move to " << sServoPos << endl;
   }
   delay(sServoMsecWait);
   return;
}  //ServoSetPosition


void CheckKeyboard() {
  if (Serial.available()) {
    Serial <<" CheckKeyboard(): Character available, call Serial.read()" << endl;
    char cChar= Serial.read();
    int sChar= cChar;
    if (sChar != 13) {
      Serial <<" CheckKeyboard(): Character read= "<< cChar <<", "<< sChar << endl;
    }
    else {
      Serial <<" CheckKeyboard(): Character read= CR" << endl;
    }
    switch (cChar) {
      case 'r':
      case 'R':
        ResetESP32();
        break;
      case 'u':
      case 'U':
        sButtonCount[sUp]++;
        break;
      case 'd':
      case 'D':
        sButtonCount[sDown]++;
        break;
      default:
        break;
    } //switch
  } //if(Serial.available()
  return;
}  //CheckKeyboard


void DisplayBegin() {
  Serial << "DisplayBegin(): Call RoverLCD.begin()" << endl;
  RoverLCD.begin();
  RoverLCD.setRotation(1);
  DisplayClear();
  return;
}  //DisplayBegin


void ShowStartScreen(void) {
  Serial << "ShowStartScreen(): Call DisplayBegin()" << endl;
  DisplayBegin();
  Serial << "ShowStartScreen(): Call sShowSplash()" << endl;
  ShowSplash();
  delay(3000);
  DisplayClear();
  return;
}  //ShowStartScreen


void ShowSplash(void) {
   DisplayClear();
/*
   //2 lines of big font takes lines 0-3
   RoverLCD.setTextColor(WROVER_YELLOW);
   sDisplayText(0, 0, sFontSize3, (char *)"PowerShift");
   RoverLCD.setTextColor(WROVER_GREEN);
   sDisplayText(2, 0, sFontSize3, (char *)"  by ShiftE");

   //2 lines in normal font
   RoverLCD.setTextColor(WROVER_RED);
   sDisplayText(5, 0, sFontSize2, (char *)"\nAlways ride safe!");
   //sDisplayText(7, 0, sFontNormal, (char *)"**Larry & Candy**");
   sDisplayText(7, 0, sFontSize2, (char *)"October 19, 2017J ");
   //sDisplayText(7, 0, sFontSize2, (char *)FileDate);
*/
   return;
}  //ShowSplash


void DisplayUpdate(void) {
 if (bScreenChanged()) {
    DisplayCurrentGear();
    DisplayServoPos();
    DisplayGs();
    DisplayPitch();
    DisplayBoost();
    DisplayMotor();
    DisplayMPH();
    //DisplayButtons();
    DisplayLowerBanner();
    ClearChangeFlags();
   } //if(bScreenChanged())
   return;
}  //DisplayUpdate


void DisplayClear() {
  FillScreen(usBackgroundColor);
  return;
}  //DisplayClear


void FillScreen(UINT16 usColor) {
  RoverLCD.fillScreen(usColor);
  return;
}  //FillScreen


bool bScreenChanged() {
   //Determine if something being displayed has changed & clear the flags.
   bool bChanged= bGyroChanged  || bPitchChanged || bGearChanged  || bButtonsChanged || bServoChanged || bModeChanged ||
                  bBoostChanged || bMotorChanged || bMPHChanged;
   return bChanged;
}  //bScreenChanged


void ClearChangeFlags(){
  bGyroChanged= bPitchChanged= bGearChanged  = bButtonsChanged= bServoChanged= bModeChanged=
                bBoostChanged= bMotorChanged = bMPHChanged= false;
  return;
} //ClearChangeFlags


void DisplayText(UINT16 usCursorX, UINT16 usCursorY, char *pcText,
                 const GFXfont *pFont, UINT8 ucSize, UINT16 usColor) {
  //240x320 3.2", 10 lines => 24 pixels/line
  RoverLCD.setFont(pFont);
  RoverLCD.setTextColor(usColor);
  RoverLCD.setTextSize(ucSize);
  RoverLCD.setTextWrap(false);
  RoverLCD.setCursor(usCursorX, usCursorY);
  RoverLCD.print(pcText);
  return;
}  //DisplayText


void ClearTextBackground(INT16 sUpperLeftX, INT16 sUpperLeftY, UINT16 usWidth, UINT16 usHeight){
  RoverLCD.fillRect(sUpperLeftX, sUpperLeftY, usWidth, usHeight, usBackgroundColor);
  return;
} //ClearTextBackground


void DisplayLine(const GFXfont stFont, UINT16 usColor, UINT16 usCursorX, UINT16 usCursorY, UINT16 usClearWidth, UINT16 usClearHeight,
                 char szText[], bool bClearText= true, UINT8 ucSize= 1) {
  INT16           sClearXstart    = usCursorX - 10;
  INT16           sClearYstart    = usCursorY - 18;

  if(bClearText){
    ClearTextBackground(sClearXstart, sClearYstart, usClearWidth, usClearHeight);
  }
  DisplayText( usCursorX, usCursorY, szText, &stFont, ucSize, usColor);
  return;
} //DisplayLine


void DisplayPitch() {
  UINT16          usCharWidth     = 25;
  UINT16          usCursorX       = 0;
  UINT16          usCursorY       = 30;   //GFX fonts Y is bottom
  UINT8           ucSize          = 1;
  UINT16          usColor         = WROVER_WHITE;
  UINT16          usRightInset    = 2;  //Number of pixels to right of justified text
  INT16           sClearLeftX     = usCursorX;
  INT16           sClearTopY      = 0;
  UINT16          usClearWidth    = 120;
  UINT16          usClearHeight   = 35;
  static UINT16   usLastClearWidth= 0;

  //Serial << "DisplayPitch(): Begin" << endl;
  if(bPitchChanged) {
    sprintf(szTempBuffer, "%+4.1f%%", dPitchPercent);
    //sprintf(szTempBuffer, "%+4.1f %+03.0f", dPitchPercent, dPitchDeg);
    //Calculate width to clear based on number of characters + 2, use that unless last width was bigger
    usClearWidth= (strlen(szTempBuffer) + 2) * usCharWidth;
    usClearWidth= std::max(usClearWidth, usLastClearWidth);
    usLastClearWidth= usClearWidth;
    ClearTextBackground(sClearLeftX, sClearTopY, usClearWidth, usClearHeight);
    DisplayLine(FreeMonoBold24pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer, false, ucSize);

    usCursorX= 50;
    usCursorY += 20;
    sprintf(szTempBuffer, "Pitch");
    DisplayLine(FreeSans9pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer, false);
  } //if(bPitchChanged)
  return;
}  //DisplayPitch


void DisplayBoost() {
  UINT16          usCharWidth     = 25;
  UINT16          usCursorX       = 0;
  UINT16          usCursorY       = usBoostTop;   //GFX fonts Y is bottom 90
  UINT8           ucSize          = 1;
  UINT16          usColor         = WROVER_WHITE;
  UINT16          usRightInset    = 2;  //Number of pixels to right of justified text
  INT16           sClearLeftX     = usCursorX;
  INT16           sClearTopY      = usCursorY - 32;
  UINT16          usClearWidth    = 120;
  UINT16          usClearHeight   = 40;
  static UINT16   usLastClearWidth= 0;

  //Serial << "DisplayBoost(): Begin" << endl;
  if(bBoostChanged) {
    sprintf(szTempBuffer, "%3.0f W", dBoostWatts);
    //Calculate width to clear based on number of characters + 2, use that unless last width was bigger
    usClearWidth= (strlen(szTempBuffer) + 2) * usCharWidth;
    usClearWidth= std::max(usClearWidth, usLastClearWidth);
    usLastClearWidth= usClearWidth;
    ClearTextBackground(sClearLeftX, sClearTopY, usClearWidth, usClearHeight);
    DisplayLine(FreeMonoBold24pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer, false, ucSize);

    usCursorX= 50;
    usCursorY += 20;
    sprintf(szTempBuffer, "Boost");
    DisplayLine(FreeSans9pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer, false);
  } //if(bBoostChanged)
  return;
}  //DisplayBoost


void DisplayCurrentGear() {
  UINT16          usCharWidth     = 120;
  UINT16          usCursorX       = 200;
  UINT16          usCursorY       = 62;   //GFX fonts Y is bottom
  UINT8           ucSize          = 2;
  UINT16          usRightInset    = 2;  //Number of pixels to right of justified text
  INT16           sClearLeftX     = usCursorX;
  INT16           sClearTopY      = 0;
  UINT16          usClearWidth    = 120;
  UINT16          usClearHeight   = usCursorY + 10;
  UINT16          usColor         = WROVER_WHITE;

  //Serial << "DisplayCurrentGear(): Begin" << endl;
  if(bGearChanged) {
    if (sCurrentMode == sNormalMode) {
      sprintf(szTempBuffer, "%2d", sCurrentGear);
      usClearWidth= strlen(szTempBuffer) * usCharWidth;
      ClearTextBackground(sClearLeftX, sClearTopY, usClearWidth, usClearHeight);
      DisplayLine(FreeMonoBold24pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer, false, ucSize);
    }  //if (sCurrentMode..
    else {
      sprintf(szTempBuffer, "%2d", 0);
      usClearWidth= strlen(szTempBuffer) * usCharWidth;
      ClearTextBackground(sClearLeftX, sClearTopY, usClearWidth, usClearHeight);
      DisplayLine(FreeMonoBold24pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer, false, ucSize);
    }  //if (sCurrentMode..else

    usCursorX= 255;
    usCursorY += 20;
    sprintf(szTempBuffer, "Gear");
    DisplayLine(FreeSans9pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer, false);
  } //if(bGearChanged)
  return;
}  //DisplayCurrentGear


void DisplayGs() {
  const UINT16    usCharWidth   = 17;
  const UINT16    usLineHeight  = 20;
  UINT16          usCursorX     = 0;
  UINT16          usCursorY     = usAccelMotorTop;
  UINT16          usClearWidth;
  UINT16          usClearHeight = 22;
  UINT16          usColor       = WROVER_YELLOW;

  //Serial << "DisplayGs(): Begin" << endl;
  sprintf(szTempBuffer, "G Accel");
  usClearWidth= strlen(szTempBuffer) * usCharWidth;
  DisplayLine(FreeMonoBold12pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer, false);

  sprintf(szTempBuffer, "X %+05.2f", adGvalueXYZ[sXAxis]);
  usClearWidth= strlen(szTempBuffer) * usCharWidth;
  usCursorY += usLineHeight;
  DisplayLine(FreeMonoBold12pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer);

  sprintf(szTempBuffer, "Y %+05.2f", adGvalueXYZ[sYAxis]);
  usClearWidth= strlen(szTempBuffer) * usCharWidth;
  usCursorY += usLineHeight;
  DisplayLine(FreeMonoBold12pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer);

  sprintf(szTempBuffer, "Z %+05.2f", adGvalueXYZ[sZAxis]);
  usClearWidth= strlen(szTempBuffer) * usCharWidth;
  usCursorY += usLineHeight;
  DisplayLine(FreeMonoBold12pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer);
  return;
}  //DisplayGs


void DisplayServoPos() {
  const UINT16    usCharWidth   = 17;
  const UINT16    usLineHeight  = 20;
  UINT16          usCursorX     = 252;
  UINT16          usCursorY     = 115;
  UINT16          usClearWidth  = 70;
  UINT16          usClearHeight = 22;
  UINT16          usColor       = WROVER_WHITE;

  //Serial << "DisplayServoPos(): Begin" << endl;
  if(bServoChanged) {
    sprintf(szTempBuffer, "%3d", sServoPosLast);
    usClearWidth= strlen(szTempBuffer) * usCharWidth;
    DisplayLine(FreeMonoBold12pt7b, usColor, usCursorX + 15, usCursorY, usClearWidth, usClearHeight, szTempBuffer);

    sprintf(szTempBuffer, "Servo");
    usClearWidth= strlen(szTempBuffer) * usCharWidth;
    usCursorY += usLineHeight;
    DisplayLine(FreeSans9pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer, false);
  } //if(bServoChanged)
  return;
}  //DisplayServoPos


void DisplayMPH() {
  const UINT16    usCharWidth   = 17;
  const UINT16    usLineHeight  = 20;
  UINT16          usCursorX     = 252;
  UINT16          usCursorY     = 175;
  UINT16          usClearWidth  = 70;
  UINT16          usClearHeight = 22;
  UINT16          usColor       = WROVER_YELLOW;

  //Serial << "DisplayMPH(): Begin" << endl;
  if(bMPHChanged) {
    sprintf(szTempBuffer, "%4.1f", dMPH);
    usClearWidth= strlen(szTempBuffer) * usCharWidth;
    DisplayLine(FreeMonoBold12pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer);

    sprintf(szTempBuffer, "MPH");
    usClearWidth= strlen(szTempBuffer) * usCharWidth;
    usCursorY += usLineHeight;
    DisplayLine(FreeSans9pt7b, usColor, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer, false);
  } //if(bMPHChanged)
  return;
}  //DisplayMPH


void DisplayMotor() {
  const UINT16    usCharWidth   = 17;
  const UINT16    usLineHeight  = 20;
  UINT16          usCursorX     = usMotorLeft;
  UINT16          usCursorY     = usAccelMotorTop;
  UINT16          usClearWidth;
  UINT16          usClearHeight = 22;

  //Serial << "DisplayMotor(): Begin" << endl;
  sprintf(szTempBuffer, "Motor");
  if(bMotorChanged) {
    usClearWidth= strlen(szTempBuffer) * usCharWidth;
    DisplayLine(FreeMonoBold12pt7b, WROVER_YELLOW, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer, false);

    sprintf(szTempBuffer, "V  %4.1f", dMotorVolts);
    usClearWidth= strlen(szTempBuffer) * usCharWidth;
    usCursorY += usLineHeight;
    DisplayLine(FreeMonoBold12pt7b, WROVER_YELLOW, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer);

    sprintf(szTempBuffer, "A  %4.1f", dMotorAmps);
    usClearWidth= strlen(szTempBuffer) * usCharWidth;
    usCursorY += usLineHeight;
    DisplayLine(FreeMonoBold12pt7b, WROVER_YELLOW, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer);

    //sprintf(szTempBuffer, "W %3.0f", dMotorWatts);
    sprintf(szTempBuffer, "W %4.1f", dMotorWatts);
    usClearWidth= strlen(szTempBuffer) * usCharWidth;
    usCursorY += usLineHeight;
    DisplayLine(FreeMonoBold12pt7b, WROVER_YELLOW, usCursorX, usCursorY, usClearWidth, usClearHeight, szTempBuffer);
  } //if(bMotorChanged)
  return;
}  //DisplayMotor


void DisplayButtons() {
  const GFXfont   *pFont          = &FreeSans9pt7b;
  UINT16          usCursorX       = 260;
  UINT16          usCursorY       = 160;
  UINT8           ucSize          = 1;
  UINT16          usColor         = WROVER_ORANGE;
  bool            bRightJustify   = false;

  //Show 2 lines at right bottom for U d, D d
  strcpy(szTempBuffer, "U ");
  itoa(sButtonCount[sUp]  ,sz100CharString  , 10);
  strcat(szTempBuffer, sz100CharString);
  DisplayText( usCursorX, usCursorY, szTempBuffer, pFont, ucSize, usColor);

  strcpy(szTempBuffer, "D ");
  itoa(sButtonCount[sDown]  ,sz100CharString  , 10);
  strcat(szTempBuffer, sz100CharString);
  usCursorY= usCursorY + 20;
  DisplayText( usCursorX, usCursorY, szTempBuffer, pFont, ucSize, usColor);
  return;
}  //DisplayButtons


void DisplayLowerBanner(){
  const GFXfont   *pFont          = &FreeSansOblique18pt7b;
  UINT16          usCursorX       = 7;
  UINT16          usCursorY       = 232;    //Was 72
  UINT8           ucSize          = 1;
  UINT16          usColor         = WROVER_CYAN;
  bool            bRightJustify   = false;

  DisplayText( usCursorX, usCursorY, "PowerShift Coach", pFont, ucSize, usColor);
  return;
} //DisplayLowerBanner


void FillGearLocations(void) {
   for (int sGear=0; sGear <= sNumGears; sGear++) {
      asGearLocation[sGear]= asDefaultGearLocation[sGear];
   }  //for
   return;
}  //sFillGearLocations


void ReadAtoD() {
	Serial << "ReadAtoD(): Call Wire.beginTransmission(" << sPCF8591 << ")" << endl;
  Wire.beginTransmission(sPCF8591);
  Serial << "ReadAtoD(): Begin"  << endl;
	Serial << "ReadAtoD(): Call Wire.write(0x04), turn on A/D"  << endl;
  Wire.write(0x04);
  Wire.endTransmission();
  //Serial << "ReadAtoD(): Call Wire.requestFrom(PCF8591, 5, stream 4 values"  << endl;
  Wire.requestFrom(sPCF8591, 5);

  //The first reading is the last value read, throw it away.
  byte cLastReading= Wire.read();
  acRawAnalogValue[eBatteryVolts]= Wire.read();
  acRawAnalogValue[eThermistor]= Wire.read();
  acRawAnalogValue[eBatteryAmps]= Wire.read();
  acRawAnalogValue[eThumbThrottle]= Wire.read();
  Serial << "ReadAtoD(): RawValues eBatteryVolts, eThermistor, eBatteryAmps, eThumbThrottle" << endl;
  for (int i= eBatteryVolts; i <= eThumbThrottle; i++){
  	Serial << "   ReadAtoD(): Index, RawAnalogVolts " << i << ", " << acRawAnalogValue[i] << endl;
  }//for

  //Compute battery volts and amps.
  // ACS712 current sense, 20A full scale, 2.5V +/-2.0V =  +/-20.0A, 0.5-4.5V
  // PCF8581 8bits, 0-3.3V
  // Amps= ((10.0 * CurrentSenseVolts) - 25.0)
  double dAtoDFullScale= 255.0;
  double dVoltsFullScale= 3.3;
  double dScaleVolts= 2.0;      //Reading is pulled off the middle of a even voltage divider
  double dCurrentSenseVoltsPerAmp= 0.100;
  double dCurrentSenseVoltsAtZeroAmps= 2.5;
  double dSlopeAmpsPerVolt= 10.0;
  double dZeroVoltsAmpOffset= -25.0;
  double dCurrentSenseVolts;
  //double dAmpsFullScale= (dVoltsFullScale - dCurrentSenseVoltsAtZeroAmps) /  dCurrentSenseVoltsPerAmp;

  //Compute actual battery volts.
  dMotorVolts= (((double)acRawAnalogValue[eBatteryVolts] / dAtoDFullScale) * dVoltsFullScale) * dScaleVolts;

  //Compute battery amps.
  dCurrentSenseVolts= ((double)acRawAnalogValue[eBatteryAmps] / dAtoDFullScale) * dVoltsFullScale;
  dMotorAmps= (dSlopeAmpsPerVolt * dCurrentSenseVolts) + dZeroVoltsAmpOffset;
  dMotorWatts= dMotorVolts * dMotorAmps;
  Serial << "ReadAtoD(): dMotorVolts= " << dMotorVolts << endl;
  Serial << "ReadAtoD(): dMotorAmps= " << dMotorAmps << endl;
  Serial << "ReadAtoD(): dMotorWatts= " << dMotorWatts << endl;
  bMotorChanged= true;
  return;
}  //ReadAtoD


void ReadAccel() {
  MPU6050_ReadGs(adGvalueXYZ, dGConvert);
  Serial << "ReadAccel(): G's X, Y, Z " << adGvalueXYZ[sXAxis] << ", " << adGvalueXYZ[sYAxis] << ", " << adGvalueXYZ[sZAxis] << endl;

  ComputePitchAndRoll();
  bGyroChanged= true;
  return;
}  //ReadAccel


void ComputePitchAndRoll() {
  dRollDeg = atan2((-adGvalueXYZ[sYAxis]), adGvalueXYZ[sZAxis]) * dRadsToDeg; //Probably has axis mixed up
/*
  dPitchDeg= atan2(adGvalueXYZ[sXAxis],
                sqrt(adGvalueXYZ[sYAxis] * adGvalueXYZ[sYAxis] +
                     adGvalueXYZ[sZAxis] * adGvalueXYZ[sZAxis])) * dRadsToDeg;
*/
  dPitchDeg= atan2(-adGvalueXYZ[sYAxis],
                   sqrt(adGvalueXYZ[sXAxis] * adGvalueXYZ[sXAxis] +
                        adGvalueXYZ[sZAxis] * adGvalueXYZ[sZAxis])) * dRadsToDeg;
  dPitchPercent= dGetPitchPercent(dPitchDeg);

  //Correct for current readings being 180 degrees off
  if (dRollDeg < 0.0) {
    //dRollDeg= dRollDeg= -180.0 - dRollDeg;
    dRollDeg= -180.0 - dRollDeg;
  } //if(dRollDeg_<0.0)
  else {
    dRollDeg= dRollDeg= 180.0 - dRollDeg;
  } //if(dRollDeg_<0.0)else
  bPitchChanged= true;
  Serial << "ComputePitchAndRoll(): Pitch Deg, Pitch%, Roll " << dPitchDeg << ", " << dPitchPercent << ", " << dRollDeg<< endl;
  return;
} //ComputePitchAndRoll


double dGetPitchPercent(double dPitchDeg) {
  double dPitchPercent= -99.99;
  //Serial << "dGetPitchPercent(): dPitchDeg_= " << dPitchDeg_ << endl;
  if ((dPitchDeg < 44.0) && (dPitchDeg > -44.0)) {
    dPitchPercent= 100.0 * tan(dPitchDeg / dRadsToDeg);
    //Serial << "dGetPitchPercent(): dPitchPercent_= " << dPitchPercent_ << endl;
  } //if((dPitchDeg_<44.0)&&...
  return dPitchPercent;
} //dGetPitchPercent


/*
void SetupPCF8591() {
   Serial << "SetupPCF8591(): Begin"<< endl;
   //Wire.begin(sI2C_SDA, sI2C_SCL);
   Wire.beginTransmission(sPCF8591);
   Serial << "SetupGyro(): Call MPU6050_PrintName()"<< endl;
   MPU6050_PrintName();

   // Clear the 'sleep' bit to start the sensor.
   MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);
   Wire.endTransmission(true);

   //Initialize the data array.
   for (int sDataType= sAccel; sDataType < sNumGyroTypes; sDataType++) {
      for (int sAxis= sXAxis; sAxis < sNumAxis; sAxis++) {
         asGyro[sDataType][sAxis]= 0;
      }  //for sDataType
   }  //for sAxis
   return;
}  //SetupPCF8591
*/


void SetupGyro() {
   Serial << "sSetupGyro(): Begin"<< endl;
   //BLog("SetupGyro(): Begin");
   //Wire.begin(sI2C_SDA, sI2C_SCL);
   Wire.beginTransmission(wMPU6050);
   Serial << "SetupGyro(): Call MPU6050_PrintName()"<< endl;
   MPU6050_PrintName();

   // Clear the 'sleep' bit to start the sensor.
   MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);
   Wire.endTransmission(true);

   //Initialize the data array.
   for (int sDataType= sAccel; sDataType < sNumGyroTypes; sDataType++) {
      for (int sAxis= sXAxis; sAxis < sNumAxis; sAxis++) {
         asGyro[sDataType][sAxis]= 0;
      }  //for sDataType
   }  //for sAxis
   return;
}  //SetupGyro


void HandleButtons(void) {
  if (!bHandleBothHeld()) {
     if (millis() > ulModeReadyTime) {
       switch(sCurrentMode) {
         case sNormalMode:
            HandleNormalMode();
            break;   //sNormalMode
         case sCalibMode:
            HandleCalibMode();
            break;   //sCalibMode
         default:
            Serial << "HandleButtons(): Unexpected switch value." << endl;
            break;
        } //switch
     }   //if(millis()...
   }  //if(!bHandleBothHeld())
   return;
}  //HandleButtons


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
               bModeChanged= true;
               Serial << sLineCount++
                      << " bHandleBothHeld(): Switch from Normal to Calib mode." << endl;
               break;
            case sCalibMode:
               sCurrentMode= sNormalMode;
               bModeChanged= true;
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


void HandleNormalMode(void) {
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
       //Serial << sLineCount++ << " sHandleNormalMode(): Button" << sButton << ", Count= " << sButtonCount[sButton] << ", sTargetChange= " << sTargetChange << endl;
       sButtonCount[sButton]--;
       bButtonsChanged= true;
    }  //if((sButtonCount[sButton]!=sHoldCode)...
  } //for

  sNewGear= constrain(sCurrentGear + sTargetChange, 1, sNumGears);
  if (sNewGear != sCurrentGear){
    sCurrentGear= sNewGear;
    sTargetLocation= asGearLocation[sCurrentGear];

    //Make the actual shift
    ServoMove(sTargetLocation);
    bGearChanged= true;
  } //if(sNewGear!=sCurrentGear)
  return;
}  //HandleNormalMode


void HandleCalibMode(void) {
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
         ServoMove(sTarget);
         bServoChanged= true;
      }  //if(sButtonCount[sButton]==sHoldCode)

      //if ((sButtonCount[sButton] != sHoldCode) && (sButtonCount[sButton] > 0)) {
      if (!abButtonBeingHeld[sButton] && (sButtonCount[sButton] > 0)) {
         sTarget= sServoPosLast + sDirection * sTrimDeltaPos;
         Serial << sLineCount++ << " sHandleCalibMode(): Button" << sButton
                << ", Count= " << sButtonCount[sButton] << ", Target= " << sTarget << endl;
         ServoMove(sTarget);
         bServoChanged= true;
         sButtonCount[sButton]--;
      }  //if((sButtonCount[sButton]!=sHoldCode)...
    } //for
 return;
}  //HandleCalibMode


void CheckButtons(void) {
/*Algorithm to determine when a button has been pressed or held.
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

#if DO_BUTTONS
  UpButton.update();
  DownButton.update();
  //SelectButton.update();

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
               //Serial << sLineCount++ << " sCheckButtons(): Button " << sButton << " count incremented." << endl;
               sButtonCount[sButton]++;
               bButtonsChanged= true;
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
      (((sButton == sUp)   && UpButton.IsHold  ()) ||
       ((sButton == sDown) && DownButton.IsHold()) )) {
         Serial << sLineCount++ << " sCheckButtons(): Button " << sButton
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
#endif  //DO_BUTTONS
  return;
}  //CheckButtons


void TestButtonPins() {
  int sUpButtonLocal    = 2;
  int sDownButtonLocal  = 4;
  Serial << "SetupPins(): Call pinMode(" << sUpButtonLocal << ", INPUT_PULLUP) for Up" << endl;
  pinMode(sUpButtonLocal, INPUT_PULLUP);

  Serial << "SetupPins(): Call pinMode(" << sDownButtonLocal << ", INPUT_PULLUP) for Down" << endl;
  pinMode(sDownButtonLocal, INPUT_PULLUP);

  int sValueUp;
  int sValueDown;
  for (int i= 0; i<10; i++){
    Serial << "SetupPins(): Read buttons, Try= " << i << endl;
    sValueUp= digitalRead(sUpButtonLocal);
    sValueDown= digitalRead(sDownButtonLocal);
    Serial << "SetupPins(): Read buttons, Pin "
        << sUpButtonLocal << "= " << sValueUp << ", Pin " << sDownButtonLocal <<  "= " << sValueDown << endl;
    delay(1000);
  } //for(inti= 0;...
  return;
}  //TestButtonPins
//Last line
