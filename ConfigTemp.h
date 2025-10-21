#ifndef CONFIG_TEMP_H
#define CONFIG_TEMP_H
#include<Arduino.h>
#include <ESP32Servo.h>
#ifndef NEW_WIFI
#include <WebServer.h>
#else
#include <ESPAsyncWebServer.h>
#endif
#include "Globals.h"


void ProcessConfigSteps();



//void IRAM_ATTR OnTempConfigTimer();
//void SetupConfigTimer();
//void ClearConfigTimer();

void SetupConfigTest();

#ifndef NEW_WIFI
void RunTempConfig (int Step);
void ProcessTempProbe();
void ProcessConfigMaxSteps();
void ProcessConfigStepTime();
void ProcessTempConfig();
void SendTempConfigData();
void SendRoastLogData();
void UpdateHeatGunSlider();
#else
void RunTempConfig (int Step);
void ProcessTempProbe(AsyncWebServerRequest *request);
void ProcessConfigMaxSteps(AsyncWebServerRequest *request);
void ProcessConfigStepTime(AsyncWebServerRequest *request);
void ProcessTempConfig(AsyncWebServerRequest *request);
void SendTempConfigData(AsyncWebServerRequest *request);
void SendRoastLogData(AsyncWebServerRequest *request);
void UpdateHeatGunSlider(AsyncWebServerRequest *request);
#endif

#endif
