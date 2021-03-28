/**************************************************************
 * 11/23/15C Add in include of Thermistor library and send as VirtualPin to Blynk
 * 11/21/15 Larry Beck
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 *
 **************************************************************
 * This example runs directly on ESP8266 chip.
 *
 * You need to install this for ESP8266 development:
 *   https://github.com/esp8266/Arduino
 *
 * Please be sure to select the right ESP8266 module
 * in the Tools -> Board menu!
 *
 * Change WiFi ssid, pass, and Blynk auth token to run :)
 *
 **************************************************************/

#include <Streaming.h>
#include <Wire.h>
static long       lLineCount= 0;      //Serial Monitor uses for clarity.
static long       lNumLoops= 1;

//#include <Thermistor.h>
//#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define LOG0      lLineCount++ << " " << millis()

//Create thermistor object on ADC pin 0
//Thermistor thermistor(0);

//Associate DegF value with VirtualPin 2
#define DegF_VirtualPin   V2

#define AnalogInputPin    A0

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "55bce1afbf894b3bb67b7ea34f29d45a";


void setup()
{
  Serial.begin(9600);
  Serial << LOG0 << " setup(): Initialized serial to 9600 baud" << endl;
  Blynk.begin(auth, "dlinky", "Qazqaz11");
} //setup


void loop()
{
  Blynk.run();
} //loop



//BLYNK_READ(VirtualPin) handles the call from a Widget requesting data for a Virtual Pin
BLYNK_READ(DegF_VirtualPin)
{
  //int sDegC= thermistor.getTemp();
  float fDegF= fGetDegF();
  Blynk.virtualWrite(DegF_VirtualPin, fDegF);
} //BLYNK_READ

//10kohm NTC thermistor connected to a measured 3.31v supply and then to a 2.151kohm resistor to ground.
//ADC pin 0 is connected to the junction of the thermisor and resistor so measures the current through the
//thermistor-resistor circuit and the voltage drop of the thermistor.
/******************************************************************/
/* Utilizes the Steinhart-Hart Thermistor Equation:       */
/*   Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]^3}   */
/*     where A = 0.001129148, B = 0.000234125 and C = 8.76741E-08  */
/******************************************************************/
float fGetDegF() {
  float fDegF;
  float fSupplyVolts      = 3.32;
  float fCurrentSenseOhms = 2151;
  float fOneVoltCount     = 1023;

  Serial << LOG0 << endl;

  int   sVoltCount= analogRead(A0);
  Serial << LOG0 << " fGetDegF(): analogRead() returned " << sVoltCount << endl;

  float fSenseVolts= (sVoltCount / fOneVoltCount);
  Serial << LOG0 << " fGetDegF(): fSenseVolts= " << fSenseVolts << endl;

  float fAmps= fSenseVolts / fCurrentSenseOhms;
  Serial << LOG0 << " fGetDegF(): fAmps= " << fAmps << endl;

  float fThermistorVolts= fSupplyVolts - fSenseVolts;
  Serial << LOG0 << " fGetDegF(): fThermistorVolts= " << fThermistorVolts << endl;

  float fThermistorOhms= fThermistorVolts / fAmps;
  Serial << LOG0 << " fGetDegF(): fThermistorOhms= " << fThermistorOhms << endl;

  float fLogR= log(fThermistorOhms);
  float fLogRcubed= fLogR * fLogR * fLogR;

  float fDegK= 1.0 / (0.001129148 + (0.000234125 * fLogR) + (0.0000000876741 * fLogRcubed));
  float fDegC= fDegK - 273.15;  // Convert Kelvin to Celsius

  fDegF= (fDegC * 1.8) + 32.0;

  Serial << LOG0 << " fGetDegF(): Returning fDegF= " << fDegF << endl;
  return fDegF;
}  //fGetDegF

//Last line.
