const char szSketchName[]  = "Beck_Firebase_12019.ino";
const char szFileDate[]    = "1/4/20g";

//#include <<span class="TextRun Highlight BCX0 SCXW174472232" lang="EN-US" xml:lang="EN-US" data-contrast="auto"><span class="SpellingError BCX0 SCXW174472232">FirebaseArduino.h></span></span>
#include <ESP8266WiFi.h>
//#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <BeckLogLib.h>
#include <Streaming.h>

#define FIREBASE_HOST 	"https://test-70884.firebaseio.com"
#define FIREBASE_AUTH 	"AIzaSyD-Nm1dYBV6ehphAOQgkM5sz4oYLKF9ahg"

//#define FIREBASE_HOST   "https://thermo-2b830.firebaseio.com/"
//#define FIREBASE_AUTH   "AIzaSyAkFumb-wjDUQ9HQjTOoHeXqTKztFSqf6o"

#define WIFI_SSID 		"Aspot24"
#define WIFI_PASSWORD 	"Qazqaz11"

//bool	_bConnected				= false;
void setup() {
  int	wNotConnectCount		= 0;
  int	wMaxNotConnectCount		= 40;	//10 sec at 250 msec delay

  Serial.begin(115200);
  delay(1000);
  Serial << LOG0 << endl << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;

  Serial << LOG0 << "setup(): Call WiFi.begin(" << WIFI_SSID << ", " << WIFI_PASSWORD << ")" << endl;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial << LOG0 << "setup(): Wait for WiFi.status() to be WL_CONNECTED" << endl;
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial << (wNotConnectCount % 10);
    if (++wNotConnectCount > wMaxNotConnectCount){
      Serial << endl << LOG0 << "setup(): WiFi did not connect" << endl;
      break;
    }	//if(++wNotConnectCount>wMaxNotConnectCount)
  }	//while
  Serial << endl;

  if (WiFi.status() == WL_CONNECTED) {
	  Serial << LOG0 << "setup(): Call Firebase.begin()" << endl;
	  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  }	//if(WiFi.status()==WL_CONNECTED)
  return;
}	//setup


void loop() {
  static int wLogCount = 0;

  if (WiFi.status() != WL_CONNECTED) {
	  Serial << LOG0 << "loop(): Do nothing because (WiFi.status() != WL_CONNECTED)" << endl;
	  delay(5000);
	  return;
  }	//if(WiFi.status()!=WL_CONNECTED)

  // set value
  Firebase.setFloat("Setpoint", 42.0);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /Setpoint failed:");
      Serial.println(Firebase.error());
      return;
  }
  delay(1000);

  // update value
  Firebase.setFloat("Setpoint", 43.0);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /Setpoint failed:");
      Serial.println(Firebase.error());
      return;
  }
  delay(1000);

  // get value
  Serial.print("Setpoint: ");
  Serial.println(Firebase.getFloat("Setpoint"));
  delay(1000);

  // remove value
  Firebase.remove("Setpoint");
  delay(1000);

  // set string value
  Firebase.setString("message", "hello world");
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /message failed:");
      Serial.println(Firebase.error());
      return;
  }
  delay(1000);

  // set bool value
  Firebase.setBool("truth", false);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /truth failed:");
      Serial.println(Firebase.error());
      return;
  }
  delay(1000);

  // append a new value to /logs
  String name = Firebase.pushInt("logs", wLogCount++);
  // handle error
  if (Firebase.failed()) {
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error());
      return;
  }
  Serial.print("pushed: /logs/");
  Serial.println(name);
  delay(1000);
  return;
}	//loop
//Last line.
