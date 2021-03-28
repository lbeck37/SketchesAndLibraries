// BeckAlexaLib.h 12/27/19b
#pragma once

#include <BeckBiotaLib.h>

extern int            wAlexaHandleCount;
extern bool           _bAlexaChanged;

void SetupAlexa             (char acAlexaName[]);
void HandleAlexa            ();
void DoAlexaCommand         (unsigned char ucDdeviceID, const char* szDeviceName,
                             bool bState, unsigned char ucValue);
//Last line.
