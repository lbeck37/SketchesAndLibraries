const String SketchName  = "BeckE32_AirLift.ino";
const String FileDate    = "Aug 22, 2018-a";
/******************************************************************************
ESP32_Water_Sensor_SMS_Example.ino
Example for sending a text message using the ESP32 Thing and IFTTT
Alex Wende @ SparkFun Electronics
February 7th 2017
******************************************************************************/

#include <Streaming.h>
#include "BluetoothSerial.h"    //Classic Bluetooth, not BLE
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

const uint8_t   ucLeftBagGPIO   = 34;
const uint8_t   ucRightBagGPIO  = 35;

// 5V pressure sensors output 0.5 to 4.5V representing 0 to 174psi
// We have a 560K resistor over a 1M => gain of 0.641 => Into A/D 0.32 to 2.88V
// Count ranges from 0 to 4095 representing 0 to 3.3V
// Voltage and pressure conversion derived in notebook 8/19/19
// Update: Sensors were so out of range I came up with a CNT to Psi linear equation
//    Psi= A * CNT + B
const double    dLeftSlope    = 0.069;
const double    dRightSlope   = 0.104;
const double    dLeftOffset   = -8.83;
const double    dRightOffset  = -51.25;

char    _szFormattedString[30];

void setup() {
  Serial.begin(115200); // Start serial communication for debug information
  Serial << endl << "setup(): Begin " << SketchName << ", " << FileDate << endl;
  Serial << "setup(): Call SerialBT.begin(Beck_AirLift082018)" << endl;
  SerialBT.begin("Beck_AirLift082018"); //Bluetooth device name
  return;
} //setup


void loop()
{
  //ReadTouchSensor();
  //ReadPressureSensor();
  //ReadDummyPressures();
  ReadPressureSensors();
  delay(1000);
  return;
} //loop


void ReadPressureSensors() {
  //Volts= (CNT / 4095) * 3.3
  //Psi= (68.0 * Volts) - 21.8
  //char      szNumber[10];
  //double    dLeftBagVolts;
  //double    dRightBagVolts;
  double    dLeftBagPsi;
  double    dRightBagPsi;
  uint32_t  usSensorCount;

  Serial << endl;
  //Read Left bag pressure sensor
  usSensorCount= analogRead(ucLeftBagGPIO);
/*
  dLeftBagVolts= ((double)usSensorCount / 4095.0) * 3.30;
  dLeftBagPsi= (68.0 * dLeftBagVolts) - 21.8;
  Serial << endl << "usSensorCount= " << usSensorCount << ", dLeftBagVolts= "
         << dLeftBagVolts << ", dLeftBagPsi= " << dLeftBagPsi << endl;
*/
  dLeftBagPsi= (dLeftSlope * (double)usSensorCount) + dLeftOffset;
  Serial << "usSensorCount= " << usSensorCount
         << ", dLeftBagPsi= " << dLeftBagPsi << endl;

  //Read Right bag pressure sensor
  usSensorCount= analogRead(ucRightBagGPIO);
/*
  dRightBagVolts= ((double)usSensorCount / 4095.0) * 3.30;
  dRightBagPsi= (68.0 * dRightBagVolts) - 21.8;
  Serial << "usSensorCount= " << usSensorCount << ", dRightBagVolts= "
         << dRightBagVolts << ", dRightBagPsi= " << dRightBagPsi << endl;
*/
  dRightBagPsi= (dRightSlope * (double)usSensorCount) + dRightOffset;
  Serial << "usSensorCount= " << usSensorCount
         << ", dRightBagPsi= " << dRightBagPsi << endl;

  sprintf(_szFormattedString, "Left= %7.2f psi, Right= %7.2f psi", dLeftBagPsi, dRightBagPsi);
  Serial << "_szFormattedString= " << _szFormattedString << endl;
  Serial << "ReadPressureSensors(): Call SerialBT.write()" << endl;
  SerialBT.write((uint8_t*)_szFormattedString, strlen(_szFormattedString));

  //dtostrf(dSensorReading, 8, 4, szNumber);
  //Write sensor reading to BT
  //SerialBT.write((uint8_t*)szNumber, strlen(szNumber));
  return;
} //ReadPressureSensors


void ReadDummyPressures() {
  //uint8_t         szFormattedString[30];
  //char          szFormattedString[30];
  static double dLeftBagPsi   = 60.00;
  static double dRightBagPsi  = 70.00;

  dLeftBagPsi  += 0.01;
  dRightBagPsi += 0.01;
  sprintf(_szFormattedString, "Left= %7.2f psi, Right= %7.2f psi", dLeftBagPsi, dRightBagPsi);
  Serial << "ReadDummyPressures(): " << _szFormattedString << endl;
  SerialBT.write((uint8_t*)_szFormattedString, strlen(_szFormattedString));
  return;
} //ReadDummyPressures


void ReadTouchSensor() {
  const byte  touchPin = 4;   // IO pin for capacitive touch sensing
  char        szNumber[10];
  //float sensorReading = 0;  // Read the analog value for the touch sensor
  double dSensorReading = 0;  // Read the analog value for the touch sensor
  //touchRead() returns an int but I use double because I don't know int version of dtostrf()
  byte sampleSize = 8;        // Number of samples we want to take

  for(byte i= 0; i < sampleSize; i++)  // Average samples together to minimize false readings
  {
    dSensorReading += touchRead(touchPin); // We sample the touch pin here
    delay(10);
  }
  Serial << "loop(): sensorReading= " << dSensorReading << endl;

  dtostrf(dSensorReading, 8, 4, szNumber);
  //Write touch sensor reading to BT
  SerialBT.write((uint8_t*)szNumber, strlen(szNumber));
  return;
} //ReadTouchSensor
//Last line.
