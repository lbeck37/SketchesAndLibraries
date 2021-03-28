// BeckAlexaLib.cpp 2/2/20a
#include <BeckAlexaLib.h>
#include <BeckBiotaLib.h>
#include <BeckDisplayLib.h>
#include <BeckLogLib.h>
#include <BeckSwitchLib.h>
#include <BeckThermoLib.h>
#include <BeckAccessPointLib.h>

//#include <fauxmoESP.h>        //Alexa Phillips Hue light emulation
#include <fauxmoESP.h>

int            wAlexaHandleCount     = 0;      //Incremented each time HandleAlexa() called
bool           bAlexaOn              = false;  //Only projects that use Alexa set this true.
bool           _bAlexaChanged        = false;  //Set true when display data changed
char           _acAlexaName[50];
fauxmoESP      Alexa;                          //Alexa emulation of Philips Hue Bulb

//Function protos
void ThermoHandleAlexa      (bool bState, unsigned char ucValue);
void PitchMeterHandleAlexa  (unsigned char ucValue);

void SetupAlexa(char szAlexaName[]){
  String szLogString= "SetupAlexa(): Begin";
  LogToSerial(szLogString);
  bAlexaOn= true;
  //SetThermoSwitch(1);
  if(bAlexaOn){
    // You have to call enable(true) once you have a WiFi connection
    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    //Beck 12/3/18 I don't know why we do enable, disable, enable but it is necessary
    Alexa.enable(true);
    Alexa.enable(false);
    Alexa.enable(true);

    // You can use different ways to invoke Alexa to modify the devices state:
    // "Alexa, turn light one on" ("light one" is the name of the first device below)
    // "Alexa, turn on light one"
    // "Alexa, set light one to fifty" (50 means 50% of brightness)

    // Add virtual devices
    Alexa.addDevice(szAlexaName);

    //Define the callback
    Alexa.onSetState([](unsigned char device_id, const char *device_name, bool state, unsigned char value)
      {
      DoAlexaCommand(device_id, device_name, state, value);
      } );  //Alexa.onSetState
  } //if(bAlexaOn)
  Serial << LOG0 << "SetupAlexa(): Alexa set up for " << szAlexaName << endl;
  return;
} //SetupAlexa


void HandleAlexa(){
  wAlexaHandleCount++;  //Track how many times this is called before next handle system (10 sec)
  Alexa.handle();
  return;
} //HandleAlexa


void DoAlexaCommand(unsigned char ucDdeviceID, const char* szDeviceName, bool bState, unsigned char ucValue){
  char    szCharString[100];
  sprintf(szCharString, "DoAlexaCommand(): Device #%d (%s) bState: %s value: %d",
      ucDdeviceID, szDeviceName, (bState ? "ON " : "OFF"), ucValue);
  String szLogString= szCharString;
  LogToSerial(szLogString);
  switch (_eProjectType){
    case eFireplace:
    case eGarage:
    case eThermoDev:
    case eHeater:
      ThermoHandleAlexa(bState, ucValue);
      break;
    case ePitchMeter:
      PitchMeterHandleAlexa(ucValue);
      break;
    case eFrontLights:
      break;
    default:
      Serial << LOG0 << "DoAlexaCommand(): Bad switch, _eProjectType= " << _eProjectType << endl;
      break;
  } //switch
  _bAlexaChanged= true;
  return;
} //DoAlexaCommand


void ThermoHandleAlexa(bool bState, unsigned char ucValue){
  int wValuePercent= round(((float)ucValue / 255.0) * 100);
  Serial << LOG0 << "ThermoHandleAlexa(): Received wValuePercent= " << wValuePercent << endl;
  if(wValuePercent == 10){    //"Alexa set Larry's Device to 10"
    SwitchProjectType(ePitchMeter);
  } //if(wValuePercent==10)
  else{
    _bThermoOn= bState;
	SetThermoSwitch(bState);
    fSetThermoSetpoint(ucValue);
  } //if(wValuePercent==10)else
  return;
} //PitchMeterHandleAlexa


void PitchMeterHandleAlexa(unsigned char ucValue){
  int wValuePercent= round(((float)ucValue / 255.0) * 100);
  Serial << LOG0 << "PitchMeterHandleAlexa(): Received wValuePercent= " << wValuePercent << endl;
/*
  if(wValuePercent == 10){    //"Alexa set Larry's Device to 10"
    ClearZeros();
  } //if(wValuePercent==10)
*/
  switch (wValuePercent){
  case 11:
    ClearZeros();
    break;
  case 20:
    SwitchProjectType(eThermoDev);
    break;
    default:
      break;
  } //switch
  return;
} //PitchMeterHandleAlexa
//Last line.
