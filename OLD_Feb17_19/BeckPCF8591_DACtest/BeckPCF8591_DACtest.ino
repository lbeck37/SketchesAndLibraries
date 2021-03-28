static const String SketchName  = "BeckPCF8591_DACtest.ino";
static const String FileDate    = "Dec 12, 2017, Lenny-c";

// Example 52.1 PCF8591 DAC demo
// http://tronixstuff.com/tutorials Chapter 52
// John Boxall June 2013
#include <Arduino.h>
#include <Wire.h>
#include <Streaming.h>
//#define PCF8591 (0x90 >> 1) // I2C bus address

static const int       sI2C_SDA              = 26;
static const int       sI2C_SCL              = 27;
static const int       sPCF8591              = 0x48;		//I2C address


void setup()
{
	 Serial.begin(115200);
	 Serial << endl << "setup(): Begin " << SketchName << ", " << FileDate << endl;
	 Serial << "setup(): Call Wire.begin(sI2C_SDA, sI2C_SCL)"  << endl;
	 Wire.begin(sI2C_SDA, sI2C_SCL);
}	//setup


void loop()
{
 for (int i=0; i<256; i++)
 {
 Wire.beginTransmission(sPCF8591); // wake up PCF8591
 Wire.write(0x40); // control byte - turn on DAC (binary 1000000)
 Wire.write(i); // value to send to DAC
 Wire.endTransmission(); // end transmission
 delay(5);
 }

 for (int i=255; i>=0; --i)
 {
 Wire.beginTransmission(sPCF8591); // wake up PCF8591
 Wire.write(0x40); // control byte - turn on DAC (binary 1000000)
 Wire.write(i); // value to send to DAC
 Wire.endTransmission(); // end transmission
 delay(5);
 }
}	//loop
