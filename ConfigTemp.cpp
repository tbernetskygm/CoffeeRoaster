#include "ProjectDefines.h"
//#ifdef SPIFFS_FS
//#include <SPIFFS.h>
//#define _FSYS SPIFFS
//#elif def LITTLEFS_FS
//#include <LittleFS.h>
//#define _FSYS LittleFS
//#endif
//#include <LittleFS.h>
// Code to run temp configuration
#include <ESP32Servo.h>

#include "Globals.h"
#include "GlobalStructs.h"
#include "RoasterControls.h"
#include "ConfigTemp.h"
#include "UtilTimer.h"
#include "FileSystemFunctions.h"
#include "UtilityFunctions.h"
#define SENDJSON
//char tempXML[1024];
// ISR for TempConfig Timer
void RunTempConfig (int Step)
{
#ifndef SENDJSON
  String fileName="/TempConfig.xml";
#else
  String fileName;
#endif
  char buff[64]= {'\0'};
  char * cdata_p=&buff[0];
  if (HEATGUNHIGH)
	  fileName=ConfigHighFile;
  else
	  fileName=ConfigFile;
  Serial.print("Begin RunTempConfig Step :");Serial.println(Step);
  Serial.print("\tRunTempConfig ConfigMaxSteps :");Serial.println(ConfigMaxSteps);
  //Serial.print("RunTempConfig called tempXML length :");Serial.println(strlen(tempXML));
  // Get Avg Temps
  tempAvgF=CalcAvgTemp(tempTotalF,ConfigTimerStartValue);
  tempAvgC=CalcAvgTemp(tempTotalC,ConfigTimerStartValue);
  
  if (Step >= 0 ){
    Serial.print("RunTempConfig step :");Serial.println(Step);
    //strcpy(buff, "{ "); 
    //appendFile(LittleFS,fileName.c_str(), cdata_p);
    sprintf(buff, "{\n\"NUM\": %d,\n", Step);
    appendFile(fileName.c_str(), cdata_p);
    sprintf(buff, "\"TMPF\": %.2f,\n", tempAvgF);
    appendFile(fileName.c_str(), cdata_p);
    sprintf(buff, "\"TMPC\": %.2f,\n", tempAvgC);
    appendFile(fileName.c_str(), cdata_p);
    sprintf(buff, "\"POS\": %d\n", servoPos);
    appendFile(fileName.c_str(), cdata_p);
    if (Step > ConfigMaxSteps) 
      strcpy(buff, "}\n");
    else 
      strcpy(buff, "},\n"); 

    appendFile(fileName.c_str(), cdata_p);

    servoPosNew=servoPos+ServoPosInc;
    Serial.print("\tRunTempConfig ServoPosInc =");Serial.println(ServoPosInc);
   // Serial.print("\tRunTempConfig ServoPos =");Serial.println(servoPos);
   // Serial.print("\tRunTempConfig ServoPosNew =");Serial.println(servoPosNew);
    //Serial.print("RunTempConfig Finished step tempXML length :");Serial.println(strlen(tempXML));
    //        // Reset values for next pass
    tempTotalC=0;
    tempTotalF=0;
    ConfigTimerValue = ConfigTimerStartValue;
  }

  if (Step > ConfigMaxSteps) {
    Serial.print("RunTempConfig Done Send xml Step :");Serial.println(Step);
    //strcat(tempXML, "</ConfigData>\n");

    sprintf(buff,"]\n}\n");
    appendFile(fileName.c_str(), cdata_p);

    //Serial.print("RunTempConfig Done Send xml length :");Serial.println(strlen(tempXML));
    servoPosNew=0;
    ServoPosInc=0;
    ClearConfigTimer(); 
    // turn off heater
    ProcessButtonHeaterPwr();
    // set servo back to 0
    servoPosNew=0;
  } 
}

void SendTempConfigData()
{
  listDir( "/", 0);
#ifndef SENDJSON
  String fileName="/TempConfig.xml";
#else
  String fileName;
#endif
  if (HEATGUNHIGH)
	  fileName=ConfigHighFile;
  else
	  fileName=ConfigFile;

  //if (LittleFS.exists(fileName)  )
  //{
  File file = openFile(fileName.c_str(),"r"); 
  if (!file || file.isDirectory()){
    Serial.print("SendTempConfigData - failed to open file : ");Serial.println(fileName);
    //Server.send(201, "text/plain", "No Config Data available!!");
    #ifndef SENDJSON
    Server.send(200, "text/xml", "<?xml version = '1.0'?>\n<ConfigData>\n</ConfigData>\n");
#else
    Server.send(200, "application/json", " {\"NUM\": \"No Data\" } ");
#endif

    return;
  }
#ifndef SENDJSON
  //Server.streamFile(file,"text/xml");
#else
  Server.streamFile(file,"application/json");
  //Server.send(200, "text/plain", "Parsing Config Data");
  parseJsonFile(fileName);
  PreheatServoPos=getServoPos(PreheatTemp);
#endif
  //file.close();
    
  //} else {
  //  //Server.send(200, "text/xml", "<?xml version = '1.0'?>\n<ConfigData>\n<msg>No Config Data</msg>\n</ConfigData>\n");
//#ifndef SENDJSON
  //  Server.send(200, "text/xml", "<?xml version = '1.0'?>\n<ConfigData>\n</ConfigData>\n");
//#else
  //  Server.send(200, "application/json", " {\"NUM\": \"No Data\" } ");
//#endif
  //}
}

void ProcessTempProbe() {
  String t_state = Server.arg("VALUE");
  Serial.print("ProcessTempProbe "); Serial.println(t_state);
  TempSensorKOhms = t_state.toInt();
  Serial.print("ProcessTempProbe "); Serial.println(TempSensorKOhms);
  strcpy(buf, "");
  sprintf(buf, "%d", TempSensorKOhms);
  sprintf(buf, buf);
  Server.send(200, "text/plain", buf); //Send web page
}


void ProcessConfigMaxSteps() {
  String t_state = Server.arg("VALUE");
  ConfigMaxSteps = t_state.toInt();
  Serial.print("ProcessConfigMaxSteps ConfigMaxSteps "); Serial.println(ConfigMaxSteps);
  //Serial.print("ProcessConfigMaxSteps ConfigMaxStepsNew: "); Serial.println(ConfigMaxStepsNew);
  sprintf(buf, "%d", ConfigMaxSteps);
  Server.send(200, "text/plain", buf); //Send web page
}

void SetupConfigTest()
{
  String fileName;

  char buff[64]= {'\0'};
  char * cdata_p=&buff[0];
  if (HEATGUNHIGH)
	  fileName=ConfigHighFile;
  else
	  fileName=ConfigFile;

  if (exists(fileName)  )
  {
    deleteFile(fileName.c_str());
  }
  strcpy(buff, "{\n\"Configuration Date\": ");
  writeFile(fileName.c_str(), cdata_p);
  sprintf(buff, "\"%s\",\n\"steps\": [\n",get_date_string().c_str());
  appendFile(fileName.c_str(), cdata_p);

  // calculate servo positions based on ConfigMaxSteps and ConfigSteps
 
  tempTotalC=0;
  tempTotalF=0;
  ServoPosInc=SERVO_MAX_STEPS/ConfigMaxSteps;
 // Serial.print("SetupConfigTest ServoPosInc =");Serial.println(ServoPosInc);
 // Serial.print("SetupConfigTest ServoPos =");Serial.println(servoPos);
  servoPosNew=servoPos+ServoPosInc;
  //Serial.print("SetupConfigTest ServoPosNew =");Serial.println(servoPosNew);

}

void ProcessTempConfig() {
  TEMP_CONFIG = !TEMP_CONFIG;
  //String t_state = Server.arg("VALUE");
  //ConfigMaxStepsNew = t_state.toInt();
  Serial.print("ProcessTempConfig received ");Serial.println(TEMP_CONFIG); 
  if (TEMP_CONFIG)
  {
    // turn on heater
    ProcessButtonHeaterPwr();
    ConfigSteps = ConfigMaxSteps;
    ConfigStep=0;
    SetupConfigTest();
    SetupConfigTimer();
    // set the timer start value
    ConfigTimerMin=0;
    ConfigTimerSec=20;
    ConfigElaspedTime=0;// in seconds
    //ConfigTimerStartValue = ConfigTimerMin * 60 + ConfigTimerSec;
    ConfigTimerValue = ConfigTimerStartValue;
    timerAlarmEnable(UtilTimer);
    timerStart(UtilTimer);
    //RunTempConfig(ConfigStep);
  } else {
    ClearConfigTimer();
    // turn off heater
    ProcessButtonHeaterPwr();
  }
  Server.send(200, "text/plain", "TempConfig started"); //Send web page
}


void ProcessConfigStepTime() {
  String t_state = Server.arg("VALUE");
  ConfigTimerStartValue = t_state.toInt();
  //Serial.print("ProcessConfigStepTime ConfigTimerStartValue "); Serial.println(ConfigTimerStartValue);
  //Serial.print("ProcessConfigMaxSteps ConfigMaxStepsNew: "); Serial.println(ConfigMaxStepsNew);
  sprintf(buf, "%d", ConfigTimerStartValue);
  Server.send(200, "text/plain", buf); //Send web page
}

void UpdateHeatGunSlider() {
  String t_state = Server.arg("VALUE");

  // convert the string sent from the web page to an int
  int tmp  = t_state.toInt();
  Serial.print("UpdateHeatgunSlider "); Serial.println(tmp);
  if (tmp == 0)
	  
	  HEATGUNHIGH=false;
  else
	  HEATGUNHIGH=true;

  sprintf(buf, "%d", tmp);
  // now send it back
  Server.send(200, "text/plain", buf); //Send web page

}


void SendRoastLogData()
{
  listDir( "/", 0);
  String fileName;
  String reqFile = "";
  reqFile = Server.arg("VALUE");

  if (reqFile.length() > 0)
    fileName=reqFile;
  else
    fileName=RoastLogFile;

  Serial.printf("SendRoastLogData -  fileName : <%s> reqFile <%s>\n",fileName.c_str(),reqFile.c_str());

  if (exists(fileName)  )
  {
    File file = openFile(fileName.c_str(),"r"); 
    if (!file || file.isDirectory()){
      Serial.print("SendRoastLogData - failed to open file : ");Serial.println(fileName);
      Server.send(201, "text/plain", "No Config Data available!!");
      return;
    }
    Server.streamFile(file,"application/json");
  } else {
    Serial.print("SendRoastLogData - file does not exist file : ");Serial.println(fileName);
    Server.send(200, "application/json", " {\"NUM\": \"No Data\" } ");
  }
}
