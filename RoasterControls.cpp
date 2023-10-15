// This has code to handle Buttons and User interface for main roaster page
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
  Serial.printf("ProcessButtonRoastStart ROAST= %d MIXPWR %d HEATERPWR %d TIMERSTART %d PREHEAT_TIMERSTART %d\n",ROAST,MIXPWR,HEATERPWR,TIMERSTART,PREHEAT_TIMERSTART);

  // if config data not loaded,get it
  if (!CONFIG_DATA_LOADED)
    readConfigData();
  if (ROAST)
  {
    //Set servo positions
    PreheatServoPos=getServoPos(PreheatTemp);
    FinishServoPos=getServoPos(FinishTemp);
    if (FinishServoPos == 0) // Temp was out of range set default
      FinishServoPos=160;
    // start logging data
    SetupRoastingLog();
    // call ProcessButtonMixPwr() to start mixer
    if (!MIXPWR)
      ProcessButtonMixPwr();
       // if the preheat timer value is set do the preheat first
    Serial.printf("ProcessButtonRoastStart PreheatTimerValue %d PreheatTimerStartValue %d\n ",
		    PreheatTimerValue,PreheatTimerStartValue);
    if(PreheatTimerStartValue > PreheatTimerValue)
	    PreheatTimerValue=PreheatTimerStartValue;

    // If there is a preheat timer value then start preheat
    if (PreheatTimerValue > 0 && !PREHEAT_TIMERSTART )
      ProcessPreheatTimerStart();
    else {
     // No preheat just roast 
      // Position servo to proper temp
      servoPosNew=FinishServoPos;
	// set tempSamples to 5
        tempSamples=5;
      //don't forget to turn heater on!!
      if (HEATERPWR)
	ProcessButtonHeaterPwr();
      // call ProcessButtonTimerStart() to start the timer
      if (!TIMERSTART)
      {
        ProcessButtonTimerStart();
      }
    }
  }

  else //Stop
  {
    // call ProcessButtonMixPwr() to stop mixer
    if (MIXPWR)
      ProcessButtonMixPwr();
    // call ProcessButtonHeaterPwr() to turn off heater
    if (!HEATERPWR)
    {
      ProcessButtonHeaterPwr();
      // Reset servo position
      servoPosNew=0;
    }
    // If stop is pressed update log set last and stop to true.
    UpdateRoastingLog(true,false,true,true);
    Serial.printf("Stop after update roasting log\n");
    // call ProcessButtonTimerStart() to stop the timer
    if (TIMERSTART)
    {
      ProcessButtonTimerStart();
    }
    CloseRoastingLog(true);
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
  t_state = Server.arg("TEXT");
  CoffeeType=t_state;
  Serial.print("ProcessCoffeeType CoffeeOpt "); Serial.println(CoffeeOpt);

  Serial.print("ProcessCoffeeType CoffeeType "); Serial.println(CoffeeType);
  sprintf(buf, "Opt %d Type %s", CoffeeOpt, CoffeeType);
  Server.send(200, "text/plain", buf); //Send web page
}


void ProcessRoastSettingsButton() {
  String t_state = Server.arg("VALUE");
  Serial.print("ProcessRoastSettingsButton using RoastLogData: "); Serial.println(t_state);
  sprintf(buf, "{ Using data }");
  Server.send(200, "text/plain", buf); //Send web page
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
  digitalWrite(PIN_MIX_POWER_ENABLE, MIXPWR);
  Serial.print("Button 0 Mixer Power "); Serial.println(MIXPWR);
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
  digitalWrite(PIN_HEATER_POWER_ENABLE, HEATERPWR);
  if ( !HEATERPWR) {
   Serial.print("Button 0 Heater Power is ON "); Serial.println(HEATERPWR);
  } else {
   Serial.print("Button 0 Heater Power is OFF "); Serial.println(HEATERPWR);
  }
  Server.send(200, "text/plain", ""); //Send web page
}

void ProcessButtonTimerStart() {
  TIMERSTART = !TIMERSTART;
  //digitalWrite(PIN_RELAY_MIXER_DIR, MIXDIR);
  // if 1 start timer
  if (TIMERSTART)
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
    first=true;
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
    first=true;
    Serial.printf("Button Timer  Stop timer %d\n",TIMERSTART);
  }
  
  Server.send(200, "text/plain", ""); //Send web page
}

// Add 1 minute to roast timer
void ProcessButtonTimerAdd() {
  TimerValue = TimerValue + 60;
  TimerMin0New +=1;
  Server.send(200, "text/plain", ""); //Send web page
}

// Subtract 1 minute from roast timer
void ProcessButtonTimerSub() {
  TimerValue = TimerValue - 60;
  TimerMin0New -=1;
  if (TimerValue < 0)
	  TimerValue=0;
  Server.send(200, "text/plain", ""); //Send web page
}

void ProcessPreheatTimerStart() {
  PREHEAT_TIMERSTART = !PREHEAT_TIMERSTART;
  Serial.printf("ProcessPreheatTimerStart  %d\n",PREHEAT_TIMERSTART);
  //digitalWrite(PIN_RELAY_MIXER_DIR, MIXDIR);
  // if 1 start timer
  if (PREHEAT_TIMERSTART)
  {
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
      PREHEAT=true;
      if (HEATERPWR)
	ProcessButtonHeaterPwr();

      // Position servo to proper temp
      servoPosNew=PreheatServoPos;
      PreheatTimerValue = PreheatTimerStartValue;
      Serial.print("ProcessPreheatTimerStart Preheat Starting Timer: "); Serial.println(PreheatTimerStartValue);
      timerAlarmEnable(UtilTimer);
      timerStart(UtilTimer);
    } else {
      Serial.print("ProcessPreheatTimerStart Preheat Timer Not SET!: "); Serial.println(PreheatTimerStartValue);
      PREHEAT_TIMERSTART=false;
      PREHEAT=false;
      Server.send(200, "text/plain", "Timer Not Set"); //Send web page
    }
  }
  else
  {
    //reset timer display
    PreheatTimerStartValue = PreTimerMin * 60 + PreTimerSec;
    PreheatTimerValue = PreheatTimerStartValue;
    // if not roasting turn heater off and reset servo position
    if( !ROAST)
    {
      if (!HEATERPWR) // if heater is on turn it off
      ProcessButtonHeaterPwr();
      servoPosNew=0;
    }
    ClearUtilTimer();
    Serial.print("ProcessPreheatTimerStart Preheat Timer Button  Stop timer"); Serial.println(PREHEAT_TIMERSTART);
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

void SetupRoastingLog()
{
  String fileName;
  char buff[64]= {'\0'};
  char * cdata_p=&buff[0];
  RoastLogFile=SetRoastFilename();
  fileName=RoastLogFile;
  String tempType;
  if (TempPref)
	  tempType="degrees F";
  else
	  tempType="degrees C";

  if (SPIFFS.exists(fileName)  )
  {
    deleteFile(SPIFFS,fileName.c_str());
  }
  Serial.printf("SetupRoastingLog file name : %s\n",fileName.c_str());
  strcpy(buff, "{\n\t\"Roasting Log Date\": ");
  writeFile(SPIFFS,fileName.c_str(), cdata_p);
  Serial.printf("SetupRoastingLog get_date\n");
  sprintf(buff, "\"%s\",\n",get_date_string().c_str());
  appendFile(SPIFFS,fileName.c_str(), cdata_p);
  sprintf(buff, "\t\"Bean Quantity (ounces)\": %d,\n",BeanQuantity);
  appendFile(SPIFFS,fileName.c_str(), cdata_p);
  Serial.printf("SetupRoastingLog Coffee Type <%s>\n",CoffeeType);
  sprintf(buff, "\t\"Coffee Type\": \"%s\",\n",CoffeeType);
  appendFile(SPIFFS,fileName.c_str(), cdata_p);
  sprintf(buff, "\t\"Coffee Opt\": %d,\n",CoffeeOpt);
  appendFile(SPIFFS,fileName.c_str(), cdata_p);
  Serial.printf("SetupRoastingLog preheat time %d\n",PreheatTimerStartValue);
  sprintf(buff, "\t\"Preheat Time (sec)\": %d,\n",PreheatTimerStartValue);
  appendFile(SPIFFS,fileName.c_str(), cdata_p);
  Serial.printf("SetupRoastingLog preheat temp %d\n",PreheatTemp);
  sprintf(buff, "\t\"Preheat Temp %s \": %d,\n",tempType.c_str(),PreheatTemp);
  appendFile(SPIFFS,fileName.c_str(), cdata_p);
  sprintf(buff, "\t\"Preheat Servo Pos\": %d,\n",PreheatServoPos);
  appendFile(SPIFFS,fileName.c_str(), cdata_p);
  Serial.printf("SetupRoastingLog Roast time %d\n",TimerStartValue);
  sprintf(buff, "\t\"Roast Time (sec)\": %d,\n",TimerStartValue);
  appendFile(SPIFFS,fileName.c_str(), cdata_p);
  Serial.printf("SetupRoastingLog final temp\n");
  sprintf(buff, "\t\"Finish Temp %s \": %d,\n",tempType.c_str(),FinishTemp);
  appendFile(SPIFFS,fileName.c_str(), cdata_p);
  sprintf(buff, "\t\"Finish Servo Pos\": %d,\n",FinishServoPos);
  appendFile(SPIFFS,fileName.c_str(), cdata_p);
  Serial.printf("SetupRoastingLog Configuration Date <%s>\n",Configuration_Date.c_str());
  sprintf(buff, "\t\"Temp Config File Date\": \"%s\",\n",Configuration_Date.c_str());
  appendFile(SPIFFS,fileName.c_str(), cdata_p);
  Serial.printf("SetupRoastingLog set heatgun speed %d\n",HEATGUNHIGH);
  if (HEATGUNHIGH == 1)
    sprintf(buff, "\t\"Heatgun Speed\": \"High\",\n");
  else
    sprintf(buff, "\t\"Heatgun Speed\": \"Low\",\n");
  appendFile(SPIFFS,fileName.c_str(), cdata_p);
  
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
    sprintf(buff,"\t],\n"); // close roast_steps array
    appendFile(SPIFFS,fileName.c_str(), cdata_p);
    sprintf(buff, "\t\"Roasting was stopped manually at\": \"%s\"\n}\n",get_date_string().c_str());
    appendFile(SPIFFS,fileName.c_str(), cdata_p);
  }
  else
  {
    // stopped because roasting was finished
    // this closes roast_steps array "]" and then the json with "}"
    sprintf(buff,"\n\t]\n}\n");
    appendFile(SPIFFS,fileName.c_str(), cdata_p);
  }

}

void readConfigData()
{
  String fileName;
  if (HEATGUNHIGH)
	  fileName=ConfigHighFile;
  else
	  fileName=ConfigFile;

  if (SPIFFS.exists(fileName)  )
  {
    File file = SPIFFS.open(fileName,"r"); 
    if (!file || file.isDirectory()){
      Serial.print("SendTempConfigData - failed to open file : ");Serial.println(fileName);
      return;
    }
    parseJsonFile(fileName);
    PreheatServoPos=getServoPos(PreheatTemp);
  }
}

// Preheat
// Call with roast = false first=true for first pass 
void UpdateRoastingLog(bool roast, bool first , bool last, bool stop)
{
	// I changed this so I could do roast without preheating
	// but it broke the logging stuff Didn't put in the 
	// "preheat_steps":[  or "roast_steps":[ t
  String fileName;
  char buff[64]= {'\0'};
  char * cdata_p=&buff[0];
  static bool endPreheat=false;
  fileName=RoastLogFile;
  
  Serial.printf("---UpdateRoastingLog file roast %d first %d endPreheat %d last %d stop %d\n",roast,first,endPreheat,last,stop);

  // If not roast and first is true then set end preheat to false so preheat entries get added
 // if (!roast && first)
 //   endPreheat=false;
   
  if (PREHEAT)
  {
	  Serial.printf("Preheat steps\n");
    // This starts the preheat_steps array
    if(!roast && first){
	  Serial.printf("** First Preheat step ***\n");
      sprintf(buff, "\t\"preheat_steps\": [\n");
      appendFile(SPIFFS,fileName.c_str(), cdata_p);
    }
    sprintf(buff, "\t{\n\t\t\"PREHEAT_TIME\": %d,\n", PreheatTimerValue);
    appendFile(SPIFFS,fileName.c_str(), cdata_p);
  } 
  else if (ROAST) 
  {
    // This closes the preheat_steps array and starts the roast_steps array	  
    //if (roast && !endPreheat && first ){
    if (roast && first ){
	  Serial.printf("** First Roast step ***\n");
      //sprintf(buff, "\n\t],\n");
      //appendFile(SPIFFS,fileName.c_str(), cdata_p);
      sprintf(buff, "\n\t\"roast_steps\": [\n");
      appendFile(SPIFFS,fileName.c_str(), cdata_p);
      //endPreheat=true;
    }
   // else if (!endPreheat && first ){
//	  Serial.printf("** >>>> First Preheat step ***\n");
//      sprintf(buff, "\t\"preheat_steps\": [\n\t{\n");
//      appendFile(SPIFFS,fileName.c_str(), cdata_p);
//      sprintf(buff, "}\n\t],\n");
//      appendFile(SPIFFS,fileName.c_str(), cdata_p);
//      sprintf(buff, "\n\t\"roast_steps\": [\n");
//      appendFile(SPIFFS,fileName.c_str(), cdata_p);
//      endPreheat=true;
//    }
     sprintf(buff, "\t{\n\t\t\"ROAST_TIME\": %d,\n", TimerValue);
    appendFile(SPIFFS,fileName.c_str(), cdata_p);
  }
  else if (stop) {
	  // If stopped in the middle add one more of these to 
	  // make json file valid 
    sprintf(buff, "\t{\n\t\t\"ROAST_TIME\": %d,\n", TimerValue);
    appendFile(SPIFFS,fileName.c_str(), cdata_p);
  }
    sprintf(buff, "\t\t\"TMPF\": %.2f,\n", tempF);
    appendFile(SPIFFS,fileName.c_str(), cdata_p);
    sprintf(buff, "\t\t\"TMPC\": %.2f,\n", tempC);
    appendFile(SPIFFS,fileName.c_str(), cdata_p);
    sprintf(buff, "\t\t\"POS\": %d\n", servoPos);
    appendFile(SPIFFS,fileName.c_str(), cdata_p);
    sprintf(buff, "\t}");
    appendFile(SPIFFS,fileName.c_str(), cdata_p);
    // put in a comma before next entry 
    if ( !last )
    {
      Serial.printf("UpdateRoastingLog adding comma file first %d endPreheat %d last %d\n",first,endPreheat,last);
      //sprintf(buff, ", first %d last %d\n", first , last);
      sprintf(buff, ",\n");
      appendFile(SPIFFS,fileName.c_str(), cdata_p);
    }
    else if (!roast && last)
    {
      Serial.printf("UpdateRoastingLog last preheat step roast %d endPreheat %d last %d\n",roast,endPreheat,last);
      sprintf(buff, "\n\t],\n");
      appendFile(SPIFFS,fileName.c_str(), cdata_p);

    } else {
      Serial.printf("UpdateRoastingLog adding newline only file first %d endPreheat %d last %d\n",first,endPreheat,last);
      //sprintf(buff, " first %d last %d\n", first,last);
      sprintf(buff, "\n");
      appendFile(SPIFFS,fileName.c_str(), cdata_p);
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


