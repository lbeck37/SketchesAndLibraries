// 08/02/15B Modified to only read meter for one second.
// YF-S201 Hall Effect Water Flow Meter / Sensor
// http://www.hobbytronics.co.uk/yf-s201-water-flow-meter

//Uses interrupt, Arduino pin must support interrupts.
//see http://arduino.cc/en/Reference/attachInterrupt

//Flowmeter produces 450 pulses/liter => 1,700 pulses/gallon
//Expect 5 gal/minute which produces 8,500 pulses/min => 142 pulses/sec
//Want to count for one second and calculate gal/min
//Multiply 1 second count by 60 and divide by 1,700.

#include <Arduino.h>
#include <Streaming.h>

#define LOG0            lLineCount++ << " " << millis()

//const int     sPulsesPerGal= 1700;
const float   fPulsesPerGal   = 1700.0;

volatile int  sFlowCount;
//Leonardo has INT0 on pin 3, Uno is on pin 2.
//Not sure if pinMode() or digitalWrite() needs to be performed as I got Leonardo
//going with pin 2 getting set up but sensor plugged into pin 3 for INT0
unsigned char ucFlowMeterPin  = 3;      // Flow Meter Pin number, must support interrupt.
long          lLineCount      = 0;      //Serial Monitor uses for clarity.

void vIncrementFlowCount()                  // Interruot function
{
   sFlowCount++;
   return;
} //vIncrementFlowCount


void setup()
{
  pinMode(ucFlowMeterPin, INPUT);
  digitalWrite(ucFlowMeterPin, HIGH);              //Turn on internal pullup.
  Serial.begin(9600);
  Serial << LOG0 << " setup(): Attach interrupt to pin "<< ucFlowMeterPin << endl;

  attachInterrupt(0, vIncrementFlowCount, RISING); // Setup Interrupt
  return;
} //setup


void loop ()
{
  //Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min. (Results in +/- 3% range)
  //(Pulse frequency x 60 min) / 7.5Q = flow rate in L/hour
  //Reset pulse count and turn on interrupts.
  sFlowCount= 0;
  sei();                            // Enable interrupts
  delay(1000);                      //Count for one second.
  cli();                            //Turn off interrupts.

  float fGalPerMin= (sFlowCount * 60.0 / fPulsesPerGal);
  Serial << LOG0 << " loop(): fGalPerMin= "<< fGalPerMin << endl;
  return;
} //loop
//Last line.
