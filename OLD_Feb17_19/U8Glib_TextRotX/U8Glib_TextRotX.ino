#include "U8glib.h"

U8GLIB_DOGS102 u8g(13, 11, 10, 9, 8);     // SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9

// graphic commands to redraw the complete screen should be placed here
void draw(void) {
  u8g_uint_t mx, my;

  mx = u8g.getWidth();
  mx >>= 1;

  my = u8g.getHeight();
  my >>= 1;

  u8g.drawStr( mx, my, "Ag");
  u8g.drawStr90( mx, my, "Ag");
  u8g.drawStr180( mx, my, "Ag");
  u8g.drawStr270( mx, my, "Ag");
}

void setup(void) {
  u8g.setFont(u8g_font_9x18);
}

void change_font_pos(void) {
  static  uint8_t dir = 0;
  static  unsigned long next = 0;

  if ( next < millis() )
  {
    switch(dir) {
      case 0: u8g.setFontPosBottom(); break;
      case 1: u8g.setFontPosBaseline(); break;
      case 2: u8g.setFontPosCenter(); break;
      case 3: u8g.setFontPosTop(); break;
    }

    dir++;
    dir &= 3;
    next = millis();
    next += 1000;
  }
}

void loop(void) {
  // change the font position
  change_font_pos();

  // picture loop
  u8g.firstPage();
  do {
    draw();
  } while( u8g.nextPage() );

  // rebuild the picture after some delay
  delay(100);
}

