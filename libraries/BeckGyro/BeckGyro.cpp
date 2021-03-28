//BeckGyro.cpp
#include <BeckGyro.h>
#include <BeckI2C.h>
#include <Wire.h>

//BeckAtoD class methods
BeckGyro::BeckGyro(BeckI2C* pBeckI2C) {
  String szLogString="BeckGyro Constructor: Begin";
  LogToSerial(szLogString);
  pBeckI2C_= pBeckI2C;
  bDevicePresent_= pBeckI2C_->bDevicePresent(eGyro);
  if (bDevicePresent_) {
		szLogString="BeckGyro Constructor: Gyro device is present";
		LogToSerial(szLogString);
  }
  else {
		szLogString="BeckGyro Constructor: Gyro device is NOT present";
		LogToSerial(szLogString);
  }
  SetupData();
  SetupI2C();
  return;
} //Constructor


void BeckGyro::Read(void) {
  INT16      asGyroReading[sNumGyroSensors_][sNumAxis_];
  String     szLogString;
   //boolean  bApplySmoothing= APPLY_SMOOTHING;

   if (millis() > ulNextGyroTime_) {
  		 pBeckI2C_->TestI2C();
  	if (bDevicePresent_) {
      Wire.beginTransmission(ucGyroAddress_);
      Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
      Wire.endTransmission(false);
      Wire.requestFrom((uint8_t)ucGyroAddress_, (size_t)14, (bool)true);  // request a total of 14 registers

      // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
      // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
      // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
      // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
      // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
      // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
      // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

      asGyroReading[eAccel][eXAxis]= Wire.read() << 8 | Wire.read();
      asGyroReading[eAccel][eYAxis]= Wire.read() << 8 | Wire.read();
      asGyroReading[eAccel][eZAxis]= Wire.read() << 8 | Wire.read();
      //szLogString="BeckGyro::Read_Gyro(): asGyroReading[eAccel][eZAxis]=";
      //LogToBoth(szLogString, asGyroReading[eAccel][eZAxis]);

      asGyroReading[eTemperature][eXAxis]= pBeckI2C_->sReadTwoBytes();

      asGyroReading[eRotation][eXAxis]=Wire.read() << 8 | Wire.read();
      asGyroReading[eRotation][eYAxis]=Wire.read() << 8 | Wire.read();
      asGyroReading[eRotation][eZAxis]=Wire.read() << 8 | Wire.read();

      //Initialize missing temperature fields.
      for (int sAxis= eYAxis; sAxis <= sNumAxis_; sAxis++) {
         asGyroReading[eTemperature][sAxis]= 0;
      }  //for

      //Apply low-pass filter to data
      for (int sSensorType= eAccel; sSensorType <= sNumGyroSensors_; sSensorType++) {
         for (int sAxis= eXAxis; sAxis <= sNumAxis_; sAxis++) {
#if APPLY_SMOOTHING
            asGyro_[sSensorType][sAxis]= FILTER_FUNC(asGyroReading[sSensorType][sAxis],
                                                  pusSmoothingMemory[sSensorType][sAxis]);
#else
            asGyro_[sSensorType][sAxis]= asGyroReading[sSensorType][sAxis];
#endif
         }  //for sAxis
      }  //for sSensorType

      //The following is for bringing up gyro
      szLogString="BeckGyro::Read_Gyro(): AccelZ";
      LogToBoth(szLogString, asGyro_[eAccel][eZAxis]);

      bGyroChanged_= true;
  	}	//if(bDevicePresent_)
  	else {
			String szLogString="BeckGyro::Read(): I2C Gyro not present";
			LogToSerial(szLogString);
  	}
    ulNextGyroTime_= millis() + ulGyroReadTime_;
   }  //if (millis()>ulNextGyroTime)
   return;
} //Read


void BeckGyro::SetupI2C(void) {
  //BLog("BeckGyro::SetupI2C: Begin");
  String szLogString;
	pBeckI2C_->TestI2C();

  szLogString="BeckGyro::SetupI2C(): Calling Wire.beginTransmission";
  LogToBoth(szLogString, ucGyroAddress_);
  Wire.beginTransmission(ucGyroAddress_);

  szLogString="BeckGyro::SetupI2C(): Call Wire.write(0x6B) & Wire.write(0)";
  LogToBoth(szLogString);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)

  szLogString="BeckGyro::SetupI2C(): Calling Wire.endTransmission(true)";
  LogToBoth(szLogString);
  Wire.endTransmission(true);

	pBeckI2C_->TestI2C();

  szLogString="BeckGyro::SetupI2C(): End";
  LogToBoth(szLogString);
  return;
} //SetupI2C


void BeckGyro::SetupData(void) {
  //Initialize the data array.
  String szLogString="BeckGyro:SetupData(): Begin";
  LogToBoth(szLogString);
  for (int sSensorType= eAccel; sSensorType <= sNumGyroSensors_; sSensorType++) {
     for (int sAxis= eXAxis; sAxis <= sNumAxis_; sAxis++) {
        asGyro_[sSensorType][sAxis]= 37;
     }  //for sSensorType
  }  //for sAxis
  return;
} //SetupData
//Last line.
