// BeckThermoLib.cpp 12/28/19b
#include <BeckThermoLib.h>
#include <BeckMiniLib.h>
#include <BeckSwitchLib.h>
#include <DallasTemperature.h>
#include <OneWire.h>

float           _fMaxHeatRangeF       = 0.10;   //Temp above setpoint before heat is turned off
float           _fLastDegF            = 37.01;  //Last temperature reading.
/*
float           _fSetpointF           = 70.0;
float           _fMinSetpoint         = 32.0;
float           _fMaxSetpoint         = 80.0;
*/
float           _fMinSetpoint         = 37.02;
float           _fSetpointF           = 37.03;
float           _fMaxSetpoint         = 37.04;
float           _fThermoOffDegF       = _fSetpointF + _fMaxHeatRangeF;
int             sThermoTimesCount     = 0;      //Number of times temperature out of range
bool            _bThermoOn;   					//Whether thermostat is running, set in BeckBiotaLib
bool            bHeatOn               = false;  //If switch is on to turn on Heat.
unsigned long   ulNextThermPrintMsec  = 0;
const uint32_t  ulThermPrintPeriodMsec= 10 * lMsecPerSec; //mSec between running system handler

//Create OneWire instance and tell Dallas Temperature Library to use oneWire Library
OneWire             oOneWire(sOneWireGPIO);
DallasTemperature   oDallasTempSensor(&oOneWire);

//Function protos
void    HandleHeatSwitch      ();

void HandleThermostat(){
  static bool     bStateChanged= false;
  unsigned long   ulStartTime;

  //Read the Dallas One-wire temperature sensor
  float fDegF= fGetDegF();

  //Only do something if the thermostat is turned on.
  if (_bThermoOn){
    if (bHeatOn){
      if (fDegF >= _fThermoOffDegF){
        bStateChanged= true;
        if (++sThermoTimesCount >= sThermoTimesInRow){
          TurnHeatOn(false);
          sThermoTimesCount= 0;
        } //if(sThermoTimesCount>=sThermoTimesInRow)
      } //if(fDegF>=_fThermoOffDegF)
      else{
        sThermoTimesCount= 0;
      } //if(fDegF>=_fThermoOffDegF)else
    } //if(bHeatOn)
    else{
      if (fDegF <= _fSetpointF){
        bStateChanged= true;
        if (++sThermoTimesCount >= sThermoTimesInRow){
          TurnHeatOn(true);
          sThermoTimesCount= 0;
        } //if(sThermoTimesCount>=sThermoTimesInRow)
      } //if(fDegF<_fSetpointF)
      else{
        sThermoTimesCount= 0;
      } //if(fDegF<_fSetpointF)else
    } //if(bHeatOn)else
/*
    if(bStateChanged || (millis() >= ulNextThermPrintMsec)){
      bStateChanged= false;
      ulNextThermPrintMsec= millis() + ulThermPrintPeriodMsec;
      LogThermostatData(fDegF);
    }
*/
  } //if(_bThermoOn)
  else{
    //String szLogString= "HandleThermostat(): bThermoOn is false, Thermostat is off";
    //LogToSerial(szLogString);
  } //if(_bThermoOn)else
  if(bStateChanged || (millis() >= ulNextThermPrintMsec)){
    bStateChanged= false;
    ulNextThermPrintMsec= millis() + ulThermPrintPeriodMsec;
    if(_bThermoOn) {
      LogThermostatData(fDegF);
    }	//if(_bThermoOn)
    else {
      String szLogString= "HandleThermostat(): bThermoOn is false, Thermostat is off";
      LogToSerial(szLogString);
    }	//if(_bThermoOn)else
  }
  HandleHeatSwitch();
  return;
} //HandleThermostat


void HandleHeatSwitch(){
  if (bHeatOn){
    SetSwitch(sHeatSwitchNum, sOn);
  } //if(bHeatOn)
  else{
    asSwitchState[sHeatSwitchNum]= sOff;
    SetSwitch(sHeatSwitchNum, sOff);
  } //if(bHeatOn)else
  return;
} //HandleHeatSwitch


void LogThermostatData(float fDegF){
  String szLogString= " " + String(bHeatOn) + String(sThermoTimesCount) + " " +
                String(fDegF) + " " + String(_fSetpointF) + " " + String(_fThermoOffDegF);
  LogToSerial(szLogString);
  return;
} //LogThermostatData


float fSetThermoSetpoint(unsigned char ucSetpoint){
  float fSetpoint= round( ((float)ucSetpoint / 255.0) * 100.0);
  fSetThermoSetpoint(fSetpoint);
  return fSetpoint;
} //fSetThermoSetpoint(unsigned char)


float fSetThermoSetpoint(float fSetpoint){
  float fLastSetpoint= _fSetpointF;
  if( (fSetpoint >= _fMinSetpoint) && (fSetpoint <= _fMaxSetpoint)){
    if(fSetpoint != _fSetpointF){
      _fSetpointF      = fSetpoint;
      _fThermoOffDegF  = _fSetpointF + _fMaxHeatRangeF;
      Serial << LOG0 << "fSetThermoSetpoint(): Set _fSetpointF to " << _fSetpointF << endl;
    } //if(fSetpoint!=_fSetpointF)
  } //if((fSetpoint>=...
  if(_fSetpointF == fLastSetpoint){
    Serial << LOG0 << "fSetThermoSetpoint(): _fSetpointF remains at " << _fSetpointF << endl;
  } //if((_fSetpointF==fLastSetpoint)
  return _fSetpointF;
} //fSetThermoSetpoint(float)


float fGetDegF(){
  oDallasTempSensor.requestTemperatures(); // Send the command to get temperatures
  _fLastDegF= oDallasTempSensor.getTempFByIndex(0);
  return _fLastDegF;
}  //fGetDegF


void TurnHeatOn(bool bTurnOn){
  if (bTurnOn){
    String szLogString= "TurnHeatOn(): ON";
    LogToSerial(szLogString);
    bHeatOn= true;
    SetHeatSwitch(sSwitchClosed);
    sThermoTimesCount= 0;
  } //if(bTurnOn)
  else{
    String szLogString= "TurnHeatOn(): OFF";
    LogToSerial(szLogString);
    bHeatOn= false;
    SetHeatSwitch(sSwitchOpen);
    sThermoTimesCount= 0;
  } //if(bTurnOn)else
  return;
} //TurnHeatOn
//Last line.
