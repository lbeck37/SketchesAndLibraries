// Beck_IMUQuaternionFilters.h, Beck 3/2/19a
#pragma once
#include <Arduino.h>

#define Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#define Ki 0.0f

extern float      q[4];
extern float      GyroMeasError;
extern float      GyroMeasDrift;
extern float      beta;   // compute beta
extern float      zeta;   // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value
//extern uint32_t   delt_t; // used to control display output rate
extern float      fDeltaT;
extern float      q[4];

void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);

//Last line.
