// BeckThermoLib.h 12/28/19b
#pragma once

const long          sThermoTimesInRow = 3;      //Max times temp is outside range before switch

extern bool         _bThermoOn;
extern float        _fLastDegF;
extern float        _fSetpointF;
extern float        _fMaxHeatRangeF;
extern float        _fThermoOffDegF;
extern float        _fMinSetpoint;
extern float        _fMaxSetpoint;

void    HandleThermostat      ();
void    LogThermostatData     (float fDegF);
float   fSetThermoSetpoint    (unsigned char ucSetpoint);
float   fSetThermoSetpoint    (float fSetpoint);

//void    HandleHeatSwitch      ();
float   fGetDegF              ();
void    TurnHeatOn            (bool bTurnOn);
//Last line.
