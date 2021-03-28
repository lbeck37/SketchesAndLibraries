#include <EasyButton.h>
#include <Streaming.h>

const int sPinButton1 = 10;
const int sPinButton2 = 12;
const int sPinLED     = 13;

const boolean bPullUp= true;
#if 1
EasyButton button(sPinButton1, NULL, CALL_NONE, bPullUp);
#else
EasyButton button(sPinButton1, vCallBackFunction, CALL_IN_ALL, bPullUp);
#endif

void vCallBackFunction()
{
  Serial << "vCallBackFunction(): Begin" << endl;
  Serial << "IsPushed = " << button.IsPushed() << endl;
  Serial << "InPush = " << button.InPush() << endl;
  Serial << "IsHold = " << button.IsHold() << endl;
  Serial << "IsRelease = " << button.IsRelease() << endl;

#if 1
  if (false && button.IsPushed()) {
    Serial << "loop(): IsPushed is TRUE" << endl;
  }
  if (button.InPush()) {
    Serial << "loop(): IsPushed is TRUE" << endl;
  }
  #endif
}  //vCallBackFunction()


void setup()
{
  Serial.begin(9600);
  Serial << "setup(): Begin" << endl;
  pinMode(sPinLED, OUTPUT);
}  //setup()


void loop()
{
  button.update();
#if 1
  if (button.IsPushed()) {
    Serial << "loop(): IsPushed is TRUE" << endl;
    digitalWrite(sPinLED, HIGH);
  }
  else {
    //Serial << "loop(): IsPushed is FALSE" << endl;
    digitalWrite(sPinLED, LOW);
 }
 //delay(500);
#endif
}  //loop()

