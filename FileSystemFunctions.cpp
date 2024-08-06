#include "ProjectDefines.h"
#include <FS.h>
#ifdef SPIFFS_FS
#include <SPIFFS.h>
#define _FSYS SPIFFS
#elif defined LITTLEFS_FS
#include <LittleFS.h>
#define _FSYS LittleFS
#endif
#include <stdio.h>
#include "Globals.h"
#include "FileSystemFunctions.h"

void listDir( const char * dirname, uint8_t levels){
   Serial.printf("FileSystemFunctions::listDir Listing directory: %s\r\n", dirname);

   File root = _FSYS.open(dirname);
   if(!root){
      Serial.println("− failed to open directory");
      return;
   }
   if(!root.isDirectory()){
      Serial.println(" − not a directory");
      return;
   }

   File file = root.openNextFile();
   while(file){
      if(file.isDirectory()){
         Serial.print("  DIR : ");
         Serial.println(file.name());
         if(levels){
            listDir(file.name(), levels -1);
         }
      } else {
         Serial.print("  FILE: ");
         Serial.print(file.name());
         Serial.print("\tSIZE: ");
         Serial.println(file.size());
      }
      file = root.openNextFile();
   }
}

void returnFail(String msg) {
  Server.send(500, "text/plain", msg + "\r\n");
}

void printDirectory() {
  Serial.printf("printDirectory\n");
  if (!Server.hasArg("dir")) {
    return returnFail("BAD ARGS");
  }
  String path = Server.arg("dir");
  Serial.printf("printDirectory path %s\n",path.c_str());
  if (path != "/" && !_FSYS.exists((char *)path.c_str())) {
    return returnFail("BAD PATH");
  }
  File dir = _FSYS.open(path);
  path = String();
  if (!dir.isDirectory()) {
    dir.close();
    return returnFail("NOT DIR");
  }
  dir.rewindDirectory();
  Server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  Server.send(200, "text/json", "");
  //WiFiClient client = Server.client();

  Server.sendContent("[");
  for (int cnt = 0; true; ++cnt) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }

    String output;
    if (cnt > 0) {
      output = ',';
    }

    output += "{\"type\":\"";
    output += (entry.isDirectory()) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += entry.path();
    output += "\"";
    output += "}";
    Server.sendContent(output);
    entry.close();
  }
  Server.sendContent("]");
  dir.close();
}

bool loadFromFS(String path){

  Serial.printf("loadFromFS useFS %d useSDCARD %d path %s\n", useFS,useSDCARD,path.c_str());
  //path.toLowerCase();
  //String dataType = "text/plain";
  String dataType = getContentType(path);
  if(path.endsWith("/")) path += "index.htm";

  Serial.printf("loadFromFS file %s dataType %s\n", path.c_str(),dataType);
  File dataFile = openFile(path.c_str());

  if (!dataFile)
    return false;

  Serial.printf("loadFromFS opened file %s sending...\n", path.c_str());
  if (Server.streamFile(dataFile, dataType) != dataFile.size()) {
    Serial.println("Sent less data than expected!");
  }

  dataFile.close();
  return true;
}

bool exists(String path){
  bool yes = false;
  File file = _FSYS.open(path, "r");
  if(!file.isDirectory()){
    yes = true;
  }
  file.close();
  return yes;
}

void handleNotFound(String file, bool portal){
  String message;
  String path;
  Serial.printf("handleNotFound useFS %d useSDCARD %d portal %d\n", useFS,useSDCARD,portal);
  Serial.printf("handleNotFound portal file: %s\n", file.c_str());
  //if (!portal)
 // {
    for (uint8_t i=0; i<Server.args(); i++){
      message += " NAME:"+Server.argName(i) + "\n VALUE:" + Server.arg(i) + "\n";
    }
  Serial.printf("handleNotFound Server args: %s\n", message.c_str());
  if (!portal)
    path = Server.arg("path");
  else
    path = file;

  Serial.printf("handleNotFound path: %s Server.uri() %s \n", path.c_str(), Server.uri().c_str());
  if (useFS)
  {
    if(loadFromFS(path)) return;
  }
#ifdef SD_FS
  if (useSDCARD)
  {
    if(loadFromFS(SD,path)) return ;
  }
#endif
  // This is an error 
  message = "FS Not Detected\n\n";
  message += "URI: ";
  message += Server.uri();
  message += "\nMethod: ";
  message += (Server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += Server.args();
  message += "\n";
  for (uint8_t i=0; i<Server.args(); i++){
    message += " NAME:"+Server.argName(i) + "\n VALUE:" + Server.arg(i) + "\n";
  }
  Server.send(404, "text/plain", message);
  Serial.println(message);

}

File openFile( const char * path, const char * mode){
   Serial.printf("Opening file: %s mode %s \n", path,mode);
   char test[]="r";
   File file;
   int rd=strcmp(mode,test);
   Serial.printf("Opening file: %s mode %s FILE_READ %s rd %d\n", path,mode,FILE_READ,rd);
   if(rd == 0 )
   {
    file = LittleFS.open(path,FILE_READ);
   }
   else
   {
    file = LittleFS.open(path,FILE_WRITE);
   }
   if(!file || file.isDirectory()){
       Serial.printf("− failed to open file for mode %s \n",mode);
       return file ;
   }
   return file;
}

File openFile( String path, const char * mode){
   Serial.printf("Opening file: %s mode %s \n", path.c_str(),mode);

   File file = _FSYS.open(path,mode);
   if(!file || file.isDirectory()){
       Serial.printf("− failed to open file for mode %s\n",mode);
       return file ;
   }
   return file;
}

void readFile(const char * path){
   Serial.printf("Reading file: %s\r\n", path);

   File file = _FSYS.open(path);
   if(!file || file.isDirectory()){
       Serial.println("− failed to open file for reading");
       return;
   }

   Serial.println("− read from file:");
   while(file.available()){
      Serial.write(file.read());
   }
}

void writeFile(const char * path, const char * message){
   Serial.printf("Writing file: %s\r\n", path);

   File file = _FSYS.open(path, FILE_WRITE);
   if(!file){
      Serial.println("− failed to open file for writing");
      return;
   }
   if(file.print(message)){
      Serial.println("− file written");
   }else {
      Serial.println("− write failed");
   }
}

void appendFile( const char * path, const char * message){
   //Serial.printf("Appending to file: %s\r\n", path);

   File file = _FSYS.open(path, FILE_APPEND);
   if(!file){
      Serial.println("− failed to open file for appending");
      return;
   }
   if(!file.print(message)){
   //   Serial.println("− message appended");
   //} else {
      Serial.println("− append failed");
   }
}

void renameFile(const char * path1, const char * path2){
   Serial.printf("Renaming file %s to %s\r\n", path1, path2);
   if (_FSYS.rename(path1, path2)) {
      Serial.println("− file renamed");
   } else {
      Serial.println("− rename failed");
   }
}

void deleteFile( const char * path){
   Serial.printf("Deleting file: %s\r\n", path);
   if(_FSYS.remove(path)){
      Serial.println("− file deleted");
   } else {
      Serial.println("− delete failed");
   }
}

String getContentType(String filename) {
  if (Server.hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}


void handleFileDelete() {
  if (Server.args() == 0) {
    return Server.send(500, "text/plain", "BAD ARGS");
  }
  String path = Server.arg(0);
  Serial.println("handleFileDelete: " + path);
  if (path == "/") {
    return Server.send(500, "text/plain", "BAD PATH");
  }
  if (!exists(path)) {
    return Server.send(404, "text/plain", "FileNotFound");
  }
  deleteFile(path.c_str());
  Server.send(200, "text/plain", "");
  path = String();
}


void handleFileUpload() {
  if (Server.uri() != "/edit") {
    return;
  }
  HTTPUpload& upload = Server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    if (exists((char *)upload.filename.c_str())) {
      deleteFile((char *)upload.filename.c_str());
    }
    Serial.print("Upload: Opening upload.filename: "); Serial.println(upload.filename.c_str());
    uploadFile = _FSYS.open(upload.filename.c_str(), FILE_WRITE);
    if (uploadFile) 
      Serial.print("Upload: START, filename: "); Serial.println(uploadFile.name());
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
      Serial.print("Upload: WRITE, Bytes: "); Serial.println(upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
      Serial.printf("Upload: File %s END, Size: %d",upload.filename,upload.totalSize);
    } else {
      Serial.print("Error Upload: could not create filename: "); Serial.println(upload.filename);
    }
    listDir("/",0);//debug
  }
}

void ProcessFileRedirect() {
  String t_state = Server.arg("VALUE");
  int i_state=t_state.toInt();
#ifdef FREOPEN
  std::File * rFile;
#endif
  Serial.print("ProcessFileRedirect "); Serial.println(t_state);
  Serial.print("ProcessFileRedirect "); Serial.println(i_state);
  strcpy(buf, "");
  sprintf(buf, "%d redirect state", i_state);
  sprintf(buf, buf);
  Server.send(200, "text/plain", buf); //Send web page
#ifdef FREOPEN
  if (i_state == 1)
  {
    rFile=freopen(FileMonitorLog.c_str(), "a", stdout);
  }
  else if (i_state == 0)
  {
    if (rFile)
     fclose(rFile);
  }
#endif
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(_FSYS.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(_FSYS.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

bool formatFS( bool flag) {
	return _FSYS.begin(flag);
}

bool ifFileExists(String file) {
	return _FSYS.exists(file);
}
