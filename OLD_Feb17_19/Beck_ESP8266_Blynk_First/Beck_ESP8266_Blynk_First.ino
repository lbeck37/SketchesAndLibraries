// 09/25/16 Beck, Sparkfun Thing Dev example
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
//#include<Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
//char szAuth[] = "55bce1afbf894b3bb67b7ea34f29d45a";
char szAuth[] = "b5f15bf27c1a4c929e0fa256ec283293";
//#define LED     D0        // Led in NodeMCU at pin GPIO16 (D0).
#define LED     5         // Blue LED on SparkFun Thing Dev board is at GP5
void setup()
{
  Serial.begin(115200);
  Serial.println("\nBegin Beck_ESP8266_Blynk_First.ino 9/25/16D, LBT");
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
  //char szRouterName[]= "Aspot24";
  char szRouterName[]= "C1200spot";
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

