// BeckI2cLib.cpp 3/23/19a
#include <BeckI2cLib.h>
#include <BeckLogLib.h>
#include <BeckMiniLib.h>
#include <BeckSwitchLib.h>
#include <Wire.h>

void SetupI2C(){
  //Serial << LOG0 << "SetupI2C(): Call Wire.begin(sSDA_GPIO, sSCL_GPIO)" << endl;
  Serial << LOG0 << "SetupI2C(): Call Wire.begin(sSDA_GPIO= " << sSDA_GPIO <<
      ", sSCL_GPIO= " << sSCL_GPIO << ")" << endl;
  Wire.begin(sSDA_GPIO, sSCL_GPIO);
  ScanForI2CDevices();
  return;
} //SetupI2C


void ScanForI2CDevices(){
  byte ucError, ucAddress;
  int nDevices;
  nDevices = 0;
  for(ucAddress = 1; ucAddress < 127; ucAddress++ ){
    //The i2c_scanner uses the return value of the Write.endTransmisstion to see if
    //a device did acknowledge to the address.
    Wire.beginTransmission(ucAddress);
    ucError = Wire.endTransmission();

    if (ucError == 0){
      Serial << LOG0 << "ScanForI2CDevices(): I2C device found at address 0x";
      if (ucAddress<16){
        Serial.print("0");
      } //if(ucAddress<16)
      Serial.println(ucAddress,HEX);
      nDevices++;
    } //if(ucError==0)
    else if (ucError==4) {
      Serial << LOG0 << "ScanForI2CDevices(): Unknown error at address 0x";
      if (ucAddress<16) {
        Serial << "0";
      } //if(ucAddress<16)
      Serial.println(ucAddress,HEX);
    } //else if(ucError==4)
  } //for
 if (nDevices == 0){
    Serial << LOG0 << "ScanForI2CDevices(): ***No I2C devices found***" << endl;
 }  //if(nDevices==0)
  return;
} //ScanForDevices
//Last line.
