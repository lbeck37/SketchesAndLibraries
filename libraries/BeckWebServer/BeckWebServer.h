//BeckWebServer.h, Beck 4/18/19a
#pragma once

#ifdef ESP8266
  #include <ESP8266WebServer.h>
  extern ESP8266WebServer    oWebServer;
#else   //ESP32
  #include <WebServer.h>
  extern WebServer           oWebServer;
#endif    //ESP8266

void StartWebServer     (const char *acHostname);
void HandleWebServer    ();
//Last line.
