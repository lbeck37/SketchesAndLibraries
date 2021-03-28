//BeckOTAWebServerLib.h, Beck 4/10/19b
#pragma once

extern bool                _bOTA_Started;   //Turns off Blynk.
extern unsigned long       _ulUpdateTimeoutMsec;

void StartOTAWebServer  (void);
void HandleOTAServer    (void);
//Last line.
