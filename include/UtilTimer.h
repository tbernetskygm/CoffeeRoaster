
#ifndef UTIL_TIMER_H
#define UTIL_TIMER_H
#include<Arduino.h>
#include "Globals.h"
#include <Ticker.h>

//void IRAM_ATTR onUtilTimer();
void onUtilTimerT(int *val);
//void IRAM_ATTR onPreHeatTimer();
//bool InitUtilTimer();
//void SetupUtilTimer();
void SetupUtilTimerT();
//void ClearConfigTimer();
//void ClearUtilTimer();
void ClearUtilTimerT();
//void SetupPreheatTimer();
#endif
