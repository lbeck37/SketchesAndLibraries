const char szSketchName[]  = "Beck_ElectronicClinic_Firebase.ino";
const char szFileDate[]    = "12/14/19a";

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <Streaming.h>
/*
#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <Streaming.h>
*/

/*
#define FIREBASE_HOST "//thermo-2b830.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyAkFumb-wjDUQ9HQjTOoHeXqTKztFSqf6o"
*/
#define FIREBASE_HOST 	"//test-70884.firebaseio.com"
#define FIREBASE_AUTH 	"AIzaSyD-Nm1dYBV6ehphAOQgkM5sz4oYLKF9ahg"

#define WIFI_SSID 		"Aspot24"
#define WIFI_PASSWORD 	"Qazqaz11"

String myString;
//int vr = A0; // variable resistor connected
//int sdata = 0; // The variable resistor value will be stored in sdata.

void setup(){
  Serial.begin(115200);
  delay(1000);
  Serial << endl << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  Serial << "Connecting to WiFi router" << endl;
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  } //while
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  return;
} //setup


void loop(){
	//sdata = analogRead(vr);
	static int wSetpoint= 22;

/*
	//Try a get first
	Serial << "loop(): Call Firebase.getInt() for SetPoint" << endl;
	int wCurrentSetpoint= Firebase.getInt("/Setpoint");
	Serial << "loop(): Firebase.error()= |" << Firebase.error() << "|" << endl;
	Serial << "loop(): wCurrentSetpoint= " << wCurrentSetpoint << endl;
	delay(2000);

*/
	Serial << "loop(): Setting Setpoint to " << ++wSetpoint << endl;
/*
	//Try a push first
	Serial << "loop(): Call Firebase.pushInt()" << endl;
	Firebase.pushInt("Dude", wSetpoint);
	Serial << "loop(): Firebase.error()= |" << Firebase.error() << "|" << endl;
	delay(2000);
*/

	Serial << "loop(): Call Firebase.setInt()" << endl;
	//Firebase.setInt("/Setpoint", wSetpoint);
	Firebase.setInt("/Setpoint", wSetpoint);
	Serial << "loop(): Firebase.error()= |" << Firebase.error() << "|" << endl;
	delay(2000);
	return;
} //loop
//Last line.
