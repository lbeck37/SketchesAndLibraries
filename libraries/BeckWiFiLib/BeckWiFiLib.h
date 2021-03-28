//BeckWiFiLib.h, 4/18/19a
#pragma once
#ifdef ESP8266
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif    //ESP8266

extern        bool     _bWiFiConnected;

void    SetupWiFi     ();
String  szWiFiStatus  (wl_status_t eWiFiStatus);

//Last line.
