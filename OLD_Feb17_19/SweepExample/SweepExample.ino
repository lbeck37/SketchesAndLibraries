/* Sweep
 by BARRAGAN <http://barraganstudio.com> 
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://arduino.cc/en/Tutorial/Sweep
*/ 

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // twelve servo objects can be created on most boards
 
int pos = 0;    // variable to store the servo position 
 
void setup() 
{ 
  myservo.attach(11);  // attaches the servo on pin 11 to the servo object 

  //Go to ), 90, 180 degrees
  for(pos = 0; pos <= 180; pos += 90)  
  {
  myservo.write(pos);            // tell servo to go to position 
  delay(1000);                   // Delay 1 seconds 
  }

  for(pos = 180; pos >= 0; pos -= 90)  
  {
  myservo.write(pos);            // tell servo to go to position 
  delay(1000);                   // Delay 1 seconds 
  }

} 
 
void loop() 
{ 
  for(pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(5);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(25);                       // waits 15ms for the servo to reach the position 
  } 
} 

