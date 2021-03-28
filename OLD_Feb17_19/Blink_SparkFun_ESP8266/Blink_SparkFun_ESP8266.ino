//Blink_SparkFun_ESP8266.ino
// 12/7/15 Beck: Added test for pin 4
#define ESP8266_LED   5
#define Pin4          4

void setup()
{
  pinMode(ESP8266_LED, OUTPUT);
  pinMode(Pin4, OUTPUT);
}

void loop()
{
  digitalWrite(ESP8266_LED, HIGH);
  digitalWrite(Pin4, HIGH);
  delay(500);
  digitalWrite(ESP8266_LED, LOW);
  digitalWrite(Pin4, LOW);
  delay(500);
}

