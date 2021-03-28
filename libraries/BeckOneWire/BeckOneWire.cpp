//BeckOneWire.cpp
#include <BeckOneWire.h>
#include <OneWire.h>

float        					fLastDegF_             = 37.37;  //Last temperature reading.

//(4) Bare DS18B20 temperature sensors
DeviceAddress		acOneWireV40	= {0x28, 0xFF, 0xBA, 0xB4, 0xA0, 0x16, 0x05, 0xAA};
DeviceAddress		acOneWireV41	= {0x28, 0xFF, 0x80, 0xF8, 0xA0, 0x16, 0x03, 0xBC};
DeviceAddress		acOneWireV42	= {0x28, 0xFF, 0x37, 0xC4, 0xA0, 0x16, 0x04, 0x7B};
DeviceAddress		acOneWireV43	= {0x28, 0xFF, 0x03, 0xAF, 0xA0, 0x16, 0x05, 0xBA};
DeviceAddress		acOneWireV44	= {0x28, 0xC6, 0x49, 0x86, 0x08, 0x00, 0x00, 0xDC}; //Shielded #1
DeviceAddress		acOneWireV45	= {0x28, 0x40, 0xB6, 0x86, 0x08, 0x00, 0x00, 0x4D}; //Shielded #2

OneWire      				oOneWire(ucOneWirePin_);
//Tell Dallas Temperature Library to use oneWire Library
DallasTemperature   oSensors(&oOneWire);

BeckOneWire::BeckOneWire(UINT8 ucOneWirePin) {
	ucOneWirePin_= ucOneWirePin;

	apOneWireSensor_[0]= new BeckOneWireSensor(acOneWireV40);
	apOneWireSensor_[1]= new BeckOneWireSensor(acOneWireV41);
	apOneWireSensor_[2]= new BeckOneWireSensor(acOneWireV42);
	apOneWireSensor_[3]= new BeckOneWireSensor(acOneWireV43);
	apOneWireSensor_[4]= new BeckOneWireSensor(acOneWireV44);
	apOneWireSensor_[5]= new BeckOneWireSensor(acOneWireV45);
	return;
}	//Constructor


float BeckOneWire::fGetDegF(VirtualPin_t eVirtualPin) {
  return(apOneWireSensor_[eVirtualPin]->fGetDegF(true));
}	//fGetDegF


BeckOneWireSensor::BeckOneWireSensor(DeviceAddress aucDeviceAddress) {
	//Copy the 8 address bytes
	for (int sByte= 0; sByte < 8; sByte++) {
			aucDeviceAddress_[sByte]= aucDeviceAddress[sByte];
	}
	oSensors.setResolution(aucDeviceAddress_, ucResolution_);
	return;
}	//Constructor


float BeckOneWireSensor::fGetDegF(bool bTakeReading) {
  float fDegFReturn= 37.37;   //Value used for default in testing w/o reading sensor. fLastDegF
  if (bTakeReading){
    oSensors.requestTemperatures(); // Send the command to get read all sensors
    fDegFReturn= oSensors.getTempF(aucDeviceAddress_);
    fLastDegF_= fDegFReturn;
  } //if(bTakeReading)
  else{
    fDegFReturn= fLastDegF_;
  } //if(bTakeReading)else
  return fDegFReturn;
}	//fGetDegF


/*
//Non-class functions
float fGetDegF(bool bTakeReading){
  float fDegFReturn= 37.37;   //Value used for default in testing w/o reading sensor. fLastDegF
#ifdef ESP8266
  if (bTakeReading){
    oSensors.requestTemperatures(); // Send the command to get temperatures
    fDegFReturn= oSensors.getTempFByIndex(0);
    fLastDegF_= fDegFReturn;
  } //if(bTakeReading)
  else{
    fDegFReturn= fLastDegF_;
  } //if(bTakeReading)else
#endif
  return fDegFReturn;
}  //fGetDegF
*/
//Last line.
