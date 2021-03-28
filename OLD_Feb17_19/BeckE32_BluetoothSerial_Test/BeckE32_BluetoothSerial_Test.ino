const String SketchName  = "BeckE32_BluetoothSerial_Test.ino";
const String FileDate    = "June 2, 2018-a";
//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include <Streaming.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  Serial << endl << "setup(): Begin " << SketchName << ", " << FileDate << endl;

  //SerialBT.begin("ESP32_RightPedal"); //Bluetooth device name
  SerialBT.begin("Beck32_Test082018"); //Bluetooth device name

  //Serial.println("The device started, now you can pair it with bluetooth!");
  Serial << "setup(): Bluetooth ESP32_RightPedalhas been started." << endl;
  Serial << "setup(): You can now Bluetooth pair it from your phone." << endl;
}

void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
  SendDummyData();
  delay(20);
}

void SendDummyData(){
  static double dDigit= 1.0;
  char szNumber[10];
  //uint8_t 	szNumber[10];
  double 		dNumber;
  for (int sLine= 1; sLine <= 4; sLine++){
    dNumber= dDigit + (2. * dDigit / 10.0) + (3. * dDigit / 100.0) + (4. * dDigit / 1000.0);
    //dNumber= 1.234;
    dtostrf(dNumber, 8, 4, szNumber);
    Serial << "SendDummyData(): dNumber= " << dNumber << endl;
    dDigit += 1.0;
  }	//for
  int sStringLength= strlen(szNumber);
  SerialBT.write((uint8_t*)szNumber, sStringLength);
  delay(1000);
  return;
}	//SendDummyData
//Last line.
