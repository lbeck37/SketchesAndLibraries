//BeckOneWire.h
#ifndef BECKONEWIRE_H
#define BECKONEWIRE_H
#include <BeckLib.h>
#include <DallasTemperature.h>

extern 				DallasTemperature   oSensors;
extern const 	UINT8 							ucOneWirePin_;

extern float    fGetDegF(bool bTakeReading);

typedef enum {
	eVP40 = 0,
	eVP41,
	eVP42,
	eVP43,
	eVP44,
	eVP45,
	eLastVP
} VirtualPin_t;


class BeckOneWireSensor {
protected:

public:
	BeckOneWireSensor(DeviceAddress aucDeviceAddress);
	float 	fGetDegF(bool bTakeReading);

private:
	DeviceAddress		aucDeviceAddress_;
	UINT8						ucResolution_				= 12;
};	//BeckOneWireSensor


class BeckOneWire {
protected:

public:
	BeckOneWire(UINT8 ucOneWirePin);
	float 	fGetDegF(VirtualPin_t eVirtualPin);

private:
	UINT8 									ucOneWirePin_;
	BeckOneWireSensor*			apOneWireSensor_[6];
};	//BeckOneWire

extern BeckOneWire*		pBeckOneWire;

#endif  //BECKONEWIRE_H
