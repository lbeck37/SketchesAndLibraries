static const char szSketchName[]  = "BeckESP32_GraphicsTest.ino";
static const char szFileDate[]    = "Oct 2, 2017, Lenny-e";
/*
  GraphicsTest.ino
  Generate some example graphics
  Universal uC Color Graphics Library
  Copyright (c) 2014, olikraus@gmail.com
  All rights reserved.
  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <Arduino.h>
#include <BeckLogLib.h>
#include <SPI.h>
#include "Ucglib.h"

#define T 4000
#define DLY() delay(2000)

/*
  Hardware SPI Pins:
    Arduino Uno    sclk=13, data=11
    Arduino Due    sclk=76, data=75
    Arduino Mega   sclk=52, data=51
    ESP-WROVER-KIT sclk=19, data=23
*/

//Ucglib_ILI9341_18x240x320_SWSPI ucg(/*sclk=*/ 19, /*data=*/ 23, /*cd=*/ 21 , /*cs=*/ 22, /*reset=*/ 18);
Ucglib_ILI9341_18x240x320_HWSPI ucg(/*cd=*/ 21 , /*cs=*/ 22, /*reset=*/ 18);

//Ucglib_ILI9341_18x240x320_SWSPI ucg(/*sclk=*/ 7, /*data=*/ 6, /*cd=*/ 5 , /*cs=*/ 3, /*reset=*/ 4);
//Ucglib_ILI9341_18x240x320_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9 , /*cs=*/ 10, /*reset=*/ 8);
//Ucglib_ILI9341_18x240x320_HWSPI ucg(/*cd=*/ 9 , /*cs=*/ 10, /*reset=*/ 8);
//Ucglib_ILI9341_18x240x320_SWSPI ucg(/*sclk=*/ 4, /*data=*/ 3, /*cd=*/ 6 , /*cs=*/ 7, /*reset=*/ 5);	/* Elec Freaks


void setup(void)
{
  Serial.begin(115200);
  Serial << endl << "setup(): Begin " << szSketchName << ", " << szFileDate << endl;
  delay(1000);
  Serial << "setup(): Call ucg.begin()" << endl;
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);					//ucglib.cpp 282
  DLY();
  Serial << "setup(): Call ucg.setFont()" << endl;
  ucg.setFont(ucg_font_ncenR14_hr);
  DLY();
  Serial << "setup(): Call ucg.clearScreen()" << endl;
  ucg.clearScreen();
} //setup


static uint8_t ucRotation = 0;
void loop(void)
{
  switch(ucRotation & 3)
  {
    case 0: ucg.undoRotate(); break;
    case 1: ucg.setRotate90(); break;
    case 2: ucg.setRotate180(); break;
    default: ucg.setRotate270(); break;
  } //switch

  if ( ucRotation > 3 )
  {
    ucg.clearScreen();
    set_clip_range();
  }
  ucRotation++;
  DLY();
  Serial << "setup(): Call ucglib_graphics_test()" << endl;
  ucglib_graphics_test();
  cross();
  pixel_and_lines();
  color_test();
  triangle();
  fonts();
  text();
  if ( ucRotation <= 3 )
    clip();
  box();
  gradient();
  //ucg.clearScreen();
  DLY();
  ucg.setMaxClipRange();
  return;
} //loop


void ucglib_graphics_test(void)
{
  Serial << "ucglib_graphics_test(): Begin" << endl;
  //ucg.setMaxClipRange();
  ucg.setColor(0, 0, 40, 80);
  ucg.setColor(1, 80, 0, 40);
  ucg.setColor(2, 255, 0, 255);
  ucg.setColor(3, 0, 255, 255);

  Serial << "ucglib_graphics_test(): Call ucg.drawGradientBox()" << endl;
  ucg.drawGradientBox(0, 0, ucg.getWidth(), ucg.getHeight());

  ucg.setColor(255, 168, 0);
  ucg.setPrintDir(0);
  ucg.setPrintPos(2,18);
  ucg.print("Ucglib");
  ucg.setPrintPos(2,18+20);
  ucg.print("GraphicsTest");

  DLY();
} //ucglib_graphics_test


void gradient(void)
{
  ucg.setColor(0, 0, 255, 0);
  ucg.setColor(1, 255, 0, 0);
  ucg.setColor(2, 255, 0, 255);
  ucg.setColor(3, 0, 255, 255);

  ucg.drawGradientBox(0, 0, ucg.getWidth(), ucg.getHeight());

  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(2,18);
  ucg.setPrintDir(0);
  ucg.print("GradientBox");

  ucg.setColor(0, 0, 255, 0);
  ucg.drawBox(2, 25, 8, 8);

  ucg.setColor(0, 255, 0, 0);
  ucg.drawBox(2+10, 25, 8, 8);

  ucg.setColor(0, 255, 0, 255);
  ucg.drawBox(2, 25+10, 8, 8);

  ucg.setColor(0, 0, 255, 255);
  ucg.drawBox(2+10, 25+10, 8, 8);

  DLY();
} //gradient


void box(void)
{
  ucg_int_t x, y, w, h;
  unsigned long m;

  ucg.setColor(0, 0, 40, 80);
  ucg.setColor(1, 60, 0, 40);
  ucg.setColor(2, 128, 0, 140);
  ucg.setColor(3, 0, 128, 140);
  ucg.drawGradientBox(0, 0, ucg.getWidth(), ucg.getHeight());

  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(2,18);
  ucg.setPrintDir(0);
  ucg.print("Box");

  m = millis() + T;

  while( millis() < m )
  {
    ucg.setColor((lcg_rnd()&127)+127, (lcg_rnd()&127)+64, lcg_rnd() & 31);
    w = lcg_rnd() & 31;
    h = lcg_rnd() & 31;
    w += 10;
    h += 10;
    x = (lcg_rnd()*(ucg.getWidth()-w))>>8;
    y = (lcg_rnd()*(ucg.getHeight()-h-20))>>8;

    ucg.drawBox(x, y+20, w, h);
  }
} //box


void pixel_and_lines(void)
{
  ucg_int_t mx;
  ucg_int_t x, xx;
  mx = ucg.getWidth() / 2;
  //my = ucg.getHeight() / 2;

  ucg.setColor(0, 0, 0, 150);
  ucg.setColor(1, 0, 60, 40);
  ucg.setColor(2, 60, 0, 40);
  ucg.setColor(3, 120, 120, 200);
  ucg.drawGradientBox(0, 0, ucg.getWidth(), ucg.getHeight());

  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(2,18);
  ucg.setPrintDir(0);
  ucg.print("Pix&Line");

  ucg.drawPixel(0, 0);
  ucg.drawPixel(1, 0);
  //ucg.drawPixel(ucg.getWidth()-1, 0);
  //ucg.drawPixel(0, ucg.getHeight()-1);

  ucg.drawPixel(ucg.getWidth()-1, ucg.getHeight()-1);
  ucg.drawPixel(ucg.getWidth()-1-1, ucg.getHeight()-1);


  for( x = 0; x  < mx; x++ )
  {
    xx = (((uint16_t)x)*255)/mx;
    ucg.setColor(255, 255-xx/2, 255-xx);
    ucg.drawPixel(x, 24);
    ucg.drawVLine(x+7, 26, 13);
  }

  DLY();
} //pixel_and_lines


void color_test(void)
{
  ucg_int_t mx;
  uint16_t c, x;
  mx = ucg.getWidth() / 2;
  //my = ucg.getHeight() / 2;

  ucg.setColor(0, 0, 0, 0);
  ucg.drawBox(0, 0, ucg.getWidth(), ucg.getHeight());

  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(2,18);
  ucg.setPrintDir(0);
  ucg.print("Color Test");

  ucg.setColor(0, 127, 127, 127);
  ucg.drawBox(0, 20, 16*4+4, 5*8+4);

  for( c = 0, x = 2; c <= 255; c+=17, x+=4 )
  {
    ucg.setColor(0, c, c, c);
    ucg.drawBox(x, 22, 4, 8);
    ucg.setColor(0, c, 0, 0);
    ucg.drawBox(x, 22+8, 4, 8);
    ucg.setColor(0, 0, c, 0);
    ucg.drawBox(x, 22+2*8, 4, 8);
    ucg.setColor(0, 0, 0, c);
    ucg.drawBox(x, 22+3*8, 4, 8);
    ucg.setColor(0, c, 255-c, 0);
    ucg.drawBox(x, 22+4*8, 4, 8);
  } //for
  DLY();
} //color_test


void cross(void)
{
  ucg_int_t mx, my;
  ucg.setColor(0, 250, 0, 0);
  ucg.setColor(1, 255, 255, 30);
  ucg.setColor(2, 220, 235, 10);
  ucg.setColor(3, 205, 0, 30);
  ucg.drawGradientBox(0, 0, ucg.getWidth(), ucg.getHeight());
  mx = ucg.getWidth() / 2;
  my = ucg.getHeight() / 2;

  ucg.setColor(0, 255, 255, 255);
  ucg.setPrintPos(2,18);
  ucg.print("Cross");

  ucg.setColor(0, 0, 0x66, 0xcc);
  ucg.setPrintPos(mx+15,my-5);
  ucg.print("dir0");
  ucg.setPrintPos(mx+5,my+26);
  ucg.print("dir1");
  ucg.setPrintPos(mx-40,my+20);
  ucg.print("dir2");
  ucg.setPrintPos(mx+5,my-25);
  ucg.print("dir3");

  ucg.setColor(0, 0, 0x66, 0xff);
  ucg.setColor(1, 0, 0x66, 0xcc);
  ucg.setColor(2, 0, 0, 0x99);

  ucg_Draw90Line(ucg.getUcg(), mx+2, my-1, 20, 0, 0);
  ucg_Draw90Line(ucg.getUcg(), mx+2, my, 20, 0, 1);
  ucg_Draw90Line(ucg.getUcg(), mx+2, my+1, 20, 0, 2);

  ucg_Draw90Line(ucg.getUcg(), mx+1, my+2, 20, 1, 0);
  ucg_Draw90Line(ucg.getUcg(), mx, my+2, 20, 1, 1);
  ucg_Draw90Line(ucg.getUcg(), mx-1, my+2, 20, 1, 2);

  ucg_Draw90Line(ucg.getUcg(), mx-2, my+1, 20, 2, 0);
  ucg_Draw90Line(ucg.getUcg(), mx-2, my, 20, 2, 1);
  ucg_Draw90Line(ucg.getUcg(), mx-2, my-1, 20, 2, 2);

  ucg_Draw90Line(ucg.getUcg(), mx-1, my-2, 20, 3, 0);
  ucg_Draw90Line(ucg.getUcg(), mx, my-2, 20, 3, 1);
  ucg_Draw90Line(ucg.getUcg(), mx+1, my-2, 20, 3, 2);

  DLY();
} //cross


void triangle(void)
{
  unsigned long m;

  ucg.setColor(0, 0, 80, 20);
  ucg.setColor(1, 60, 80, 20);
  ucg.setColor(2, 60, 120, 0);
  ucg.setColor(3, 0, 140, 30);
  ucg.drawGradientBox(0, 0, ucg.getWidth(), ucg.getHeight());

  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(2,18);
  ucg.print("Triangle");

  m = millis() + T;

  while( millis() < m )
  {
    ucg.setColor((lcg_rnd()&127)+127, lcg_rnd() & 31, (lcg_rnd()&127)+64);

    ucg.drawTriangle(
      (lcg_rnd()*(ucg.getWidth()))>>8,
      ((lcg_rnd()*(ucg.getHeight()-20))>>8)+20,
      (lcg_rnd()*(ucg.getWidth()))>>8,
      ((lcg_rnd()*(ucg.getHeight()-20))>>8)+20,
      (lcg_rnd()*(ucg.getWidth()))>>8,
      ((lcg_rnd()*(ucg.getHeight()-20))>>8)+20
    );
  }
} //triangle


void text(void)
{
  ucg_int_t x, y, w, h, i;
  unsigned long m;

  ucg.setColor(0, 80, 40, 0);
  ucg.setColor(1, 60, 0, 40);
  ucg.setColor(2, 20, 0, 20);
  ucg.setColor(3, 60, 0, 0);
  ucg.drawGradientBox(0, 0, ucg.getWidth(), ucg.getHeight());

  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(2,18);
  ucg.setPrintDir(0);
  ucg.print("Text");

  m = millis() + T;
  i = 0;
  while( millis() < m )
  {
    ucg.setColor(lcg_rnd() & 31, (lcg_rnd()&127)+127, (lcg_rnd()&127)+64);
    w = 40;
    h = 22;
    x = (lcg_rnd()*(ucg.getWidth()-w))>>8;
    y = (lcg_rnd()*(ucg.getHeight()-h))>>8;

    ucg.setPrintPos(x,y+h);
    ucg.setPrintDir((i>>2)&3);
    i++;
    ucg.print("Ucglib");
  }
  ucg.setPrintDir(0);
} //text


void fonts(void)
{
  ucg_int_t d = 5;
  ucg.setColor(0, 0, 40, 80);
  ucg.setColor(1, 150, 0, 200);
  ucg.setColor(2, 60, 0, 40);
  ucg.setColor(3, 0, 160, 160);

  ucg.drawGradientBox(0, 0, ucg.getWidth(), ucg.getHeight());

  ucg.setColor(255, 255, 255);
  ucg.setPrintDir(0);
  ucg.setPrintPos(2,18);
  ucg.print("Fonts");

  ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);

  ucg.setColor(255, 200, 170);
  ucg.setFont(ucg_font_helvB08_hr);
  ucg.setPrintPos(2,30+d);
  ucg.print("ABC abc 123");
  ucg.setFont(ucg_font_helvB10_hr);
  ucg.setPrintPos(2,45+d);
  ucg.print("ABC abc 123");
  ucg.setFont(ucg_font_helvB12_hr);
  //ucg.setPrintPos(2,62+d);
  //ucg.print("ABC abc 123");
  ucg.drawString(2,62+d, 0, "ABC abc 123"); // test drawString

  ucg.setFontMode(UCG_FONT_MODE_SOLID);

  ucg.setColor(255, 200, 170);
  ucg.setColor(1, 0, 100, 120);   // background color in solid mode
  ucg.setFont(ucg_font_helvB08_hr);
  ucg.setPrintPos(2,75+30+d);
  ucg.print("ABC abc 123");
  ucg.setFont(ucg_font_helvB10_hr);
  ucg.setPrintPos(2,75+45+d);
  ucg.print("ABC abc 123");
  ucg.setFont(ucg_font_helvB12_hr);
  ucg.setPrintPos(2,75+62+d);
  ucg.print("ABC abc 123");

  ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);

  /* big fonts removed, some trouble with the Arduino IDE */
  /*
  ucg.setFont(ucg_font_helvB14_hr);
  ucg.setPrintPos(2,79+d);
  ucg.print("ABC abc 123");
  ucg.setFont(ucg_font_helvB18_hr);
  ucg.setPrintPos(2,79+22+d);
  ucg.print("ABC abc 123");
  */

  ucg.setFont(ucg_font_ncenR14_hr);
  DLY();
} //fonts


void clip(void)
{
  ucg.setColor(0, 0x00, 0xd1, 0x5e);    // dark green
  ucg.setColor(1, 0xff, 0xf7, 0x61);    // yellow
  ucg.setColor(2, 0xd1, 0xc7, 0x00);      // dark yellow
  ucg.setColor(3, 0x61, 0xff, 0xa8);    // green

  ucg.drawGradientBox(0, 0, ucg.getWidth(), ucg.getHeight());

  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(2,18);
  ucg.setPrintDir(0);
  ucg.print("ClipRange");

  ucg.setColor(0xd1, 0x00, 0x073);

  ucg.setFont(ucg_font_helvB18_hr);

  ucg.setPrintPos(25,45);
  ucg.setPrintDir(0);
  ucg.print("Ucg");
  ucg.setPrintDir(1);
  ucg.print("Ucg");
  ucg.setPrintDir(2);
  ucg.print("Ucg");
  ucg.setPrintDir(3);
  ucg.print("Ucg");

  ucg.setMaxClipRange();
  ucg.setColor(0xff, 0xff, 0xff);
  ucg.drawFrame(20-1,30-1,15+2,20+2);
  ucg.setClipRange(20, 30, 15, 20);
  ucg.setColor(0xff, 0x61, 0x0b8);
  ucg.setPrintPos(25,45);
  ucg.setPrintDir(0);
  ucg.print("Ucg");
  ucg.setPrintDir(1);
  ucg.print("Ucg");
  ucg.setPrintDir(2);
  ucg.print("Ucg");
  ucg.setPrintDir(3);
  ucg.print("Ucg");

  ucg.setMaxClipRange();
  ucg.setColor(0xff, 0xff, 0xff);
  ucg.drawFrame(60-1,35-1,25+2,18+2);
  ucg.setClipRange(60, 35, 25, 18);
  ucg.setColor(0xff, 0x61, 0x0b8);
  ucg.setPrintPos(25,45);
  ucg.setPrintDir(0);
  ucg.print("Ucg");
  ucg.setPrintDir(1);
  ucg.print("Ucg");
  ucg.setPrintDir(2);
  ucg.print("Ucg");
  ucg.setPrintDir(3);
  ucg.print("Ucg");

  ucg.setMaxClipRange();
  ucg.setColor(0xff, 0xff, 0xff);
  ucg.drawFrame(7-1,58-1,90+2,4+2);
  ucg.setClipRange(7, 58, 90, 4);
  ucg.setColor(0xff, 0x61, 0x0b8);
  ucg.setPrintPos(25,45);
  ucg.setPrintDir(0);
  ucg.print("Ucg");
  ucg.setPrintDir(1);
  ucg.print("Ucg");
  ucg.setPrintDir(2);
  ucg.print("Ucg");
  ucg.setPrintDir(3);
  ucg.print("Ucg");

  ucg.setFont(ucg_font_ncenR14_hr);
  ucg.setMaxClipRange();
  DLY();
} //clip


void set_clip_range(void)
{
  ucg_int_t x, y, w, h;
  w = lcg_rnd() & 31;
  h = lcg_rnd() & 31;
  w += 25;
  h += 25;
  x = (lcg_rnd()*(ucg.getWidth()-w))>>8;
  y = (lcg_rnd()*(ucg.getHeight()-h))>>8;

  ucg.setClipRange(x, y, w, h);
} //set_clip_range


static uint8_t z = 127;  // start value
uint32_t lcg_rnd(void) {
  z = (uint8_t)((uint16_t)65*(uint16_t)z + (uint16_t)17);
  return (uint32_t)z;
} //lcg_rnd
//Last line
