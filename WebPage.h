
#include <AutoConnect.h>

void handleNewRoot() {
  //AUTOCONNECT_LINK(BAR_24)
  // Add new way to get data from SPIFFS
  File file = SPIFFS.open("/Roast.htm","r"); 
  if (!file || file.isDirectory()){
    Serial.println("handleNewRoot - failed to open file for reading");
    return;
  }
  Server.streamFile(file,"text/html");
  file.close();

} 

void sendStyle() {
  
  // Add new way to get data from SPIFFS
  File file = SPIFFS.open("/style.css","r"); 
  if (!file || file.isDirectory()){
    Serial.println("sendStyle - failed to open file for reading");
    return;
  }
  Server.streamFile(file,"text/css");
  file.close();

} 
