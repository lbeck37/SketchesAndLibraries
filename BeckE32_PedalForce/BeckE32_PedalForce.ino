const String SketchName  = "BeckE32_PedalForce";
const String FileDate    = "May 29, 2018-g";
//Beck 2/13/17, from Adafruit example ssd1306_128x64_i2c.ino
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HX711.h>
#include <BeckE32_BLE_ServerLib.h>
#include <soc/rtc.h>
#include <Streaming.h>
#include <iostream>   // std::cout
#include <string>     // std::string, std::to_string

//#define OLED_RESET 4
//Adafruit_SSD1306 oDisplay(OLED_RESET);

Adafruit_SSD1306    oDisplay(-1);   //Looks like -1 is default

#if (SSD1306_LCDHEIGHT != 64)
  #error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

const uint32_t	ulOneMillion				= 1000000;
const uint32_t	ulCPUFreq						= 80 * ulOneMillion;
const uint16_t	usClockDivider			= 80;
const uint32_t	ulTicksPerSec				= ulCPUFreq / usClockDivider;
const uint32_t	ulRedAlarmTicks			= ulOneMillion / 2;
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

  //Slow down CPU so HX711 will work with ESP32
  //Serial.println("setup(): Call rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M)");
  Serial << "setup(): Call rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M)" << endl;
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

  SetupLEDTimers();

  Serial << "setup(): Setup load cells" << endl;
  oPedalForce.begin(cHX711_DOUT, cHX711_SCK);   //Use default gain
  oPedalForce.power_down();             // put the ADC in sleep mode

  Serial << "setup(): Call Wire.begin(21, 22)" << endl;
  Wire.begin(21, 22);   //Beck 1-3-18
  ScanForDevices();

  StartOLED();
  oPedalForce.power_up();
  SetupBLEServer();
  Serial << "setup(): Waiting for a client connection to notify..." << endl;
  return;
} //setup


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


void loop() {
  double    dLbs;
  //TestDashboard();
  dLbs= ReadPedal();
  DisplayPedal(dLbs);
  DoBLENotify(dLbs);
  //LogPedal(dLbs);
  //delay(1000);
  delay(300);	//~One loop per second with 80MHz CPU
  //delay(10);    //32 samples/sec with 80MHz CPU to Android app
  return;
} //loop


void StartOLED(){
  Serial << "StartOLED(): Call oDisplay.begin() " << endl;
  //By default, we'll generate the high voltage from the 3.3v line internally! (neat!) Beck????
  //oDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
  oDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  // Clear the buffer.
  Serial << "StartOLED(): Setup and clear OLED." << endl;
  oDisplay.setTextSize(2);
  oDisplay.setTextColor(WHITE);
  oDisplay.clearDisplay();
  oDisplay.display();
  return;
} //StartOLED


double ReadPedal(){
  long    lValue;
  double  dLbs;

  oPedalForce.power_up();
  lValue= oPedalForce.read();
  oPedalForce.power_down();             // put the ADC in sleep mode

  dLbs= ((double)lValue - dZeroCnt) / dCntsPerLb;
  //dtostrf(dLbs, 8, 4, szNumber);
  //Serial << "ReadPedal(): Count= " << lValue << ", Lbs= " << szNumber << endl;
  _lValue= lValue;
  return(dLbs);
} //ReadPedal


void LogPedal(double dLbs){
  char szNumber[10];

  dtostrf(dLbs, 8, 4, szNumber);
  Serial << "LogPedal(): Count= " << _lValue << ", Lbs= " << szNumber << endl;
  return;
} //LogPedal


void DisplayPedal(double dLbs){
  char szNumber[10];
  oDisplay.clearDisplay();
  //oDisplay.setTextSize(2);
  oDisplay.setTextColor(WHITE);
  oDisplay.setCursor(0,0);

  oDisplay.setTextSize(1);
  oDisplay.println(SketchName);
  oDisplay.println(FileDate);
  oDisplay.println();

  oDisplay.setTextSize(2);
  oDisplay.println(_lValue);

  oDisplay.setTextSize(1);
  oDisplay.println();

  dtostrf(dLbs, 7, 2, szNumber);
  oDisplay.setTextSize(2);
  oDisplay.println(szNumber);

  oDisplay.display();
  return;
} //DisplayPedal


void TestDashboard(void) {
  //float val;
  char szNumber[10];
  static double dDigit= 1.0;
  double dNumber;
  oDisplay.setTextSize(2);
  oDisplay.setTextColor(WHITE);
  oDisplay.setCursor(0,0);
  for (int sLine= 1; sLine <= 4; sLine++){
    dNumber= dDigit + (2. * dDigit / 10.0) + (3. * dDigit / 100.0) + (4. * dDigit / 1000.0);
    //dNumber= 1.234;
    dtostrf(dNumber, 8, 4, szNumber);
    Serial << "TestDashboard(): dNumber= " << dNumber << endl;
    oDisplay.println(szNumber);
    dDigit += 1.0;
  }
  oDisplay.display();
  Serial << endl;
/*
  oDisplay.println("+3.101");
  oDisplay.println("+3.202");
  oDisplay.println("+3.303");
  oDisplay.println("+3.404");
*/
  delay(5000);
  //Serial.println("Dashboard(): Call clearDisplay()");
  oDisplay.clearDisplay();
  oDisplay.display();
  //delay(1000);
  return;
} //TestDashboard


void ScanForDevices(void){
  byte ucError, ucAddress;
  int nDevices;
  Serial.println("ScanForDevices(): Begin");

  Serial.println("Scanning...");
  nDevices = 0;
  for(ucAddress = 1; ucAddress < 127; ucAddress++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(ucAddress);
    ucError = Wire.endTransmission();

    if (ucError == 0)
    {
      //SetDevicePresent(ucAddress);
      Serial.print("I2C device found at address 0x");
      if (ucAddress<16) {
        Serial.print("0");
      }
      Serial.print(ucAddress,HEX);
      Serial.println("  !");
      nDevices++;
    }
    else if (ucError==4)
    {
      Serial.print("Unknown error at address 0x");
      if (ucAddress<16) {
        Serial.print("0");
      }
      Serial.println(ucAddress,HEX);
    }
  }
 if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done");
  return;
} //ScanForDevices
//Lastline
