
#include "Globals.h"
#include "UtilTimer.h"
// ISR for TempConfig Timer
void IRAM_ATTR OnTempConfigTimer() {
  
   // Serial.print("OnTempConfigTimer() ConfigStep = "); Serial.println(ConfigStep);
  if (ConfigTimerValue == 0)
  {
    //Serial.print("OnTempConfigTimer() Finished Step ConfigTimerValue = "); Serial.println(ConfigTimerValue);
    //ConfigStep++;
    //Serial.print("OnTempConfigTimer() ConfigStep = "); Serial.println(ConfigStep);
    if (ConfigStep <= ConfigMaxSteps-1)
    {
      //Serial.println("OnTempConfigTimer() Next Step "); 
      // Give a semaphore that we can check in the loop
      xSemaphoreGiveFromISR(utilTimerSemaphore, NULL);
      //RunTempConfig(ConfigStep);
    }
    else
    {
      //End of config
       // Give a semaphore for last step
      xSemaphoreGiveFromISR(utilTimerSemaphore, NULL);
      ClearConfigTimer();
    }
    // Next step
  } else {
//    Serial.println("OnTempConfigTimer() Collecting Temps = "); 
    // Collect Temp values
    //tempTotalC+=tempC;
    //tempTotalF+=tempF;
    xSemaphoreGiveFromISR(utilTimerSemaphore, NULL);
    ConfigTimerValue--;
  }
}

// ISR for PreHeat Timer
void IRAM_ATTR onPreHeatTimer() {
  
  if (PreheatTimerValue == 0)
  {
    //Serial.printf("onPreHeatTimer() Finished Step PreHeatTimerValue = %d\n",PreheatTimerValue);
      // Give a semaphore that we can check in the loop
        // Next step
  } else {
    //Serial.printf("onPreHeatTimer() %d \n",PreheatTimerValue);
    
    PreheatTimerValue--;
  }
  // this is handled in UpdateRoastState
  // that has logic to update the roast log and handle the timers stuff
  xSemaphoreGiveFromISR(roastTimerSemaphore, NULL);
}

void SetupConfigTimer()
{
  //timerDetachInterrupt(UtilTimer); // this causes exception
 // Setup for timer timer 1 prescaler 80 count up (true)
  //UtilTimer = timerBegin(1, 80, true);
  // new timer API set to 1Mhz
  UtilTimer = timerBegin(1000000);
  // Attach to interrupt handler
  timerAttachInterrupt(UtilTimer, &OnTempConfigTimer);
  //Specify timer timeout value
  // 1000000 microseconds = 1 second
  //timerWrite(UtilTimer, 1000000);
  //Serial.println("setupConfigTimer() ");
}


void ClearConfigTimer()
{
  //Serial.println("ClearConfigTimer() ");
  if (UtilTimer)
  {
    timerStop(UtilTimer);
    timerAlarm(UtilTimer,0,false,0);
    // Detach to interrupt handler
    timerDetachInterrupt(UtilTimer);
  }
  TEMP_CONFIG = false;
  //Serial.println("ClearConfigTimer() ");
}

void ClearUtilTimer()
{
  //Serial.println("ClearUtilTimer() ");
  if (UtilTimer)
  {
    timerStop(UtilTimer);
    timerAlarm(UtilTimer,0,false,0);
    // Detach to interrupt handler
    timerDetachInterrupt(UtilTimer);
  }
  //Serial.println("ClearUtilTimer() ");
}


void SetupPreheatTimer()
{
  //timerDetachInterrupt(UtilTimer); // this causes exception
 // Setup for timer timer 1 prescaler 80 count up (true)
  UtilTimer = timerBegin(1000000);
  // Attach to interrupt handler
  timerAttachInterrupt(UtilTimer, &onPreHeatTimer);
  //Specify timer timeout value
  // 1000000 microseconds = 1 second
  //timerAlarmWrite(UtilTimer, 1000000, true);
  //Serial.println("setupPreheatTimer() ");
}



