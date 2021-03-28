// BeckSwitchLib.h 12/28/19a
#pragma once
#include <BeckMiniLib.h>

static const int    sAlexaPin             =  2;     //GPIO 2 is D4 and Blue LED on NodeMCU
static const int    sSDA_GPIO             =  4;   //I2C, GPIO 4 is D2 on NodeMCU
static const int    sSCL_GPIO             =  5;   //I2C, GPIO 5 is D1 on NodeMCU and labeled D2
static const int    sOneWireGPIO          = 12;   //GPIO 12 is D6 on NodeMCU
static const int    sHeatSwitchGPIO       = 14;   //GPIO 14 is D5 on NodeMCU

static const int    sSwitchOpen           = 0;
static const int    sSwitchClosed         = 1;
static const int    sOff                  = 0;
static const int    sOn                   = 1;
static const int    sNotInit              = -3737;
static const int    sNoSwitch             = -1;
static const int    sNumSwitches          = 2;
static const int    sHeatSwitchNum        = 1;      //Switch number that turns Heat on and off.
static const int    sThermoSwitchNum      = 2;      //Switch number that turns Thermostat on and off.
static const int    sThermoDummySwitch    = 0;      //Thermostat Blynk LED lives at unused switch #0.
static const int    asSwitchPin[]         = {-1, sHeatSwitchGPIO, sAlexaPin, sNoSwitch, sNoSwitch};    //0 is not a switch, switches are at 1,2,3,4
static const bool   abSwitchInverted[]    = {0, true, true, true, true};  //Opto-isolated relays close when pulled low.

extern int          asSwitchState[];
extern int          asSwitchLastState[];

void  SetupSwitches     ();
void  SetHeatSwitch     (int sSwitchState);
void  SetThermoSwitch   (int sSwitchState);
void  SetSwitch         (int sSwitch, int sSwitchState);

//Last line
