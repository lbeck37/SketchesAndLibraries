static const String SketchName  = "BeckPCF8591_ADCtest.ino";
static const String FileDate    = "Dec 15, 2017, Lenny-a";

#include <Arduino.h>
#include "Wire.h"
#include <Streaming.h>

/*
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
*/

//Address is 0x90 (144d) shifted 1 bit which divides by 2 to 0x48 (72d)
//#define PCF8591 (0x90 >> 1)

static const int       sI2C_SDA              = 26;
static const int       sI2C_SCL              = 27;
static const int       sPCF8591              = 0x48;		//I2C address

byte adcvalue0, adcvalue1, adcvalue2, adcvalue3;

void setup()
{
 //Wire.begin();
 Serial.begin(115200);
 Serial << endl << "setup(): Begin " << SketchName << ", " << FileDate << endl;
Serial << "setup(): Call Wire.begin(sI2C_SDA, sI2C_SCL)"  << endl;
 Wire.begin(sI2C_SDA, sI2C_SCL);
}	//setup


void loop()
{
	//Wire.beginTransmission(PCF8591);
	Serial << "loop(): Call Wire.beginTransmission(" << sPCF8591 << ")" << endl;
	Wire.beginTransmission(sPCF8591);
	Serial << "loop(): Call Wire.write(0x04), turn on A/D"  << endl;
	Wire.write(0x04);
	Wire.endTransmission();
	Serial << "loop(): Call Wire.requestFrom(PCF8591, 5, stream 4 values"  << endl;
	Wire.requestFrom(sPCF8591, 5);

	adcvalue0=Wire.read();
	adcvalue0=Wire.read();
	adcvalue1=Wire.read();
	adcvalue2=Wire.read();
	adcvalue3=Wire.read();

	Serial.print(adcvalue0);
	Serial.print(" ,");
	Serial.print(adcvalue1);
	Serial.print(" ,");
	Serial.print(adcvalue2);
	Serial.print(" ,");
	Serial.print(adcvalue3);
	Serial.println();

	delay(2000);
	return;
}	//loop
