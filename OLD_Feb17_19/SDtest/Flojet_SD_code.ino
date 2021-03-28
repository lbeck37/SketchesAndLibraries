// 7/17/15 Code from early Flojet.ino that wrote to SD card
#include <Arduino.h>
#include <Streaming.h>
#ifdef USE_SD
  #include <SPI.h>
  #include <SD.h>
#endif

#ifdef USE_SD
//SD card information
// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
static const int sChipSelect            = 4;
static const int sDefaultChipSelectPin  = 10;
static const int    sMotorVoltsPin  = 0;      //Arduino A0 pin.
static const float  fMotorVoltsConvert= 19.55;  //(4 * 5V *1000 / 1023)
static const float  fMinVoltsToStop   = 12.80;  //Pump is dry if running volts are above this.
static float      fMotorVoltage;
//Reads motor voltage at A0, scaled 4:1, 20V FS => 5V at A0.

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


int sReadMotorVolts(){
  int sMotorVoltsRaw= analogRead(sMotorVoltsPin);
  fMotorVoltage = (fMotorVoltsConvert * sMotorVoltsRaw) / 1000.0;
  return 1;
}  //sReadMotorVolts


int sLogMotorVolts(){
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
    Serial << lLineCount++ << " sLogMotorVolts(): error opening " << "PUMPLOG.txt" << endl;
  }
  return 1;
}  //sLogMotorVolts
#endif  //USE_SD
// Last line.