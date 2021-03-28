static const String SketchName  = "BeckE32_RGB_LEDTest.ino";
static const String FileDate    = "Dec 19, 2017, Lenny-a";

//#include <Wire.h>
#include <BeckMiniLib.h>
//#include <BeckAtoD.h>
//
//static const INT16       sI2C_SDA				= 26;
//static const INT16       sI2C_SCL				= 27;
//
//BeckI2C     I2C(sI2C_SDA, sI2C_SCL);
//BeckAtoD    AtoD(&I2C, eADS1115);

void setup(void){
  Serial.begin(115200);
  Serial << endl << "setup(): Begin " << SketchName << ", " << FileDate << endl;
  //I2C.Begin();
} //setup


void loop(void){
/*
	static double	dAmps;
	static double	dVolts;

  for (int sChan= sMotorVoltsChan; sChan <= sDACVddChan; sChan++){
		switch (sChan) {
			case sMotorVoltsChan:
			case sACS712VccChan:
			case sDACVddChan:
				dVolts= AtoD.dReadRealVolts(sChan);
				Serial << "loop(): Channel= " << sChan << ", dVolts=  " << dVolts << endl << endl;
				break;
			case sMotorAmpChan:
	  		dAmps= AtoD.dReadAmps(sChan);
				Serial << "loop(): Channel= " << sChan << ", dAmps=  " << dAmps << endl << endl;
				break;
			default:
				Serial << "loop(): Bad switch, sChan= " << sChan << endl;
				break;
		} //switch
  } //for(int sChan=0;

  for (double dVolt= 0; dVolt <= 3.00; dVolt += 0.50){
  		AtoD.WriteDACVolts(dVolt);
  	  delay(1000);
  }	//for
  delay(5000);
*/
	static INT16	sCount= 0;
	Serial << "loop(): sCount= " << sCount++ << endl;
  delay(1000);
} //loop
//Last line.
