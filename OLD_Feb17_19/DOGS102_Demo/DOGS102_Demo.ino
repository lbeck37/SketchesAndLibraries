// 5/14/15 Beck Copied in from C:\Dev\Arduino\DOGS_LCD\Arduino meets EA DOGS102
// 7/14/15 Trying to get DOGS102 to show anything
#include <Arduino.h>
#include <Streaming.h>
#include <SPI.h>
#include <dog_1701.h>
//None of the font includes are protected from multiple includes
#include <font_16x32nums.h>
#include <font_6x8.h>
#include <font_8x16.h>
#include <font_8x8.h>
#include <logo_BLH.h>

/*Available functions in dog_1701 Libraray:
  void initialize  (byte p_cs, byte p_si, byte p_clk, byte p_a0, byte p_res, byte type);
  void clear       (void);
  void contrast    (byte contr);
  void view        (byte direction);
  void string      (byte column, byte page, const byte *font_adress, const char *str);
  void rectangle   (byte start_column, byte start_page, byte end_column, byte end_page, byte pattern);
  void picture     (byte column, byte page, const byte *pic_adress);
*/

/*
extern const byte ea_logo[];
extern const byte font_6x8[];
extern const byte font_8x16[];
*/

dog_1701 DOG;

//the following port definitions are used by our demo board "EA PCBARDDOG7565"
//int led       = 3;
int led       = 6;
int led_green = 5;
int led_red   = 3;

String acSketchName  = "DOGS102_demo.ino";
String acFileDate    = "July 17, 2017 Ace D";

#if 1	//Pro Mini
const byte       cSPICmdDataPin       = 16;
const byte       cSPIChipSelectPin    =  1;
const byte       cSPI_MOSIPin       	= 13;
const byte       cSPI_CLKPin    			= 14;
const byte       cBogusResetPin      	=  4;
const byte       cHW_SPI             	=  0; 		//This is what their demo used.
#endif

#if 0	//NodeMCU
const byte       cSPICmdDataPin       =  4;			//Was 16
const byte       cSPIChipSelectPin    = 15;
const byte       cSPI_MOSIPin       	= 13;
const byte       cSPI_CLKPin    			= 14;
const byte       cBogusResetPin      	=  4;
//const byte       cBogusResetPin      	=  9;			//NodeMCU shows pin 9 as SD1, secondary data?
const byte       cHW_SPI             	=  0; 		//This is what their demo used.
#endif

//void init_backlight(boolean mono);
//void mono_backlight(byte brightness);
//void gr_backlight(byte green, byte red);


void sample_screen(void);

//initialize the backlight and the display
void setup()
{
	long	lSerialMonitorBaud= 115200;
	Serial.begin(lSerialMonitorBaud);
	Serial << endl;
  Serial << "setup(): Initialized serial to " + String(lSerialMonitorBaud) + " baud" << endl;
  Serial << "Sketch: " + acSketchName + ", " + acFileDate << endl;
  //init_backlight(true); //use RGB backlight in this sample code. Please change it to your configuration

	Serial << "setup(): Call DOG.initialize()" << endl;
  //DOG.initialize(10,0,0,9,4,DOGS102);   //SS = 10, 0,0= use Hardware SPI, 9 = A0, 4 = RESET, EA DOGS102-6 (=102x64 dots)


  DOG.initialize(cSPIChipSelectPin, cHW_SPI       , cHW_SPI,
                 cSPICmdDataPin   , cBogusResetPin, DOGS102);

/*
  DOG.initialize(cSPIChipSelectPin, cSPI_MOSIPin  , cSPI_CLKPin,
                 cSPICmdDataPin   , cBogusResetPin, DOGS102);
*/

	Serial << "setup(): Call DOG.view()" << endl;
  DOG.view(VIEW_BOTTOM);  //default viewing direction
}

//create a sample sceen content
void sample_screen(void)
{
	//Serial << "sample_screen(): Begin" << endl;

	//Serial << "sample_screen(): Call DOG.clear()" << endl;
  DOG.clear();  //clear whole display

	//Serial << "sample_screen(): Call DOG.picture()" << endl;
  DOG.picture(0,0,ea_logo);

  //DOG.string(71,0,font_8x16,"DOGS");      //font size 8x16 first page
  DOG.string(71,0,font_8x16,"Tuna");      //font size 8x16 first page
  DOG.rectangle(71,2,127,2,0x03);              //show small line (filled pattern = 0x03), to the left and right of 'DOGL128'
  //DOG.string(0,3,font_6x8,"-3.3V single sup.");
  //DOG.string(0,4,font_6x8,"-UC1701");
  //DOG.string(0,5,font_6x8,"-LED backlights");
  //DOG.string(0,6,font_6x8,"-extrem low power");
  //DOG.string(0,7,font_6x8,"lcd-module.com");
  DOG.string(0,4,font_6x8,"PowerShift");
  DOG.string(10,5,font_6x8,"by ShiftE");
  //DOG.string(0,4,font_6x8,"-DOGS102-6");
  //DOG.string(0,6,font_6x8,"PowerShift by ShiftE");
  DOG.string(0,7,font_6x8,"The Dude abides");
}	//sample_screen


//main loop
void loop()
{
  DOG.view(VIEW_BOTTOM);  //default viewing direction
  sample_screen();        //show content
#if 0
  mono_backlight(255);    //BL full brightness
  delay(1000);
  DOG.view(VIEW_TOP);    //alternate viewing direction
  sample_screen();
  delay(1000);
//#endif
  for (int sPercentBrightness= 100; sPercentBrightness >= 0; sPercentBrightness -= 25) {
   mono_backlight((sPercentBrightness * 255) / 100);    //BL full brightness
   delay(1000);
  }   //for
  delay(1000);
#endif
}	//loop


//The following functions controll the backlight with a PWM. Not needed for the display content
/*
void init_backlight(boolean mono)
{
  if(mono) //EA LED39X41-W, EA LED39X41-A
  {
    pinMode(led,  OUTPUT);
    mono_backlight(255);
  }
  else  //EA LED39X41-GR
  {
    pinMode(led_green, OUTPUT);
    pinMode(led_red,   OUTPUT);
    rgb_backlight(255,0);
  }
}
//Use this funtion for monochrome backlight
void mono_backlight(byte brightness)
{
  analogWrite(led, brightness);
}
//Use this function for RGB backlight
void rgb_backlight(byte green, byte red)
{
  analogWrite(led_red,  red);
  analogWrite(led_green, green);
}
*/
