const char szSketchName[]  = "BeckE8266_fauxmoESP_Basic.ino";
const char szFileDate[]    = "Lenny 1/28/19m";

//#include <BeckMiniLib.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include "fauxmoESP.h"

#define WIFI_SSID         "Aspot24"
#define WIFI_PASS         "Qazqaz11"

#define SERIAL_BAUDRATE   115200
#define LED                    2

static const char   szAlexaName[]       = "Larry's Device";

fauxmoESP fauxmo;

void wifiSetup() {
    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    return;
} //wifiSetup


void setup(){
    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    delay(10);
    //Serial.println();
    //Serial.println();
    Serial.printf("\n\nsetup(): Sketch: %s, IP address: %s\n", szSketchName, szFileDate);

    // Wifi
    wifiSetup();

    // LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);

    // You have to call enable(true) once you have a WiFi connection
    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    fauxmo.enable(true);
    fauxmo.enable(false);
    fauxmo.enable(true);

    // You can use different ways to invoke alexa to modify the devices state:
    // "Alexa, turn light one on" ("light one" is the name of the first device below)
    // "Alexa, turn on light one"
    // "Alexa, set light one to fifty" (50 means 50% of brightness)

    // Add virtual devices
    Serial.printf("setup(): Call fauxmo.addDevice() szAlexaName= %s\n", szAlexaName);
    fauxmo.addDevice(szAlexaName);

    // You can add more devices
    //fauxmo.addDevice("light 2");
    //fauxmo.addDevice("light 3");
    //fauxmo.addDevice("light 4");
    //fauxmo.addDevice("light 5");
    //fauxmo.addDevice("light 6");
    //fauxmo.addDevice("light 7");
    //fauxmo.addDevice("light 8");

    // fauxmoESP 2.0.0 has changed the callback signature to add the device_id,
    // this way it's easier to match devices to action without having to compare strings.
    Serial.println("setup(): Call fauxmo.onSetState()");
    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
        digitalWrite(LED, !state);
    });
    return;
} //setup


void loop() {
    // Since fauxmoESP 2.0 the library uses the "compatibility" mode by
    // default, this means that it uses WiFiUdp class instead of AsyncUDP.
    // The later requires the Arduino Core for ESP8266 staging version
    // whilst the former works fine with current stable 2.3.0 version.
    // But, since it's not "async" anymore we have to manually poll for UDP
    // packets
    fauxmo.handle();

    static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
        //Serial << LOG0 << "loop(): Call Free heap= " << ESP.getFreeHeap() << endl;
    }
    return;
} //loop
//Last line.
