// 4/10/15 Beck- Port from Arduino 0022 to 1.6.3
#include <ST7565.h>

//ST7565 LCD is 128 x 64 pixels, which is 8 lines of 21 chars.
//  I tested with a 35 character string and it showed 21 chars
//  with a space on the right side a little larger than than the left side.
//  If the character cell is 6x8 then there are 2 extra pixels (21*6= 126)

int sDelay;  //mSec between clearing screen
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

  // turn on backlight
  pinMode(BACKLIGHT_LED, OUTPUT);
  digitalWrite(BACKLIGHT_LED, HIGH);

  // initialize and set the contrast to 0x18
  glcd.begin(0x18);

  //Buffer is already loaded with splash scree.
  glcd.display(); // show splashscreen
  delay(1000);
  glcd.clear();

  sDelay= 100;
  while (true) {
  //Fill the screen with text and character count
  glcd.drawstring(0, 0, "Line #1 1012345678901");
  glcd.drawstring(0, 1, "Line #2 1012345678901");
  glcd.drawstring(0, 2, "Line #3 1012345678901");
  glcd.drawstring(0, 3, "Line #4 1012345678901");
  glcd.drawstring(0, 4, "Line #5 1012345678901");
  glcd.drawstring(0, 5, "Line #6 1012345678901");
  glcd.drawstring(0, 6, "Line #7 1012345678901");
  glcd.drawstring(0, 7, "Line #8 1012345678901");
  glcd.display();
  delay(sDelay);
  glcd.clear();
  
  glcd.drawstring(0, 0, "AXBXCXDXEXFXGxixjxkyz");
  glcd.drawstring(0, 1, "AXBXCXDXEXFXGxixjxkyz");
  glcd.drawstring(0, 2, "AXBXCXDXEXFXGxixjxkyz");
  glcd.drawstring(0, 3, "AXBXCXDXEXFXGxixjxkyz");
  glcd.drawstring(0, 4, "AXBXCXDXEXFXGxixjxkyz");
  glcd.drawstring(0, 5, "AXBXCXDXEXFXGxixjxkyz");
  glcd.drawstring(0, 6, "AXBXCXDXEXFXGxixjxkyz");
  glcd.drawstring(0, 7, "AXBXCXDXEXFXGxixjxkyz");
  glcd.display();
  delay(sDelay);
  
 }  //while
  
  return;
}    //Setup()


void loop()
{}

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
