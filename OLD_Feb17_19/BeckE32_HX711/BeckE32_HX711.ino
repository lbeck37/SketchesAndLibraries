#include "HX711.h"
#include <soc/rtc.h>
#include <Streaming.h>

// HX711.DOUT	- pin #A1
// HX711.PD_SCK	- pin #A0

const byte 		cHX711_DOUT		= 25;				//IO pin number was 34 and 35
const byte		cHX711_SCK		= 26;
const byte		cHX711_Gain		= 128;			//Default gain is 128

//double			dZeroCnt		= -127000.0;
//double			dZeroCnt		= -164000.0; 		//Count= -216951, Lbs=   5.2951
//double			dZeroCnt		= -220000.0;			//Count= -127098, Lbs=  -9.2902
//double			dZeroCnt		= -124000.0;
double			dZeroCnt		= -118000.0;
double			dCntsPerLb	=  -10000.0;

//HX711			oPedalForce(cHX711_DOUT, cHX711_SCK, cHX711_Gain);
HX711			oPedalForce;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("BeckE32_HX711.ino, Apr 22, 2018-n");

  Serial.println("setup(): Call rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M)");
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

  oPedalForce.begin(cHX711_DOUT, cHX711_SCK);		//Use default gain
  oPedalForce.power_down();			        // put the ADC in sleep mode
/*
  //Set the scale.
  Serial.print("oPedalForce.set_scale(2280.f): \t\t");
  oPedalForce.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details

  Serial.println("Call oPedalForce.tar()");
  oPedalForce.tare();				        // reset the scale to 0

  Serial.println("After setting up the scale:");
  TakeReadings();
*/
  return;
}	//setup


void loop() {
	long		lValue;
	double	dLbs;
	char szNumber[10];
  oPedalForce.power_up();
	lValue= oPedalForce.read();
  oPedalForce.power_down();			        // put the ADC in sleep mode

	dLbs= ((double)lValue - dZeroCnt) / dCntsPerLb;
	dtostrf(dLbs, 8, 4, szNumber);
  Serial << endl << "loop(): Count= " << lValue << ", Lbs= " << szNumber << endl;

  delay(1000);
  return;
}	//loop


void TakeReadings(){
	long		lValue;
  Serial.println("TakeReadings(): Call oPedalForce.read()");
  //Serial.print("read: \t\t");
  //Serial.println(oPedalForce.read());			// print a raw reading from the ADC
  lValue= oPedalForce.read();
  Serial << "TakeReadings(): oPedalForce.read() returned " << lValue << endl;

/*
  ulValue= oPedalForce.read_average(20);
  Serial << "TakeReadings(): oPedalForce.read_average(20) returned " << ulValue << endl;
*/

  lValue= oPedalForce.get_value(1);
  Serial << "TakeReadings(): oPedalForce.get_value(1) returned " << lValue << endl;

  // Print the average of n readings from the ADC minus tare weight (not set) divided
	// by the SCALE parameter (not set yet)
  lValue= oPedalForce.get_units(1);
  Serial << "TakeReadings(): oPedalForce.get_units(1) returned " << lValue << endl;

	return;
}	//TakeReadings
//Last line
