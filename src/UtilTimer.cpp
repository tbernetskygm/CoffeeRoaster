
#include "Globals.h"
#include "UtilTimer.h"
#include "UtilityFunctions.h"

// ISR for TempConfig Timer
void IRAM_ATTR onUtilTimer() {
  
   // Serial.print("OnTempConfigTimer() ConfigStep = "); Serial.println(ConfigStep);
   if (rState->doConfig)
   {
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
      ClearUtilTimer();
      rState->doConfig=false;
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
  else if (rState->preheat)
  {
    if (PreheatTimerValue >0)
      PreheatTimerValue --;
       xSemaphoreGiveFromISR(utilTimerSemaphore, NULL);
  }
}

/*
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
  xSemaphoreGiveFromISR(utilTimerSemaphore, NULL);
}
*/
bool InitUtilTimer()
{
  //UtilTimer = timerBegin(1, 80, true);
  timerAttachInterrupt(UtilTimer, &onUtilTimer,true);
  Serial.println("InitConfigTimer() done ");
  return true;
}

void SetupUtilTimer()
{
  Serial.print("SetupUtilTimer() ") ; Serial.println((int64_t)UtilTimer);
  
  //timerDetachInterrupt(UtilTimer); // this causes exception
 // Setup for timer timer 1 prescaler 80 count up (true)
  UtilTimer = timerBegin(1, 80, true);
  Serial.print("SetupUtilTimer() ") ; Serial.println((int64_t)UtilTimer);
  // new timer API set to 1Mhz
  //UtilTimer = timerBegin(1000000);
  
  // Attach to interrupt handler
  if (!rState->utilTimerIntSetup)
    rState->utilTimerIntSetup= InitUtilTimer(); // make sure we have a new timer each time
  printState();
    //timerDetachInterrupt(UtilTimer);
  //timerAttachInterrupt(UtilTimer, &onUtilTimer,true);
  //Specify timer timeout value
  // 1000000 microseconds = 1 second
  //timerWrite(UtilTimer, 1000000);
  // At this point the timer is running
  // I think doing timerAlarmEnable lets it start
  // sending interrupts
  timerAlarmWrite(UtilTimer,1000000,true);
  timerAlarmEnable(UtilTimer); // orig way
  Serial.println("SetupUtilTimer() done ");
  //Serial.print("SetupUtilTimer() ") ; Serial.println((int64_t)UtilTimer);
}



void ClearUtilTimer()
{
  Serial.print("ClearUtilTimer() ") ; Serial.println((int64_t)UtilTimer);
  if (UtilTimer)
  {
    if(timerStarted(UtilTimer))
    {
      Serial.println("b4 timerStop(UtilTimer)");
      timerStop(UtilTimer);
      Serial.print("ClearUtilTimer() timerstop ") ; Serial.println((int64_t)UtilTimer);
    }
    timerEnd(UtilTimer);
    Serial.print("ClearUtilTimer() timerEnd ") ; Serial.println((int64_t)UtilTimer);
    Serial.println("b4 alarmDisable");
    //timerAlarmDisable(UtilTimer);
    //Serial.println("b4 DetachInterrrupt");
    //timerDetachInterrupt(UtilTimer);
    //Serial.println("after DetachInterrrupt");
    
    //timerAlarmDisable(UtilTimer); // orig way
    // new way timerAlarm(UtilTimer,0,false,0);
    // Detach to interrupt handler
    UtilTimer=NULL; // no more timer
    
  }
  Serial.println("ClearUtilTimer() returning");
  Serial.print("ClearUtilTimer() ") ; Serial.println((int64_t)UtilTimer);
}

