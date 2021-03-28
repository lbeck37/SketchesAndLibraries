/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "WROVER_KIT_LCD.h"
#include "esp_wp1.h"
#include "esp_wp2.h"

#include "FS.h"
#include "SD_MMC.h"

#define min(X, Y) 		(((X) < (Y)) ? (X) : (Y))
#define PAUSE_DELAY		delay(2000)

WROVER_KIT_LCD 		RoverLCD;
static uint8_t 		cardType= SD_MMC.cardType();

void setup() {
  Serial.begin(115200);
 
  RoverLCD.begin();
  RoverLCD.setRotation(1);

  uint8_t x = 0;
  uint32_t id = RoverLCD.readId();
  if(id){
      Serial.println("======= WROVER ST7789V Display Test ========");
  } else {
      Serial.println("======= WROVER ILI9341 Display Test ========");
  }
  Serial.println("============================================");
  Serial.printf("Display ID:      0x%06X\n", id);

  x = RoverLCD.readcommand8(WROVER_RDDST);
  Serial.print("Status:          0x"); Serial.println(x, HEX);
  x = RoverLCD.readcommand8(WROVER_RDDPM);
  Serial.print("Power Mode:      0x"); Serial.println(x, HEX);
  x = RoverLCD.readcommand8(WROVER_RDDMADCTL);
  Serial.print("MADCTL Mode:     0x"); Serial.println(x, HEX);
  x = RoverLCD.readcommand8(WROVER_RDDCOLMOD);
  Serial.print("Pixel Format:    0x"); Serial.println(x, HEX);
  x = RoverLCD.readcommand8(WROVER_RDDIM);
  Serial.print("Image Format:    0x"); Serial.println(x, HEX);
  x = RoverLCD.readcommand8(WROVER_RDDSDR);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);
  
  Serial.println(F("Benchmark                Time (microseconds)"));
  delay(10);

  StartSDcard();

  Serial.println(F("Done!"));
  return;
}	//setup


void loop(void) {
	//TestSDcard();
	//delay(5000);
  TestLoop();
  return;
}	//loop


void StartSDcard() {
	RoverLCD.fillScreen(WROVER_BLACK);
	RoverLCD.setCursor(0, 0);
	RoverLCD.setTextColor(WROVER_WHITE);
	RoverLCD.setTextSize(1);

	Serial.println("StartSDcard(): Begin");
	RoverLCD.println("StartSDcard(): Begin");
	if(!SD_MMC.begin()){
		Serial.println("Card Mount Failed");
		RoverLCD.println("Card Mount Failed");
		return;
	}
	uint8_t cardType= SD_MMC.cardType();

	if(cardType == CARD_NONE){
		Serial.println("No SD_MMC card attached");
		RoverLCD.println("No SD_MMC card attached");
	}
	return;
}	//StartSDcard


void TestSDcard() {
	Serial.println("TestSDcard(): Begin");
	RoverLCD.println("TestSDcard(): Begin");
  Serial.print("SD_MMC Card Type: ");
	RoverLCD.print("SD_MMC Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
    	RoverLCD.println("MMC");
  } else if(cardType == CARD_SD){
      Serial.println("SDSC");
    	RoverLCD.println("SDSC");
  } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
    	RoverLCD.println("SDHC");
  } else if(cardType == CARD_NONE){
      Serial.println("No SD_MMC card attached");
    	RoverLCD.println("No SD_MMC card attached");
  } else {
      Serial.println("UNKNOWN");
    	RoverLCD.println("UNKNOWN");
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

  listDir(SD_MMC, "/", 0);
  createDir(SD_MMC, "/mydir");
  listDir(SD_MMC, "/", 0);
  removeDir(SD_MMC, "/mydir");
  listDir(SD_MMC, "/", 2);
  writeFile(SD_MMC, "/hello.txt", "Hello ");
  appendFile(SD_MMC, "/hello.txt", "World!\n");
  readFile(SD_MMC, "/hello.txt");
  deleteFile(SD_MMC, "/foo.txt");
  renameFile(SD_MMC, "/hello.txt", "/foo.txt");
  readFile(SD_MMC, "/foo.txt");
  testFileIO(SD_MMC, "/test.txt");
  Serial.printf("Total space: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));

	Serial.println("TestSDcard(): End");
	RoverLCD.println("TestSDcard(): End");
	return;
}	//TestSDcard


void TestLoop(void) {
  RoverLCD.setRotation(2);

  Serial.print(F("Screen fill              "));
  Serial.println(testFillScreen());
  PAUSE_DELAY;

  Serial.print(F("Text                     "));
  Serial.println(testText());
  delay(5000);

  Serial.print(F("Lines                    "));
  Serial.println(testLines(WROVER_CYAN));
  PAUSE_DELAY;

  Serial.print(F("Horiz/Vert Lines         "));
  Serial.println(testFastLines(WROVER_RED, WROVER_BLUE));
  PAUSE_DELAY;

  Serial.print(F("Rectangles (outline)     "));
  Serial.println(testRects(WROVER_GREEN));
  PAUSE_DELAY;

  Serial.print(F("Rectangles (filled)      "));
  Serial.println(testFilledRects(WROVER_YELLOW, WROVER_MAGENTA));
  PAUSE_DELAY;

  Serial.print(F("Circles (filled)         "));
  Serial.println(testFilledCircles(10, WROVER_MAGENTA));

  Serial.print(F("Circles (outline)        "));
  Serial.println(testCircles(10, WROVER_WHITE));
  PAUSE_DELAY;

  Serial.print(F("Triangles (outline)      "));
  Serial.println(testTriangles());
  PAUSE_DELAY;

  Serial.print(F("Triangles (filled)       "));
  Serial.println(testFilledTriangles());
  PAUSE_DELAY;

  Serial.print(F("Rounded rects (outline)  "));
  Serial.println(testRoundRects());
  PAUSE_DELAY;

  Serial.print(F("Rounded rects (filled)   "));
  Serial.println(testFilledRoundRects());
  PAUSE_DELAY;

  RoverLCD.setRotation(2);

  RoverLCD.drawJpg(esp_wp1_jpg, esp_wp1_jpg_len);
  PAUSE_DELAY;

  RoverLCD.drawJpg(esp_wp2_jpg, esp_wp2_jpg_len);
  PAUSE_DELAY;

	return;
}	//TestLoop


unsigned long testFillScreen() {
  unsigned long start = micros();
  RoverLCD.fillScreen(WROVER_BLACK);
  yield();
  RoverLCD.fillScreen(WROVER_RED);
  yield();
  RoverLCD.fillScreen(WROVER_GREEN);
  yield();
  RoverLCD.fillScreen(WROVER_BLUE);
  yield();
  RoverLCD.fillScreen(WROVER_BLACK);
  yield();
  return micros() - start;
}	//testFillScreen


unsigned long testText() {
  RoverLCD.fillScreen(WROVER_BLACK);
  unsigned long start = micros();
  RoverLCD.setCursor(0, 0);
  RoverLCD.setTextColor(WROVER_WHITE);
  RoverLCD.setTextSize(1);
  RoverLCD.println("Hello World!");

  RoverLCD.setTextColor(WROVER_YELLOW);
  RoverLCD.setTextSize(2);
  RoverLCD.println(1234.56);

  RoverLCD.setTextColor(WROVER_RED);
  RoverLCD.setTextSize(3);
  RoverLCD.println(0xDEADBEEF, HEX);
  RoverLCD.println();

  RoverLCD.setTextColor(WROVER_GREEN);
  RoverLCD.setTextSize(5);
  RoverLCD.println("Groop");

  RoverLCD.setTextSize(2);
  RoverLCD.println("I implore thee,");

  RoverLCD.setTextSize(1);
  RoverLCD.println("my foonting turlingdromes.");
  RoverLCD.println("And hooptiously drangle me");
  RoverLCD.println("with crinkly bindlewurdles,");
  RoverLCD.println("Larry loves Candy");
  RoverLCD.println("Or I will rend thee");
  RoverLCD.println("in the gobberwarts");
  RoverLCD.println("with my blurglecruncheon,");
  RoverLCD.println("see if I don't!");

  return micros() - start;
}	//testText


unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = RoverLCD.width(),
                h = RoverLCD.height();

  RoverLCD.fillScreen(WROVER_BLACK);
  yield();
  
  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) RoverLCD.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) RoverLCD.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing

  yield();
  RoverLCD.fillScreen(WROVER_BLACK);
  yield();

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) RoverLCD.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) RoverLCD.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  yield();
  RoverLCD.fillScreen(WROVER_BLACK);
  yield();

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) RoverLCD.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) RoverLCD.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  yield();
  RoverLCD.fillScreen(WROVER_BLACK);
  yield();

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) RoverLCD.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) RoverLCD.drawLine(x1, y1, x2, y2, color);

  yield();
  return micros() - start;
}	//testLines


unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = RoverLCD.width(), h = RoverLCD.height();

  RoverLCD.fillScreen(WROVER_BLACK);
  start = micros();
  for(y=0; y<h; y+=5) RoverLCD.drawFastHLine(0, y, w, color1);
  for(x=0; x<w; x+=5) RoverLCD.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}	//testFastLines


unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = RoverLCD.width()  / 2,
                cy = RoverLCD.height() / 2;

  RoverLCD.fillScreen(WROVER_BLACK);
  n     = min(RoverLCD.width(), RoverLCD.height());
  start = micros();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    RoverLCD.drawRect(cx-i2, cy-i2, i, i, color);
  }

  return micros() - start;
}	//testRects


unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = RoverLCD.width()  / 2 - 1,
                cy = RoverLCD.height() / 2 - 1;

  RoverLCD.fillScreen(WROVER_BLACK);
  n = min(RoverLCD.width(), RoverLCD.height());
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    start = micros();
    RoverLCD.fillRect(cx-i2, cy-i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    RoverLCD.drawRect(cx-i2, cy-i2, i, i, color2);
    yield();
  }

  return t;
}	//testFilledRects


unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = RoverLCD.width(), h = RoverLCD.height(), r2 = radius * 2;

  RoverLCD.fillScreen(WROVER_BLACK);
  start = micros();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      RoverLCD.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}	//testFilledCircles


unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                w = RoverLCD.width()  + radius,
                h = RoverLCD.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      RoverLCD.drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}	//testCircles


unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = RoverLCD.width()  / 2 - 1,
                      cy = RoverLCD.height() / 2 - 1;

  RoverLCD.fillScreen(WROVER_BLACK);
  n     = min(cx, cy);
  start = micros();
  for(i=0; i<n; i+=5) {
    RoverLCD.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      RoverLCD.color565(i, i, i));
  }

  return micros() - start;
}	//testTriangles


unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = RoverLCD.width()  / 2 - 1,
                   cy = RoverLCD.height() / 2 - 1;

  RoverLCD.fillScreen(WROVER_BLACK);
  start = micros();
  for(i=min(cx,cy); i>10; i-=5) {
    start = micros();
    RoverLCD.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      RoverLCD.color565(0, i*10, i*10));
    t += micros() - start;
    RoverLCD.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      RoverLCD.color565(i*10, i*10, 0));
    yield();
  }
  return t;
}	//testFilledTriangles


unsigned long testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = RoverLCD.width()  / 2 - 1,
                cy = RoverLCD.height() / 2 - 1;

  RoverLCD.fillScreen(WROVER_BLACK);
  w     = min(RoverLCD.width(), RoverLCD.height());
  start = micros();
  for(i=0; i<w; i+=6) {
    i2 = i / 2;
    RoverLCD.drawRoundRect(cx-i2, cy-i2, i, i, i/8, RoverLCD.color565(i, 0, 0));
  }

  return micros() - start;
}	//testRoundRects


unsigned long testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = RoverLCD.width()  / 2 - 1,
                cy = RoverLCD.height() / 2 - 1;

  RoverLCD.fillScreen(WROVER_BLACK);
  start = micros();
  for(i=min(RoverLCD.width(), RoverLCD.height()); i>20; i-=6) {
    i2 = i / 2;
    RoverLCD.fillRoundRect(cx-i2, cy-i2, i, i, i/8, RoverLCD.color565(0, i, 0));
    yield();
  }

  return micros() - start;
}	//testFilledRoundRects


//SD Card tests
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}


void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}


void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}


void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
}


void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
}


void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
}


void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}


void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}


void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}	//testFileIO
//Last line
