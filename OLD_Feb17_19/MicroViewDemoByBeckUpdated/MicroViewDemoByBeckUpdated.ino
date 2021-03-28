// 4/25/15 Merge fixes back in from MicroViewFontDemo
// 4/24/15 Copied from MicroView-Arduino-Library-master\examples\MicroViewDemo
//         Rearranged and dramatically cleared up code, mostly for showing text.
//         The first pass on loop() is good, second pass is missing last demos
//         and then it hangs on clock.

#include <MicroView.h>
#include <Time.h>       //Get this from http://www.pjrc.com/teensy/td_libs_Time.html
#include <Streaming.h>  //Get this from http://arduiniana.org/libraries/streaming/
// Streaming.h allows me to send semi-formated output to the Serial Manager like this:
//    Serial << "Value= " << sValue << ", Speed= " << sMph << " MPH" << endl;
// Then when I've got the Arduino hooked up to a computer I can open the Serial Manager
// and see the logging output come out on the screen.

#define clocksize 24

//(sic)On delay in milliseconds, if there is no on delay, the erase will be too fast to clean up the screen.
uint16_t    onDelay=5;

//Create some strings for building strings.
static char        szValue[10];
static char        szLineBuffer[25];
boolean     bClearPage;       //Flag to use calling sDisplayText()
boolean     bDoDisplay;       //Flag to use calling sDisplayText()

void setup() {
   Serial.begin(9600);
   Serial << "Begin setup()" << endl;
   sDisplaySplashScreen();
   sInitializeClock();
   return;
}  //setup


void loop() {
   sDoClockDemo(3);

   sDoSpriteDemo();

   //sRunMysteryCode();

   sDoRandomLines();

   sDoRectangleDemo();

   sDoCircleDemo();

   sDoFontDemo();

   sDoCounterDemo();

   //sDoSimpleTutorial();
   return;
}  //loop


int sDisplaySplashScreen() {
   uView.begin();    // begin of MicroView
   uView.clear(ALL); // erase hardware memory inside the OLED controller
   uView.display();  // display the content in the buffer memory, by default it is the MicroView logo
   delay(2000);
   uView.clear(PAGE);   // erase the memory buffer, when next uView.display() is called, the OLED will be cleared.
   return 1;
}  //sDisplaySplashScreen


int sInitializeClock() {
   setTime(6, 00, 01, 17, 1, 2014);
   return 1;
}  //sInitializeClock


int sDisplayClockFace() {
   uView.setFontType(0);      // set font type 0, please see declaration in MicroView.cpp
   uView.setCursor(27,0);     // points cursor to x=27 y=0
   uView.print(12);
   uView.setCursor(30,uView.getLCDHeight()-uView.getFontHeight());
   uView.print(6);
   uView.setCursor(0,uView.getLCDHeight() /2-(uView.getFontHeight()/2));
   uView.print(9);
   uView.setCursor(uView.getLCDWidth()-uView.getFontWidth(),uView.getLCDHeight()/2-(uView.getFontHeight()/2));
   uView.print(3);
   uView.display();        // display the memory buffer drawn
   return 1;
}  //sDisplayClockFace


int sDoClockDemo(int sClockOnSeconds) {
   static boolean       drawnFirst=false;
   unsigned long        ulSecond;
   unsigned long        ulEndMsec;
   float                degresshour,degressmin,degresssec;
   float                hourx,houry;
   float                minx,miny;
   float                secx,secy;

   sDisplayClockFace();

   //The original demo's clock on-time logic baffled me.LBeck 4/24/15
   ulEndMsec= millis() + (sClockOnSeconds * 1000);

   Serial << "sDoClockDemo(): End Seconds= " << ulEndMsec / 1000 << endl;

   ulSecond= millis() + 1000;
   while (millis() < ulEndMsec) {
      if (ulSecond!=(unsigned long)second()) {
         if (drawnFirst) {
            uView.line(32,24,32+hourx,24+houry,WHITE,XOR);
            uView.line(32,24,32+minx,24+miny,WHITE,XOR);
            uView.line(32,24,32+secx,24+secy,WHITE,XOR);
         }

         degresshour = (((hour() * 360) / 12) + 270) * (PI / 180);
         degressmin = (((minute() * 360) / 60) + 270) * (PI / 180);
         degresssec = (((second() * 360) / 60) + 270) * (PI / 180);

         hourx = cos(degresshour) * (clocksize / 2.5);
         houry = sin(degresshour) * (clocksize / 2.5);

         minx = cos(degressmin) * (clocksize / 1.4);
         miny = sin(degressmin) * (clocksize / 1.4);

         secx = cos(degresssec) * (clocksize / 1.1);
         secy = sin(degresssec) * (clocksize / 1.1);


         uView.line(32,24,32+hourx,24+houry,WHITE,XOR);
         uView.line(32,24,32+minx,24+miny,WHITE,XOR);
         uView.line(32,24,32+secx,24+secy,WHITE,XOR);
         drawnFirst=true;
         uView.display();

         ulSecond=second();
      }  //if
   }  //while
   drawnFirst=false;
   uView.clear(PAGE);
   return 1;
}  //sDoClockDemo


int sDoSpriteDemo() {
   int maxX=40;
   onDelay=30;
   uView.setFontType(0);
   uView.setCursor(0,40);
   uView.print("  SPRITE  ");
   for (int x=0; x<maxX;x+=2) {
      uView.setFontType(4);
      uView.drawChar(x,0,48,WHITE, XOR);
      uView.setFontType(5);
      uView.drawChar(maxX-x,10,48,WHITE,XOR);

      uView.setFontType(6);
      uView.drawChar(x,32,48,WHITE,XOR);

      uView.display();
      delay(onDelay);
      uView.setFontType(4);
      uView.drawChar(x,0,48,WHITE, XOR);
      uView.setFontType(5);
      uView.drawChar(maxX-x,10,48,WHITE,XOR);
      uView.setFontType(6);
      uView.drawChar(x,32,48,WHITE,XOR);

      uView.display();
      uView.setFontType(4);
      uView.drawChar(x,0,49,WHITE, XOR);
      uView.setFontType(5);
      uView.drawChar(maxX-x,10,49,WHITE,XOR);
      uView.setFontType(6);
      uView.drawChar(x,32,49,WHITE,XOR);

      uView.display();
      delay(onDelay);
      uView.setFontType(4);
      uView.drawChar(x,0,49,WHITE, XOR);
      uView.setFontType(5);
      uView.drawChar(maxX-x,10,49,WHITE,XOR);
      uView.setFontType(6);
      uView.drawChar(x,32,49,WHITE,XOR);
      uView.display();
   }  //for
   return 1;
}  //sDoSpriteDemo


int sDisplayText(boolean bClearPage, boolean bDoDisplay, int sFontType, int sCursorX, int sCursorY, char *pcText) {
   if (bClearPage) {
      uView.clear(PAGE);
   }  //if(bClearPage)

   uView.setFontType(sFontType);
   uView.setCursor(sCursorX, sCursorY);
   uView.print(pcText);
   if (bDoDisplay) {
      uView.display();
   }  //if (bDoDisplay)

   return 1;
}  //sDisplayText


int sDoFontDemo(void) {
   int         sFontType;
   int         sCursorX;
   int         sCursorY;
   int         sLineHeight[]  = {10, 12, 18, 24};  //dots per line for each font
   int         sNumLines[]    = {4, 3, 2, 1};      //dots per line for each font
   int         sLineNumber;                        //Top line is zero
   char        *szLineTail[]= {" Abcd6789", " Abcd6", " 345", "  45"};

   for (int sFont= 0; sFont < 4; sFont++) {
      //Display label at bottom in small font
      sLineNumber = 4;
      sFontType   = 0;
      sCursorX    = 0;
      sCursorY    = sLineNumber * sLineHeight[sFontType];
      bClearPage  = true;
      bDoDisplay  = true;

      itoa(sFont, szValue, 10);
      strcpy(szLineBuffer, "   F");
      strcat(szLineBuffer, szValue);

      sDisplayText(bClearPage, bDoDisplay, sFontType, sCursorX, sCursorY, szLineBuffer);

      sFontType   = sFont;
      sCursorX    = 0;
      bClearPage  = false;
      for (int sLine= 0; sLine < sNumLines[sFont]; sLine++) {
         sCursorY= sLine * sLineHeight[sFont];
         itoa(sLine + 1, szValue, 10);    //Display line number as first character.
         strcpy(szLineBuffer, szValue);
         strcat(szLineBuffer, szLineTail[sFont]);

         //sDisplayText(bClearPage, sFontType, sCursorX, sCursorY, szLineBuffer);
         sDisplayText(bClearPage, bDoDisplay, sFontType, sCursorX, sCursorY, szLineBuffer);
      }  //for(int sLine=...

      //delay(1500);
      delay(2000);
   }  //for(int sFont=...

   uView.clear(PAGE);
   return 1;
}  //sDoFontDemo


int sDoRectangleDemo() {
   static uint8_t x0,y0,x1,y1;
   uView.setCursor(0,40);
   uView.print("RECTANGLE ");
   uView.display();
   delay(500);

   x0=0;y0=0;x1=0;y1=0;
   for (int i=1; i<64;i++) {
      y1=i;
      if (y1>47) y1=47;
      // draw rectangle from x0,y0 with width of i and height of y1 using WHITE color and XOR draw mode
      uView.rect(x0,y0,i,y1,WHITE,XOR);
      uView.display();
      delay(onDelay);
      uView.rect(x0,y0,i,y1,WHITE,XOR);
      uView.display();
   }  //for
   return 1;
}  //sDoRectangleDemo


int sDoCircleDemo() {
   static uint8_t x0,y0,x1,y1;
   uView.setCursor(0,40);
   uView.print("  CIRCLE  ");
   uView.display();
   delay(500);

   x0=32;y0=24;
   for (int i=0;i<32;i++) {
      // draw circle at x0,y0 with radius of i using WHITE color and XOR draw mode
      uView.circle(x0,y0,i,WHITE,XOR);
      uView.display();
      delay(onDelay);
      uView.circle(x0,y0,i,WHITE,XOR);
      uView.display();
      delay(onDelay);
   }  //for
   return 1;
}  //sDoCircleDemo


int sDoRandomLines() {
   static uint8_t x0,y0,x1,y1;
   onDelay=5;
   uView.setFontType(0);
   uView.setCursor(0,40);
   uView.print("   LINE   ");
   uView.display();
   delay(500);

   for (int i=0; i<150;i++) {
      x0=random(64);
      x1=random(64);
      y0=random(48);
      y1=random(48);

      // draw line from x0,y0 to x1,y1 using WHITE color and XOR draw mode
      uView.line(x0,y0,x1,y1, WHITE, XOR);
      uView.display();
      delay(onDelay);
      uView.line(x0,y0,x1,y1, WHITE,XOR);
      uView.display();
   }  //for
   return 1;
}  //sDoRandomLines


int sDoCounterDemo() {
   long        wIntCounter;
   double      dFloatCounter;
   int         sFontType;
   int         sCursorX;
   int         sCursorY;

   sFontType   = 3;
   sCursorX    = 0;
   sCursorY    = 0;
   bClearPage  = true;
   bDoDisplay  = true;
   wIntCounter= 99999;
   while (wIntCounter-- > 99800) {
      ltoa(wIntCounter, szValue, 10);
      strcpy(szLineBuffer, szValue);
      sDisplayText(bClearPage, bDoDisplay, sFontType, sCursorX, sCursorY, szLineBuffer);
   }  //while


   // dtostrf(floatVal, 4, 3, charVal);
   //4 is mininum width, 3 is precision; float value is copied onto buff
   float    fFloatStart  = 19.99;
   float    fFloatEnd    = 23.00;
   float    fFloatCounter= fFloatStart;
   float    fFloatDisplay;
   char     szDisplayString[25];
   char     szConvertedString[25];
   do {
      dtostrf(fFloatDisplay, 4, 3, szConvertedString);
      sFontType   = 2;
      sCursorX    = 0;
      sCursorY    = 0;
      bClearPage  = true;
      bDoDisplay  = false;
      sDisplayText(bClearPage, bDoDisplay, sFontType, sCursorX, sCursorY, szConvertedString);

      dtostrf(fFloatCounter - fFloatStart, 4, 3, szConvertedString);
      sFontType   = 2;
      sCursorX    = 0;
      sCursorY    = uView.getFontHeight();
      bClearPage  = false;
      bDoDisplay  = false;
      sDisplayText(bClearPage, bDoDisplay, sFontType, sCursorX, sCursorY, szConvertedString);

      //4/24/15 Why is this not adding a character to beginning?
      fFloatDisplay= fFloatStart - fFloatCounter;
      if (fFloatDisplay < 0.00) {
         strcpy(szDisplayString, "-");
      }
      else {
         strcpy(szDisplayString, "0");
      }
      dtostrf(fFloatDisplay, 4, 3, szConvertedString);
      strcat(szDisplayString, szConvertedString);
      sFontType   = 2;
      sCursorX    = 0;
      sCursorY    = uView.getFontHeight() * 2;
      bClearPage  = false;
      bDoDisplay  = true;
      sDisplayText(bClearPage, bDoDisplay, sFontType, sCursorX, sCursorY, szDisplayString);

      fFloatCounter += 0.01;
  } while(fFloatCounter < fFloatEnd);

   uView.clear(PAGE);
   return 1;
}  //sDoCounterDemo


// sRunMysteryCode() is from the original demo's loop() routine
// I called it this because I don't know what it does and it doesn't appear
// to do anything to the screen.
int sRunMysteryCode() {
   int maxX=40;
   for (int x=maxX; x>0;x-=2) {
      uView.setFontType(4);
      uView.drawChar(x,10,48,WHITE, XOR);
      uView.setFontType(5);
      uView.drawChar(40-x,0,48,WHITE,XOR);
      uView.setFontType(6);
      uView.drawChar(x,32,48,WHITE,XOR);

      uView.display();
      delay(onDelay);
      uView.setFontType(4);
      uView.drawChar(x,10,48,WHITE, XOR);
      uView.setFontType(5);
      uView.drawChar(40-x,0,48,WHITE,XOR);
      uView.setFontType(6);
      uView.drawChar(x,32,48,WHITE,XOR);

      uView.display();
      uView.setFontType(4);
      uView.drawChar(x,10,49,WHITE, XOR);
      uView.setFontType(5);
      uView.drawChar(40-x,0,49,WHITE,XOR);
      uView.setFontType(6);
      uView.drawChar(x,32,49,WHITE,XOR);

      uView.display();
      delay(onDelay);
      uView.setFontType(4);
      uView.drawChar(x,10,49,WHITE, XOR);
      uView.setFontType(5);
      uView.drawChar(40-x,0,49,WHITE,XOR);
      uView.setFontType(6);
      uView.drawChar(x,32,49,WHITE,XOR);

      uView.display();
   }  //for
   return 1;
}  //sRunMysteryCode
//Last line.