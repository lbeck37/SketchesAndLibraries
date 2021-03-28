// BeckMPU9150.h 4/3/19a
#pragma once
#include <BeckMiniLib.h>

enum Sensor{
  eAccel = 0,
  eGyro,
  eMag,
  ePRY,
  eLastSensor
};

enum Axis{
  eX = 0,
  eY,
  eZ,
  eLastAxis
};

enum PRY{
  ePitch = 0,
  eRoll,
  eYaw,
  eLastPRY
};

extern int      wSumCount;
extern float    afSumPRY[eLastPRY];
extern float    afAccGyroMagPRY[eLastSensor][eLastAxis];
extern float    fDegC;

//Function protos
int   SetupMPU9150    (const char *szSketchName, const char *szFileDate, uint32_t ulUpdatePeriodMsec);
void  HandleMPU9150   ();
//Last line.
