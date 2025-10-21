#ifdef AUTO_CONNECT
#include <AutoConnect.h>
#endif

#include "FileSystemFunctions.h"

void handleNewRoot(AsyncWebServerRequest *request) {
  //AUTOCONNECT_LINK(BAR_24)
  Serial.println(F("handleNewRoot"));
  // Add new way to get data from SPIFFS
  String fileName = "/Roast.htm";
  File file = openFile(fileName,"r"); 
  if (!file || file.isDirectory()){
    Serial.println("handleNewRoot - failed to open file for reading");
    Serial.println("WebPage.h getting directory listing");
    listDir("/", 0);
    return;
  }
  #ifndef NEW_WIFI
  Server.streamFile(file,"text/html");
  #else
  
  request->send(LittleFS,fileName,"text/html");
  #endif
  file.close();

} 

void sendStyle(AsyncWebServerRequest *request) {
  
  // Add new way to get data from SPIFFS
  String fileName = "/style.css";
  Serial.println("sendStyle");
  File file = openFile(fileName,"r"); 
  if (!file || file.isDirectory()){
    Serial.println("sendStyle - failed to open file for reading");
    return;
  }
  #ifndef NEW_WIFI
  Server.streamFile(file,"text/css");
  #else
  //AsyncWebServerRequest *request; 
  request->send(LittleFS,fileName,"text/css");
  #endif
  file.close();

} 
