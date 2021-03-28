static const char szSketchName[]  = "Exercist.ino";
static const char szFileDate[]    = "Oct 1, 2017, Lenny-a";

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
#include <Arduino.h>
#include <Streaming.h>
#include <BeckLogLib.h>
#include <SPI.h>
#include <Wire.h>
#include <EasyButton.h>
#include <Servo.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <dog_1701.h>
#include <font_16x32nums.h>
#include <font_6x8.h>
#include <font_8x16.h>
#include <font_8x8.h>
#include <logo_BLH.h>

//using namespace std;

const int MPU= 0x68;  // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

//Here come the const's
//static const int  asDefaultGearLocation[]= {0, 150, 119, 92, 74, 64, 48, 17};
//static const int asDefaultGearLocation[]= {0, 122, 101, 74, 68, 56, 35, 20};   //9-spd cogs 3-9
//static const int asDefaultGearLocation[]= {0, 152, 137, 122, 101, 74, 68, 56, 35, 27, 20};	//10-speed estimates
//static const int asDefaultGearLocation[]= {0, 168, 135, 113, 101, 79, 66, 58, 41, 29, 14};	//10-speed
static const int asDefaultGearLocation[]= {0, 135, 113, 101, 81, 68, 58, 41, 29, 14};	//10-speed. cogs 2-10
static const int       sServoMin             = 0;
static const int       sServoMax             = 180;
static const int       sServoMsecWait        = 15;
static const int       sNumGears             = 9;
static const boolean   bServoOn              = true;
static const int       sHoldDeltaPos         = 2; //Servo move size when button held.
static const int       sTrimDeltaPos         = 1; //Servo move size when button clicked.

static const int       sNormalMode           = 0;
static const int       sCalibMode            = 1;

static const int       sUp                   = 0;
static const int       sDown                 = 1;
//static const int       sSelect               = 2;
static const int       sFirstButton          = sUp;
static const int       sLastButton           = sDown;

static const boolean   bButtonPullUp         = true;

//Digital Pins
#if 1   //Pro Mini
//static const int       sSelectButton         = A3;
static const int       sDownButton           = A2;
static const int       sUpButton             = A1;
//static const int       sBacklightPin         =  6;
static const int       sServoPin             =  7;
static const byte      cSPICmdDataPin        =  9;
static const byte      cSPIChipSelectPin     = 10;
#endif

#if 0   //NodeMCU
//static const int       sServoPin             =  9;		//Pin 9, SD2
//static const int       sServoPin             =  4;		//Pin 4, D2, I2C SDA
//static const int       sServoPin             =  1;		//Pin 1, D10, TXD0
static const int       sServoPin             =  3;		//Pin 3, D9, RXD0
static const int       sDownButton           =  2;		//Pin 2, D4
static const int       sUpButton             =  0;		//Pin 0, D3
static const byte      cSPICmdDataPin        = 16;		//Pin 16, D0
//static const byte      cSPICmdDataPin        =  5;		//Pin 5, D1, ILI9341 display article
static const byte      cSPIChipSelectPin     = 15;		//Pin 15, D8
//static const byte      cSPIChipSelectPin     =  4;		//Pin 4, D2, ILI9341 display article
static const byte      cSPI_MOSI_Pin         = 13;		//Pin 13, D7
static const byte      cSPI_CLK_Pin          = 14;		//Pin 14, D5
#endif

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

static const int       sDefaultFont       =  0;
static const int       sMaxButtonPresses  = 10;

static const unsigned long    ulModeSwitchTime  = 1000;  //Minimum msecs between mode changes
static const unsigned long    ulModeWaitTime    = 2000;  //Minimum msecs before mode is on
static const unsigned long    ulGyroReadTime    = 5000;   //Gyro reads spaced by this.

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

static const int       sFontNormal         =   1;     //6x8
static const int       sFontBig            =   2;     //8x16
static const int       sFontBigNum         =   3;     //16x32nums
static const int       sFontSquare         =   4;     //8x8

static const byte       cBogusResetPin      = 4;
static const byte       cHW_SPI             = 0;      //This is what their demo used.
static const byte       cDisplayType        = DOGS102;

//End of the const's

static int asGearLocation[sNumGears + 1];
static int sCurrentGear                   = 2;

static int asGyro[sNumGyroTypes][sNumAxis];

static int sCurrentMode                   = sNormalMode;
static int sServoPosLast                  = 0;

//Create servo object to control the servo
Servo myservo;

//Create display object fot the DOGS102-6 (102x64) display
dog_1701 DOG;

//Create EasyButton objects to handle button presses.
EasyButton UpButton     (sUpButton,     NULL, CALL_NONE, bButtonPullUp);
EasyButton DownButton   (sDownButton,   NULL, CALL_NONE, bButtonPullUp);
//EasyButton SelectButton (sSelectButton, NULL, CALL_NONE, bButtonPullUp);


//Number of unhandled presses, up to sMaxButtonPresses
static int              sButtonCount[]       = { 0, 0, 0};
//static int              sButtonCountLast[]   = { 0, 0, 0};
static boolean          abButtonBeingHeld[]  = { false, false, false};
static unsigned long    ulNextModeTime       = 0;  //msec when a mode switch can take place
static unsigned long    ulModeReadyTime      = 0;  //msec when button presses can be handled
static unsigned long    ulNextGyroTime       = 0;  //msec when the gyro will be read

//State of display items being changed and needing refresh.
static boolean     bButtonsChanged          = false;
static boolean     bGearChanged             = false;
static boolean     bServoChanged            = false;
static boolean     bModeChanged             = false;
static boolean     bGyroChanged             = false;

static int         sLineCount= 0;     //Used in outputs to Serial Monitor for clarity.

static char       szTempBuffer[25];   //DOGS102 line is 17 chars with 6x8 normal font.
static char       sz10CharString[11];
//static char				szFloatBuffer[15];

//MPU6050 acceleration and pitch things
const double			dGConvert					= 16384.0;	//MPU6050 16-bit +/- 2G Full scale
const double			dRadsToDeg				= 180.0/PI;
double						adGvalueXYZ[3];
double						dRollDeg_;
double						dPitchDeg_;
double						dPitchPercent_;

// The Arduino setup() method runs once, when the sketch starts
void setup()   {
  Serial.begin(115200);
  //delay(500);
  //Serial << endl << "Exercist.ino, Aug 10, 2017 Ace-A Pro Mini" << endl;
  //Serial << endl << ("setup: Call BLog()" + acSketchName + ", " + acFileDate) << endl;
  //BLog("Sketch: " + acSketchName + ", " + acFileDate);
	//Serial << endl << "Begin " + acSketchName + ", " + acFileDate << endl;
	//Serial << endl << "Begin " << endl;
	Serial << endl << "setup(): Begin " << szSketchName << ", " << szFileDate << endl;

  Serial << "setup(): Call sSetupGyro()" << endl;
  sSetupGyro();
  Serial << "setup(): Call sFillGearLocations()" << endl;
  sFillGearLocations();
	Serial << "setup(): Call sServoInit()" << endl;
  sServoInit();
	Serial << "setup(): Call sShowStartScreen()" << endl;
  sShowStartScreen();

  //Dither the servo once so it's position shows on the LCD.
  sServoDither(1, 1); // +/- 1 degree, once
  return;
}  //setup


// The Arduino loop() method gets called over and over.
void loop() {
  sCheckButtons();
  sLoopI2C();
  sDisplayUpdate();
  sHandleButtons();
  return;
}  //loop()


int sLoopI2C() {
   int      asGyroReading[sNumGyroTypes][sNumAxis];
   //boolean  bApplySmoothing= APPLY_SMOOTHING;

   if (millis() > ulNextGyroTime) {
  	 //Serial << "sLoopI2C(): Reading gyro, milllis()= " << millis() << endl;
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
      Wire.endTransmission(false);
      //Wire.requestFrom(MPU,14,true);  // request a total of 14 registers
      //bool	bTrue= true;
      //Wire.requestFrom((uint8_t)MPU, (size_t)14, (bool)true);  // request a total of 14 registers
      Wire.requestFrom((uint8_t)MPU, (uint8_t)14, (uint8_t)true);  // request a total of 14 registers, with a stop

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

      //Serial << "sLoopI2C(): X Accel= " << asGyroReading[sAccel][sXAxis] << endl;
      //BLog("sLoopI2C(): XAcc   YAcc   ZAcc");
      //BLog("          ", asGyroReading[sAccel][sXAxis], asGyroReading[sAccel][sYAxis], asGyroReading[sAccel][sZAxis]);

      //Apply low-pass filter to data
      for (int sDataType= sAccel; sDataType < sNumGyroTypes; sDataType++) {
         for (int sAxis= sXAxis; sAxis < sNumAxis; sAxis++) {
#if APPLY_SMOOTHING
            asGyro[sDataType][sAxis]= FILTER_FUNC(asGyroReading[sDataType][sAxis],
                                                  pusSmoothingMemory[sDataType][sAxis]);
#else
            asGyro[sDataType][sAxis]= asGyroReading[sDataType][sAxis];
#endif
         }  //for sDataType
      }  //for sAxis

      //Convert raw accel readings into G's and correct axis
    	//Because accel on BB is pointing down I am converting the axis
    	//Report 	-Y for X
    	//				+Z for Y
    	//				-X for Z
/*
      for (int sAxis= sXAxis; sAxis < sNumAxis; sAxis++) {
      		adGvalueXYZ[sAxis]= (double)asGyroReading[sAccel][sAxis] / dGConvert;
      }  //for sAxis
*/
  		adGvalueXYZ[sXAxis]= -(double)asGyroReading[sAccel][sYAxis] / dGConvert;
  		adGvalueXYZ[sYAxis]=  (double)asGyroReading[sAccel][sZAxis] / dGConvert;
  		adGvalueXYZ[sZAxis]= -(double)asGyroReading[sAccel][sXAxis] / dGConvert;

      Serial << "G's X, Y, Z " << adGvalueXYZ[sXAxis] << ", "
      		   << adGvalueXYZ[sYAxis] << ", " << adGvalueXYZ[sZAxis] << endl;
      ComputeRollPitch();
      bGyroChanged= true;
      ulNextGyroTime= millis() + ulGyroReadTime;
   }  //if (millis()>ulNextGyroTime)
   return 1;
}  //sLoopI2C


void ComputeRollPitch() { //
	dRollDeg_ = atan2((-adGvalueXYZ[sYAxis]), adGvalueXYZ[sZAxis]) * dRadsToDeg;
	dPitchDeg_= atan2(adGvalueXYZ[sXAxis],
								sqrt(adGvalueXYZ[sYAxis] * adGvalueXYZ[sYAxis] +
										 adGvalueXYZ[sZAxis] * adGvalueXYZ[sZAxis])) * dRadsToDeg;
	dPitchPercent_= dGetPitchPercent(dPitchDeg_);

	//Correct for current readings being 180 degrees off
	if (dRollDeg_ < 0.0) {
		dRollDeg_= dRollDeg_= -180.0 - dRollDeg_;
	}	//if(dRollDeg_<0.0)
	else {
		dRollDeg_= dRollDeg_= 180.0 - dRollDeg_;
	}	//if(dRollDeg_<0.0)else

  Serial << "Pitch Deg, Pitch%, Roll " << dPitchDeg_ << ", " << dPitchPercent_
  		   << ", " << dRollDeg_<< endl;
  return;
}	//ComputeRollPitch


double dGetPitchPercent(double dPitchDeg) {
	double dPitchPercent= -99.99;
	Serial << "dGetPitchPercent(): dPitchDeg_= " << dPitchDeg_ << endl;
	if ((dPitchDeg_ < 44.0) && (dPitchDeg_ > -44.0)) {
		dPitchPercent= 100.0 * tan(dPitchDeg_ / dRadsToDeg);
		Serial << "dGetPitchPercent(): dPitchPercent_= " << dPitchPercent_ << endl;
	}	//if((dPitchDeg_<44.0)&&...
	return dPitchPercent;
}	//dGetPitchPercent


int sDisplayPitchRoll() {
	//sprintf(szTempBuffer, "P %f", 123.45);
  strcpy(szTempBuffer, "P");
	dtostrf( dPitchDeg_, 5, 1, sz10CharString);
  strcat(szTempBuffer, sz10CharString);
  strcat(szTempBuffer, "%");
	//Serial << "sDisplayPitchRoll(): szTempBuffer= " << szTempBuffer << endl;
  sDisplayText(6,28, sFontNormal, szTempBuffer);

  strcpy(szTempBuffer, "R");
	dtostrf( dRollDeg_, 6, 1, sz10CharString);
  strcat(szTempBuffer, sz10CharString);
  strcat(szTempBuffer, "Deg");
  sDisplayText(7,28, sFontNormal, szTempBuffer);
  return 1;
}  //sDisplayPitchRoll


int sSetupGyro() {
   Serial << "sSetupGyro(): Begin"<< endl;
   //BLog("sSetupGyro(): Begin");
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


int sDisplayBegin() {
	Serial << "sDisplayBegin(): Call DOG.initialize()" << endl;
	DOG.initialize(cSPIChipSelectPin, cHW_SPI       , cHW_SPI,
								 cSPICmdDataPin   , cBogusResetPin, DOGS102);
/*
	DOG.initialize(cSPIChipSelectPin, cSPI_MOSI_Pin , cSPI_CLK_Pin,
								 cSPICmdDataPin   , cBogusResetPin, DOGS102);
	Serial << "sDisplayBegin(): Call DOG.view()" << endl;
*/
	DOG.view(sDisplayNormal);  //View screen Normal or Flipped
	return 1;
}  //sDisplayBegin


int sShowStartScreen(void) {
	Serial << "sShowStartScreen(): Call sDisplayBegin()" << endl;
	sDisplayBegin();
	Serial << "sShowStartScreen(): Call sShowSplash()" << endl;
	sShowSplash();
	delay(3000);
	return 1;
}  //sShowStartScreen


int sShowSplash(void) {
   sDisplayClear();

   //2 lines of big font takes lines 0-3
   sDisplayText(0, 0, sFontBig, (char *)"PowerShift");
   sDisplayText(2, 0, sFontBig, (char *)"  by ShiftE");

   //2 lines in normal font
   sDisplayText(5, 0, sFontNormal, (char *)"Always ride safe!");
   //sDisplayText(7, 0, sFontNormal, (char *)"**Larry & Candy**");
   sDisplayText(7, 0, sFontNormal, (char *)"July 19, 2017 ");
   return 1;
}  //sShowSplash


/*
int sDisplaySetBrightness(int sBrightness){
   analogWrite(sBacklightPin, sBrightness);
   return 1;
}  //sDisplaySetBrightness
*/


int sDisplayUpdate(void) {
   if (bScreenChanged()) {
      Serial << "sDisplayUpdate(): Refreshing screen" << endl;
      sDisplayClear();
      //sDisplayButtons();
      sDisplayCurrentGear();
      //sDisplayWatts();
      //sDisplayPitchRoll();
      sDisplayServoPos();
       //sDisplayOdometer();
   }  //if(bScreenChanged())
   return 1;
}  //sDisplayUpdate


int sDisplayClear() {
   DOG.clear();  //clear whole display
   return 1;
}  //sDisplayClear


int sDisplayText(int sLineNumber, int sPixelStart, int sFont, char *pcText) {
   switch (sFont) {
      case sFontNormal:
         DOG.string(sPixelStart, sLineNumber, font_6x8, pcText);
         break;
      case sFontBig:
         DOG.string(sPixelStart, sLineNumber, font_8x16, pcText);
         break;
      case sFontBigNum:
         DOG.string(sPixelStart, sLineNumber, font_16x32nums, pcText);
         break;
      case sFontSquare:
         DOG.string(sPixelStart, sLineNumber, font_8x8, pcText);
         break;
      default:
         Serial << "sDisplayText(): Bad case in switch()= " << sFont << endl;
         break;
   }  //switch
   return 1;
}  //sDisplayText


boolean bScreenChanged() {
   //Determine if something being displayed has changed & clear the flags.
   boolean bChanged= bGearChanged || bButtonsChanged || bServoChanged || bModeChanged;
   bGearChanged= bButtonsChanged= bServoChanged= bModeChanged= false;
   return bChanged;
}  //bScreenChanged


int sDisplayWatts() {
  strcpy(szTempBuffer, "You  105W");
  sDisplayText(0,28, sFontBig, szTempBuffer);

  strcpy(szTempBuffer, "All  105W");
  sDisplayText(2,28, sFontBig, szTempBuffer);

  strcpy(szTempBuffer, "Hill-125W");
  sDisplayText(4,28, sFontBig, szTempBuffer);
  return 1;
}  //sDisplayWatts


int sDisplayButtons() {
   //Show 3 lines at right bottom for U d, D d, S d
   //String will be 3 char long => 18 pixels, start so 2 pixels remain on right
   strcpy(szTempBuffer, "U ");
   itoa(sButtonCount[sUp]  ,sz10CharString  , 10);
   strcat(szTempBuffer, sz10CharString);
   sDisplayText(6, 82, sFontNormal, szTempBuffer);	//Move this and next to lines 6 and 7

   strcpy(szTempBuffer, "D ");
   itoa(sButtonCount[sDown]  ,sz10CharString  , 10);
   strcat(szTempBuffer, sz10CharString);
   sDisplayText(7, 82, sFontNormal, szTempBuffer);

/*
   strcpy(szLineBuffer, "S ");
   itoa(sButtonCount[sSelect]  ,sz10CharString  , 10);
   strcat(szLineBuffer, sz10CharString);
   sDisplayText(7, 82, sFontNormal, szLineBuffer);
*/
   return 1;
}  //sDisplayButtons


int sDisplayCurrentGear() {
   if (sCurrentMode == sNormalMode) {
       switch(sCurrentGear) {
         case 10:
       	    strcpy(sz10CharString, "0");
            break;
         default:
            itoa(sCurrentGear, sz10CharString, 10);
            break;
        } //switch
      //itoa(sCurrentGear, sz10CharString, 10);
      Serial << "sDisplayCurrentGear(): Mode= " << sCurrentMode
             << ", sz10CharString= " << sz10CharString << endl;
      sDisplayText(1, 6, sFontBigNum, sz10CharString);    //1st char in 4 pixels.
   }  //if (sCurrentMode..
   else {
      //We're in Calib mode so let's put a zero for the gear.
      sDisplayText(1, 6, sFontBigNum, (char *)"0");               //1st char in 4 pixels.
   }  //if (sCurrentMode..else

   return 1;
}  //sDisplayCurrentGear


int sDisplayServoPos() {
   itoa(sServoPosLast, sz10CharString, 10);
   strcpy(szTempBuffer, "Servo ");
   strcat(szTempBuffer, sz10CharString);
   //Text will end 2 pixels in from right edge when servo value is 3 digits.
   sDisplayText(1, 23, sFontBig, szTempBuffer);
   return 1;
}  //sDisplayServoPos


int sDisplayOdometer() {
   strcpy(szTempBuffer, "21.50");
   sDisplayText(5, 2, sFontBig, szTempBuffer);  //Start 2 pixels in.

   strcpy(szTempBuffer, "1123.00");
   sDisplayText(7, 2, sFontNormal, szTempBuffer);  //Start 2 pixels in.
   return 1;
}  //sDisplayOdometer


int sFillGearLocations(void) {
   for (int sGear=0; sGear <= sNumGears; sGear++) {
      asGearLocation[sGear]= asDefaultGearLocation[sGear];
   }  //for
   return 1;
}  //sFillGearLocations


int sHandleButtons(void) {
/*
   int          sButton;
   int          sGearChange;
   int          sNewGear;
   int          sTargetLocation;
   int          sTargetChange= 0;
*/

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
         bButtonsChanged= true;
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
         bServoChanged= true;
      }  //if(sButtonCount[sButton]==sHoldCode)

      //if ((sButtonCount[sButton] != sHoldCode) && (sButtonCount[sButton] > 0)) {
      if (!abButtonBeingHeld[sButton] && (sButtonCount[sButton] > 0)) {
         sTarget= sServoPosLast + sDirection * sTrimDeltaPos;
         Serial << sLineCount++ << " sHandleCalibMode(): Button" << sButton
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
               Serial << sLineCount++ << " sCheckButtons(): Button " << sButton
                      << " count incremented." << endl;
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
  return 1;
}  //sCheckButtons


int sServoInit() {
	Serial << "sServoInit(): Check bServoOn" << endl;
	if (bServoOn) {
		Serial << "sServoInit(): Call myservo.attach() for pin " << sServoPin << endl;
		myservo.attach(sServoPin);
	}  //if(bServoOn)
	Serial << "sServoInit(): Call sServoMove()" << endl;
	sServoMove(asGearLocation[sCurrentGear]);
	return 1;
} //sServoInit

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
   else {
     Serial << "sServoSetPosition():bServoOn is false, skip move to " << sServoPos << endl;
   }
   delay(sServoMsecWait);
   return 1;
}  //sServoSetPosition
//Last line.
