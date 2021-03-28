static const char szSketchName[]  = "BeckFirstESP32.ino";
static const char szFileDate[]    = "Apr 3, 2018, Lenny-b";

#include "Arduino.h"
#include <Streaming.h>

//The setup function is called once at startup of the sketch
void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");
  Serial << endl << "setup(): Begin " << szSketchName << ", " << szFileDate << endl;
}

// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here
}
