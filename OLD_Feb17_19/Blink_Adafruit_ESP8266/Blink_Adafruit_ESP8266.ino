// 08/07/15 Add Serial Monitor output
#include <Arduino.h>
#include <Streaming.h>

#define LOG0      lLineCount++ << " " << millis()
static long       lLineCount= 0;      //Serial Monitor uses for clarity.

void setup() {
  Serial.begin(115200);
  //sWaitForSerialMonitor();
  Serial << LOG0 << " setup(): Begin" << endl;
  //pinMode(0, OUTPUT);
}  //setup

void loop() {
   Serial << LOG0 << " loop(): Begin" << endl;
   //digitalWrite(0, HIGH);
   //delay(1500);
   delay(500);
   //digitalWrite(0, LOW);
   //delay(500);
}  //loop


int sWaitForSerialMonitor() {
  //The following (for Leonardo only) waits for SerialMonitor to start on PC.
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  return 1;
}  //sWaitForSerialMonitor
//Last line