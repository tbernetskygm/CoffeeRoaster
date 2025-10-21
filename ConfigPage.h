#ifdef AUTO_CONNECT
#include <AutoConnect.h>
#endif
//#include "CommonPageData.h"
#include "FileSystemFunctions.h"

void handleConfigPage(AsyncWebServerRequest *request) {
  // Add new way to get data from SPIFFS
  String fileName="/SysConfig.htm";
  File file = openFile(fileName,"r"); 
  if (!file || file.isDirectory()){
    Serial.println("handleConfigPage - failed to open file for reading");
    return;
  }
  #ifndef NEW_WIFI
  Server.streamFile(file,"text/html");
  #else
  //AsyncWebServerRequest *request; 
  request->send(LittleFS,fileName,"text/html");
  #endif
  file.close();

} 

void handleGraphsPage(AsyncWebServerRequest *request) {
  // Add new way to get data from SPIFFS
  String fileName="/Graphs.htm";
  File file = openFile(fileName,"r"); 
  if (!file || file.isDirectory()){
    Serial.print("handleConfigPage - failed to open file : ");Serial.println(fileName);
    return;
  }
  #ifndef NEW_WIFI
  Server.streamFile(file,"text/html");
  #else
  request->send(LittleFS,fileName,"text/html");
  #endif
  file.close();

} 


void handleChartsJsPage(AsyncWebServerRequest *request) {
  // Add new way to get data from SPIFFS
  String fileName="/charts.js.gz";
  File file = openFile(fileName,"r"); 
  if (!file || file.isDirectory()){
    Serial.print("handleChartsJsPage - failed to open file : ");Serial.println(fileName);
    return;
  }
  #ifndef NEW_WIFI
  Server.sendHeader(F("Content-Encoding"), F("gzip"));
  Server.streamFile(fileName,"application/x-gzip");
  #else
  AsyncWebServerResponse *response = request->beginResponse(200,"application/x=gzip");
  //FIXME
  response->addHeader(F("Content-Encoding"), F("gzip"));
  request->send(response);
  #endif
  file.close();

} 


void handleGraphsJsPage(AsyncWebServerRequest *request) {
  // Add new way to get data from SPIFFS
  String fileName="/graphs.js.gz";
  File file = openFile(fileName,"r"); 
  if (!file || file.isDirectory()){
    Serial.print("handleGraphsJsPage - failed to open file : ");Serial.println(fileName);
    return;
  }
  #ifndef NEW_WIFI
  Server.sendHeader(F("Content-Encoding"), F("gzip"));
  Server.streamFile(fileName,"application/x-gzip");
  #else
  
  AsyncWebServerResponse *response = request->beginResponse(200,"application/x=gzip");
  //FIXME
  response->addHeader(F("Content-Encoding"), F("gzip"));
  request->send(response);
  #endif
  file.close();

} 


void handleFilesPage(AsyncWebServerRequest *request) {
  // Add new way to get data from SPIFFS
  Serial.printf("handleFilesPage\n");
  String fileName="/Files.htm";
  File file = openFile(fileName,"r"); 
  if (!file || file.isDirectory()){
    Serial.print("handleFilesPage - failed to open file : ");Serial.println(fileName);
    return;
  }
  #ifndef NEW_WIFI
  Server.streamFile(file,"text/html");
  #else
  request->send(LittleFS,fileName,"text/html");
  #endif
  file.close();
} 
