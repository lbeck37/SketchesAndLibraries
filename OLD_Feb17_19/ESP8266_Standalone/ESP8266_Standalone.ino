// 09/21/16 Beck, Sparkfun Thing example
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include<Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char szAuth[] = "55bce1afbf894b3bb67b7ea34f29d45a";
#define LED     D0        // Led in NodeMCU at pin GPIO16 (D0).

void setup()
{
  Serial.begin(115200);
  Serial.println("\nBegin ESP8266_Standalone.ino 9/21/16, Vista64");
  pinMode(LED, OUTPUT);   // LED ESP G16 pin as output. 
  Serial.print("LED Pin D0=c");
  Serial.println(LED);
  FlashLED(2);
  Serial.println("setup(): Call SetupWiFi()");
  SetupWiFi();
  FlashLED(3);
  //char szLogline[]= "setup(): Call Blynk.config(" + szAuth + ")";
  Serial.print("setup(): Call Blynk.config ");
  Serial.println(szAuth);
  Blynk.config(szAuth);
  //Blynk.begin(auth, "C1200spot", "Qazqaz11");
  Serial.println("Back from Blynk.begin()");
  FlashLED(4);
}

void loop()
{
  Blynk.run();
}

void FlashLED(int sNumber){
  for(int i= 0; i < sNumber; i++){
    digitalWrite(LED, LOW);          // turn the LED on.
    delay(500);
    digitalWrite(LED, HIGH);          // turn the LED off.  
    delay(100);
  }
  delay(2000);
  return;
}

void SetupWiFi(void) {
  WiFi.mode(WIFI_AP_STA);
  //Serial << LOG0 << " SetupServer(): Call WiFi.begin("<< szRouterName << ", " << szRouterPW << ")" << endl;
  char szRouterName[]= "Aspot24";
  char szRouterPW[]  = "Qazqaz11";
  //char szLogline[]   = " SetupWiFi(): Call WiFi.begin(" + szRouterName + ", " + szRouterPW +")";
  Serial.print(" SetupWiFi(): Call WiFi.begin ");
  Serial.print(szRouterName);
  Serial.print(",");
  Serial.println(szRouterPW);
  WiFi.begin(szRouterName, szRouterPW);
  if(WiFi.waitForConnectResult() == WL_CONNECTED) {
  } //if(WiFi.waitForConnectResult()==WL_CONNECTED)
  else {
    Serial.println("WiFi Failed");
  } //if(WiFi.waitForConnectResult()==WL_CONNECTED)else
  return;
} //SetupWiFi

