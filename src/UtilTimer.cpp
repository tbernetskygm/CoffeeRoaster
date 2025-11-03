
#include "Globals.h"
#include "UtilTimer.h"
#include "UtilityFunctions.h"
// Callback for Ticker Timer
// just decrement the value and send semaphore
void onUtilTimerT(int * val) {
  //Serial.printf("OnUtilTimerT val = %d or %d \n", val, &val);
  val--;
  UtilTimerValue--;
  xSemaphoreGiveFromISR(utilTimerTSemaphore, NULL);
}


void SetupUtilTimerT()
{
  // Using Ticker timer so things don't crash
  // should be able to just re-attach to call back?
  UtilTimerT.attach_ms(1000,onUtilTimerT,&UtilTimerValue);
}



void ClearUtilTimerT()
{
  // just have to detach
  UtilTimerT.detach();
  Serial.printf("ClearUtilTimerT detached.\n");
}


