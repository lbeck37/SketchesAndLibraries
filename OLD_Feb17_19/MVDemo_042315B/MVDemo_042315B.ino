// 4/24/15 Copied from MicroView-Arduino-Library-master\examples\MicroViewDemo
/*
   MicroView Arduino Library
   Copyright (C) 2014 GeekAmmo

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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

void setup() {
   sDisplaySplashScreen();
   sInitializeClock();
   return;
}  //setup


void loop() {
   sDoClockDemo(3);

   sDoSpriteDemo();

   //sRunMysteryCode();
#if 0
   sDoRandomLines();

   sDoRectangleDemo();

   sDoCircleDemo();
#endif
   sDoFontDemo();

   sDoCounterDemo();

   //sDoSimpleTutorial();
   return;
}  //loop


int sDisplaySplashScreen() {
   uView.begin();    // begin of MicroView
   uView.clear(ALL); // erase hardware memory inside the OLED controller
   uView.display();  // display the content in the buffer memory, by default it is the MicroView logo
   //delay(700);
   delay(2000);
   uView.clear(PAGE);   // erase the memory buffer, when next uView.display() is called, the OLED will be cleared.
   return 1;
}  //sDisplaySplashScreen


int sInitializeClock() {
   //setTime(8, 51, 01, 17, 1, 2014);
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
   static boolean          drawnFirst=false;
   static unsigned long    mSec=millis()+1000;

   static float         degresshour,degressmin,degresssec;
   static float         hourx,houry;
   static float         minx,miny;
   static float         secx,secy;
   static int           sEndMsec;

   sDisplayClockFace();

   //The original demo's clock on-time logic baffled me.LBeck 4/24/15
   sEndMsec= millis() + (sClockOnSeconds * 1000);

   while (millis() < sEndMsec) {
      if (mSec!=(unsigned long)second()) {
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

         mSec=second();
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


int sDisplayText(boolean bClearPage, int sFontType, int sCursorX, int sCursorY, char *pcText) {
   if (bClearPage) {
      uView.clear(PAGE);
   }  //if(bClearPage)

   uView.setFontType(sFontType);
   uView.setCursor(sCursorX, sCursorY);
   uView.print(pcText);
   uView.display();
   return 1;
}  //sDisplayText


int sDoFontDemo(void) {
   int         sFontType;
   int         sCursorX;
   int         sCursorY;
   int         sLineHeight[]  = {10, 12, 18, 24};  //dots per line for each font
   int         sNumLines[]    = {4, 3, 2, 1};      //dots per line for each font
   int         sLineNumber;                        //Top line is zero
   boolean     bClearPage;
   char        szValue[10];
   char        szLineBuffer[25];
   char        *szLineTail[]= {" Abcd6789", " Abcd6", " 345", " 34"};

   for (int sFont= 0; sFont < 4; sFont++) {
      //Display label at bottom in small font
      sLineNumber = 4;
      sFontType   = 0;
      sCursorX    = 0;
      sCursorY    = sLineNumber * sLineHeight[sFontType];
      bClearPage  = true;

      itoa(sFont, szValue, 10);
      strcpy(szLineBuffer, "  Font ");
      strcat(szLineBuffer, szValue);

      sDisplayText(bClearPage, sFontType, sCursorX, sCursorY, szLineBuffer);

      sFontType   = sFont;
      sCursorX    = 0;
      bClearPage  = false;
      for (int sLine= 0; sLine < sNumLines[sFont]; sLine++) {
         sCursorY= sLine * sLineHeight[sFont];
         itoa(sLine + 1, szValue, 10);    //Display line number as first character.
         strcpy(szLineBuffer, szValue);
         strcat(szLineBuffer, szLineTail[sFont]);

         sDisplayText(bClearPage, sFontType, sCursorX, sCursorY, szLineBuffer);
      }  //for(int sLine=...

      //delay(1500);
      delay(3000);
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
   static unsigned long    uwNextUpdateMsec= millis() + 1000;
   static double           dCounter=99999;

   while (dCounter>99970) {

      if (millis() >= uwNextUpdateMsec) {

         uView.setFontType(3);
         uView.setCursor(0,0);
         uView.print(dCounter);

         dCounter--;
         uView.display();
         uwNextUpdateMsec= millis() + 100;
      }
   }
   uView.clear(PAGE);

   dCounter=19.99;
   while (dCounter<20.2) {

      if (millis() >= uwNextUpdateMsec) {

         uView.setFontType(2);
         uView.setCursor(0,0);
         uView.print(dCounter + ((int)(dCounter*100)%20));

         uView.setCursor(0,uView.getFontHeight());
         uView.print(50-dCounter);

         uView.setCursor(0,(uView.getFontHeight()*2));
         uView.print(75+dCounter+0.02);

         dCounter+=0.01;
         uView.display();
         uwNextUpdateMsec= millis() + 100;
      }
   }
   uView.clear(PAGE);
   return 1;
}  //sDoCounterDemo


void displayConnect(char * value, char * text) {
   int y=0;
   uView.clear(PAGE);
   uView.setCursor(0,y);
   uView.print("Connect a");
   if (strlen(value)>0) {
      uView.setCursor(0,y+10);
      uView.print(value);
      uView.setCursor(0,y+20);
   }
   else
   uView.setCursor(0,y+10);
   uView.print(text);
   uView.display();
}  //displayConnect

void displayRemove(char * text) {
   int y=0;
   uView.clear(PAGE);
   uView.setCursor(0,y);
   uView.print("Well done!");
   uView.setCursor(0,y+9);
   uView.print("Remove the");
   uView.setCursor(0,y+18);
   uView.print(text);
   uView.setCursor(0,y+27);
   uView.print("to proceed");
   uView.display();
}

int getXpos(int pin) {
   byte xpos[] = {0,0,8,17,26,34,42,51,59,59,51,42,34,26,17,8,0};
   return xpos[pin];
}

int getYpos(int pin) {
   if ((pin >0) && (pin <9))  {
      return 41;
   }
   else {
      return 6;
   }
}

void drawPin(int pin) {
   int x,y, dir;
   int color=WHITE;

   if ((pin >0) && (pin <9))  {
      dir=0;
   }
   else {
      dir=1;
   }

   x=getXpos(pin);
   y=getYpos(pin);

   if (dir==0) {
      uView.drawChar(x,y-10,pin+48,color,XOR);

      uView.line(x,y,x,y+4,color,XOR);
      x++;
      uView.line(x,y,x,y+5,color,XOR);
      x++;
      uView.line(x,y,x,y+6,color,XOR);
      x++;
      uView.line(x,y,x,y+5,color,XOR);
      x++;
      uView.line(x,y,x,y+4,color,XOR);
   }
   else {
      if (pin<10)
      uView.drawChar(x,y+3,pin+48,color,XOR);
      else {
         char str[5];
         sprintf(str, "%d", pin);
         uView.drawChar(x,y+3,str[0],color,XOR);
         uView.drawChar(x,y+11, str[1],color,XOR);
      }

      uView.line(x,y,x,y-4,color,XOR);
      x++;
      uView.line(x,y,x,y-5,color,XOR);
      x++;
      uView.line(x,y,x,y-6,color,XOR);
      x++;
      uView.line(x,y,x,y-5,color,XOR);
      x++;
      uView.line(x,y,x,y-4,color,XOR);

   }
   uView.display();
}

void fromPinToPin(int pin1, int pin2) {
   int x1,y1,x2,y2;
   x1=getXpos(pin1);
   y1=getYpos(pin1);
   x2=getXpos(pin2);
   uView.line(x1+6,y1-6, x2-1, y1-6);
   uView.display();
   drawPin(pin1);
   drawPin(pin2);
}

void doJumper(int pin1, int pin2) {
   int result;
   int sense;

   switch(pin1) {
   case 7: sense=A0; break;
   case 6: sense=A1; break;
   case 5: sense=A2; break;
   case 4: sense=A3; break;
   case 3: sense=A4; break;
   case 2: sense=A5; break;
   default: return;
   }
   displayConnect("","jumper");

   pinMode(sense,INPUT);
   digitalWrite(sense,HIGH);

   result=digitalRead(sense);
   while(result==1) {
      fromPinToPin(pin1,pin2);  // on
      delay(300);
      fromPinToPin(pin1,pin2);  // off
      delay(300);
      fromPinToPin(pin1,pin2);  // on
      delay(300);
      result=digitalRead(sense);

   }

   uView.clear(PAGE);

   displayRemove("jumper");

   result=digitalRead(sense);
   while(result==0) {
      result=digitalRead(sense);
   }
   uView.clear(PAGE);
   uView.display();
}

void doResistor(int pin1, int pin2, int value, int LED) {
   int result,result2;
   int sense;
   int analogValue;
   int ledout;
   char resvalue[20];

   if (value==330)
   analogValue=25;
   else
   analogValue=230;

   switch(pin1) {
   case 7: sense=A0; break;
   case 6: sense=A1; break;
   case 5: sense=A2; break;
   case 4: sense=A3; break;
   case 3: sense=A4; break;
   case 2: sense=A5; break;
   default: return;
   }

   sprintf(resvalue,"%d",value);
   strcat(resvalue," ohm");
   displayConnect(resvalue,"resistor");

   pinMode(sense,INPUT);
   digitalWrite(sense,HIGH);

   while(1) {
      fromPinToPin(pin1,pin2);  // on
      delay(300);
      fromPinToPin(pin1,pin2);  // off
      delay(300);
      fromPinToPin(pin1,pin2);  // on
      delay(300);
      result=analogRead(sense);
      Serial.println(result);
      if ((result>(analogValue-20)) && (result<(analogValue+20))) {
         break;
      }
   }

   uView.clear(PAGE);

   if (LED==0) {
      displayRemove("resistor");
      result=analogRead(sense);
      while(1) {
         result=analogRead(sense);
         if(result>(analogValue+20))
         break;
      }
   }
   else {
      uView.clear(PAGE);

      int outpin=pin1-1;
      switch(outpin) {
      case 7: ledout=A0; break;
      case 6: ledout=A1; break;
      case 5: ledout=A2; break;
      case 4: ledout=A3; break;
      case 3: ledout=A4; break;
      case 2: ledout=A5; break;
      default: return;
      }
      pinMode(ledout,OUTPUT);
      displayConnect("","LED");
      fromPinToPin(pin1,pin2);
      drawPin(pin1);
      uView.display();
      while(1) {
         digitalWrite(ledout,HIGH);
         fromPinToPin(pin1-1,pin1);  // on
         delay(300);
         digitalWrite(ledout,LOW);
         fromPinToPin(pin1-1,pin1);  // off
         delay(300);
         result=analogRead(sense);
         digitalWrite(ledout,HIGH);
         fromPinToPin(pin1-1,pin1);  // on
         delay(300);
         result2=analogRead(sense);
         Serial.print(result);
         Serial.print(",");
         Serial.println(result2);
         if ((result2-result)>400) {
            Serial.println("LED is blinking...");
            break;
         }
      }
      uView.clear(PAGE);
      uView.setCursor(0,0);
      uView.print("Well done!");
      uView.setCursor(0,10);
      uView.print("The LED is");
      uView.setCursor(0,20);
      uView.print("BLINKING!");

      uView.setCursor(0,40);
      uView.print("Thanks.");
      uView.display();

      long longdly=millis();

      while(1) {
         digitalWrite(ledout,HIGH);
         delay(300);
         digitalWrite(ledout,LOW);
         delay(300);
         if ((millis()-longdly)>6000)
         break;
      }
   }
}

void displayTry() {
   int y=0;
   uView.clear(PAGE);
   uView.setCursor(0,y);
   uView.print("Now you");
   uView.setCursor(0,y+10);
   uView.print("have seen");
   uView.setCursor(0,y+20);
   uView.print("our demo.");
   uView.display();
   delay(onDelay);
   uView.clear(PAGE);
   uView.setCursor(0,y);
   uView.print("Let's try");
   uView.setCursor(0,y+10);
   uView.print("our");
   uView.setCursor(0,y+20);
   uView.print("built-in");
   uView.setCursor(0,y+30);
   uView.print("tutorials.");
   uView.display();
   delay(onDelay);
   uView.clear(PAGE);
   uView.setCursor(0,y);
   uView.print("When you");
   uView.setCursor(0,y+10);
   uView.print("see PINs");
   uView.setCursor(0,y+20);
   uView.print("blinking,");
   uView.setCursor(0,y+30);
   uView.print("connect");
   uView.setCursor(0,y+40);
   uView.print("the PINs");
   uView.display();
   delay(onDelay);
   uView.clear(PAGE);
   uView.setCursor(0,y);
   uView.print("to the");
   uView.setCursor(0,y+10);
   uView.print("component");
   uView.setCursor(0,y+20);
   uView.print("mentioned.");
   uView.display();
   delay(onDelay);
}  //displayTry

void displayEnd() {
   int y=0;
   uView.clear(PAGE);
   uView.setCursor(0,y);
   uView.print("Please");
   uView.setCursor(0,y+10);
   uView.print("proceed to");
   uView.setCursor(0,y+20);
   uView.print("MicroView");
   uView.setCursor(0,y+30);
   uView.print("website.");
   uView.display();
   delay(onDelay);
}  //displayEnd


//The Tutorial should be part of a different Sketch as it interfers
//with this Sketch's purpose of showing how to use the MicroView display.
int sDoSimpleTutorial() {
   // Simple Tutorial
   uView.setFontType(0);
   onDelay=3500;        // set 3.5 second between each message
   displayTry();        // show please try our tutorial
   doJumper(5,8);       // jumper tutorial
   doJumper(3,8);       // jumper
   doJumper(2,8);
   doResistor(4,8,330,0);
   doResistor(4,8,10000,0);
   doResistor(5,8,330,1);    // Do resistor with LED ends the tutorial.
   displayEnd();
   uView.clear(PAGE);
   return 1;
}  //sDoSimpleTutorial


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
