//BeckControlLib.h
#ifndef BECKCONTROLLIB_H
#define BECKCONTROLLIB_H
#include <BeckLib.h>
#include <BeckOneWire.h>

extern bool         abSwitchState_[];
extern bool         abSwitchLastState_[];
extern float        fLastDegF_;
extern int          sSetpointF_;
extern int          sThermoTimesCount_;
extern bool         bThermoOn_;
extern bool         bHeatOn_;
extern float        fThermoOffDegF_;
extern const long   sThermoTimesInRow;
extern const float  fMaxHeatRangeF;

extern const int    sSwitchOpen;
extern const int    sSwitchClosed;
extern const int    sOff;
extern const int    sOn;
extern const int    sNotInit;

extern const int    sNumSwitches_;
extern const int    asSwitchPin[];    //0 is not a switch, switches are at 1,2,3,4
extern const int		sFlowSensorPin_;
extern const int    sOverheatSwitchNum;
extern const float	fOverheatDegF_;

extern bool         bFlowState_;
extern bool         bFlowLastState_;
extern bool					bNoFlow_;
extern bool					bOverheatFailure_;


//ESP32 AtoD Input Pins
extern const int  sNumTanks;
extern const int  sPinsPerTank;

extern const uint8_t ucGrey1PowerPin;
extern const uint8_t ucGrey1LevelPin;
extern const uint8_t ucBlackPowerPin;
extern const uint8_t ucBlackLevelPin;
extern const uint8_t ucGrey2PowerPin;
extern const uint8_t ucGrey2LevelPin;

//Function protos.
extern void			SetupHotTub();
extern void 		ResetHotTub();
extern void     HandleHeatSwitch();
extern void     TurnHeatOn(bool bTurnOn);
extern void     SetHeatSwitch(int sSwitchState);
extern void     SetThermoState(int sSwitchState);
extern void 		HandleOverheat();
extern bool 		bCheckOverheat(bool bSetup);
extern void 		SetOverheatSwitch(int sSwitchState);
extern void     SetSwitch(int sSwitch, int sSwitchState);
extern bool 		bTurnPumpOn(bool bTurnOn);
extern void 		SetPumpSwitch(int sSwitchState);
extern bool 		bCheckFlowSensor(bool bExpectedReturn);
extern bool 		bReadFlowSensor();

#endif  //BECKCONTROLLIB_H
