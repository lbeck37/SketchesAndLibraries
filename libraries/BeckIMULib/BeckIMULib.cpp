//BeckIMULib.cpp 2/18/19b
#include <BeckIMULib.h>
#include <BeckDisplayLib.h>
#include <BeckLogLib.h>
#include <BeckMiniLib.h>
#include <BeckIMU_Data.h>
#include <Wire.h>
#include <Streaming.h>

const int       _sXAxis             = 0;
const int       _sYAxis             = 1;
const int       _sZAxis             = 2;
const int       _sNumAxis           = 3;
const double    _dGConvert          = 16383.0;  // 12/06/17 I'm seeing values up to -16,580 with a vertical orientation, 0x3FFF = 16,383
const double    _dRadsToDeg         = 180.0/PI;

//From PowershiftE32_Rover.ino
const int       _sAccel             = 0;
const int       _sNumGyroTypes      = 3;

int             _asGyro[_sNumGyroTypes][_sNumAxis];

boolean         _bGyroChanged             = true;
boolean         _bPitchChanged            = true;
boolean         _bBoostChanged            = true;

double          _adGvalueXYZ[3];
double          _dRollDeg;
double          _dPitchDeg;
double          _dPitchPercent   = 0.0;

//Local prototypes
void    ComputePitchAndRoll   ();
double  dGetPitchPercent      (double dPitchDeg);


void SetupIMU() {
   Serial << LOG0 << "SetupIMU(): Begin"<< endl;
   Wire.beginTransmission(wMPU6050);
   Serial << LOG0 << "SetupIMU(): Call MPU6050_PrintName()"<< endl;
   MPU6050_PrintName();

   // Clear the 'sleep' bit to start the sensor.
   MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);
   Wire.endTransmission(true);

   //Initialize the data array.
   for (int sDataType= _sAccel; sDataType < _sNumGyroTypes; sDataType++) {
      for (int sAxis= _sXAxis; sAxis < _sNumAxis; sAxis++) {
         _asGyro[sDataType][sAxis]= 0;
      }  //for sDataType
   }  //for sAxis
   return;
}  //SetupIMU


void HandleIMU() {
  MPU6050_ReadGs(_adGvalueXYZ, _dGConvert);
  //Biota Box Y axis is backwards
  _adGvalueXYZ[_sYAxis]= -_adGvalueXYZ[_sYAxis];
  Serial << LOG0 << "HandleIMU(): G's X, Y, Z " << _adGvalueXYZ[_sXAxis] << ", " << _adGvalueXYZ[_sYAxis] << ", " << _adGvalueXYZ[_sZAxis] << endl;

  ComputePitchAndRoll();
  _bGyroChanged= true;
  return;
}  //HandleIMU


void ComputePitchAndRoll() {
  _dRollDeg = atan2((-_adGvalueXYZ[_sYAxis]), _adGvalueXYZ[_sZAxis]) * _dRadsToDeg; //Probably has axis mixed up
/*
  _dPitchDeg= atan2(_adGvalueXYZ[_sXAxis],
                sqrt(_adGvalueXYZ[_sYAxis] * _adGvalueXYZ[_sYAxis] +
                     _adGvalueXYZ[_sZAxis] * _adGvalueXYZ[_sZAxis])) * _dRadsToDeg;
*/
  _dPitchDeg= atan2(-_adGvalueXYZ[_sYAxis],
                   sqrt(_adGvalueXYZ[_sXAxis] * _adGvalueXYZ[_sXAxis] +
                        _adGvalueXYZ[_sZAxis] * _adGvalueXYZ[_sZAxis])) * _dRadsToDeg;
  //Correct for Biota Box
  //_dPitchDeg= _dPitchDeg - 90.0;

  _dPitchPercent= dGetPitchPercent(_dPitchDeg);
  //Correct for current readings being 180 degrees off
  if (_dRollDeg < 0.0) {
    _dRollDeg= -180.0 - _dRollDeg;
  } //if(dRollDeg_<0.0)
  else {
    _dRollDeg= 180.0 - _dRollDeg;
  } //if(dRollDeg_<0.0)else
  _bPitchChanged= true;
  Serial << LOG0 << "ComputePitchAndRoll(): Pitch Deg, Pitch%, Roll " << _dPitchDeg << ", " << _dPitchPercent << ", " << _dRollDeg<< endl;
  UpdateIMUDisplay();
  return;
} //ComputePitchAndRoll


double dGetPitchPercent(double dPitchDeg) {
  double dPitchPercent= -99.99;
  //Serial << LOG0 << "dGetPitchPercent(): dPitchDeg_= " << dPitchDeg_ << endl;
  if ((dPitchDeg < 44.0) && (dPitchDeg > -44.0)) {
    dPitchPercent= 100.0 * tan(dPitchDeg / _dRadsToDeg);
    //Serial << LOG0 << "dGetPitchPercent(): dPitchPercent_= " << dPitchPercent_ << endl;
  } //if((dPitchDeg_<44.0)&&...
  return dPitchPercent;
} //dGetPitchPercent


void MPU6050_PrintName(){
  int wError;
  uint8_t ucResult;

  wError = MPU6050_read(MPU6050_WHO_AM_I, &ucResult, 1);
  Serial << LOG0 << "MPU6050_PrintName(): WHO_AM_I : " << ucResult << ", wError = " << wError << endl;

  // According to the datasheet, the 'sleep' bit
  // should read a '1'.
  // That bit has to be cleared, since the sensor
  // is in sleep mode at power-up.
  wError = MPU6050_read (MPU6050_PWR_MGMT_1, &ucResult, 1);
  Serial << LOG0 << "MPU6050_PrintName(): PWR_MGMT_1 : " << ucResult << ", wError = " << wError << endl;
	return;
}	//MPU6050_PrintName


void MPU6050_ReadGs(double adG_XYZ[], double dGConvert)
{
  int error;
  double dT;
  accel_t_gyro_union accel_t_gyro;

  //Serial.println(F(""));
  //Serial.println(F("MPU6050_ReadGs"));

  // Read the raw values.
  // Read 14 bytes at once,
  // containing acceleration, temperature and gyro.
  // With the default settings of the MPU-6050,
  // there is no filter enabled, and the values
  // are not very stable.
  error = MPU6050_read (MPU6050_ACCEL_XOUT_H, (uint8_t *) &accel_t_gyro, sizeof(accel_t_gyro));
  //Serial.print(F("Read accel, temp and gyro, error = "));
  //Serial.println(error,DEC);

  // Swap all high and low bytes.
  // After this, the registers values are swapped,
  // so the structure name like x_accel_l does no
  // longer contain the lower byte.
  uint8_t swap;
  #define SWAP(x,y) swap = x; x = y; y = swap

  SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
  SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
  SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
  SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
  SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
  SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
  SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);

  adG_XYZ[_sXAxis]= (double)accel_t_gyro.value.x_accel / dGConvert;
  adG_XYZ[_sYAxis]= (double)accel_t_gyro.value.y_accel / dGConvert;
  adG_XYZ[_sZAxis]= (double)accel_t_gyro.value.z_accel / dGConvert;

  return;
}	//MPU6050_ReadGs


void MPU6050_ReadTest(void)
{
  //int error;
  double dT;
  accel_t_gyro_union accel_t_gyro;

  Serial.println(F(""));
  Serial.println(F("MPU-6050"));

  /*
  // Read the raw values.
  // Read 14 bytes at once,
  // containing acceleration, temperature and gyro.
  // With the default settings of the MPU-6050,
  // there is no filter enabled, and the values
  // are not very stable.
  error = MPU6050_read (MPU6050_ACCEL_XOUT_H, (uint8_t *) &accel_t_gyro, sizeof(accel_t_gyro));
  Serial.print(F("Read accel, temp and gyro, error = "));
  Serial.println(error,DEC);

  // Swap all high and low bytes.
  // After this, the registers values are swapped,
  // so the structure name like x_accel_l does no
  // longer contain the lower byte.
  uint8_t swap;
  #define SWAP(x,y) swap = x; x = y; y = swap

  SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
  SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
  SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
  SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
  SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
  SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
  SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);
*/

  // Print the raw acceleration values
  Serial.print(F("accel x,y,z: "));
  Serial.print(accel_t_gyro.value.x_accel, DEC);
  Serial.print(F(", "));
  Serial.print(accel_t_gyro.value.y_accel, DEC);
  Serial.print(F(", "));
  Serial.print(accel_t_gyro.value.z_accel, DEC);
  Serial.println(F(""));


  // The temperature sensor is -40 to +85 degrees Celsius.
  // It is a signed integer.
  // According to the datasheet:
  //   340 per degrees Celsius, -512 at 35 degrees.
  // At 0 degrees: -512 - (340 * 35) = -12412

  Serial.print(F("temperature: "));
  dT = ( (double) accel_t_gyro.value.temperature + 12412.0) / 340.0;
  Serial.print(dT, 3);
  Serial.print(F(" degrees Celsius"));
  Serial.println(F(""));

  // Print the raw gyro values.
  Serial.print(F("gyro x,y,z : "));
  Serial.print(accel_t_gyro.value.x_gyro, DEC);
  Serial.print(F(", "));
  Serial.print(accel_t_gyro.value.y_gyro, DEC);
  Serial.print(F(", "));
  Serial.print(accel_t_gyro.value.z_gyro, DEC);
  Serial.print(F(", "));
  Serial.println(F(""));
  return;
}	//MPU6050_ReadTest


// --------------------------------------------------------
// MPU6050_read
//
// This is a common function to read multiple bytes
// from an I2C device.
//
// It uses the boolean parameter for Wire.endTransMission()
// to be able to hold or release the I2C-bus.
// This is implemented in Arduino 1.0.1.
//
// Only this function is used to read.
// There is no function for a single byte.
//
int MPU6050_read(int start, uint8_t *buffer, int size)
{
  int i, n;

  //Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  Wire.beginTransmission(wMPU6050);
  n = Wire.write(start);
  if (n != 1)
    return (-10);

  n = Wire.endTransmission(false);    // hold the I2C-bus
  if (n != 0)
    return (n);

  // Third parameter is true: relase I2C-bus after data is read.
  //Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  Wire.requestFrom(wMPU6050, size, true);
  i = 0;
  while(Wire.available() && i<size)
  {
    buffer[i++]=Wire.read();
  }
  if ( i != size)
    return (-11);

  return (0);  // return : no error
}	//MPU6050_read


// --------------------------------------------------------
// MPU6050_write
//
// This is a common function to write multiple bytes to an I2C device.
//
// If only a single register is written,
// use the function MPU_6050_write_reg().
//
// Parameters:
//   start : Start address, use a define for the register
//   pData : A pointer to the data to write.
//   size  : The number of bytes to write.
//
// If only a single register is written, a pointer
// to the data has to be used, and the size is
// a single byte:
//   int data = 0;        // the data to write
//   MPU6050_write (MPU6050_PWR_MGMT_1, &c, 1);
//
int MPU6050_write(int start, const uint8_t *pData, int size)
{
  int n, wError;

  //Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  Wire.beginTransmission(wMPU6050);
  n = Wire.write(start);        // write the start address
  if (n != 1)
    return (-20);

  n = Wire.write(pData, size);  // write data bytes
  if (n != size)
    return (-21);

  wError = Wire.endTransmission(true); // release the I2C-bus
  if (wError != 0)
    return (wError);

  return (0);         // return : no error
}	//MPU6050_write


// --------------------------------------------------------
// MPU6050_write_reg
//
// An extra function to write a single register.
// It is just a wrapper around the MPU_6050_write()
// function, and it is only a convenient function
// to make it easier to write a single register.
//
int MPU6050_write_reg(int reg, uint8_t data)
{
  int wError;

  wError = MPU6050_write(reg, &data, 1);
  return (wError);
}	//MPU6050_write_reg
//Last line.
