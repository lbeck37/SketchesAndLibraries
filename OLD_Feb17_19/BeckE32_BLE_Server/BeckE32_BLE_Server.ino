const char szSketchName[]  = "BeckE32_BLE_Server.ino";
const char szFileDate[]    = "May 13, 2018-b";
#include <BeckE32_BLE_ServerLib.h>

void setup() {
  Serial.begin(115200);
  Serial << endl << endl<< endl<< endl;
  Serial << "setup(): Begin " << szSketchName << ", " << szFileDate << endl;

  //Slow down CPU since HX711 needs to work with ESP32, verify BTE works
  Serial.println("setup(): Call rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M)");
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
  //pinMode(LED, OUTPUT);

  SetupBLEServer();
  Serial.println("Waiting for a client connection to notify...");
  return;
} //setup


void loop() {
  //static int  sCount= 1;
  dNotifyValue += 2.0;
  DoBLENotify(dNotifyValue);
/*
  char szNotifyString[16]; // make sure this is big enough
  if (deviceConnected){
    dNotifyValue += 2.0;
    //Convert the value to a string:
    dtostrf(dNotifyValue, 1, 4, szNotifyString);  // float_val, min_width, digits_after_decimal, char_buffer
    //pCharacteristic->setValue(&txValue, 1); // To send the integer value
    //pCharacteristic->setValue("Hello!");    // Sending a test message
    pNotifyCharact->setValue(szNotifyString);
    pNotifyCharact->notify();                 //Send the value to the client
    Serial << "Server: loop(): Sent Notify Value= " << szNotifyString << endl;
  } //if (deviceConnected)
*/
  //delay(1000);
  delay(300);	//~One loop per second with 80MHz CPU
  //delay(10);    //32 samples/sec with 80MHz CPU to Android app
  return;
} //Loop
//Last line.
