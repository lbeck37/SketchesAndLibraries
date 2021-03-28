// 08/02/15 Beck
/* YF-S201 Hall Effect Water Flow Meter / Sensor
   http://www.hobbytronics.co.uk/yf-s201-water-flow-meter
   Uses interrupt routine.
   Read Water Flow Meter and output reading in litres/hour
*/

volatile int  sFlowCount;  // Measures flow meter pulses
unsigned int  usLitersPerHour;          // Calculated litres/hour
unsigned char ucFlowMeterPin = 2;  // Flow Meter Pin number
unsigned long ulCurrentMsec;
unsigned long ulLastPrintMsec;

void vIncrementFlowCount()                  // Interruot function
{
   sFlowCount++;
   return;
} //vIncrementFlowCount


void setup()
{
   pinMode(ucFlowMeterPin, INPUT);
   Serial.begin(9600);
   //Turn on interrupt.
   attachInterrupt(0, vIncrementFlowCount, RISING); // Setup Interrupt
                                     // see http://arduino.cc/en/Reference/attachInterrupt
   sei();                            // Enable interrupts
   ulCurrentMsec= millis();
   ulLastPrintMsec= ulCurrentMsec;

   return;
} //setup


void loop ()
{
  //Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min. (Results in +/- 3% range)
  //(Pulse frequency x 60 min) / 7.5Q = flow rate in L/hour
  //Datasheet shows:
  //  Flow range: 100 L/Hr to 1800 L/Hr?

  ulCurrentMsec= millis();
  // Every second, calculate and print litres/hour
  if(ulCurrentMsec >= (ulLastPrintMsec + 1000)) {
    ulLastPrintMsec= ulCurrentMsec;
    usLitersPerHour= (sFlowCount * 60 / 7.5);
    sFlowCount= 0;                   // Reset Counter
    Serial.print(usLitersPerHour, DEC);            // Print litres/hour
    Serial.println(" Liter/hour");
  } //if(ulCurrentMsec>=...
} //loop
//Last line.
