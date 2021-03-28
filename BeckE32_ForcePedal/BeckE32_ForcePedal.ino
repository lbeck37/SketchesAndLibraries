const String SketchName  = "BeckE32_ForcePedal.ino";
const String FileDate    = "June 8, 2018-e";
#include <HX711.h>
#include <Streaming.h>
#include <soc/rtc.h>

#include "BluetoothSerial.h"		//Classic Bluetooth, not BLE
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

const uint32_t	ulOneMillion				= 1000000;
const uint32_t	ulCPUFreq						= 80 * ulOneMillion;
const uint16_t	usClockDivider			= 80;
const uint32_t	ulTicksPerSec				= ulCPUFreq / usClockDivider;
const uint32_t	ulRedAlarmTicks			= ulOneMillion;
const uint32_t	ulGreenAlarmTicks		= ulOneMillion / 4;
const uint8_t		ucRedTimerNum				= 0;
const uint8_t		ucGreenTimerNum			= 1;

const int    sRedLED   			= 12;
const int    sGreenLED   		= 15;

const byte    cHX711_DOUT   = 25;       //IO pin number was 34 and 35
const byte    cHX711_SCK    = 26;
const byte    cHX711_Gain   = 128;      //Default gain is 128

const double      dZeroCnt    = -118000.0;
const double      dCntsPerLb  =  -10000.0;

long      _lValue;
HX711     oPedalForce;

volatile byte cRedLEDState 			= LOW;
volatile byte cGreenLEDState 		= LOW;

//Create timers for LEDs
hw_timer_t 		*pRedLEDTimer 		= NULL;
hw_timer_t 		*pGreenLEDTimer 	= NULL;


void IRAM_ATTR onRedLEDTimer(){
	cRedLEDState = !cRedLEDState;
  digitalWrite(sRedLED, cRedLEDState);
  return;
}	//onRedLEDTimer


void IRAM_ATTR onGreenLEDTimer(){
	cGreenLEDState = !cGreenLEDState;
  digitalWrite(sGreenLED, cGreenLEDState);
  return;
}	//onGreenLEDTimer


void setup()   {
  Serial.begin(115200);
  Serial << endl << "setup(): Begin " << SketchName << ", " << FileDate << endl;

  SerialBT.begin("Beck_RightPedal"); //Bluetooth device name

  //Slow down CPU so HX711 will work with ESP32
  Serial << "setup(): Call rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M)" << endl;
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

  SetupLEDTimers();

  Serial << "setup(): Setup load cells" << endl;
  oPedalForce.begin(cHX711_DOUT, cHX711_SCK);   //Use default gain
  //oPedalForce.begin(cHX711_DOUT, cHX711_SCK, 32);   //Use default gain
  //oPedalForce.power_down();             // put the ADC in sleep mode

//  Serial << "setup(): Call Wire.begin(21, 22)" << endl;
//  Wire.begin(21, 22);   //Beck 1-3-18

  //oPedalForce.power_up();
  return;
} //setup


void loop() {
	static int  sLogCount= 0;
	static long	lLastMillis= 0;
	long				lDeltaMillis;
	long				lCurrentMillis;
	int					sLogSkip= 25;
  double    	dLbs= 3.7;
  char 				szNumber[10];

  dLbs= ReadPedal();
  if ((sLogCount++ % sLogSkip) == 0) {
  	lCurrentMillis= millis();
  	lDeltaMillis= lCurrentMillis - lLastMillis;
  	lLastMillis= lCurrentMillis;
  	LogPedal(sLogCount, dLbs, lDeltaMillis);
  }	//if ((sLogCount++MOD...
  dtostrf(dLbs, 8, 4, szNumber);
  //Write pedal force to BT
  SerialBT.write((uint8_t*)szNumber, strlen(szNumber));

  //delay(1000);
  //delay(300);	//delay(300) is ~1 loop per second with 80MHz CPU
  //delay(10);    //delay(10) is 32 samples/sec with 80MHz CPU to Android app
  return;
} //loop


void SetupLEDTimers(){
  Serial << "SetupLEDTimers(): Begin" << endl;
  pinMode(sRedLED, OUTPUT);
  pinMode(sGreenLED, OUTPUT);

  /* Use 1st timer of 4 */
  /* 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up */
  //Timers 0, 1, 2, 3 are available usClockDivider
  pRedLEDTimer 	= timerBegin(ucRedTimerNum, usClockDivider, true);
  pGreenLEDTimer= timerBegin(ucGreenTimerNum, usClockDivider, true);

  /* Attach onTimer function to our timer */
  timerAttachInterrupt(pRedLEDTimer, &onRedLEDTimer, true);
  timerAttachInterrupt(pGreenLEDTimer, &onGreenLEDTimer, true);

  /* Set alarm to call onTimer function every second 1 tick is 1us
  => 1 second is 1000000us */
  /* Repeat the alarm (third parameter) */
  timerAlarmWrite(pRedLEDTimer, ulRedAlarmTicks, true);

  timerAlarmWrite(pGreenLEDTimer, ulGreenAlarmTicks, true);

  //Start alarm for timers
  timerAlarmEnable(pRedLEDTimer);
  timerAlarmEnable(pGreenLEDTimer);

  Serial << "SetupLEDTimers(): Timers started" << endl;
	return;
}	//SetupLEDTimer


double ReadPedal(){
  long    lValue= 3737;
  double  dLbs;

  //oPedalForce.power_up();
  //lValue= oPedalForce.read();
  lValue= oPedalForce.read_average(4);
  //oPedalForce.power_down();             // put the ADC in sleep mode

  dLbs= ((double)lValue - dZeroCnt) / dCntsPerLb;
  _lValue= lValue;
  return(dLbs);
} //ReadPedal


void LogPedal(int sCount, double dLbs, long lDeltaMillis){
  char szNumber[10];

  dtostrf(dLbs, 8, 4, szNumber);
  //Serial << "LogPedal(): Count= " << _lValue << ", Lbs= " << szNumber << endl;
  Serial << "LogPedal(): Reading number " << sCount << ", Lbs= " << szNumber
  		   << ", lDeltaMillis= " << lDeltaMillis << endl;
  return;
} //LogPedal
//Lastline
