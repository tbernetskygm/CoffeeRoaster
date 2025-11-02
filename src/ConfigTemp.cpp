#include "ProjectDefines.h"
//#ifdef SPIFFS_FS
//#include <SPIFFS.h>
//#define _FSYS SPIFFS
//#elif def LITTLEFS_FS
//#include <LittleFS.h>
//#define _FSYS LittleFS
//#endif
#include <LittleFS.h>
// Code to run temp configuration
#include <ESP32Servo.h>

//#include "ProjectDefines.h"
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
  String fileData;
  if (HEATGUNHIGH)
	  fileName=ConfigHighFile;
  else
	  fileName=ConfigFile;
  //WHENDEBUG(2)
  //{
    //Serial.print("Begin RunTempConfig Step :");Serial.println(Step);
    //Serial.print("\tRunTempConfig ConfigMaxSteps :");Serial.println(ConfigMaxSteps);
  //}
  //Serial.print("RunTempConfig called tempXML length :");Serial.println(strlen(tempXML));
  // Get Avg Temps
  tempAvgF=CalcAvgTemp(tempTotalF,ConfigTimerStartValue);
  tempAvgC=CalcAvgTemp(tempTotalC,ConfigTimerStartValue);
  
  sprintf(buff, "\0");
  if (Step >= 0 ){
    //WHENDEBUG(2)
      //Serial.print("RunTempConfig step :");Serial.println(Step);
    //strcpy(buff, "{ "); 
    //appendFile(LittleFS,fileName.c_str(), cdata_p);
    //sprintf(buf, "{\n\"NUM\": %d,\n", Step);
    //strcat(buff,buf);
    fileData = "{\n\"NUM\" :" + String(Step) + ",\n";
    //WHENDEBUG(5)
      //Serial.printf("** buff size %d\n",strlen(buff));
    //appendFile(fileName.c_str(), cdata_p);
    //sprintf(buf, "\"TMPF\": %.2f,\n", tempAvgF);
    //strcat(buff,buf);
    fileData+= "\"TMPF\": " + String(tempAvgF) + ",\n";
    //WHENDEBUG(5)
      //Serial.printf("** buff size %d\n",strlen(buff));
    //appendFile(fileName.c_str(), cdata_p);
    //sprintf(buf, "\"TMPC\": %.2f,\n", tempAvgC);
    //strcat(buff,buf);
    fileData+= "\"TMPC\": " + String(tempAvgC) + ",\n";
    //WHENDEBUG(5)
      //Serial.printf("** buff size %d\n",strlen(buff));
    //appendFile(fileName.c_str(), cdata_p);
    //sprintf(buf, "\"POS\": %d\n", servoPos);
    //strcat(buff,buf);
    fileData+= "\"POS\": " + String(servoPos) + ",\n";
    //WHENDEBUG(5)
      //Serial.printf("** buff size %d\n",strlen(buff));
    //appendFile(fileName.c_str(), cdata_p);
    if (Step > ConfigMaxSteps) 
    {
      //sprintf(buf, "}\n");
      //strcat(buff,buf);
      //WHENDEBUG(5)
        //Serial.printf("** buff size %d\n",strlen(buff));
      fileData += "}\n";
    }
    else
    { 
      //sprintf(buf, "},\n"); 
      //strcat(buff,buf);
      //WHENDEBUG(5)
        //Serial.printf("** buff size %d\n",strlen(buff));
      fileData += "},\n";
    }

    //appendFile(fileName.c_str(), cdata_p);

    servoPosNew=servoPos+ServoPosInc;
    //WHENDEBUG(2)
      //Serial.print("\tRunTempConfig ServoPosInc =");Serial.println(ServoPosInc);
   // Serial.print("\tRunTempConfig ServoPos =");Serial.println(servoPos);
   // Serial.print("\tRunTempConfig ServoPosNew =");Serial.println(servoPosNew);
    //Serial.print("RunTempConfig Finished step tempXML length :");Serial.println(strlen(tempXML));
    //        // Reset values for next pass
    tempTotalC=0;
    tempTotalF=0;
    ConfigTimerValue = ConfigTimerStartValue;
  }

  if (Step > ConfigMaxSteps) {
    //Serial.print("RunTempConfig Done Send xml Step :");Serial.println(Step);
    //strcat(tempXML, "</ConfigData>\n");

    //sprintf(buf,"]\n}\n");
    //strcat(buff,buf);
    //Serial.printf("** buff size %d\n",strlen(buff));
    fileData += "]\n}\n";

    //Serial.print("RunTempConfig Done Send xml length :");Serial.println(strlen(tempXML));
    servoPosNew=0;
    ServoPosInc=0;
    ClearUtilTimer(); 
    // turn off heater
    ProcessButtonHeaterPwr();
    // set servo back to 0
    servoPosNew=0;
  } 
  appendFile(fileName.c_str(), fileData);
}

#ifndef NEW_WIFI
void SendTempConfigData()
#else
void SendTempConfigData(AsyncWebServerRequest *request)
#endif
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
      #ifndef NEW_WIFI
    Server.send(200, "text/xml", "<?xml version = '1.0'?>\n<ConfigData>\n</ConfigData>\n");
      #else
    
    request->send(200,"<?xml version = '1.0'?>\n<ConfigData>\n</ConfigData>\n","text/xml");
      #endif
    #else
      #ifndef NEW_WIFI
    Server.send(200, "application/json", " {\"NUM\": \"No Data\" } ");
      #else
    request->send(200, " {\"NUM\": \"No Data\" } ", "application/json" );
      #endif
#endif //NDEF SENDJSON

    return;
  }
#ifndef SENDJSON
  //Server.streamFile(file,"text/xml");
#else
   #ifndef NEW_WIFI
  Server.streamFile(file,"application/json");
  #else
  request->send(LittleFS, fileName, "application/json");
  #endif
  //Server.send(200, "text/plain", "Parsing Config Data");
  parseJsonFile(fileName);
  PreheatServoPos=getServoPos(PreheatTemp);
#endif
}

#ifndef NEW_WIFI
void ProcessTempProbe() {
  String t_state = Server.arg("VALUE");
#else
void ProcessTempProbe(AsyncWebServerRequest *request) {
  String t_state ;
  char buf[64];
  char * buf_p = &buf[0];
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
#endif
  //Serial.print("ProcessTempProbe "); Serial.println(t_state);
  TempSensorKOhms = t_state.toInt();
  //Serial.print("ProcessTempProbe "); Serial.println(TempSensorKOhms);
  strcpy(buf, "");
  sprintf(buf, "%d", TempSensorKOhms);
  sprintf(buf, buf);
  #ifndef NEW_WIFI
  Server.send(200, "text/plain", buf); //Send web page
  #else
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", String(buf));
  request->send(resp);
  #endif
}


#ifndef NEW_WIFI
void ProcessConfigMaxSteps() {
  String t_state = Server.arg("VALUE");
#else
void ProcessConfigMaxSteps(AsyncWebServerRequest *request) {
  String t_state ;
  char buf[64];
  char * buf_p = &buf[0];
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
#endif
  ConfigMaxSteps = t_state.toInt();
  Serial.print("ProcessConfigMaxSteps ConfigMaxSteps "); Serial.println(ConfigMaxSteps);
  //Serial.print("ProcessConfigMaxSteps ConfigMaxStepsNew: "); Serial.println(ConfigMaxStepsNew);
  sprintf(buf, "%d", ConfigMaxSteps);
  #ifndef NEW_WIFI
  Server.send(200, "text/plain", buf); //Send web page
  #else
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", String(buf));
  request->send(resp);
  #endif
}

void SetupConfigTest()
{
  String fileName;

  //char buff[64]= {'\0'};
  //char * cdata_p=&buff[0];
  Serial.printf("SetupConfigTest\n");
  String fileData="{\n\"Configuration Date\": ";
  if (HEATGUNHIGH)
	  fileName=ConfigHighFile;
  else
	  fileName=ConfigFile;

  if (exists(fileName)  )
  {
    deleteFile(fileName.c_str());
  }
  //sprintf(buff, "\0");
  //strcpy(buf, "{\n\"Configuration Date\": ");
  //strcat(buff,buf);
  fileData += "\"" + get_date_string() + "\"";
  fileData += "\n\"File Name\" : \"" + fileName + "\","; 
  fileData += ",\n\"steps\": [\n";
  //sprintf(buf, "\"%s\",\n\"steps\": [\n",get_date_string().c_str());
  //strcat(buff,buf);
  writeFile(fileName.c_str(), fileData);
  //appendFile(fileName.c_str(), cdata_p);

  // calculate servo positions based on ConfigMaxSteps and ConfigSteps
 
  tempTotalC=0;
  tempTotalF=0;
  ServoPosInc=SERVO_MAX_STEPS/ConfigMaxSteps;
 // Serial.print("SetupConfigTest ServoPosInc =");Serial.println(ServoPosInc);
 // Serial.print("SetupConfigTest ServoPos =");Serial.println(servoPos);
  servoPosNew=servoPos+ServoPosInc;
  //Serial.print("SetupConfigTest ServoPosNew =");Serial.println(servoPosNew);

  Serial.printf("SetupConfigTest done\n");
}

#ifndef NEW_WIFI
void ProcessTempConfig() {
#else
void ProcessTempConfig(AsyncWebServerRequest *request) {
  char buf[64];
  char * buf_p = &buf[0];
#endif
  TEMP_CONFIG = !TEMP_CONFIG;

  Serial.print("ProcessTempConfig received ");Serial.println(TEMP_CONFIG); 
  if (TEMP_CONFIG)
  {
    rState->doConfig = true;
    // turn on heater
    ProcessButtonHeaterPwr(request);
    ConfigSteps = ConfigMaxSteps;
    ConfigStep=0;
    SetupConfigTest();
    SetupUtilTimer();
    // set the timer start value
    ConfigTimerMin=0;
    ConfigTimerSec=20;
    ConfigElaspedTime=0;// in seconds
    
    ConfigTimerValue = ConfigTimerStartValue;
    //timerAlarm(UtilTimer, 1000000, true,0);
     timerAlarmEnable(UtilTimer);
     timerStart(UtilTimer);
    
  } else {
    ClearUtilTimer();
    rState->doConfig = false;
    // turn off heater
    ProcessButtonHeaterPwr(request);
  }
  #ifndef NEW_WIFI
  Server.send(200, "text/plain", "TempConfig started"); //Send web page
  #else
  request->send(200,buf_p,"text/plain");
  #endif
}


#ifndef NEW_WIFI
void ProcessConfigStepTime() {
  String t_state = Server.arg("VALUE");
#else
void ProcessConfigStepTime(AsyncWebServerRequest *request) {
  char buf[64];
  char * buf_p = &buf[0];
  String t_state;
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
#endif
  ConfigTimerStartValue = t_state.toInt();
  //Serial.print("ProcessConfigStepTime ConfigTimerStartValue "); Serial.println(ConfigTimerStartValue);
  //Serial.print("ProcessConfigMaxSteps ConfigMaxStepsNew: "); Serial.println(ConfigMaxStepsNew);
  sprintf(buf, "%d", ConfigTimerStartValue);
#ifndef NEW_WIFI
  Server.send(200, "text/plain", buf); //Send web page
#else
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", String(buf));
  request->send(resp);
#endif
}

#ifndef NEW_WIFI
void UpdateHeatGunSlider() {
  String t_state = Server.arg("VALUE");
#else
void UpdateHeatGunSlider(AsyncWebServerRequest *request) {
  //Check if GET arg exists
  String t_state;
  char buf[64];
  char * buf_p = &buf[0];
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");

#endif
  // convert the string sent from the web page to an int
  int tmp  = t_state.toInt();
  Serial.print("UpdateHeatgunSlider "); Serial.println(tmp);
  if (tmp == 0)
	  
	  HEATGUNHIGH=false;
  else
	  HEATGUNHIGH=true;

  sprintf(buf, "%d", tmp);
  // now send it back
  #ifndef NEW_WIFI
  Server.send(200, "text/plain", buf); //Send web page
  #else
  request->send(200, buf_p,"text/plain");
  #endif

}

#ifndef NEW_WIFI
void SendRoastLogData()
#else
void SendRoastLogData(AsyncWebServerRequest *request)
#endif
{
  listDir( "/", 0);
  String fileName;
  String reqFile = "";
  String download="false";
#ifndef NEW_WIFI
  reqFile = Server.arg("VALUE");
#else

  if(request->hasParam("VALUE"))
    reqFile = request->getParam("VALUE")->value();
  
  if(request->hasParam("download"))
    download = request->getParam("download")->value();
#endif

  // Handle download if download=true
  if (download.equals("true"))
  {
    Serial.printf("Download file <%s>\n",reqFile);
    loadFromFS(request,reqFile,download);
  }
  else
  {
    if (reqFile.length() > 0)
      fileName=reqFile;
    else
      fileName=RoastLogFile;

    Serial.printf("SendRoastLogData -  fileName : <%s> reqFile <%s> download <%s>\n",fileName.c_str(),reqFile.c_str(), download.c_str());

    if (exists(fileName)  )
    {
      File file = openFile(fileName.c_str(),"r"); 
      if (!file || file.isDirectory()){
        Serial.print("SendRoastLogData - failed to open file : ");Serial.println(fileName);
#ifndef NEW_WIFI
        Server.send(201, "text/plain", "No Config Data available!!");
#else
        request->send(201,"No Config Data Available!", "text/plain");
#endif
        return;
      }
#ifndef NEW_WIFI
      Server.streamFile(file,"application/json");
#else
      request->send(LittleFS, fileName, "application/json");
#endif
    } else {
      Serial.print("SendRoastLogData - file does not exist file : ");Serial.println(fileName);
#ifndef NEW_WIFI
      Server.send(200, "application/json", " {\"NUM\": \"No Data\" } ");
#else
      request->send(200, " {\"NUM\": \"No Data\" } ", "application/json");
#endif
    }
  }
}
