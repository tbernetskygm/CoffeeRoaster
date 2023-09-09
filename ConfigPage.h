#include <AutoConnect.h>
//#include "CommonPageData.h"
#include "FileSystemFunctions.h"

void handleConfigPage() {
  // Add new way to get data from SPIFFS
  File file = SPIFFS.open("/SysConfig.htm","r"); 
  if (!file || file.isDirectory()){
    Serial.println("handleConfigPage - failed to open file for reading");
    return;
  }
  Server.streamFile(file,"text/html");
  file.close();

} 

void handleGraphsPage() {
  // Add new way to get data from SPIFFS
  String fileName="/Graphs.htm";
  File file = SPIFFS.open(fileName,"r"); 
  if (!file || file.isDirectory()){
    Serial.print("handleConfigPage - failed to open file : ");Serial.println(fileName);
    return;
  }
  Server.streamFile(file,"text/html");
  file.close();

} 


void handleChartsJsPage() {
  // Add new way to get data from SPIFFS
  String fileName="/charts.js.gz";
  File file = SPIFFS.open(fileName,"r"); 
  if (!file || file.isDirectory()){
    Serial.print("handleChartsJsPage - failed to open file : ");Serial.println(fileName);
    return;
  }
  
  Server.sendHeader(F("Content-Encoding"), F("gzip"));
  Server.streamFile(file,"application/x-gzip");
  
  file.close();

} 


void handleGraphsJsPage() {
  // Add new way to get data from SPIFFS
  String fileName="/graphs.js.gz";
  File file = SPIFFS.open(fileName,"r"); 
  if (!file || file.isDirectory()){
    Serial.print("handleGraphsJsPage - failed to open file : ");Serial.println(fileName);
    return;
  }

  Server.sendHeader(F("Content-Encoding"), F("gzip"));
  Server.streamFile(file,"application/x-gzip");
  
  file.close();

} 


void handleFilesPage() {
  // Add new way to get data from SPIFFS
  Serial.printf("handleFilesPage\n");
  String fileName="/Files.htm";
  File file = SPIFFS.open(fileName,"r"); 
  if (!file || file.isDirectory()){
    Serial.print("handleFilesPage - failed to open file : ");Serial.println(fileName);
    return;
  }
  Server.streamFile(file,"text/html");
  file.close();
} 

