const char szSketchName[]  = "Beck_FirebaseRawTest.ino";
const char szFileDate[]    = "8/28/19b";

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BeckFirebase.h>
/*
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>
*/
#include <Streaming.h>
//#include "Creds.h"

#define SSID "Aspot24"
#define PASSWORD "Qazqaz11"

//#define FIREBASE_HOST "https://fir-mariestep1.firebaseio.com/"
//#define FIREBASE_HOST "fir-mariestep1.firebaseio.com/"

#define FIREBASE_HOST "//simpleiot-65405.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyDTXPBe2Bgy003pA8RgOTPVIURZ8EEnG3Y"

/*
#define FIREBASE_HOST "//fir-mariestep1.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyBVOUnaaCjuoFWLsiyVi7ZSW_PsiHLam1A"
*/


void connectToWiFi() {
    delay(10);
    Serial << "connectToWiFi(): Connecting to " << SSID << endl;
    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
    would try to act as both a client and an access-point and could cause
    network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial << endl << "connectToWiFi(): WiFi connected to " << SSID << "IP address: " << WiFi.localIP() << endl << endl;
} //connectToWiFi


void setup() {
    Serial.begin(115200);
    delay(100);
    Serial << endl << endl << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;
    connectToWiFi();
    //Serial << "setup(): Call Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH)" << endl;
/*
    Serial << "setup(): Call Firebase.begin(" << FIREBASE_HOST << ", " << FIREBASE_AUTH << ")" << endl;
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   //Does not set error
*/
    return;
} //setup


void loop() {
    static float  fCount= 20.0;
    // === Push dummy temperature value to Firebase ===
    fCount += 0.01;
    Serial << endl << "loop(): Begin" << endl;
/*
    Serial << endl << "loop(): Call Firebase.setFloat(DegF, " << fCount << ")" << endl;
    Firebase.setFloat("DegF", fCount);

    if (Firebase.failed()) {
      //Serial.print("loopp():Error setting DegF, error= ");
      //Serial.println(Firebase.error());
      Serial << "loop():Error setting DegF, error= " << Firebase.error() << endl;
    } //if(Firebase.failed())
    else {
      Serial << "loop(): Set DegF to " << fCount << " in Firebase" << endl;
    } //if(Firebase.failed()) else

    // === Get Setpoint from Firebase ===
    Serial << "loop(): Call Firebase.getFloat(Setpoint)" << ")" << endl;
    float fNewSetpoint= Firebase.getFloat("Setpoint");

    if (Firebase.success()) {
      Serial << "loop(): fNewSetpoint= " << fNewSetpoint << endl;
    } //if(Firebase.failed())
    else {
      //Serial << "loop(): Set DegF to " << fCount << " in Firebase" << endl;
      Serial << "loop(): Error getting fNewSetpoint, error= " << Firebase.error() << endl;
    } //if(Firebase.failed()) else

*/
    delay(5000);
    return;
} //loop
//Last line
