#ifndef CONFIG_TEMP_H
#define CONFIG_TEMP_H
#include<Arduino.h>
#include <ESP32Servo.h>
#include <WebServer.h>
#include "Globals.h"

void RunTempConfig (int Step);
void ProcessTempProbe();
void ProcessConfigSteps();
void ProcessConfigMaxSteps();
void ProcessTempConfig();
void ProcessConfigStepTime();
//void IRAM_ATTR OnTempConfigTimer();
//void SetupConfigTimer();
//void ClearConfigTimer();
void SendTempConfigData();
void SetupConfigTest();
void UpdateHeatGunSlider();
void SendRoastLogData();

#endif
