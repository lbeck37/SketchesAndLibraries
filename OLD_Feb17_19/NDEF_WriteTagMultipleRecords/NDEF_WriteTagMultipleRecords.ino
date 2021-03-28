// 1/25/16 Beck
#if 1
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);
#else

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
#endif

void setup() {
    Serial.begin(9600);
    Serial.println("NDEF Writer");
    nfc.begin();
}

void loop() {
    Serial.println("\nPlace a formatted Mifare Classic NFC tag on the reader.");
    Serial.println("A Text, Uri and another Text field will be written.");
    if (nfc.tagPresent()) {
        NdefMessage message = NdefMessage();
        Serial.println("Add text field: Hello, Larry!");
        message.addTextRecord("Hello, Arduino!");
        Serial.println("Add Uri field: http://arduino.cc");
        message.addUriRecord("http://arduino.cc");
        Serial.println("Add text field: Goodbye, Larry!");
        message.addTextRecord("Goodbye, Arduino!");
        boolean success = nfc.write(message);
        if (success) {
            Serial.println("Success. Try reading this tag with your phone.");
        } else {
            Serial.println("Write failed");
        }
    }
    delay(3000);
}
