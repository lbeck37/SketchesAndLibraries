/**************************************************************
 * 9/21/16 F Larry Beck
 * 11/21/15 Larry Beck
 **************************************************************/

//#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//#define LOG0    szLogLineHeader(++lLineCount)
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
//char auth[] = "55bce1afbf894b3bb67b7ea34f29d45a";
//char auth[] = "3c398dc9bf8643618ab3a5a091544453";
static const char acBlynkAuthToken[]  = "55bce1afbf894b3bb67b7ea34f29d45a";
static const char   szRouterName[]        = "Aspot24";
static const char   szRouterPW[]          = "Qazqaz11";

void setup()
{
  //Serial.begin(9600);
  //Serial.begin(9600);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Hello from Blynk_ESP8266_Standalone_Example, 115200 baud");
  //Serial.println("Call Blynk.begin(auth, Aspot24, Qazqaz11)");
  //Blynk.begin(auth, "C1200spot", "Qazqaz11");
  //Blynk.begin(auth, "Aspot24", "Qazqaz11");
  //SetupWiFi();
  Serial.print("Call Blynk.config(");
  //Serial.println(acBlynkAuthToken);
  Blynk.begin(acBlynkAuthToken, szRouterName, szRouterPW);
  Serial.println("Back from Blynk.begin()");
}

void loop()
{
  Blynk.run();
}

void SetupWiFi(void) {
  Serial.println("SetupWiFi(): Begin");
  WiFi.mode(WIFI_AP_STA);
	Serial.print("SetupWiFi(): Call WiFi.begin(");
	Serial.print(szRouterName);
	Serial.print(", ");
	Serial.println(szRouterPW);
	WiFi.begin(szRouterName, szRouterPW);
	Serial.print("SetupWiFi(): Call WiFi.waitForConnectResult()");
  if(WiFi.waitForConnectResult() == WL_CONNECTED) {
    Serial.println("WiFi Success");
  } //if(WiFi.waitForConnectResult()==WL_CONNECTED)
  else {
    Serial.println("WiFi Failed");
  } //if(WiFi.waitForConnectResult()==WL_CONNECTED)else
  return;
} //SetupWiFi
