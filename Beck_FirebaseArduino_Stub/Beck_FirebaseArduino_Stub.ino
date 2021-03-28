const char szSketchName[]  = "Beck_FirebaseArduino_Stub.ino";
const char szFileDate[]    = "1/31/20a_Sloeber 4.3.2A Debug";

#ifndef ESP8266
  #define ESP8266
#endif

#define DO_ACCESS_POINT       false
#define DO_DEBUG              true

#if DO_DEBUG
	#include "GDBstub.h"
#endif

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <BeckLogLib.h>

#if DO_ACCESS_POINT
  #include <BeckAccessPointLib_Simple.h>
  const char*   _acAccessPointSSID    = "StubSpot";
  const char*   _acAccessPointPW      = "Qazqaz11";
#endif

//const long      lSerialMonitorBaud  = 115200;

const char*     WiFi_SSID           = "Aspot24";
const char*     WiFi_Password       = "Qazqaz11";

//#define FIREBASE_HOST   "https://thermo-2b830.firebaseio.com/"
//#define FIREBASE_AUTH   "AIzaSyAkFumb-wjDUQ9HQjTOoHeXqTKztFSqf6o"

//#define FIREBASE_HOST 			"https://test-70884.firebaseio.com"
//#define FIREBASE_AUTH 			"AIzaSyD-Nm1dYBV6ehphAOQgkM5sz4oYLKF9ahg"

/*
const firebaseConfig = {
  apiKey: "AIzaSyBVOUnaaCjuoFWLsiyVi7ZSW_PsiHLam1A",
  authDomain: "fir-mariestep1.firebaseapp.com",
  databaseURL: "https://fir-mariestep1.firebaseio.com",
  projectId: "fir-mariestep1",
  storageBucket: "fir-mariestep1.appspot.com",
  messagingSenderId: "754022099819",
  appId: "1:754022099819:web:50b456b2ed48aa27"
};
*/
//#define FIREBASE_HOST "fir-mariestep1.appspot.com"
//#define FIREBASE_HOST "fir-mariestep1.firebaseapp.com"        //Connection refused

#define FIREBASE_HOST "fir-mariestep1.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyBVOUnaaCjuoFWLsiyVi7ZSW_PsiHLam1A"

#if DO_DEBUG
  #define RAMFUNC     __attribute__((section(".entry.text")))
#else
  #define RAMFUNC
#endif

bool    _bWiFiConnected;

//Function protos
void 		setup				();
void  	loop				();
void    HandleLoop	();

void setup() {
  uart_div_modify(0, UART_CLK_FREQ / 115200);		//UART_CLK_FREQ is 80x10^6. CPU freq
  Serial.begin(115200);
  delay(100);

#if DO_DEBUG
  gdbstub_init();
#endif

  Serial << endl << endl;
  Serial << LOG0 << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;
  Serial << LOG0 << "Setup(): Connecting to WiFi" << endl;
  WiFi.begin(WiFi_SSID, WiFi_Password);

  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  } //while
  Serial << endl;

  if (WiFi.status() == WL_CONNECTED) {
    _bWiFiConnected= true;
    Serial << LOG0 << "setup(): Connected with IP: " << WiFi.localIP() << endl;
  }
  else {
    _bWiFiConnected= false;
    Serial << LOG0 << "setup(): WiFi did not connect" << endl;
  }

  if (_bWiFiConnected) {
	  Serial << LOG0 << "setup(): Call Firebase.begin()" << endl;
	  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  }	//if(_bWiFiConnected)

#if DO_ACCESS_POINT
  SetupAccessPt(_acAccessPointSSID, _acAccessPointPW);
#endif  //DO_ACCESS_POINT

  Serial << LOG0 << "setup(): Done" << endl;
  return;
} //setup


void RAMFUNC loop() {
		HandleLoop();
		delay(5000);
		return;
	}   //loop


void HandleLoop(){
  if (WiFi.status() != WL_CONNECTED) {
	  Serial << LOG0 << "loop(): Do nothing because (WiFi.status() != WL_CONNECTED)" << endl;
	  return;
  }	//if(WiFi.status()!=WL_CONNECTED)

  // get value
  Serial << LOG0 << "loop(): Read a value from the database" << endl;
  Serial << LOG0 << "loop(): Call Firebase.getFloat(\"Setpoint\")" << endl;
  float fValue= Firebase.getFloat("Setpoint");
  Serial << LOG0 << "loop(): Call Firebase.getFloat() returned fValue= " << fValue << endl;
  if (Firebase.failed()) {
		Serial << LOG0 << "loop(): Reading /Setpoint failed, error= |" << Firebase.error() << "|" << endl;
		Serial << LOG0 << "loop(): Firebase.error()= |" << Firebase.error() << "|" << endl;
  }


/*
  delay(1000);
  // set value
  //Serial << LOG0 << "loop(): Set a value in the database" << endl;
  //Serial << LOG0 << "loop(): Call Firebase.setFloat(\"Setpoint\", 42.0)" << endl;
  Serial << LOG0 << "loop(): Set a value into the database" << endl;
  Serial << LOG0 << "loop(): Call Firebase.setFloat(\"Setpoint\", 42.0)" << endl;
  Firebase.setFloat("Setpoint", 42.0);
  // handle error
  if (Firebase.failed()) {
		Serial << LOG0 << "loop(): Setting /Setpoint failed, error= |" << Firebase.error() << "|" << endl;
		Serial << LOG0 << "loop(): Firebase.error()= |" << Firebase.error() << "|" << endl;
  }
  Serial << LOG0 << "loop(): Delay 5 seconds" << endl;
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

  static int wLogCount = 0;

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
*/

  return;
}   //HandleLoop
//Last line.
