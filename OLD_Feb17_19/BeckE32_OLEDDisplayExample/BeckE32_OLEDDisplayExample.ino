// Beck 2/13/17, from Adafruit example ssd1306_128x64_i2c.ino
#define SKETCH_HEAD "\n****\nBeckE32_OLEDDisplayExample.ino, Nov 29, 2018a"
/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers
This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)
Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//#define OLED_RESET 4
//Adafruit_SSD1306 oDisplay(OLED_RESET);

Adafruit_SSD1306 		oDisplay(-1);		//Looks like -1 is default

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {
  Serial.begin(115200);
	Serial.println(SKETCH_HEAD);
	Serial.println("Setup(): Call Wire.begin(21, 22)");
  //Wire.begin(21, 22);		//ESP32
  Wire.begin(0,2);		//NodeMCU ESP8266
  ScanForI2CDevices();

  Serial.println("Setup(): Call display.begin()");

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  //oDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
  oDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  //ScanForDevices();
  // init done

  // Clear the buffer.
  oDisplay.clearDisplay();

  /*
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  oDisplay.display();
  delay(2000);

  // draw a single pixel
  oDisplay.drawPixel(10, 10, WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  oDisplay.display();
  delay(2000);
  oDisplay.clearDisplay();

  // draw many lines
  testdrawline();
  oDisplay.display();
  delay(2000);
  oDisplay.clearDisplay();

  // draw rectangles
  testdrawrect();
  oDisplay.display();
  delay(2000);
  oDisplay.clearDisplay();

  // draw multiple rectangles
  testfillrect();
  oDisplay.display();
  delay(2000);
  oDisplay.clearDisplay();

  // draw mulitple circles
  testdrawcircle();
  oDisplay.display();
  delay(2000);
  oDisplay.clearDisplay();

  // draw a white circle, 10 pixel radius
  oDisplay.fillCircle(oDisplay.width()/2, oDisplay.height()/2, 10, WHITE);
  oDisplay.display();
  delay(2000);
  oDisplay.clearDisplay();

  testdrawroundrect();
  delay(2000);
  oDisplay.clearDisplay();

  testfillroundrect();
  delay(2000);
  oDisplay.clearDisplay();

  testdrawtriangle();
  delay(2000);
  oDisplay.clearDisplay();

  testfilltriangle();
  delay(2000);
  oDisplay.clearDisplay();
*/
  while (true) {
/*
		// draw the first ~12 characters in the font
		testdrawchar();
		oDisplay.display();
		delay(2000);
		oDisplay.clearDisplay();
*/

  	Dashboard();

		// draw scrolling text
		testscrolltext();
		delay(2000);
		oDisplay.clearDisplay();

/*
		// text display tests
		oDisplay.setTextSize(1);
		oDisplay.setTextColor(WHITE);
		oDisplay.setCursor(0,0);
		oDisplay.println("Hello, world!");
		oDisplay.setTextColor(BLACK, WHITE); // 'inverted' text
		oDisplay.println(3.141592);
		oDisplay.setTextSize(2);
		oDisplay.setTextColor(WHITE);
		oDisplay.print("0x"); oDisplay.println(0xDEADBEEF, HEX);
		oDisplay.display();
		delay(2000);
		oDisplay.clearDisplay();
*/
  }	//while

  // miniature bitmap display
  oDisplay.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  oDisplay.display();
  delay(1000);

  // invert the display
  oDisplay.invertDisplay(true);
  delay(1000);
  oDisplay.invertDisplay(false);
  delay(1000);
  oDisplay.clearDisplay();

  // draw a bitmap icon and 'animate' movement
  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
}	//setup


void loop() {

}	//loop


void Dashboard(void) {
	oDisplay.setTextSize(2);
	oDisplay.setTextColor(WHITE);
	oDisplay.setCursor(0,0);
	oDisplay.println("+3.101");
	oDisplay.println("+3.202");
	oDisplay.println("+3.303");
	oDisplay.println("+3.404");
	oDisplay.display();
	delay(5000);
	oDisplay.clearDisplay();
}	//Dashboard


void testscrolltext(void) {
  oDisplay.setTextSize(2);
  oDisplay.setTextColor(WHITE);
  oDisplay.setCursor(10,0);
  oDisplay.clearDisplay();
  oDisplay.println("I Love U");
  oDisplay.display();
  delay(1);

  oDisplay.startscrollright(0x00, 0x0F);
  delay(2000);
  oDisplay.stopscroll();
  delay(1000);
  oDisplay.startscrollleft(0x00, 0x0F);
  delay(2000);
  oDisplay.stopscroll();
  delay(1000);
  oDisplay.startscrolldiagright(0x00, 0x07);
  delay(2000);
  oDisplay.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  oDisplay.stopscroll();
}	//testscrolltext


void ScanForI2CDevices(void){
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
}	//ScanForDevices


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];

  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(oDisplay.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;

    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      oDisplay.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
    }
    oDisplay.display();
    delay(200);

    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      oDisplay.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > oDisplay.height()) {
        icons[f][XPOS] = random(oDisplay.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
   }
}


void testdrawchar(void) {
  oDisplay.setTextSize(1);
  oDisplay.setTextColor(WHITE);
  oDisplay.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    oDisplay.write(i);
    if ((i > 0) && (i % 21 == 0))
      oDisplay.println();
  }
  oDisplay.display();
  delay(1);
}

void testdrawcircle(void) {
  for (int16_t i=0; i<oDisplay.height(); i+=2) {
    oDisplay.drawCircle(oDisplay.width()/2, oDisplay.height()/2, i, WHITE);
    oDisplay.display();
    delay(1);
  }
}

void testfillrect(void) {
  uint8_t color = 1;
  for (int16_t i=0; i<oDisplay.height()/2; i+=3) {
    // alternate colors
    oDisplay.fillRect(i, i, oDisplay.width()-i*2, oDisplay.height()-i*2, color%2);
    oDisplay.display();
    delay(1);
    color++;
  }
}

void testdrawtriangle(void) {
  for (int16_t i=0; i<min(oDisplay.width(),oDisplay.height())/2; i+=5) {
    oDisplay.drawTriangle(oDisplay.width()/2, oDisplay.height()/2-i,
                     oDisplay.width()/2-i, oDisplay.height()/2+i,
                     oDisplay.width()/2+i, oDisplay.height()/2+i, WHITE);
    oDisplay.display();
    delay(1);
  }
}

void testfilltriangle(void) {
  uint8_t color = WHITE;
  for (int16_t i=min(oDisplay.width(),oDisplay.height())/2; i>0; i-=5) {
    oDisplay.fillTriangle(oDisplay.width()/2, oDisplay.height()/2-i,
                     oDisplay.width()/2-i, oDisplay.height()/2+i,
                     oDisplay.width()/2+i, oDisplay.height()/2+i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    oDisplay.display();
    delay(1);
  }
}

void testdrawroundrect(void) {
  for (int16_t i=0; i<oDisplay.height()/2-2; i+=2) {
    oDisplay.drawRoundRect(i, i, oDisplay.width()-2*i, oDisplay.height()-2*i, oDisplay.height()/4, WHITE);
    oDisplay.display();
    delay(1);
  }
}

void testfillroundrect(void) {
  uint8_t color = WHITE;
  for (int16_t i=0; i<oDisplay.height()/2-2; i+=2) {
    oDisplay.fillRoundRect(i, i, oDisplay.width()-2*i, oDisplay.height()-2*i, oDisplay.height()/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    oDisplay.display();
    delay(1);
  }
}

void testdrawrect(void) {
  for (int16_t i=0; i<oDisplay.height()/2; i+=2) {
    oDisplay.drawRect(i, i, oDisplay.width()-2*i, oDisplay.height()-2*i, WHITE);
    oDisplay.display();
    delay(1);
  }
}

void testdrawline() {
  for (int16_t i=0; i<oDisplay.width(); i+=4) {
    oDisplay.drawLine(0, 0, i, oDisplay.height()-1, WHITE);
    oDisplay.display();
    delay(1);
  }
  for (int16_t i=0; i<oDisplay.height(); i+=4) {
    oDisplay.drawLine(0, 0, oDisplay.width()-1, i, WHITE);
    oDisplay.display();
    delay(1);
  }
  delay(250);

  oDisplay.clearDisplay();
  for (int16_t i=0; i<oDisplay.width(); i+=4) {
    oDisplay.drawLine(0, oDisplay.height()-1, i, 0, WHITE);
    oDisplay.display();
    delay(1);
  }
  for (int16_t i=oDisplay.height()-1; i>=0; i-=4) {
    oDisplay.drawLine(0, oDisplay.height()-1, oDisplay.width()-1, i, WHITE);
    oDisplay.display();
    delay(1);
  }
  delay(250);

  oDisplay.clearDisplay();
  for (int16_t i=oDisplay.width()-1; i>=0; i-=4) {
    oDisplay.drawLine(oDisplay.width()-1, oDisplay.height()-1, i, 0, WHITE);
    oDisplay.display();
    delay(1);
  }
  for (int16_t i=oDisplay.height()-1; i>=0; i-=4) {
    oDisplay.drawLine(oDisplay.width()-1, oDisplay.height()-1, 0, i, WHITE);
    oDisplay.display();
    delay(1);
  }
  delay(250);

  oDisplay.clearDisplay();
  for (int16_t i=0; i<oDisplay.height(); i+=4) {
    oDisplay.drawLine(oDisplay.width()-1, 0, 0, i, WHITE);
    oDisplay.display();
    delay(1);
  }
  for (int16_t i=0; i<oDisplay.width(); i+=4) {
    oDisplay.drawLine(oDisplay.width()-1, 0, i, oDisplay.height()-1, WHITE);
    oDisplay.display();
    delay(1);
  }
  delay(250);
}
