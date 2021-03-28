const String SketchName  = "BeckE32_TankMonitor.ino";
const String FileDate    = "April 7, 2019a";
/******************************************************************************
ESP32_Water_Sensor_SMS_Example.ino
Example for sending a text message using the ESP32 Thing and IFTTT
Alex Wende @ SparkFun Electronics
February 7th 2017
******************************************************************************/

#include <Streaming.h>
/*
#include "BluetoothSerial.h"		//Classic Bluetooth, not BLE
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
*/

//BluetoothSerial SerialBT;

const byte  touchPin = 4;                             // IO pin for capacitive touch sensing

void setup() {
	Serial.begin(115200); // Start serial communication for debug information
	Serial << endl << "setup(): Begin " << SketchName << ", " << FileDate << endl;

  //SerialBT.begin("Beck_TankMonitor"); //Bluetooth device name
	return;
}	//setup


void loop()
{
  char 				szNumber[10];
  //float sensorReading = 0;  // Read the analog value for the touch sensor
  double dSensorReading = 0;  // Read the analog value for the touch sensor
		//touchRead() returns an int but I use double because I don't know int version of dtostrf()
	byte sampleSize = 8;      // Number of samples we want to take

	for(byte i= 0; i < sampleSize; i++)  // Average samples together to minimize false readings
	{
		dSensorReading += touchRead(touchPin); // We sample the touch pin here
		delay(10);
	}
	Serial << "loop(): sensorReading= " << dSensorReading << endl;

  dtostrf(dSensorReading, 8, 4, szNumber);
  //Write touch sensor reading to BT
  //SerialBT.write((uint8_t*)szNumber, strlen(szNumber));

	delay(1000);
	return;
}	//loop
//Last line.
