//BeckIMULib.h 2/18/19
#pragma once
#include <Streaming.h>

//IMU defines
extern const int       _sXAxis;
extern const int       _sYAxis;
extern const int       _sZAxis;
extern const int       _sNumAxis;

//From PowershiftE32_Rover.ino
extern const int       _sAccel;
extern const int       _sNumGyroTypes;

extern boolean        _bGyroChanged;
extern boolean        _bPitchChanged;
extern boolean        _bBoostChanged;

extern const double   _dGConvert;
extern const double   _dRadsToDeg;
extern double         _adGvalueXYZ[];
extern double         _dRollDeg;
extern double         _dPitchDeg;
extern double         _dPitchPercent;


// Default I2C address for the MPU-6050 is 0x68.
// But only if the AD0 pin is low.
// Some sensor boards have AD0 high, and the
// I2C address thus becomes 0x69.
const int wMPU6050= 0x68;  // I2C address of the MPU-6050 gyro

//Function prototypes
void    SetupIMU              ();
void    HandleIMU             ();
//Last line.
