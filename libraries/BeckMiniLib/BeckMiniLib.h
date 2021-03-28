//BeckMiniLib.h, 3/3/19a
#pragma once
#include <BeckLogLib.h>
#include <Streaming.h>

#define INT8        int8_t
#define UINT8       uint8_t
#define INT16       int16_t
#define UINT16      uint16_t
#define INT32       int32_t
#define UINT32      uint32_t
#define INT64       int64_t
#define UINT64      uint64_t

extern const long     lMsecPerSec;
extern const long     lMsecPerMin;
extern const long     lMsecPerHour;
extern const long     lMsecPerDay;
extern const long     lSerialMonitorBaud;
extern unsigned long  ulLastTaskMsec;

void      CheckTaskTime         (String szTask);
void      ClearTaskTime2        (unsigned long* pulLastTaskMsec);
void      CheckTaskTime2        (String szTask, unsigned long* pulLastTaskMsec= NULL);
//Last line.
