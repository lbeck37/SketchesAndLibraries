#define	BLACK           0x00
#define	BLUE            0xE0
#define	RED             0x03
#define	GREEN           0x1C
#define	DGREEN           0x0C
#define YELLOW          0x1F
#define WHITE           0xFF
#define ALPHA           0xFE
#define	BROWN           0x32

uint8_t amtcolors=7;
uint8_t colors[]={BLACK,BLUE,RED,GREEN,WHITE,DGREEN,YELLOW};

uint8_t i=0;
  
uint8_t nextColor(){
  if(i>=amtcolors)i=0;
  return colors[i++];
}
  
#include <TinyScreen.h>
#include <SPI.h>
#include <Wire.h>

TinyScreen display = TinyScreen(0);


void setup(void) {
  Wire.begin();
  display.begin();
}

void loop() {
  display.setFont(liberationSans_10ptFontInfo);
  display.fontColor(WHITE,BLACK);
  
  for(int i=5;i<20;i++){
    display.setCursor((3000-(i*5))%70,(i*14)%50);
    display.print("text");
    display.setCursor((i*7)%70,(6000-(i*6))%50);
    display.print("text");
    delay(150);
  }
  for(int i=0;i<5;i++){
    display.setCursor(0,i*12);
    display.print("FASTFASTFASTFAST");
  }
  delay(200);
  
  for(int i=5;i<70;i++){
    display.setCursor((3000-(i*5))%70,(i*14)%50);
    display.fontColor(i|(i<<4),nextColor());
    display.print("color");
    display.setCursor((i*7)%70,(6000-(i*6))%50);
    display.print("COLOR");
  }
  display.fontColor(WHITE,BLACK);
  display.clearWindow(0,0,96,64);
  display.setFont(liberationSans_8ptFontInfo);
  display.setCursor(0,0);
  display.print("SIZE");
  delay(200);
  display.setFont(liberationSans_12ptFontInfo);
  display.setCursor(10,10);
  display.print("SIZE");
  delay(200);
  display.setFont(liberationSans_14ptFontInfo);
  display.setCursor(24,24);
  display.print("SIZE");
  delay(200);
  display.setFont(liberationSans_16ptFontInfo);
  display.setCursor(40,40);
  display.print("SIZE");
  delay(200);
  
  
  display.setCursor(15,20);
  display.fontColor(BLACK,WHITE);
  display.clearWindow(0,0,96,64);
  display.setBrightness(15);
  display.drawRect(0,0,96,64,1,WHITE);
  display.print("BRIGHT!");
  delay(500);
  display.fontColor(WHITE,BLACK);
  display.setCursor(30,20);
  display.clearWindow(0,0,96,64);
  display.setBrightness(0);
  display.print("DIM!");
  delay(500);
  display.setBrightness(5);
  
  
  display.fontColor(YELLOW,BLACK);
  display.setCursor(15,15);
  display.clearWindow(0,0,96,64);
  display.print("Boxes!");
  delay(200);
  for(int i=5;i<800;i++){
    int x=(3000-(i*5))%90;
    int y=(i*14)%60;
    int width=((i*7)%(90-x));
    int height=((6000-(i*6))%(60-y));
    display.drawRect(x,y,constrain(width,5,20),constrain(height,5,15),i&1,nextColor());
    if(i<10)
      delay(40);
    if(i<50)
      delay(40);
  }
  delay(500);
  display.clearWindow(0,0,96,64);
}
