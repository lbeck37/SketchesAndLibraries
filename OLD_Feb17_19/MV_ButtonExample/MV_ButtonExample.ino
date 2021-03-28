//4/23/15 Microview pushbutton example.

#include <MicroView.h>     // include MicroView library

int buttonPin = A0;        // push button pin
int buttonState = 0;    // variable to store the pushbutton status

void setup() {
   uView.begin();       // start MicroView
   uView.clear(PAGE);      // clear page

   pinMode(buttonPin, INPUT);    // initialize the pushbutton pin as an input
   digitalWrite(buttonPin,HIGH);    // set Internal pull-up
}

void loop() {
   buttonState = digitalRead(buttonPin);  // read the state of the pushbutton value

   // check if the pushbutton is pressed.
   // if it is not pressed, the buttonState is HIGH:
   if (buttonState == HIGH) {
 #if 0
      uView.setCursor(0,0);      // set cursor at 0,0
      uView.print("OFF");        // print OFF
      uView.display();
 #endif
      vPrintLine(0, 0, "Level is HIGH");
   }
   else {
#if 0
      uView.setCursor(0,0);
      uView.print("ON ");
      uView.display();
#endif
      vPrintLine(0, 0, "Level is LOW");
  }
}  //loop

void vPrintLine( int wX, int wY, char *pcText) {
      uView.setCursor(0,0);      // set cursor at 0,0
      uView.print(pcText);        // print OFF
      uView.display();
  return;
}  //vPrintLine
