// 7/23/15 Beck TinyDuino w/GPS and logging to SD card
// This Arduino sketch will log GPS NMEA data to a SD card every second */
// Was GPS-datalogger.ino
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

// The Arduino pins used by the GPS module
static const int GPS_ONOFFPin = A3;
static const int GPS_SYSONPin = A2;
static const int GPS_RXPin    = A1;
static const int GPS_TXPin    = A0;
static const int GPSBaud      = 9600;
static const int chipSelect   = 10;

// The GPS connection is attached with a software serial port
SoftwareSerial Gps_serial(GPS_RXPin, GPS_TXPin);

int inByte                    = 0;         // incoming serial byte
byte pbyGpsBuffer[100];
int byBufferIndex             = 0;

void setup()
{
  //Init the GPS Module to wake mode
  pinMode(GPS_SYSONPin, INPUT);
  pinMode(GPS_ONOFFPin, OUTPUT);
  digitalWrite(GPS_ONOFFPin, LOW);
  delay(5);
  if (digitalRead(GPS_SYSONPin) == LOW) {
     //Need to wake the module
    digitalWrite( GPS_ONOFFPin, HIGH );
    delay(5);
    digitalWrite( GPS_ONOFFPin, LOW );
  } //f(digitalRead(GPS_SYSONPin)==LOW)

  //Open the debug serial port at 9600
  Serial.begin(9600);

  //Open the GPS serial port
  Gps_serial.begin(GPSBaud);

  Serial.print("Initializing SD card...");
  //Make sure that the default chip select pin is set to
  //output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  } //if(!SD.begin(chipSelect))
  Serial.println("card initialized.");
} //setup


void loop()
{
  byte byDataByte;
  if (Gps_serial.available()) {
    byDataByte= Gps_serial.read();
    //Serial.write(byDataByte);
    Serial.println("loop() wrting to SD card");
    pbyGpsBuffer[byBufferIndex++]= byDataByte;

    if(byBufferIndex >= 100) {
      byBufferIndex = 0;
      File dataFile = SD.open("TinyGPS_3.txt", FILE_WRITE);

      // if the file is available, write to it:
      if (dataFile) {
        dataFile.write(pbyGpsBuffer, 100);
        dataFile.close();
      } //if(dataFile)
      // if the file isn't open, pop up an error:
      else {
        Serial.println("error opening TinyGPS datafile.");
      } //if(dataFile)else
    } //if(byBufferIndex>=100)
  } //if(Gps_serial.available())
} //loop

//Last line.
