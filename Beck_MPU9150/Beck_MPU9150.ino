// This was the example sketch for getting Pitch, Roll and Yaw from MPUU9150 IMU
// The BeckMPU9150Lib library contains code from this sketch originally.
// Beck_Bionic.ino had the calls in here merged to it.

const char szSketchName[]  = "MPU9150";
const char szFileDate[]    = " 03/05/19c";

#include <BeckDisplayLib.h>
#include <BeckMiniLib.h>
#include <BeckMPU9150Lib.h>
#include <BeckSwitchLib.h>
#include "Wire.h"

const uint32_t  ulDisplayPeriodMsec   = 200;

//Function protos
void  SetupI2C    ();

void setup(){
  Serial.begin(115200);
  delay(100);
  Serial << endl << LOG0 << "setup(): Sketch: " << szSketchName << "," << szFileDate << endl;
  SetupI2C();
  SetupDisplay(ePitchMeter);
  ClearDisplay();
  SetupMPU9150(szSketchName, szFileDate, ulDisplayPeriodMsec);
  return;
} //setup

void loop(){
  HandleMPU9150();
  UpdateDisplay();
  return;
} //loop


void SetupI2C(){
  Serial << LOG0 << "SetupI2C(): Call Wire.begin(sSDA_GPIO= " << sSDA_GPIO << ", sSCL_GPIO= " << sSCL_GPIO << ")" << endl;
  Wire.begin(sSDA_GPIO, sSCL_GPIO);
  return;
} //SetupI2C
//Last line.
