/***** BlynkESP8266_Fireplace.ino *****
 * 11/29/15 Created from Blynk_ESP8266_Thermostat.ino
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 **************************************************************
 * This example runs directly on ESP8266 chip.
 * You need to install this for ESP8266 development:
 *   https://github.com/esp8266/Arduino
 * Please be sure to select the right ESP8266 module
 * in the Tools -> Board menu!
 * Change WiFi ssid, pass, and Blynk auth token to run :)
 **************************************************************/

#include <Streaming.h>
//#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>

//Get Blynk Auth Token from the Blynk App, go to the Project Settings (nut icon).
char acBlynkAuthToken[] = "55bce1afbf894b3bb67b7ea34f29d45a";

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;

int16_t   ax, ay, az;
int16_t   gx, gy, gz;
int16_t   mx, my, mz;

float     fDegF;

#define LOG0      lLineCount++ << " " << millis()

//Associate DegF value with VirtualPins 2 and 3
#define DegF_VirtualPin1  V1
#define DegF_VirtualPin2  V2
#define DegF_VirtualPin3  V3
#define AnalogInputPin    A0

static int        sGyroTemperature;
static long       lLineCount= 0;      //Serial Monitor uses for clarity.
static long       lNumLoops= 1;

void setup()
{
  Serial.begin(9600);
  Serial << LOG0 << " setup(): Initialized serial to 9600 baud" << endl;
  Wire.begin();
  accelgyro.initialize();

  // verify connection
  Serial << LOG0 << " setup(): Calling testConnection()" << endl;

  if (accelgyro.testConnection()) {
    Serial << LOG0 << " setup(): MPU6050 connection successful" << endl;
  }
  else {
    Serial << LOG0 << " setup(): MPU6050 connection failed" << endl;
  }

  Blynk.begin(acBlynkAuthToken, "dlinky", "Qazqaz11");
} //setup


void loop()
{
  Blynk.run();
} //loop



//BLYNK_READ(VirtualPin) handles the call from a Widget requesting data for a Virtual Pin
BLYNK_READ(DegF_VirtualPin1)
{
  Blynk.virtualWrite(DegF_VirtualPin1, sGyroTemperature);
} //BLYNK_READ(DegF_VirtualPin1)


//BLYNK_READ(VirtualPin) handles the call from a Widget requesting data for a Virtual Pin
BLYNK_READ(DegF_VirtualPin2)
{
  float fDegF= fGetGyroDegF();
  Serial << LOG0 << " BLYNK_READ(DegF_VirtualPin2): fGetGyroDegF() returned " << fDegF << endl;
  Blynk.virtualWrite(DegF_VirtualPin2, fDegF);
} //BLYNK_READ(DegF_VirtualPin2)


BLYNK_READ(DegF_VirtualPin3)
{
  float fDegF= fGetGyroDegF();
  Serial << LOG0 << " BLYNK_READ(DegF_VirtualPin3): fGetGyroDegF() returned " << fDegF << endl;
  Blynk.virtualWrite(DegF_VirtualPin3, fDegF);
} //BLYNK_READ(DegF_VirtualPin3)


float fGetGyroDegF() {
  float fDegC;
  float fDegF;
  //accelgyro.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
  sGyroTemperature= accelgyro.getTemperature();

  //fDegC= (sGyroTemperature / 340.0) + 36.53;    //Numbers from datasheet.
  fDegC= (sGyroTemperature / 340.0) + 33.0;
  fDegF= (fDegC * 1.8) + 32.0;

  return fDegF;
}  //fGetGyroDegF
//Last line.
