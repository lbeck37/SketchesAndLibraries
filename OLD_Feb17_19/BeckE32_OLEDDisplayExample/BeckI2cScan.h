// BeckI2cScan.h
void ScanForDevices(void){
  byte ucError, ucAddress;
  int nDevices;
  Serial.println("ScanForDevices(): Begin");

  Serial.println("Scanning...");
  nDevices = 0;
  for(ucAddress = 1; ucAddress < 127; ucAddress++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(ucAddress);
    ucError = Wire.endTransmission();

    if (ucError == 0)
    {
      //SetDevicePresent(ucAddress);
      Serial.print("I2C device found at address 0x");
      if (ucAddress<16) {
        Serial.print("0");
      }
      Serial.print(ucAddress,HEX);
      Serial.println(" !");
      nDevices++;
    }
    else if (ucError==4)
    {
      Serial.print("Unknown error at address 0x");
      if (ucAddress<16) {
        Serial.print("0");
      }
      Serial.println(ucAddress,HEX);
    }
  }
 if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("Scan done");
  return;
} //ScanForDevices
