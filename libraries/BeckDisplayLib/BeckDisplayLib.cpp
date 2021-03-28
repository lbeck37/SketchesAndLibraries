// BeckDisplayLib.cpp 4/11/19a
#include <BeckDisplayLib.h>
#include <BeckLogLib.h>
#include <BeckMiniLib.h>
#include <BeckMPU9150Lib.h>
#include <BeckThermoLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>     ////For I2C OLED display

//Declaration for an SSD1306 display connected to I2C
const int       wScreenWidth          = 128;
const int       wScreenHeight         =  64;
const uint8_t   ucDisplayAddress      = 0x3C;
const uint8_t   ucVccState            = SSD1306_SWITCHCAPVCC;
const float     fDegToRadians         = PI/180.0;
const uint32_t  ulPrintPeriodMsec     = 5000;
const int       wBuffChar             = 20;
const uint32_t  ulBeforeZerosMsec     = 10000;
const uint8_t   ucLandscape           = 2;

static       uint32_t   ulNextPrintMsec     = 0;
static       uint32_t   ulGetZerosMsec      = 0;

static ProjectType     _eDisplayProjectType;

char            aszAccGyroMagPRY      [eLastSensor][eLastAxis][wBuffChar];
char            szDegC                [wBuffChar];
float           afZeroAccGyroMagPRY   [eLastSensor][eLastAxis];
bool            bZeroSet              = false;  //Set true when zero values have been set.

Adafruit_SSD1306    oDisplay(wScreenWidth, wScreenHeight);

//Function protos
void  UpdateThermDisplay      ();
void  Update4LinePitchDisplay ();
void  UpdateJustPitchDisplay  ();
void  Update4LineDisplay      (String szLine1, String szLine2, String szLine3, String szLine4);
void  UpdateOneLineDisplay    (String szLine1, uint8_t ucTextSize);
void  BuildDisplayStrings     ();
void  SetZeros                ();

void SetupDisplay(ProjectType eDisplayProjectType){
  _eDisplayProjectType= eDisplayProjectType;
  Serial << LOG0 << "SetupDisplay(): Call oDisplay.begin()" << endl;
  oDisplay.begin        (ucVccState, ucDisplayAddress);
  oDisplay.setRotation(ucLandscape);
  oDisplay.clearDisplay ();
  oDisplay.display      ();
  delay(10);
  //ClearZeros();
  return;
} //SetupDisplay


void ClearDisplay(){
  oDisplay.clearDisplay ();
  oDisplay.setTextSize  (2);
  oDisplay.setTextColor (WHITE);
  oDisplay.setCursor    (0,0);
  return;
} //ClearDisplay


void SetZeros(){
  if ((millis() >= ulGetZerosMsec) && !bZeroSet){
    Serial << LOG0 << "BeckDisplayLib.cpp: SetZeros(): Setting " << endl;
    for (int wSensor= 0; wSensor < eLastSensor; wSensor++){
      for (int wAxis= 0; wAxis < eLastAxis; wAxis++){
        afZeroAccGyroMagPRY[wSensor][wAxis]= afAccGyroMagPRY[wSensor][wAxis];
      } //for(int wAxis=0;...
    } //for(int wSensor=0;...
    bZeroSet= true;
  } //if(millis()>=ulGetZerosMsec)...
  return;
} //SetZeros


void ClearZeros(){
  Serial << LOG0 << "BeckDisplayLib.cpp: ClearZeros(): Clearing zero values and setting timer for new values" << endl;
  ulGetZerosMsec= millis() + ulBeforeZerosMsec;
  for (int wSensor= 0; wSensor < eLastSensor; wSensor++){
    for (int wAxis= 0; wAxis < eLastAxis; wAxis++){
      afZeroAccGyroMagPRY[wSensor][wAxis]= 0.00;
    } //for(int wAxis=0;...
  } //for(int wSensor=0;...
  bZeroSet= false;
  return;
} //ClearZeros


void UpdateDisplay(){
  switch (_eDisplayProjectType){
    case eFireplace:
    case eGarage:
    case eThermoDev:
    case eHeater:
      UpdateThermDisplay();
      break;
    case ePitchMeter:
      if(!bZeroSet){
        SetZeros();
      } //if(!bZeroSet)
      UpdateJustPitchDisplay();
      break;
    case eFrontLights:
      break;
    default:
      Serial << LOG0 << "BeckDisplayLib.cpp: UpdateDisplay(): Bad switch, _eDisplayProjectType= " << _eDisplayProjectType << endl;
      break;
  } //switch
  return;
} //UpdateDisplay


void UpdateThermDisplay(){
  String szDisplayLine1= "Now " + String(_fLastDegF);
  String szDisplayLine2= "Set " + String(_fSetpointF);
  String szDisplayLine3= "Off " + String(_fThermoOffDegF);

  Update4LineDisplay(szDisplayLine1, szDisplayLine2, szDisplayLine3, "");
  return;
} //UpdateThermDisplay


void Update4LinePitchDisplay(){
  BuildDisplayStrings();

  String szDisplayLine1= String(aszAccGyroMagPRY[ePRY][ePitch]);
  String szDisplayLine2= String(aszAccGyroMagPRY[ePRY][eRoll]);
  String szDisplayLine3= String(aszAccGyroMagPRY[ePRY][eYaw]);

  Update4LineDisplay(szDisplayLine1, szDisplayLine2, szDisplayLine3, "");

  if (millis() >= ulNextPrintMsec){
    ulNextPrintMsec= millis() + ulPrintPeriodMsec;
    Serial << LOG0 << "Update4LinePitchDisplay(): Pitch= " << aszAccGyroMagPRY[ePRY][ePitch] <<
        ", Roll= " << aszAccGyroMagPRY[ePRY][eRoll] << ", Yaw= " << aszAccGyroMagPRY[ePRY][eYaw] << endl;
  } //if(millis()>=ulNextPrintMsec)
  return;
} //Update4LinePitchDisplay


void UpdateJustPitchDisplay(){
  float   fPitchPercent;
  char    szBuffer[wBuffChar];
  float   fCorrectedPitch;   //wSumCount afSumPRY
  int     wCount= wSumCount;

  //Serial << LOG0 << "UpdateJustPitchDisplay(): wSumCount= " << wSumCount << endl;
  //for(PRY eAxis= ePitch; eAxis <= eYaw; eAxis++) {
  for(int eAxis= ePitch; eAxis <= eYaw; eAxis++) {
    afAccGyroMagPRY[ePRY][eAxis]= afSumPRY[eAxis] / wSumCount;
    afSumPRY[eAxis]= 0;
  } //for(int eAxis=ePitch...
  wSumCount= 0;

  //Compute Pitch as a percent rise or fall
  fCorrectedPitch= afAccGyroMagPRY[ePRY][ePitch] - afZeroAccGyroMagPRY[ePRY][ePitch];
  //fPitchPercent= 100.0 * tan(fDegToRadians * afAccGyroMagPRY[ePRY][ePitch]);
  fPitchPercent= 100.0 * tan(fDegToRadians * fCorrectedPitch);
/*
  fPitchPercent= fmin(+99.99, fPitchPercent);
  fPitchPercent= fmax(-99.99, fPitchPercent);
*/
  fPitchPercent= fmin(+99.0, fPitchPercent);
  fPitchPercent= fmax(-99.0, fPitchPercent);

  strcpy(aszAccGyroMagPRY[ePRY][ePitch], "");
  //dtostrf(fPitchPercent, 5, 2, szBuffer);
  dtostrf(fPitchPercent, 3, 0, szBuffer);
  strcat(aszAccGyroMagPRY[ePRY][ePitch], szBuffer);
  strcat(aszAccGyroMagPRY[ePRY][ePitch], "%");

  String szDisplayLine1= String(aszAccGyroMagPRY[ePRY][ePitch]);
  UpdateOneLineDisplay(szDisplayLine1, 5);

/*
  if (millis() >= ulNextPrintMsec){
    ulNextPrintMsec= millis() + ulPrintPeriodMsec;
    Serial << LOG0 << "UpdateJustPitchDisplay(): Pitch= " << aszAccGyroMagPRY[ePRY][ePitch] << endl;
  } //if(millis()>=ulNextPrintMsec)
*/
  Serial << LOG0 << "UpdateJustPitchDisplay(): wSumCount= "<< wCount << ", Pitch= " << aszAccGyroMagPRY[ePRY][ePitch] << endl;
  return;
} //UpdateJustPitchDisplay


void Update4LineDisplay(String szLine1, String szLine2, String szLine3, String szLine4){
  //Display up to (4) lines of (10) characters
  oDisplay.clearDisplay();
  oDisplay.setTextSize(2);
  oDisplay.setTextColor(WHITE);
  oDisplay.setCursor(0,0);

  oDisplay.println(szLine1);
  oDisplay.println(szLine2);
  oDisplay.println(szLine3);
  oDisplay.println(szLine4);

  oDisplay.display();
  return;
} //Update4LineDisplay


void UpdateOneLineDisplay(String szLine1, uint8_t ucTextSize){
  //Display one line of text
  //ucTextSize= 2 is 10 characters
  //ucTextSize= 3 is  7 characters "-99.99%"
  //ucTextSize= 4 is  5 characters "-100%"
  oDisplay.clearDisplay();
  //oDisplay.setTextSize(2);
  oDisplay.setTextSize(ucTextSize);
  oDisplay.setTextColor(WHITE);
  oDisplay.setCursor(0,0);

  oDisplay.println(szLine1);

  oDisplay.display();
  return;
} //UpdateOneLineDisplay


void BuildDisplayStrings(){
  //dtostrf(floatvar, StringLengthIncDecimalPoint, numVarsAfterDecimal, charbuf)
  float   fPitchPercent;
  char    szBuffer[wBuffChar];
  float   fCorrectedRoll;
  float   fCorrectedPitch;

  for (int eSensor= eAccel; eSensor <= ePRY; eSensor++){
    switch (eSensor){
      case eAccel:
      case eGyro:
      case eMag:
        for (int eAxis= eX; eAxis <= eZ; eAxis++){
          dtostrf(afAccGyroMagPRY[eSensor][eAxis], 5, 2, aszAccGyroMagPRY[eSensor][eAxis]);
        } //for(int eAxis=eX;...
        break;
      case ePRY:
        fCorrectedRoll= afAccGyroMagPRY[eSensor][eRoll] - afZeroAccGyroMagPRY[eSensor][eRoll];
        strcpy(aszAccGyroMagPRY[eSensor][eRoll], "R ");
        //dtostrf(afAccGyroMagPRY[eSensor][eRoll], 5, 2, szBuffer);
        dtostrf(fCorrectedRoll, 5, 2, szBuffer);
        strcat(aszAccGyroMagPRY[eSensor][eRoll], szBuffer);

        strcpy(aszAccGyroMagPRY[eSensor][eYaw], "Y ");
        dtostrf(afAccGyroMagPRY[eSensor][eYaw] , 5, 2, szBuffer);
        strcat(aszAccGyroMagPRY[eSensor][eYaw], szBuffer);
        //strcat(aszAccGyroMagPRY[eSensor][eYaw], ", ");

        //Compute Pitch as a percent rise or fall
        fCorrectedPitch= afAccGyroMagPRY[eSensor][ePitch] - afZeroAccGyroMagPRY[eSensor][ePitch];
        //fPitchPercent= 100.0 * tan(fDegToRadians * afAccGyroMagPRY[ePRY][ePitch]);
        fPitchPercent= 100.0 * tan(fDegToRadians * fCorrectedPitch);
        fPitchPercent= fmin(+99.99, fPitchPercent);
        fPitchPercent= fmax(-99.99, fPitchPercent);

        strcpy(aszAccGyroMagPRY[eSensor][ePitch], "P ");
        dtostrf(fPitchPercent, 5, 2, szBuffer);
        strcat(aszAccGyroMagPRY[eSensor][ePitch], szBuffer);
        strcat(aszAccGyroMagPRY[eSensor][ePitch], "%");
        break;
      default:
        Serial << LOG0 << "FillSensorData() Bad switch= " << eSensor << endl;
        break;
    } //switch
  } //for(int eSensor=eAccel;...

  //Build temperature string
  dtostrf(fDegC, 4, 1, szDegC);
  strcat(szDegC, "C");
  return;
} //BuildDisplayStrings
//Last Line.
