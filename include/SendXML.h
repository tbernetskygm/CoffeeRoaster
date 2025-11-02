#include "Globals.h"
#ifndef NEW_WIFI
void SendXML();
#else
void SendXML(AsyncWebServerRequest *request);
#endif
