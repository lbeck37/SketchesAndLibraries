const char szSketchName[]  = "BeckE32_httpUpdate.ino";
const char szFileDate[]    = "Jan 6, 2018c Lenny";
//httpUpdate.ino, Created on: 27.11.2015
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>
#include <Streaming.h>

#define USE_SERIAL Serial

const char* ssid = "Aspot24";
const char* password = "Qazqaz11";

void setup (){
	USE_SERIAL.begin (115200);
	Serial << " setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;
	// USE_SERIAL.setDebugOutput(true);
	Serial << "setup(): Do a flush 4 times, 1 second apart" << endl;
	for (uint8_t t = 4; t > 0; t--){
		USE_SERIAL.printf ("setup(): WAIT %d...\n", t);
		USE_SERIAL.flush ();
		delay (1000);
	}	//for
	//WiFi.begin("SSID", "PASSWORD");
	Serial << "setup(): Call WiFi.begin(" << ssid << ", " << password << ")" << endl;
	WiFi.begin (ssid, password);
	Serial << "setup(): End" << endl;
	return;
}	//setup


void loop (){
	// wait for WiFi connection
	if(WiFi.status() == WL_CONNECTED)
	{
		Serial << "loop(): WiFi.status() returned WL_CONNECTED" << endl;
		t_httpUpdate_return ret = ESPhttpUpdate.update ("http://server/file.bin");
		switch (ret)
			{
			case HTTP_UPDATE_FAILED:
				USE_SERIAL.printf ("HTTP_UPDATE_FAILD Error (%d): %s",
													 ESPhttpUpdate.getLastError (),
													 ESPhttpUpdate.getLastErrorString ().c_str ());
				break;
			case HTTP_UPDATE_NO_UPDATES:
				USE_SERIAL.println ("HTTP_UPDATE_NO_UPDATES");
				break;
			case HTTP_UPDATE_OK:
				USE_SERIAL.println ("HTTP_UPDATE_OK");
				break;
			default:
				break;
			}	//switch
	}	//if(WiFi.status()==WL_CONNECTED)
}	//loop
//Last line.
