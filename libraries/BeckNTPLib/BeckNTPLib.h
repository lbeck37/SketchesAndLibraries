//BeckNTPLib.h, Beck 3/5/19a
#pragma once
#include <NTPClient.h>

extern NTPClient    oNTPClient;

void    SetupNTP        (void);
void    HandleNTPUpdate (void);
//Last line.
