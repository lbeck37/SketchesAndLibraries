#include <OneWire.h>
#include <DallasTemperature.h>
#include <Phpoc.h>
#define SAMPLE_INTERVAL 1000 // in ms

// Data wire is plugged into port 8 on the Arduino
OneWire oneWire(8);
DallasTemperature sensors(&oneWire);

unsigned long lastSampleTime;

void setup() {
	Serial.begin(9600);
	while(!Serial)
		;

	sensors.begin();
	Phpoc.begin();

	lastSampleTime = millis();
}

void loop() {
	if((millis() - lastSampleTime) > SAMPLE_INTERVAL) {
		sensors.requestTemperatures(); 
		float temp = sensors.getTempCByIndex(0);

		Serial.println(temp);

		lastSampleTime = millis();
	}
}