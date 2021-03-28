const char szSketchName[]  = "Beck_FirebaseDemo_ESP8266.ino";
const char szFileDate[]    = "11/6/19k";
//
// Copyright 2015 Google Inc.
//
// FirebaseDemo_ESP8266 is a sample that demo the different functions
// of the FirebaseArduino API.

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <GDBStub.h>
#include <Streaming.h>

// Set these to run example.
#define FIREBASE_HOST "//thermo-2b830.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyAkFumb-wjDUQ9HQjTOoHeXqTKztFSqf6o"
#define WIFI_SSID "Aspot24"
#define WIFI_PASSWORD "Qazqaz11"

//#define FIREBASE_HOST "//thermo-2b830.firebaseio.com/"
//#define FIREBASE_HOST "//thermo-2b830.web.app/"
//#define FIREBASE_AUTH "AIzaSyAkFumb-wjDUQ9HQjTOoHeXqTKztFSqf6o"

void setup() {
  Serial.begin(115200);
  delay(1000);
  //Serial << "setup(): Call gdbstub_init()" << endl;
  //Start GDB
  gdbstub_init();
  Serial << endl << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;
  // connect to wifi.
  Serial << "setup(): Call WiFi.begin( " << WIFI_SSID << ", " << WIFI_PASSWORD << " )" << endl;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("setup(): connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("setup(): connected: ");
  Serial.println(WiFi.localIP());
  
  Serial << "setup(): Call Firebase.begin( " << FIREBASE_HOST << ", " << FIREBASE_AUTH << " )" << endl << endl;
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  return;
}	//setup


int n = 0;
void loop() {
  delay(5000);
  // set value
  Serial << "loop(): Call Firebase.setFloat(DegF, " << 42.2 << " )" << endl;
  Firebase.setFloat("DegF", 42.0);
  // handle error
  if (Firebase.failed()) {
      //Serial.print("setting /DegF failed:");
      Serial << "loop(): Call to Firebase.setFloat() failed" << endl;
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);
  
  // update value
  Firebase.setFloat("DegF", 43.0);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /DegF failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);

  // get value 
  Serial.print("DegF: ");
  Serial.println(Firebase.getFloat("DegF"));
  delay(1000);

  // remove value
  Firebase.remove("DegF");
  delay(1000);

//  // set string value
//  Firebase.setString("message", "hello world");
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /message failed:");
//      Serial.println(Firebase.error());
//      return;
//  }
//  delay(1000);
  
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
  String name = Firebase.pushInt("logs", n++);
  // handle error
  if (Firebase.failed()) {
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error());  
      return;
  }
  Serial.print("pushed: /logs/");
  Serial.println(name);
  delay(1000);
}
