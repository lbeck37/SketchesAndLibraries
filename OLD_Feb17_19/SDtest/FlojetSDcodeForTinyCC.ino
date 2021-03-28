// 7/17/15 Code from early Flojet.ino that wrote to SD card
#include <Arduino.h>
#include <Streaming.h>

#include <SPI.h>
#include <SD.h>

//SD card information
// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
static const int sChipSelect            = 4;
static const int sDefaultChipSelectPin  = 10;


int sSetupSD(){
  Serial << lLineCount++ <<" sSetupSD(): Initializing SD card"<< endl;
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(sDefaultChipSelectPin, OUTPUT);

  // see if the card is present and can be initialized:
  if (SD.begin(sChipSelect)) {
    Serial << lLineCount++ <<" sSetupSD(): SD card is setup"<< endl;
  }
  else {
    Serial << lLineCount++ <<" sSetupSD(): SD card init failed, or not present"<< endl;
  }
  return 1;
}  //sSetupSD


int sWriteGPStoSD(){
  String  szLogLine= "";
  szLogLine += String(lCurrentMsec);
  szLogLine += ", ";
  szLogLine += String(lLineCount);
  if (bPumpIsOn) {
    szLogLine += ", ON , ";
  }
  else {
    szLogLine += ", OFF, ";
  }
  szLogLine += String(fMotorVoltage);

  File LogFile= SD.open("PUMPLOG.txt", FILE_WRITE);
  if (LogFile) {
    LogFile.println(szLogLine);
    LogFile.close();
  }
  else {
    Serial << lLineCount++ << " sWriteGPStoSD(): error opening " << "PUMPLOG.txt" << endl;
  }
  return 1;
}  //sWriteGPStoSD