
#ifndef UTIL_TIMER_H
#define UTIL_TIMER_H
#include<Arduino.h>
#include "Globals.h"

void IRAM_ATTR OnTempConfigTimer();
void IRAM_ATTR onPreHeatTimer();
void SetupConfigTimer();
void ClearConfigTimer();
void ClearUtilTimer();
void SetupPreheatTimer();
#endif
