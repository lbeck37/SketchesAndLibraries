// 4/10/15 Beck- Port from Arduino 0022 to 1.6.3
// 4/11/15- Remove non-text items
// 4/13/15- Add servo with pot code

#include <ST7565.h>
#include <Servo.h>

Servo myservo;      // create servo object to control a servo
int potpin = 2;     // analog pin used to connect the potentiometer

int ledPin =  13;    // LED connected to digital pin 13

// the LCD backlight is connected up to a pin so you can turn it on & off
#define BACKLIGHT_LED 10

// pin 9 - Serial data out (SID)
// pin 8 - Serial clock out (SCLK)
// pin 7 - Data/Command select (RS or A0)
// pin 6 - LCD reset (RST)
// pin 5 - LCD chip select (CS)
ST7565 glcd(9, 8, 7, 6, 5);

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

// a bitmap of a 16x16 fruit icon
//static unsigned char __attribute__ ((progmem)) logo16_glcd_bmp[]={
static const unsigned char __attribute__ ((progmem)) logo16_glcd_bmp[]={
0x30, 0xf0, 0xf0, 0xf0, 0xf0, 0x30, 0xf8, 0xbe, 0x9f, 0xff, 0xf8, 0xc0, 0xc0, 0xc0, 0x80, 0x00,
0x20, 0x3c, 0x3f, 0x3f, 0x1f, 0x19, 0x1f, 0x7b, 0xfb, 0xfe, 0xfe, 0x07, 0x07, 0x07, 0x03, 0x00, };

// The setup() method runs once, when the sketch starts
void setup()   {
  Serial.begin(9600);
  Serial.print(freeRam());

  myservo.attach(11);  // attaches the servo on pin 11 to the servo object
  
  // turn on backlight
  pinMode(BACKLIGHT_LED, OUTPUT);
  digitalWrite(BACKLIGHT_LED, HIGH);

  // initialize and set the contrast to 0x18
  glcd.begin(0x18);

  glcd.display(); // show splashscreen
  delay(2000);
  glcd.clear();

  // draw a string at location (0,0)
  glcd.drawstring(0, 0, "You are the Dude2!");
  glcd.display();
  delay(1000);
  glcd.clear();
}

void loop() 
{ 
  int sPotVal;        // variable to read the value from the analog pin
  int sServoDegrees;   
  char szServoDegrees[10];
  char szLineBuffer[50];
  
  sPotVal = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023) 
  sServoDegrees = map(sPotVal, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180) 
  
  myservo.write(sServoDegrees);                  // sets the servo position according to the scaled value 
  delay(15);                           // waits for the servo to get there 
  
  itoa(sServoDegrees, szServoDegrees, 10);
  strcpy(szLineBuffer, "Servo = ");
  strcat(szLineBuffer, szServoDegrees);
  strcat(szLineBuffer, " degrees");
  
  glcd.clear();
  glcd.drawstring(0, 0, szLineBuffer);
  glcd.display();
  //delay(1000);  
  glcd.clear();
} 

// this handy function will return the number of bytes currently free in RAM, great for debugging!
int freeRam(void)
{
  extern int  __bss_end;
  extern int  *__brkval;
  int free_memory;
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
  }
  return free_memory;
}

void testdrawchar(void) {
  for (uint8_t i=0; i < 168; i++) {
    glcd.drawchar((i % 21) * 6, i/21, i);
  }
}
//Last line.
