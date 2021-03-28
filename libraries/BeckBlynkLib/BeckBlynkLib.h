//BeckBlynkLib.h, 1/26/19
#pragma once

void BlynkLogLine   (String szString);
void BlynkLogLine   (String szString, String szLogValue);
void BlynkLogLine   (String szString, int sValue);
void BlynkLogLine   (String szString, float fValue);
void SendIntToBlynk (int sVirtualPin, int sValue){
//Last line.
