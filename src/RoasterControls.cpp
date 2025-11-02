// This has code to handle Buttons and User interface for main roaster page
#include "ProjectDefines.h"
#include "Globals.h"
#include "RoasterControls.h"
#include "UtilTimer.h"
#include "FileSystemFunctions.h"
#include "UtilityFunctions.h"
#include "ConfigTemp.h"
// ISR for RoastTimer
void IRAM_ATTR onRoastTimer() {
  //Serial.printf("onRoastTimer() TimerValue = %d\n",TimerValue);
  if (TimerValue <= 0)
  {
    Serial.println("onRoastTimer() Timer Stop!!! ");
    timerStop(RoastTimer);
    //new way timerAlarm(RoastTimer, 0 , false,0);
    timerAlarmDisable(RoastTimer);
  }
  else
  {
    TimerValue--;
  }
  xSemaphoreGiveFromISR(roastTimerSemaphore, NULL);
}


// This should start the whole process
// turn on the mixer power
// turn on the heater
// start the roasting timer
#ifndef NEW_WIFI
void ProcessButtonRoastStart() {
#else
void ProcessButtonRoastStart(AsyncWebServerRequest *request) {
#endif
  char buff[256];
  ROAST = !ROAST;
  rState->doRoast=!rState->doRoast;
  Serial.printf("ProcessButtonRoastStart doRoast %d ROAST= %d MIXPWR %d HEATERPWR %d TIMERSTART %d PREHEAT_TIMERSTART %d\n",
		  rState->doRoast,rState->roast,rState->mixerpwr,rState->heaterpwr,rState->timerStart,rState->preheatTimerStart);

  // if config data not loaded,get it
  if (!CONFIG_DATA_LOADED)
    readConfigData();
  if (rState->doRoast)
  {
    //Set servo positions
    PreheatServoPos=getServoPos(PreheatTemp);
    FinishServoPos=getServoPos(FinishTemp);
    if (FinishServoPos == 0) // Temp was out of range set default
      FinishServoPos=160;
    // start logging data
    SetupRoastingLog();
    // call ProcessButtonMixPwr() to start mixer
    if (!rState->mixerpwr)
      ProcessButtonMixPwr();
       // if the preheat timer value is set do the preheat first
    Serial.printf("ProcessButtonRoastStart PreheatTimerValue %d PreheatTimerStartValue %d\n ",
		    PreheatTimerValue,PreheatTimerStartValue);
    if(PreheatTimerStartValue > PreheatTimerValue)
	    PreheatTimerValue=PreheatTimerStartValue;

    // If there is a preheat timer value then start preheat
    if (PreheatTimerValue > 0 && !rState->preheatTimerStart )
      ProcessPreheatTimerStart();
    else {
     // No preheat just roast 
      // Position servo to proper temp
      servoPosNew=FinishServoPos;
	// set tempSamples to 5
        tempSamples=5;
      //don't forget to turn heater on!!
      //if (HEATERPWR)
      if (rState->heaterpwr)
	ProcessButtonHeaterPwr();
      // call ProcessButtonTimerStart() to start the timer
      if (!rState->timerStart)
      {
        ProcessButtonTimerStart();
      }
    }
  }

  else //Stop
  {
    // call ProcessButtonMixPwr() to stop mixer 
    //if (MIXPWR)
    if (rState->mixerpwr)
      ProcessButtonMixPwr();
    // call ProcessButtonHeaterPwr() to turn off heater
    //if (!HEATERPWR)
    if (!rState->heaterpwr)
    {
      ProcessButtonHeaterPwr();
      // Reset servo position
      servoPosNew=0;
    }
    // If stop is pressed update log set last and stop to true.
    Serial.printf("Stop after update roasting log\n");
    // call ProcessButtonTimerStart() to stop the timer
    if (rState->timerStart)
    {
      ProcessButtonTimerStart();
    }
    CloseRoastingLog(true);
    // reset variables
    TimerAdjust=false;
    TimerMin0New =0;
    TimerAdjustMin =0;
  }
#ifndef NEW_WIFI
  Server.send(200, "text/plain", "ButtonRoastStart"); //Send web page
  #else
  request->send(200,"ButtonRoastStart","text/plain");
  #endif
}

#ifndef NEW_WIFI
void ProcessStartPreHeat()
#else
void ProcessStartPreHeat(AsyncWebServerRequest *request)
#endif
{
  Serial.println("ProcessStartPreHeat() call SetupPreheatTimer ");
  SetupUtilTimer();
  Serial.println("ProcessStartPreHeat() Done ");
}

#ifndef NEW_WIFI
void ProcessMinButton_0() {
  String t_state = Server.arg("VALUE");
#else
void ProcessMinButton_0(AsyncWebServerRequest *request) {
  String t_state ;
  char buf[64];
  char * buf_p = &buf[0];
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
#endif
  TimerMin0New = t_state.toInt();
  //Serial.print("ProcessMinButton0 TimerMin0 "); Serial.println(TimerMin0);
  //Serial.print("ProcessMinButton0 TimerMin0New: "); Serial.println(TimerMin0New);
  sprintf(buf, "%d", TimerMin0New);
#ifndef NEW_WIFI
  Server.send(200, "text/plain", buf); //Send web page
  #else
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", String(buf));
  request->send(resp);
  #endif
}

#ifndef NEW_WIFI
void ProcessSecButton_0() {
  String t_state = Server.arg("VALUE");
#else
void ProcessSecButton_0(AsyncWebServerRequest *request) {
  String t_state ;
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
#endif
  char buf[64];
  char * buf_p = &buf[0];
  TimerSec0New = t_state.toInt();
  //Serial.print("ProcessSecButton0 TimerSec0 "); Serial.println(TimerSec0);
  //Serial.print("ProcessSecButton0 TimerSec0New "); Serial.println(TimerSec0New);
  sprintf(buf, "%d", TimerSec0New);
#ifndef NEW_WIFI
  Server.send(200, "text/plain", buf); //Send web page
  #else
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", String(buf));
  request->send(resp);
  #endif
}


#ifndef NEW_WIFI
void ProcessPreMinButton_0() {
  String t_state = Server.arg("VALUE");
#else
void ProcessPreMinButton_0(AsyncWebServerRequest *request) {
  String t_state ;
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
#endif
  char buf[64];
  char * buf_p = &buf[0];
  PreTimerMinNew = t_state.toInt();
  //Serial.print("ProcessPreMinButton0 PreTimerMin "); Serial.println(PreTimerMin);
  //Serial.print("ProcessPreMinButton0 PreTimerMinNew: "); Serial.println(PreTimerMinNew);
  sprintf(buf, "%d", PreTimerMinNew);
#ifndef NEW_WIFI
  Server.send(200, "text/plain", buf); //Send web page
  #else
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", String(buf));
  request->send(resp);
  #endif
}

#ifndef NEW_WIFI
void ProcessPreSecButton_0() {
  String t_state = Server.arg("VALUE");
#else
void ProcessPreSecButton_0(AsyncWebServerRequest *request) {
  String t_state ;
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
#endif
  char buf[64];
  char * buf_p = &buf[0];
  PreTimerSecNew = t_state.toInt();
  Serial.print("ProcessPreSecButton0 PreTimerSec "); Serial.println(PreTimerSec);
  Serial.print("ProcessPreSecButton0 PreTimerSecNew "); Serial.println(PreTimerSecNew);

  //Serial.print("ProcessSecButton0 TimerSec0New "); Serial.println(TimerSec0New);
  sprintf(buf, "%d", PreTimerSecNew);
#ifndef NEW_WIFI
  Server.send(200, "text/plain", buf); //Send web page
  #else
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", String(buf));
  request->send(resp);
  #endif
}

#ifndef NEW_WIFI
void ProcessPreTempButton() {
  String t_state = Server.arg("VALUE");
#else
void ProcessPreTempButton(AsyncWebServerRequest *request) {
  String t_state ;
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
#endif
  char buf[64];
  char * buf_p = &buf[0];
  PreheatTemp = t_state.toInt();
  Serial.print("ProcessPreTempButton PreheatTemp "); Serial.println(PreheatTemp);
  //Serial.print("ProcessSecButton0 TimerSec0New "); Serial.println(TimerSec0New);
  sprintf(buf, "%d", PreheatTemp);
#ifndef NEW_WIFI
  Server.send(200, "text/plain", buf); //Send web page
  #else
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", String(buf));
  request->send(resp);
  #endif
  // if config data not loaded,get it
  if (!CONFIG_DATA_LOADED)
    readConfigData();
  
  PreheatServoPos=getServoPos(PreheatTemp);
}


#ifndef NEW_WIFI
void ProcessFinalTempButton() {
  String t_state = Server.arg("VALUE");
#else
void ProcessFinalTempButton(AsyncWebServerRequest *request) {
  String t_state ;
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
#endif
  char buf[64];
  char * buf_p = &buf[0];
  FinishTemp = t_state.toInt();
  Serial.print("ProcessFinalTempButton FinishTemp "); Serial.println(FinishTemp);
  //Serial.print("ProcessSecButton0 TimerSec0New "); Serial.println(TimerSec0New);
  sprintf(buf, "%d", FinishTemp);
#ifndef NEW_WIFI
  Server.send(200, "text/plain", buf); //Send web page
  #else
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", String(buf));
  request->send(resp);
  #endif
  // if config data not loaded,get it
  if (!CONFIG_DATA_LOADED)
    readConfigData();

  FinishServoPos=getServoPos(FinishTemp);
}
/**/


#ifndef NEW_WIFI
void ProcessBeanQuantityButton() {
  String t_state = Server.arg("VALUE");
#else
void ProcessBeanQuantityButton(AsyncWebServerRequest *request) {
  String t_state ;
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
#endif
  char buf[64];
  char * buf_p = &buf[0];
  BeanQuantity = t_state.toInt();
  Serial.print("ProcessBeanQuantityButton BeanQuantity "); Serial.println(BeanQuantity);
  sprintf(buf, "%d", BeanQuantity);
#ifndef NEW_WIFI
  
  Server.send(200, "text/plain", buf); //Send web page
  #else
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", String(buf));
  request->send(resp);
  #endif
}

#ifndef NEW_WIFI
void ProcessCoffeeType() {
  String t_state = Server.arg("VALUE");
  String t_text = Server.arg("TEXT");
#else
void ProcessCoffeeType(AsyncWebServerRequest *request) {
  String t_state ;
  char buf[64];
  char * buf_p = &buf[0];
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
  String t_text ;
  if(request->hasArg("TEXT"))
    t_text = request->arg("TEXT");
#endif
  CoffeeOpt = t_state.toInt();
  CoffeeType=t_text;
  //Serial.print("ProcessCoffeeType CoffeeOpt "); Serial.println(CoffeeOpt);

  //Serial.print("ProcessCoffeeType CoffeeType "); Serial.println(CoffeeType);
  sprintf(buf, "Opt %d Type %s text %s", CoffeeOpt, CoffeeType,t_text);
#ifndef NEW_WIFI
  
  Server.send(200, "text/plain", buf); //Send web page
  #else
  request->send(200,buf_p,"text/plain");
  #endif
}


#ifndef NEW_WIFI
void ProcessRoastSettingsButton() {
  String t_state = Server.arg("VALUE");
#else
void ProcessRoastSettingsButton(AsyncWebServerRequest *request) {
  String t_state ;
  char buf[64];
  char * buf_p = &buf[0];
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
#endif
  Serial.print("ProcessRoastSettingsButton using RoastLogData: "); Serial.println(t_state);
  sprintf(buf, "{ \"Using data From Log\": 0 }");
#ifndef NEW_WIFI
  Server.send(200, "application/json", buf); //Send web page
  #else
  AsyncWebServerResponse *resp = request->beginResponse(200, "application/json", String(buf));
  request->send(resp);
  #endif
  parseRoastData(t_state);
}


#ifndef NEW_WIFI
void UpdateSlider() {
  String t_state = Server.arg("VALUE");
#else
void UpdateSlider(AsyncWebServerRequest *request) {
  String t_state ;
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
#endif

  // convert the string sent from the web page to an int
  FanSpeed = t_state.toInt();
  Serial.print("UpdateSlider "); Serial.println(FanSpeed);
  FanRPM = map(FanSpeed, 0, 255, 0, 2400);
  strcpy(buf, "");
  sprintf(buf, "%d", FanRPM);
  sprintf(buf, buf);

  // now send it back
#ifndef NEW_WIFI
  Server.send(200, "text/plain", buf); //Send web page
  #else
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", String(buf));
  request->send(resp);
  #endif

}

#ifndef NEW_WIFI
void ProcessButtonMixPwr() {
#else
void ProcessButtonMixPwr(AsyncWebServerRequest *request) {
#endif
  MIXPWR = !MIXPWR;
  rState->mixerpwr = !rState->mixerpwr;
  digitalWrite(PIN_MIX_POWER_ENABLE, rState->mixerpwr);
  Serial.print("Button 0 Mixer Power "); Serial.println(rState->mixerpwr);
#ifndef NEW_WIFI
  Server.send(200, "text/plain", ""); //Send web page
  #else
  if (request)
    request->send(200,"MixerPowerButton","text/plain");
  #endif
}

#ifndef NEW_WIFI
void ProcessButtonMixDir() {
#else
void ProcessButtonMixDir(AsyncWebServerRequest *request) {
#endif
  MIXDIR = !MIXDIR;
  digitalWrite(PIN_MIXER_DIR, MIXDIR);
  Serial.print("Button 1 Mixer Direction "); Serial.println(MIXDIR);
#ifndef NEW_WIFI
  Server.send(200, "text/plain", ""); //Send web page
  #else
  request->send(200,"Mixer Direction","text/plain");
  #endif
}

// If using SSR logic is reversed 1 turns off
#ifndef NEW_WIFI
void ProcessButtonHeaterPwr() {
#else
void ProcessButtonHeaterPwr(AsyncWebServerRequest *request) {
#endif
  HEATERPWR = !HEATERPWR;
  rState->heaterpwr = !rState->heaterpwr;
  digitalWrite(PIN_HEATER_POWER_ENABLE, rState->heaterpwr);
  //if ( !HEATERPWR) {
  if (!rState->heaterpwr)
  {
   Serial.print("Button 0 Heater Power is ON "); Serial.println(rState->heaterpwr);
  } else {
   Serial.print("Button 0 Heater Power is OFF "); Serial.println(rState->heaterpwr);
  }
#ifndef NEW_WIFI
  Server.send(200, "text/plain", ""); //Send web page
  #else
  if (request)
    request->send(200,"HeaterPower","text/plain");
  #endif
}

#ifndef NEW_WIFI
void ProcessButtonTimerStart() {
#else
void ProcessButtonTimerStart(AsyncWebServerRequest *request) {
#endif
  rState->timerStart = !rState->timerStart;
  //digitalWrite(PIN_RELAY_MIXER_DIR, MIXDIR);
  // if 1 start timer
  if (rState->timerStart)
  {
    SetupRoastTimer();
    // get the timer start value
    TimerStartValue = TimerMin0 * 60 + TimerSec0;
    TimerValue = TimerStartValue;
    //timerAlarm(RoastTimer,TimerValue, false,0);
    //Set servo positions
    PreheatServoPos=getServoPos(PreheatTemp);
    FinishServoPos=getServoPos(FinishTemp);
    if (FinishServoPos == 0) // Temp was out of range set default
      FinishServoPos=160;
    tempTotalF=0;
    tempTotalC=0;
    rState->first=true;
    rState->roast=true;
    Serial.printf("Button Timer Start Roast TimerValue %d \n",TimerValue);
    //new way timerAlarm(RoastTimer,1000000,true,0);
    timerAlarmEnable(RoastTimer); /// old way
    //timerAlarmWrite(RoastTimer,1000000,true);
    timerStart(RoastTimer);
  }
  else
  {
    //reset timer display
    Serial.printf("Button Timer reset Start Roast TimerValue %d\n",TimerValue);
    TimerStartValue = TimerMin0 * 60 + TimerSec0;
    TimerValue = TimerStartValue;
    ClearRoastTimer();
    rState->first=true;
    rState->roast=false;
    Serial.printf("Button Timer  Stop timer %d\n",rState->timerStart);
  }
#ifndef NEW_WIFI  
  Server.send(200, "text/plain", ""); //Send web page
#else
  if (request)
    request->send(200, "TimerStart", "text/plain"); //Send web page
#endif
}

// Add 1 minute to roast timer
#ifndef NEW_WIFI
void ProcessButtonTimerAdd() {
#else
void ProcessButtonTimerAdd(AsyncWebServerRequest *request) {
#endif
  TimerValue = TimerValue + 60;
  TimerMin0New +=1;
  TimerAdjustMin +=1;
  TimerAdjust = true;
#ifndef NEW_WIFI
  Server.send(200, "text/plain", ""); //Send web page
#else
  request->send(200,"TimerAdd", "text/plain"); //Send web page
#endif
}

// Subtract 1 minute from roast timer
#ifndef NEW_WIFI
void ProcessButtonTimerSub() {
#else
void ProcessButtonTimerSub(AsyncWebServerRequest *request) {
#endif
  TimerValue = TimerValue - 60;
  TimerMin0New -=1;
  TimerAdjustMin -=1;
  if (TimerValue < 0)
	  TimerValue=0;
  TimerAdjust = true;
#ifndef NEW_WIFI
  Server.send(200, "text/plain", ""); //Send web page
#else
  request->send(200,"TimerSub", "text/plain"); //Send web page
#endif
}

#ifndef NEW_WIFI
void ProcessPreheatTimerStart() {
#else
void ProcessPreheatTimerStart(AsyncWebServerRequest *request) {
#endif
  printState();
  PREHEAT_TIMERSTART = !PREHEAT_TIMERSTART;
  rState->preheatTimerStart=!rState->preheatTimerStart;
  Serial.printf("ProcessPreheatTimerStart  %d\n",rState->preheatTimerStart);
  //digitalWrite(PIN_RELAY_MIXER_DIR, MIXDIR);
  // if 1 start timer
  if (rState->preheatTimerStart)
  {
    rState->last=false; // this ensures that process keeps going, the second time
    // If just doing preheat set up log
    if (!rState->doRoast)
	    SetupRoastingLog(true);
    //Set servo position
    PreheatServoPos=getServoPos(PreheatTemp);
    // get the timer start value
    PreheatTimerStartValue = PreTimerMin * 60 + PreTimerSec;
    // If there is a sort preheat time (testing)
    if (PreheatTimerStartValue>0 && PreheatTimerStartValue <= tempSamples)
    {
      tempSamples=(int) PreheatTimerStartValue/2;
      Serial.printf("ProcessPreheatTimerStart changed tempSamples %d\n",tempSamples);
    }
    tempTotalF=0;
    tempTotalC=0;
    if (PreheatTimerStartValue > 0)
    {
      
      rState->preheat=true;
      if (rState->heaterpwr)
	      ProcessButtonHeaterPwr();

      // if not doing a full roast call ProcessButtonMixPwr() to start mixer
      if (!rState->doRoast)
        ProcessButtonMixPwr();
      // Position servo to proper temp
      servoPosNew=PreheatServoPos;
      PreheatTimerValue = PreheatTimerStartValue;
      
      Serial.printf("ProcessPreheatTimerStart Preheat Starting Util Timer Seconds: %d  \n",PreheatTimerStartValue);
      // new way timerAlarm(UtilTimer,1000000,true,0);
      
      //timerAlarmEnable(UtilTimer); // orig way
      SetupUtilTimer();
      printf("AfterAlarmEnable");
      //timerStart(UtilTimer);
      //printf("After TimerStart");
    } else {
      Serial.print("ProcessPreheatTimerStart Preheat Timer Not SET!: "); Serial.println(PreheatTimerStartValue);
      rState->preheatTimerStart=false;
      rState->preheat=false;
#ifndef NEW_WIFI
      Server.send(200, "text/plain", "Timer Not Set"); //Send web page
#else
      if (request)
        request->send(200,"Preheat Timer Not Set", "text/plain"); //Send web page
#endif
    }
  }
  else
  {
    //reset timer display
    PreheatTimerStartValue = PreTimerMin * 60 + PreTimerSec;
    PreheatTimerValue = PreheatTimerStartValue;
    // if not roasting turn heater off and reset servo position
    if( !rState->doRoast)
    {
      //if (!HEATERPWR) // if heater is on turn it off
      if (!rState->heaterpwr)
        ProcessButtonHeaterPwr();
      servoPosNew=0;
    }
    ClearUtilTimer();
    rState->preheat=false;
    Serial.print("ProcessPreheatTimerStart Preheat Timer Button  Stop timer "); Serial.println(rState->preheatTimerStart);
  }
  
#ifndef NEW_WIFI
  Server.send(200, "text/plain", ""); //Send web page
#else
  if (request)
    request->send(200,"PreheatTimerStart", "text/plain"); //Send web page
#endif
}

String SetRoastFilename()
{
  String fname;
  //if (HEATGUNHIGH)
  //  fname="/RoastLogHigh"+get_short_date_string()+"json";
  //else
    fname="/RoastLog"+get_short_date_string()+"json";
  Serial.printf("SetupRoastLogName file name is : %s\n",fname.c_str());
  return fname;
}

void SetupRoastingLog(bool onlyPreheat)
{
  String fileName;
  //char buff[512]= {'\0'};
  //char buf[64] = {'\0'};
  String fileData="{\n\t\"Roasting Log Date\": \"";
  //char * cdata_p=&buff[0];
  RoastLogFile=SetRoastFilename();
  rState->fileName=RoastLogFile;
  fileName=RoastLogFile;
  String tempType;
  if (TempPref)
	  tempType="degrees F";
  else
	  tempType="degrees C";

  if (exists(fileName)  )
  {
    deleteFile(fileName.c_str());
  }
  Serial.printf("SetupRoastingLog file name : %s\n",fileName.c_str());
  
  fileData += get_date_string() + "\",\n";
  if (onlyPreheat)
  {
    
    fileData += "\t\"*** Preheat Only ***\": 0,\n";
    
  }

  
  fileData += "\t\"Bean Quantity (ounces)\": " + String(BeanQuantity) + ",\n";
  

  
  fileData += "\t\"Coffee Type\": \"" + CoffeeType + "\",\n";
  
  fileData += "\t\"Coffee Opt\": " + String(CoffeeOpt) + ",\n";
  
  fileData += "\t\"Preheat Time (sec)\": " + String(PreheatTimerStartValue) + ",\n";
  
 
  fileData += "\t\"Preheat Temp " + tempType + "\": " + String(PreheatTemp) + ",\n";
 
  fileData += "\t\"Preheat Servo Pos\": " + String(PreheatServoPos) + ",\n";
  
  fileData += "\t\"Roast Time (sec)\": " + String(TimerStartValue) + ",\n";
  
  
  fileData += "\t\"Finish Temp " + tempType + " \": " + String(FinishTemp) + ",\n";
  

  fileData += "\t\"Finish Servo Pos\": " + String(FinishServoPos) + ",\n";
 
  fileData += "\t\"Temp Config File Date\": \"" + Configuration_Date + "\",\n";
 
  if (HEATGUNHIGH == 1)
  {
    
    fileData += "\t\"Heatgun Speed\": \"High\",\n";
    
  }
  else
  {
   
    fileData += "\t\"Heatgun Speed\": \"Low\",\n";
    
  }
  
  writeFile(fileName.c_str(), fileData);
  Serial.printf("SetupRoastingLog done\n");
}


void CloseRoastingLog(bool manualStop)
{
  String fileName;
  String fileData;
  //char buff[128]= {'\0'};
  //char * cdata_p=&buff[0];
  //Serial.printf("--- CloseRoastingLog ---\n");
  fileName=RoastLogFile;
  if (manualStop)
  {
    //Serial.printf("--- Manual Stop ---\n");
    // Stopped manually for some reason
    //sprintf(buf,"\t],\n"); // close roast_steps array
    //strcat(buff,buf);
    fileData += "\t],\n"; // close roast_steps array
    //Serial.printf("** buff size %d\n",strlen(buff));
    //sprintf(buf, "\t\"Roasting was stopped manually at\": \"%s\"",get_date_string().c_str());
    //strcat(buff,buf);
    fileData += "\t\"Roasting was stopped manually at\": \"" + get_date_string() + "\"";
    //Serial.printf("** buff size %d\n",strlen(buff));
    if (TimerAdjust)
    {
      //sprintf(buf, ",\n\t\"Roast time adjustment\": %d\n",TimerAdjustMin);
      //strcat(buff,buf);
      fileData += ",\n\t\"Roast time adjustment\": " + String(TimerAdjustMin) + "\n";
      //Serial.printf("** buff size %d\n",strlen(buff));
    }
    // close the json string
    //sprintf(buf,"\n}\n");
    //strcat(buff,buf);
    fileData += "\n}\n";
    //Serial.printf("** buff size %d\n",strlen(buff));
  }
  else
  {
    // stopped because roasting was finished
    // this closes roast_steps array "]" and then the json with "}"
    //sprintf(buf,"\n\t]");
    //strcat(buff,buf);
    fileData += "\n\t]";
    //Serial.printf("** buff size %d\n",strlen(buff));
    if (TimerAdjust)
    {
      //sprintf(buf, ",\n\t\"Roast time adjustment\": %d\n",TimerAdjustMin);
      //strcat(buff,buf);
      //Serial.printf("** buff size %d\n",strlen(buff));
      fileData += ",\n\t\"Roast time adjustment\": " + String(TimerAdjustMin) + "\n";
    }
    //sprintf(buf, ",\t\"Roasting Finished at\": \"%s\"",get_date_string().c_str());
    //strcat(buff,buf);
    fileData += ",\t\"Roasting Finished at\": \"" + get_date_string() + "\"";
    // close the json string
    //sprintf(buf,"}\n");
    //strcat(buff,buf);
    fileData += "}\n";
    //Serial.printf("** buff size %d\n",strlen(buff));
  }
  appendFile(fileName.c_str(), fileData);

}

void readConfigData()
{
  String fileName;
  if (HEATGUNHIGH)
	  fileName=ConfigHighFile;
  else
	  fileName=ConfigFile;

  if (exists(fileName)  )
  {
    File file = openFile(fileName,"r"); 
    if (!file || file.isDirectory()){
      Serial.print("SendTempConfigData - failed to open file : ");Serial.println(fileName);
      return;
    }
    parseJsonFile(fileName);
    PreheatServoPos=getServoPos(PreheatTemp);
  }
}

// This will control all the relays etc
// using rState stucture
void SetMachineState()
{
  if (rState->doRoast)
  {
    rState->roast=!rState->roast;
    rState->first=true;
    // set tempSamples back to 10
    tempSamples=10;
    rState->doRoast=false;
    rState->last=false;
    rState->fileName="";
  }
  else if (!rState->doRoast && rState->preheat)
  {
    rState->preheat=false;
    rState->first=true;
    // set tempSamples back to 10
    tempSamples=10;
    rState->doRoast=false;
    rState->last=false;
    rState->fileName="";
  }
  // mixer power relay
  if(rState->mixerpwr)
    ProcessButtonMixPwr();
  // heater power relay (inverted logic)
  if(!rState->heaterpwr)
    ProcessButtonHeaterPwr();
  CloseRoastingLog();
  // Zero servo
  servoPosNew=0;
}

// Preheat
// Call with roast = false first=true for first pass 
void UpdateRoastState(void * rState) //bool roast, bool first , bool last, bool stop)
{
  RoastState * state = (RoastState *)rState;
  // I changed this so I could do roast without preheating
  // but it broke the logging stuff Didn't put in the 
  // "preheat_steps":[  or "roast_steps":[ t
  String fileName;
  //char buff[128]= {'\0'};
  //char * cdata_p=&buff[0];
  String fileData;
  static bool endPreheat=false;
  int buflen=0;
  int t=0;
  while (1)
  {
    if(xSemaphoreTake(roastTimerSemaphore, 0) == pdTRUE)
    {
      //WHENDEBUG(1)
      //{
        Serial.printf("---UpdateRoastState Semaphore taken\n");
        Serial.printf("---UpdateRoastState file: %s doRoast %d roast %d first %d last %d preheat %d stop %d TimerValue %d PreheatTimerValue %d tempSamples %d\n",
		    state->fileName.c_str(),state->doRoast,state->roast,state->first,
		    state->last,state->preheat,state->stopit,TimerValue,PreheatTimerValue,tempSamples);
      //}
      fileName=state->fileName;//RoastLogFile;
      fileData="";
      // things to do when timer finishes if not in preheat mode
      if(state->roast && TimerValue <=0)
      {
        //WHENDEBUG(1)
          //Serial.printf("UpdateRoastState STOPPING ROAST time : %d Servo Pos %d FinishServoPos : %d\n",TimerValue,servoPos,FinishServoPos);
        state->last=true;
        state->timerStart=false;
        tempSamples=10;
        ClearRoastTimer();
        //SetMachineState();
      }
     

      if (TimerValue%tempSamples==0 && state->roast) // dont do update if roast was stopped
      {
        //sprintf(buff, "\0");
	      //if (state->roast)
          //WHENDEBUG(1)
	        //Serial.printf(" UpdateRoastState time  : %d ROASTing: %d preheat %d \n",TimerValue,state->roast, state->preheat);
      }

      if (PreheatTimerValue%tempSamples==0 && state->preheat) // dont do update if roast was stopped
      {
        //sprintf(buff, "\0");
	      //if (state->preheat)
          //WHENDEBUG(1)
	          //Serial.printf(" UpdateRoastState time  : %d PREHEATing: %d preheat %d \n",PreheatTimerValue,state->roast, state->preheat);
      }

      if (TimerValue%tempSamples==0 && state->roast) //|| 
	      //(PreheatTimerValue%tempSamples == 0 && state->preheat)) // dont do update if roast was stopped
      {
	                      
        if (state->roast)//ROAST) 
        {
          // This closes the preheat_steps array and starts the roast_steps array	  
          if (state->roast && state->first )
          {
	          state->first=false;
            //WHENDEBUG(1)
              Serial.printf("** First Roast step ***\n");
            //sprintf(buf, "\n\t\"roast_steps\": [\n");
            //strcat(buff,buf);
            fileData += "\n\t\"roast_steps\": [\n";
            //WHENDEBUG(5)
              //Serial.printf("** buff size %d\n",strlen(buff));
          }
          //sprintf(buf, "\t{\n\t\t\"ROAST_TIME\": %d,\n", TimerValue);
          //strcat(buff,buf);
          fileData += "\t{\n\t\t\"ROAST_TIME\": " + String(TimerValue) + ",\n";
          // this gets added to each entry
          
          fileData += "\t\t\"SETTMP\": " + String(FinishTemp) + ",\n";
          
          fileData += "\t\t\"TMPF\": " + String(tempF) + ",\n";
        
          fileData += "\t\t\"TMPC\": " + String(tempC) + ",\n";
        
          fileData += "\t\t\"POS\": " + String(servoPos) + "\n";
          fileData += "\t}";
        }
        else if (state->stopit)
        {
          // If stopped in the middle add one more of these to 
          // make json file valid 
          //sprintf(buf, "\t{\n\t\t\"ROAST_TIME\": %d,\n", TimerValue);
          //strcat(buff,buf);
          fileData += "\t{\n\t\t\"ROAST_TIME\": " + String(TimerValue) + ",\n";
          fileData += "\t}";
          
        }
        
         
        // put in a comma before next entry 
        if ( !state->last )
        {
          //WHENDEBUG(1)
            //Serial.printf("UpdateRoastState adding comma file first %d endPreheat %d last %d\n",
		        //state->first,endPreheat,state->last);
          //sprintf(buf, ",\n");
          //strcat(buff,buf);
          //buflen+=strlen(buf);
          fileData += ",\n";
          //WHENDEBUG(1)
            //Serial.printf("** buff size %d\n",strlen(buff));
        }
        else if (state->preheat && state->last)
        {
          
          Serial.printf("UpdateRoastState last preheat step roast %d endPreheat %d last %d\n",
		          state->roast,endPreheat,state->last);
	        if (state->doRoast)
	        {
	          state->preheat=false;
	          state->last=false;
	        }
          //sprintf(buf, "\n\t],\n");
          //strcat(buff,buf);
          fileData += "\n\t],\n";
          //WHENDEBUG(1)
            //Serial.printf("** buff size %d\n",strlen(buff));
          //buflen+=strlen(buf);

        } 
        else 
        {
          
          fileData += "\n";
          
        }
  
        
	        buflen=0;
        if (state->fileName.length() > 0)
          appendFile(fileName.c_str(), fileData);
          fileData="";
	// Turn off stuff when done
	      if(state->doRoast && state->roast && state->last)
	      {
          //WHENDEBUG(1)
            Serial.printf("UpdateRoastState Should be done roasting!\n");
	        SetMachineState();
	      }
	      else if(!state->doRoast && state->preheat && state->last)
	      {
          //WHENDEBUG(1)
            Serial.printf("UpdateRoastState Should be done PREHEAT only!\n");
	        SetMachineState();
	      }
      } // end of update to roasting log
    }
    else if(xSemaphoreTake(utilTimerSemaphore, 0) == pdTRUE)
    {
      //WHENDEBUG(1)
      //{
        Serial.printf("UpdateRoastState---UtilTimer Semaphore taken\n");
        Serial.printf("---UpdateRoastState file: %s doRoast %d roast %d first %d last %d \
          preheat %d stop %d TimerValue %d PreheatTimerValue %d tempSamples %d, Config %d\n",
		    state->fileName.c_str(),state->doRoast,state->roast,state->first,
		    state->last,state->preheat,state->stopit,TimerValue,PreheatTimerValue,tempSamples,state->doConfig);
        fileData = "";
        fileName=state->fileName;//RoastLogFile;
#ifdef CONFIG_PAGE
      if (TEMP_CONFIG)
      {
        if (ConfigTimerValue == 0)
        {
	        Serial.print("In UpdateRoastState -- Config -- utilTimerSemaphore is true finished step " );Serial.println(ConfigStep);
	        RunTempConfig(++ConfigStep);
	        ConfigElaspedTime++;// in seconds
        } else 
        {
          //	Serial.println("in loop UtilTimerSemaphore Collecting Temps = "); 
	        // Collect Temp values
	        tempTotalC+=tempC;
	        tempTotalF+=tempF;
        }
      }

      
      
      if(!state->doRoast && state->preheat && state->last)
	    {
         //WHENDEBUG(1)
        Serial.printf("UpdateRoastState Should be done PREHEAT only!\n");
        ClearUtilTimer();
	      SetMachineState();
	    }

      if (PreheatTimerValue%tempSamples == 0 && state->preheat) // dont do update if roast was stopped
      {
	      
        if (state->preheat)//PREHEAT)
        {
          //WHENDEBUG(1)
            Serial.printf("preheat Preheat steps roast %d preheat %d first %d\n", state->roast, state->preheat, state->first);
          // This starts the preheat_steps array
          if(state->preheat && state->first){
	          state->first=false;
            
	          fileData = "\t\"preheat_steps\": \n\t[\n";
            Serial.println("*** First preheat step *** ");
          }
          
	        fileData += "\t{\n\t\t\"PREHEAT_TIME\": " + String(PreheatTimerValue) + ",\n";
          
          // this gets added to each entry

          fileData += "\t\t\"SETTMP\": " + String(PreheatTemp) + ",\n";

          fileData += "\t\t\"TMPF\": " + String(tempF) + ",\n";
        
          fileData += "\t\t\"TMPC\": " + String(tempC) + ",\n";
        
          fileData += "\t\t\"POS\": " + String(servoPos) + "\n";
        
          fileData += "\t}";
        }
        // put in a comma before next entry 
        if ( PreheatTimerValue > 0 )
        {
          fileData += ",\n";
          
        }
        // Transition between preheat and roast
      //
      if(state->preheat && PreheatTimerValue <=0)
      {
        //WHENDEBUG(1)
        Serial.printf("UpdateRoastState Preheat done preheat time : %d Servo Pos %d FinishServoPos : %d timerstart %d\n",
            PreheatTimerValue,servoPos,FinishServoPos, state->timerStart);
        state->last=true;
        state->preheatTimerStart=false;
        
        tempSamples=5;
        
        Serial.printf("UpdateRoastState last preheat step roast %d endPreheat %d last %d\n",
		          state->roast,endPreheat,state->last);
	      if (state->doRoast)
	      {
	        state->preheat=false;
	        state->last=false;
	      }
         
        // close preheat steps array
        if (!state->doRoast)
        {
          // only doing preheat reset last
          //state->last=false;
          fileData += "\n\t]\n}"; // No Comma and close json if only doing preheat
        }
        else
          fileData += "\n\t],\n";
        Serial.println("UpdateRoastState preheat -- here");
        //ClearUtilTimer();
        Serial.println("UpdateRoastState preheat here");
        //SetMachineState();
	      if (state->doRoast && !state->timerStart)
	      {
          Serial.println("UpdateRoastState preheat here 2");
	        // Have to turn up the temp!!
	        if (FinishServoPos < servoPos)
          {
            Serial.println("UpdateRoastState preheat here 3");
	          servoPosNew= servoPos + 20; // incase final temp is set higher than config data
          }
	        else
          {
            Serial.println("UpdateRoastState preheat here 4");
	          servoPosNew=FinishServoPos;
          }
	        Serial.println("UpdateRoastState Before trying to start timer"); 
          ProcessButtonTimerStart();
	      }
      }
        if (state->fileName.length() > 0)
          appendFile(fileName.c_str(), fileData);
      


     
        //SetMachineState();
      }
     
      
#endif
    } // end of Util Semaphore taken
    
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }// end of while
  Serial.println("UpdateRoastState out of while loop");
}

void SetupRoastTimer()
{
  //timerDetachInterrupt(UtilTimer); // this causes exception
 // Setup for timer timer 0 prescaler 80 count up (true)
  RoastTimer = timerBegin(0, 80,true); // orig way
  // new way RoastTimer = timerBegin(1000000);
  // Attach to interrupt handler
  // new way timerAttachInterrupt(RoastTimer, &onRoastTimer );
  timerAttachInterrupt(RoastTimer, &onRoastTimer , true); // orig way
  //Specify timer timeout value
  // 1000000 microseconds = 1 second
  timerAlarmWrite(RoastTimer, 1000000 ,true);
  Serial.println("SetupRoastTimer() ");
}


void ClearRoastTimer()
{
  Serial.println("ClearRoastTimer() ");
  if (RoastTimer)
  {
    timerStop(RoastTimer);
    //new way timerAlarm(RoastTimer,0,false,0); //Does this disable it?
    timerAlarmDisable(RoastTimer);
    // Detach to interrupt handler
    timerDetachInterrupt(RoastTimer);
  }
  Serial.println("ClearRoastTimer() ");
}
