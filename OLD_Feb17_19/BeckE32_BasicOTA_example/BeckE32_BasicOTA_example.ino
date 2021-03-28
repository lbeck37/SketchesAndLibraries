#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "Aspot24";
const char* password = "Qazqaz11";

void setup() {
  Serial.begin(115200);
  Serial.println("setup(): Begin BeckE32_BasicOTA_example.ino, Jan 6, 2018j");
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");
  ArduinoOTA.setHostname("picoesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");
  ArduinoOTA.setPassword("tuna37");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println(".onStart(): Start updating " + type);
    })
    .onEnd([]() {
      Serial.println(".onEnd(): End");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf(".onProgress(): Progress: %u%%\n", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf(".onError(): Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("setup(): Ready");
  Serial.print("setup(): IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
}
