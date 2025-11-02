
#ifndef UTIL_TIMER_H
#define UTIL_TIMER_H
#include<Arduino.h>
#include "Globals.h"

void IRAM_ATTR onUtilTimer();
//void IRAM_ATTR onPreHeatTimer();
bool InitUtilTimer();
void SetupUtilTimer();
//void ClearConfigTimer();
void ClearUtilTimer();
//void SetupPreheatTimer();
#endif
