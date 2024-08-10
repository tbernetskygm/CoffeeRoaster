// This has code to handle Buttons and User interface for main roaster page
#include "ProjectDefines.h"
#include "Globals.h"
#include "RoasterControls.h"
#include "UtilTimer.h"
#include "FileSystemFunctions.h"
#include "UtilityFunctions.h"
// ISR for RoastTimer
void IRAM_ATTR onRoastTimer() {
  Serial.printf("onRoastTimer() TimerValue = %d\n",TimerValue);
  if (TimerValue <= 0)
  {
    Serial.println("onRoastTimer() Timer Stop!!! ");
    timerStop(RoastTimer);
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
void ProcessButtonRoastStart() {
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
    //UpdateRoastingLog(true,false,true,true);
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
  }
}

void ProcessStartPreHeat()
{
  Serial.println("ProcessStartPreHeat() call SetupPreheatTimer ");
  SetupPreheatTimer();
  Serial.println("ProcessStartPreHeat() Done ");
}

void ProcessMinButton_0() {
  String t_state = Server.arg("VALUE");
  TimerMin0New = t_state.toInt();
  Serial.print("ProcessMinButton0 TimerMin0 "); Serial.println(TimerMin0);
  //Serial.print("ProcessMinButton0 TimerMin0New: "); Serial.println(TimerMin0New);
  sprintf(buf, "%d", TimerMin0New);
  Server.send(200, "text/plain", buf); //Send web page
}

void ProcessSecButton_0() {
  String t_state = Server.arg("VALUE");
  TimerSec0New = t_state.toInt();
  Serial.print("ProcessSecButton0 TimerSec0 "); Serial.println(TimerSec0);
  //Serial.print("ProcessSecButton0 TimerSec0New "); Serial.println(TimerSec0New);
  sprintf(buf, "%d", TimerSec0New);
  Server.send(200, "text/plain", buf); //Send web page
}


void ProcessPreMinButton_0() {
  String t_state = Server.arg("VALUE");
  PreTimerMinNew = t_state.toInt();
  Serial.print("ProcessPreMinButton0 PreTimerMin "); Serial.println(PreTimerMin);
  Serial.print("ProcessPreMinButton0 PreTimerMinNew: "); Serial.println(PreTimerMinNew);
  sprintf(buf, "%d", PreTimerMinNew);
  Server.send(200, "text/plain", buf); //Send web page
}

void ProcessPreSecButton_0() {
  String t_state = Server.arg("VALUE");
  PreTimerSecNew = t_state.toInt();
  Serial.print("ProcessPreSecButton0 PreTimerSec "); Serial.println(PreTimerSec);
  Serial.print("ProcessPreSecButton0 PreTimerSecNew "); Serial.println(PreTimerSecNew);

  //Serial.print("ProcessSecButton0 TimerSec0New "); Serial.println(TimerSec0New);
  sprintf(buf, "%d", PreTimerSecNew);
  Server.send(200, "text/plain", buf); //Send web page
}

void ProcessPreTempButton() {
  String t_state = Server.arg("VALUE");
  PreheatTemp = t_state.toInt();
  Serial.print("ProcessPreTempButton PreheatTemp "); Serial.println(PreheatTemp);
  //Serial.print("ProcessSecButton0 TimerSec0New "); Serial.println(TimerSec0New);
  sprintf(buf, "%d", PreheatTemp);
  Server.send(200, "text/plain", buf); //Send web page
  // if config data not loaded,get it
  if (!CONFIG_DATA_LOADED)
    readConfigData();
  
  PreheatServoPos=getServoPos(PreheatTemp);
}


void ProcessFinalTempButton() {
  String t_state = Server.arg("VALUE");
  FinishTemp = t_state.toInt();
  Serial.print("ProcessFinalTempButton FinishTemp "); Serial.println(FinishTemp);
  //Serial.print("ProcessSecButton0 TimerSec0New "); Serial.println(TimerSec0New);
  sprintf(buf, "%d", FinishTemp);
  Server.send(200, "text/plain", buf); //Send web page
  // if config data not loaded,get it
  if (!CONFIG_DATA_LOADED)
    readConfigData();

  FinishServoPos=getServoPos(FinishTemp);
}
/**/


void ProcessBeanQuantityButton() {
  String t_state = Server.arg("VALUE");
  BeanQuantity = t_state.toInt();
  Serial.print("ProcessBeanQuantityButton BeanQuantity "); Serial.println(BeanQuantity);
  sprintf(buf, "%d", BeanQuantity);
  Server.send(200, "text/plain", buf); //Send web page
}

void ProcessCoffeeType() {
  String t_state = Server.arg("VALUE");
  CoffeeOpt = t_state.toInt();
  String t_text = Server.arg("TEXT");
  CoffeeType=t_text;
  Serial.print("ProcessCoffeeType CoffeeOpt "); Serial.println(CoffeeOpt);

  Serial.print("ProcessCoffeeType CoffeeType "); Serial.println(CoffeeType);
  sprintf(buf, "Opt %d Type %s text %s", CoffeeOpt, CoffeeType,t_text);
  Server.send(200, "text/plain", buf); //Send web page
}


void ProcessRoastSettingsButton() {
  String t_state = Server.arg("VALUE");
  Serial.print("ProcessRoastSettingsButton using RoastLogData: "); Serial.println(t_state);
  sprintf(buf, "{ \"Using data From Log\": 0 }");
  Server.send(200, "application/json", buf); //Send web page
  parseRoastData(t_state);
}


void UpdateSlider() {
  String t_state = Server.arg("VALUE");

  // convert the string sent from the web page to an int
  FanSpeed = t_state.toInt();
  Serial.print("UpdateSlider "); Serial.println(FanSpeed);
  FanRPM = map(FanSpeed, 0, 255, 0, 2400);
  strcpy(buf, "");
  sprintf(buf, "%d", FanRPM);
  sprintf(buf, buf);

  // now send it back
  Server.send(200, "text/plain", buf); //Send web page

}

void ProcessButtonMixPwr() {
  MIXPWR = !MIXPWR;
  rState->mixerpwr = !rState->mixerpwr;
  digitalWrite(PIN_MIX_POWER_ENABLE, rState->mixerpwr);
  Serial.print("Button 0 Mixer Power "); Serial.println(rState->mixerpwr);
  Server.send(200, "text/plain", ""); //Send web page
}

void ProcessButtonMixDir() {
  MIXDIR = !MIXDIR;
  digitalWrite(PIN_MIXER_DIR, MIXDIR);
  Serial.print("Button 1 Mixer Direction "); Serial.println(MIXDIR);
  Server.send(200, "text/plain", ""); //Send web page
}

// If using SSR logic is reversed 1 turns off
void ProcessButtonHeaterPwr() {
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
  Server.send(200, "text/plain", ""); //Send web page
}

void ProcessButtonTimerStart() {
  rState->timerStart = !rState->timerStart;
  //digitalWrite(PIN_RELAY_MIXER_DIR, MIXDIR);
  // if 1 start timer
  if (rState->timerStart)
  {
    SetupRoastTimer();
    // get the timer start value
    TimerStartValue = TimerMin0 * 60 + TimerSec0;
    TimerValue = TimerStartValue;
    timerAlarmEnable(RoastTimer);
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
  
  Server.send(200, "text/plain", ""); //Send web page
}

// Add 1 minute to roast timer
void ProcessButtonTimerAdd() {
  TimerValue = TimerValue + 60;
  TimerMin0New +=1;
  TimerAdjust = true;
  Server.send(200, "text/plain", ""); //Send web page
}

// Subtract 1 minute from roast timer
void ProcessButtonTimerSub() {
  TimerValue = TimerValue - 60;
  TimerMin0New -=1;
  if (TimerValue < 0)
	  TimerValue=0;
  TimerAdjust = true;
  Server.send(200, "text/plain", ""); //Send web page
}

void ProcessPreheatTimerStart() {
  PREHEAT_TIMERSTART = !PREHEAT_TIMERSTART;
  rState->preheatTimerStart=!rState->preheatTimerStart;
  Serial.printf("ProcessPreheatTimerStart  %d\n",rState->preheatTimerStart);
  //digitalWrite(PIN_RELAY_MIXER_DIR, MIXDIR);
  // if 1 start timer
  if (rState->preheatTimerStart)
  {
    // If just doing preheat set up log
    if (!rState->doRoast)
	    SetupRoastingLog(true);
    //Set servo position
    PreheatServoPos=getServoPos(PreheatTemp);
    // get the timer start value
    PreheatTimerStartValue = PreTimerMin * 60 + PreTimerSec;
    // If there is a sort preheat time (testing)
    if (PreheatTimerStartValue <= tempSamples)
    {
      tempSamples=(int) PreheatTimerStartValue/2;
      Serial.printf("ProcessPreheatTimerStart changed tempSamples %d\n",tempSamples);
    }
    tempTotalF=0;
    tempTotalC=0;
    if (PreheatTimerStartValue > 0)
    {
      SetupPreheatTimer();
      rState->preheat=true;
      if (rState->heaterpwr)
	ProcessButtonHeaterPwr();

      // if not doing a full roast call ProcessButtonMixPwr() to start mixer
      if (!rState->doRoast)
        ProcessButtonMixPwr();
      // Position servo to proper temp
      servoPosNew=PreheatServoPos;
      PreheatTimerValue = PreheatTimerStartValue;
      Serial.print("ProcessPreheatTimerStart Preheat Starting Timer: "); Serial.println(PreheatTimerStartValue);
      timerAlarmEnable(UtilTimer);
      timerStart(UtilTimer);
    } else {
      Serial.print("ProcessPreheatTimerStart Preheat Timer Not SET!: "); Serial.println(PreheatTimerStartValue);
      rState->preheatTimerStart=false;
      rState->preheat=false;
      Server.send(200, "text/plain", "Timer Not Set"); //Send web page
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
    Serial.print("ProcessPreheatTimerStart Preheat Timer Button  Stop timer"); Serial.println(rState->preheatTimerStart);
  }
  
  Server.send(200, "text/plain", ""); //Send web page
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
  //char buff[64]= {'\0'};
  char * cdata_p=&buff[0];
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
  sprintf(buff, "\0");
  sprintf(buf, "{\n\t\"Roasting Log Date\": ");
  strcat(buff,buf);
  WHENDEBUG(5)
    Serial.printf("** buff size %d\n",strlen(buff));
 
  Serial.printf("SetupRoastingLog get_date buf length %d\n",strlen(buf));
  sprintf(buf, "\"%s\",\n",get_date_string().c_str());
  strcat(buff,buf);
  
  if (onlyPreheat)
  {
    sprintf(buf, "\t\"*** Preheat Only ***\",\n");
    strcat(buff,buf);
  }

  sprintf(buf, "\t\"Bean Quantity (ounces)\": %d,\n",BeanQuantity);
  strcat(buff,buf);

  Serial.printf("SetupRoastingLog Coffee Type <%s>\n",CoffeeType);
  sprintf(buf, "\t\"Coffee Type\": \"%s\",\n",CoffeeType);
  strcat(buff,buf);
  WHENDEBUG(5)
    Serial.printf("** buff size %d\n",strlen(buff));

  sprintf(buf, "\t\"Coffee Opt\": %d,\n",CoffeeOpt);
  strcat(buff,buf);
  Serial.printf("** buff size %d\n",strlen(buff));

  Serial.printf("SetupRoastingLog preheat time %d\n",PreheatTimerStartValue);
  sprintf(buf, "\t\"Preheat Time (sec)\": %d,\n",PreheatTimerStartValue);
  strcat(buff,buf);
  WHENDEBUG(5)
    Serial.printf("** buff size %d\n",strlen(buff));

  Serial.printf("SetupRoastingLog preheat temp %d\n",PreheatTemp);
  sprintf(buf, "\t\"Preheat Temp %s \": %d,\n",tempType.c_str(),PreheatTemp);
  strcat(buff,buf);
  WHENDEBUG(5)
    Serial.printf("** buff size %d\n",strlen(buff));

  sprintf(buf, "\t\"Preheat Servo Pos\": %d,\n",PreheatServoPos);
  strcat(buff,buf);
  WHENDEBUG(5)
    Serial.printf("** buff size %d\n",strlen(buff));

  Serial.printf("SetupRoastingLog Roast time %d\n",TimerStartValue);
  sprintf(buf, "\t\"Roast Time (sec)\": %d,\n",TimerStartValue);
  strcat(buff,buf);
  WHENDEBUG(5)
    Serial.printf("** buff size %d\n",strlen(buff));

  Serial.printf("SetupRoastingLog final temp\n");
  sprintf(buf, "\t\"Finish Temp %s \": %d,\n",tempType.c_str(),FinishTemp);
  strcat(buff,buf);
  WHENDEBUG(5)
    Serial.printf("** buff size %d\n",strlen(buff));

  sprintf(buf, "\t\"Finish Servo Pos\": %d,\n",FinishServoPos);
  strcat(buff,buf);
  WHENDEBUG(5)
    Serial.printf("** buff size %d\n",strlen(buff));

  Serial.printf("SetupRoastingLog Configuration Date <%s>\n",Configuration_Date.c_str());
  sprintf(buf, "\t\"Temp Config File Date\": \"%s\",\n",Configuration_Date.c_str());
  strcat(buff,buf);
  WHENDEBUG(5)
    Serial.printf("** buff size %d\n",strlen(buff));

  Serial.printf("SetupRoastingLog set heatgun speed %d\n",HEATGUNHIGH);
  if (HEATGUNHIGH == 1)
  {
    sprintf(buf, "\t\"Heatgun Speed\": \"High\",\n");
    strcat(buff,buf);
    WHENDEBUG(5)
      Serial.printf("** buff size %d\n",strlen(buff));
  }
  else
  {
    sprintf(buf, "\t\"Heatgun Speed\": \"Low\",\n");
    strcat(buff,buf);
    WHENDEBUG(5)
      Serial.printf("** buff size %d\n",strlen(buff));
  }
  writeFile(fileName.c_str(), cdata_p);
  
  Serial.printf("SetupRoastingLog done\n");
}


void CloseRoastingLog(bool manualStop)
{
  String fileName;
  char buff[128]= {'\0'};
  char * cdata_p=&buff[0];
  Serial.printf("--- CloseRoastingLog ---\n");
  fileName=RoastLogFile;
  if (manualStop)
  {
    Serial.printf("--- Manual Stop ---\n");
    // Stopped manually for some reason
    sprintf(buf,"\t],\n"); // close roast_steps array
    strcat(buff,buf);
    //Serial.printf("** buff size %d\n",strlen(buff));
    sprintf(buf, "\t\"Roasting was stopped manually at\": \"%s\"",get_date_string().c_str());
    strcat(buff,buf);
    //Serial.printf("** buff size %d\n",strlen(buff));
    if (TimerAdjust)
    {
      sprintf(buf, ",\n\t\"Roasting time adjusted during roast by (min)\": %d\n",TimerMin0New);
      strcat(buff,buf);
      //Serial.printf("** buff size %d\n",strlen(buff));
    }
    // close the json string
    sprintf(buf,"\n}\n");
    strcat(buff,buf);
    //Serial.printf("** buff size %d\n",strlen(buff));
  }
  else
  {
    // stopped because roasting was finished
    // this closes roast_steps array "]" and then the json with "}"
    sprintf(buf,"\n\t]");
    strcat(buff,buf);
    Serial.printf("** buff size %d\n",strlen(buff));
    if (TimerAdjust)
    {
      sprintf(buf, ",\n\t\"Roasting time was adjusted during roast by (min)\": %d\n",TimerMin0New);
      strcat(buff,buf);
      Serial.printf("** buff size %d\n",strlen(buff));
    }
    sprintf(buf, ",\t\"Roasting Finished at\": \"%s\"",get_date_string().c_str());
    strcat(buff,buf);
    // close the json string
    sprintf(buf,"}\n");
    strcat(buff,buf);
    Serial.printf("** buff size %d\n",strlen(buff));
  }
  appendFile(fileName.c_str(), cdata_p);

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
void UpdateRoastingLog(void * rState) //bool roast, bool first , bool last, bool stop)
{
  RoastState * state = (RoastState *)rState;
  // I changed this so I could do roast without preheating
  // but it broke the logging stuff Didn't put in the 
  // "preheat_steps":[  or "roast_steps":[ t
  String fileName;
  //char buff[256]= {'\0'};
  char * cdata_p=&buff[0];
  static bool endPreheat=false;
  int buflen=0;
  while (1)
  {
    if(xSemaphoreTake(roastTimerSemaphore, 0) == pdTRUE)
    {
      Serial.printf("---UpdateRoastingLog Semaphore taken\n");
      Serial.printf("---UpdateRoastingLog file: %s doRoast %d roast %d first %d last %d preheat %d stop %d TimerValue %d PreheatTimerValue %d tempSamples %d\n",
		    state->fileName.c_str(),state->doRoast,state->roast,state->first,
		    state->last,state->preheat,state->stopit,TimerValue,PreheatTimerValue,tempSamples);
      fileName=state->fileName;//RoastLogFile;
      // things to do when timer finishes if not in preheat mode
      if(state->roast && TimerValue <=0)
      {
        Serial.printf("UpdateRoastingLog STOPPING ROAST time : %d Servo Pos %d FinishServoPos : %d\n",TimerValue,servoPos,FinishServoPos);
        state->last=true;
        state->timerStart=false;
        tempSamples=10;
        ClearRoastTimer();
        //SetMachineState();
      }
      // Transition between preheat and roast
      //
      if(state->preheat && PreheatTimerValue <=0)
      {
        Serial.printf("UpdateRoastingLog Preheat done preheat time : %d Servo Pos %d FinishServoPos : %d\n",PreheatTimerValue,servoPos,FinishServoPos);
        state->last=true;
        state->preheatTimerStart=false;
        tempSamples=5;
        ClearUtilTimer();
	if (state->doRoast && !state->timerStart)
	  ProcessButtonTimerStart();
        //SetMachineState();
      }

      if (TimerValue%tempSamples==0 && state->roast) // dont do update if roast was stopped
      {
        sprintf(buff, "\0");
	if (state->roast)
	  Serial.printf(" UpdateRoastingLog time  : %d ROASTing: %d preheat %d \n",TimerValue,state->roast, state->preheat);
      }

      if (PreheatTimerValue%tempSamples==0 && state->preheat) // dont do update if roast was stopped
      {
        sprintf(buff, "\0");
	if (state->preheat)
	  Serial.printf(" UpdateRoastingLog time  : %d PREHEATing: %d preheat %d \n",PreheatTimerValue,state->roast, state->preheat);
      }

      if ((TimerValue%tempSamples==0 && state->roast)|| (PreheatTimerValue%tempSamples == 0 && state->preheat)) // dont do update if roast was stopped
      {
	if (state->roast)
	  Serial.printf(" UpdateRoastingLog time  : %d ROAST: %d preheat %d \n",TimerValue,state->roast, state->preheat);
	if (state->preheat)
	  Serial.printf(" UpdateRoastingLog time  : %d PREHEAT: %d preheat %d \n",PreheatTimerValue,state->roast, state->preheat);
  
        sprintf(buff, "\0");
        if (state->preheat)//PREHEAT)
        {
          Serial.printf("Preheat steps\n");
          // This starts the preheat_steps array
          if(!state->roast && state->first){
	    state->first=false;
            sprintf(buf, "\t\"preheat_steps\": [\n");
            strcat(buff,buf);
            WHENDEBUG(5)
              Serial.printf("** buff size %d\n",strlen(buff));
            Serial.printf("** First Preheat step *** buf size %d\n",strlen(buff));
            buflen+=strlen(buff);
          }
          sprintf(buf, "\t{\n\t\t\"PREHEAT_TIME\": %d,\n", PreheatTimerValue);
          strcat(buff,buf);
          WHENDEBUG(5)
            Serial.printf("** buff size %d\n",strlen(buff));
          buflen+=strlen(buf);
        } 
        else if (state->roast)//ROAST) 
        {
          // This closes the preheat_steps array and starts the roast_steps array	  
          if (state->roast && state->first )
          {
	    state->first=false;
            Serial.printf("** First Roast step ***\n");
            sprintf(buf, "\n\t\"roast_steps\": [\n");
            strcat(buff,buf);
            WHENDEBUG(5)
              Serial.printf("** buff size %d\n",strlen(buff));
          }
          sprintf(buf, "\t{\n\t\t\"ROAST_TIME\": %d,\n", TimerValue);
          strcat(buff,buf);
          WHENDEBUG(5)
            Serial.printf("** buff size %d\n",strlen(buff));
        }
        else if (state->stopit)
        {
          // If stopped in the middle add one more of these to 
          // make json file valid 
          sprintf(buf, "\t{\n\t\t\"ROAST_TIME\": %d,\n", TimerValue);
          strcat(buff,buf);
          WHENDEBUG(5)
            Serial.printf("** buff size %d\n",strlen(buff));
        }
        // this gets added to each entry
        sprintf(buf, "\t\t\"TMPF\": %.2f,\n", tempF);
        strcat(buff,buf);
        WHENDEBUG(5)
          Serial.printf("** buff size %d\n",strlen(buff));
        buflen+=strlen(buf);
   
        sprintf(buf, "\t\t\"TMPC\": %.2f,\n", tempC);
        strcat(buff,buf);
        WHENDEBUG(5)
          Serial.printf("** buff size %d\n",strlen(buff));
        buflen+=strlen(buf);
   
        sprintf(buf, "\t\t\"POS\": %d\n", servoPos);
        strcat(buff,buf);
        WHENDEBUG(5)
          Serial.printf("** buff size %d\n",strlen(buff));
        buflen+=strlen(buf);
   
        sprintf(buf, "\t}");
        strcat(buff,buf);
        WHENDEBUG(5)
          Serial.printf("** buff size %d\n",strlen(buff));
        buflen+=strlen(buf);
  
        // put in a comma before next entry 
        if ( !state->last )
        {
          Serial.printf("UpdateRoastingLog adding comma file first %d endPreheat %d last %d\n",
		      state->first,endPreheat,state->last);
          sprintf(buf, ",\n");
          strcat(buff,buf);
          buflen+=strlen(buf);
          WHENDEBUG(1)
            Serial.printf("** buff size %d\n",strlen(buff));
        }
        else if (state->preheat && state->last)
        {
          Serial.printf("UpdateRoastingLog last preheat step roast %d endPreheat %d last %d\n",
		       state->roast,endPreheat,state->last);
	  if (state->doRoast)
	  {
	    state->preheat=false;
	    state->last=false;
	  }
          sprintf(buf, "\n\t],\n");
          strcat(buff,buf);
          WHENDEBUG(1)
            Serial.printf("** buff size %d\n",strlen(buff));
          buflen+=strlen(buf);

        } 
        else 
        {
          Serial.printf("UpdateRoastingLog adding newline only file first %d endPreheat %d last %d\n",
		       state->first,endPreheat,state->last);
          //sprintf(buff, " first %d last %d\n", state->first,state->last);
          sprintf(buf, "\n");
          strcat(buff,buf);
          WHENDEBUG(1)
            Serial.printf("** buff size %d\n",strlen(buff));
          buflen+=strlen(buf);
        }
  
        Serial.printf("UpdateRoastingLog total buffer length %d\n",buflen);
        WHENDEBUG(1)
          Serial.printf("** buff size %d\n",strlen(buff));
	buflen=0;
        if (state->fileName.length() > 0)
          appendFile(fileName.c_str(), cdata_p);
	// Turn off stuff when done
	if(state->doRoast && state->roast && state->last)
	{
          Serial.printf("UpdateRoastingLog Should be done roasting!\n");
	  SetMachineState();
	}
	else if(!state->doRoast && state->preheat && state->last)
	{
          Serial.printf("UpdateRoastingLog Should be done PREHEAT only!\n");
	  SetMachineState();
	}
      }
    }
  }
}

void SetupRoastTimer()
{
  //timerDetachInterrupt(UtilTimer); // this causes exception
 // Setup for timer timer 0 prescaler 80 count up (true)
  RoastTimer = timerBegin(0, 80, true);
  // Attach to interrupt handler
  timerAttachInterrupt(RoastTimer, &onRoastTimer , true);
  //Specify timer timeout value
  // 1000000 microseconds = 1 second
  timerAlarmWrite(RoastTimer, 1000000, true);
  Serial.println("setupRoastTimer() ");
}


void ClearRoastTimer()
{
  Serial.println("ClearRoastTimer() ");
  if (RoastTimer)
  {
    timerStop(RoastTimer);
    timerAlarmDisable(RoastTimer);
    // Detach to interrupt handler
    timerDetachInterrupt(RoastTimer);
  }
  Serial.println("ClearRoastTimer() ");
}


