
#include <AutoConnect.h>
#include "FileSystemFunctions.h"

void handleNewRoot() {
  //AUTOCONNECT_LINK(BAR_24)
  // Add new way to get data from SPIFFS
  File file = openFile("/Roast.htm","r"); 
  if (!file || file.isDirectory()){
    Serial.println("handleNewRoot - failed to open file for reading");
    Serial.println("WebPage.h getting directory listing");
    listDir("/", 0);
    return;
  }
  Server.streamFile(file,"text/html");
  file.close();

} 

void sendStyle() {
  
  // Add new way to get data from SPIFFS
  File file = openFile("/style.css","r"); 
  if (!file || file.isDirectory()){
    Serial.println("sendStyle - failed to open file for reading");
    return;
  }
  Server.streamFile(file,"text/css");
  file.close();

} 
