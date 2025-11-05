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

// Initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.printf("An Error occured while mounting LittelFS \n");
  }
  //Serial.printf("LittleFS mounted sucessfully\n");
}

void listDir( const char * dirname, uint8_t levels){
   //Serial.printf("FileSystemFunctions::listDir Listing directory: %s\r\n", dirname);

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

#ifndef NEW_WIFI
void returnFail(String msg) {
  Server.send(500, "text/plain", msg + "\r\n");
}
#else
void returnFail(AsyncWebServerRequest *request, String msg) {
  request->send(500, msg + "\r\n" ,"text/plain");
}
#endif

//#ifndef NEW_WIFI
//void printDirectory() {
//  Serial.printf("printDirectory\n");
//  if (!Server.hasArg("dir")) {
//    return returnFail("BAD ARGS");
//  }
//  String f_path = Server.arg("dir");
//#else
void printDirectory(AsyncWebServerRequest *request) {
  Serial.printf("printDirectory\n");
  if(!request->hasArg("dir"))
    return returnFail(request, "BAD ARGS");
  String f_path = request->arg("dir");
//#endif
  String output;

  Serial.printf("printDirectory path %s\n",f_path.c_str());
  if (f_path != "/" && !_FSYS.exists((char *)f_path.c_str())) {
#ifndef NEW_WIFI
    return returnFail("BAD PATH");
#else
    return returnFail(request, "BAD PATH");
#endif
  }
  File dir = _FSYS.open(f_path);
  //fpath = String();
  if (!dir.isDirectory()) {
    dir.close();
#ifndef NEW_WIFI
    return returnFail("NOT DIR");
#else
    return returnFail(request, "BAD PATH");
#endif
  }
  dir.rewindDirectory();
#ifndef NEW_WIFI
  Server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  Server.send(200, "text/json", "");
  //WiFiClient client = Server.client();

  Server.sendContent("[");
#else
  output += "[";
#endif

  for (int cnt = 0; true; ++cnt) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }

    //String output;
    if (cnt > 0) {
      output += ',';
    }

    output += "{\"type\":\"";
    output += (entry.isDirectory()) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += entry.path();
    output += "\"";
    output += "}";
#ifndef NEW_WIFI
    Server.sendContent(output);
#endif
    entry.close();
  }
#ifndef NEW_WIFI
  Server.sendContent("]");
#else
    output += "]";// close json data
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/json", output);
  request->send(resp);
#endif
  dir.close();
}

#ifndef NEW_WIFI
bool loadFromFS(String path){
#else
bool loadFromFS(AsyncWebServerRequest *request, String path, String download){
#endif
  //Serial.printf("loadFromFS useFS %d useSDCARD %d path %s\n", useFS,useSDCARD,path.c_str());
  //path.toLowerCase();
  //String dataType = "text/plain";
  String dataType = getContentType(request, path);
  if(path.endsWith("/")) path += "index.htm";

  //Serial.printf("loadFromFS file %s dataType %s\n", path.c_str(),dataType);
  File dataFile = openFile(path.c_str());

  if (!dataFile)
    return false;

  //Serial.printf("loadFromFS opened file %s sending...\n", path.c_str());
#ifndef NEW_WIFI
  if (Server.streamFile(dataFile, dataType) != dataFile.size()) {
    //Serial.println("Sent less data than expected!");
  }
#else
    Serial.println("NEW_WIFI Downloading File");
    if (download.equals("true")) {
      // try and download the file
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, path, "application/octet-stream", true);
      request->send(response);
    }
#endif

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

#ifndef NEW_WIFI
void handleNotFound(String file, bool portal){
#else
void handleNotFound(AsyncWebServerRequest *request){
#endif
  String message;
  String path;
  //Serial.printf("handleNotFound useFS %d useSDCARD %d portal %d\n", useFS,useSDCARD,portal);
  //Serial.printf("handleNotFound portal file: %s\n", file.c_str());
#ifndef NEW_WIFI
  //if (!portal)
 // {
    for (uint8_t i=0; i<Server.args(); i++){
      message += " NAME:"+Server.argName(i) + "\n VALUE:" + Server.arg(i) + "\n";
    }
#else
    int params = request->params();
    for (int i=0; i < params ; i++) {
      const AsyncWebParameter* p = request->getParam(i);
      //Serial.printf("PARAM[%u]: %s = %s\n", i, p->name().c_str(), p->value().c_str());
      message += " NAME:"+p->name() + "\n VALUE:" + p->value().c_str() + "\n";
    }
    for (uint8_t i=0; i<request->args(); i++){
      message += " NAME:"+request->arg(i) + "\n VALUE:" + request->arg(i) + "\n";
    }
#endif
  Serial.printf("handleNotFound Server args: %s\n", message.c_str());
#ifndef NEW_WIFI
  if (!portal)
    path = Server.arg("path");
  else
    path = file;

  Serial.printf("handleNotFound path: %s Server.uri() %s \n", path.c_str(), Server.uri().c_str());
#else
  if(request->hasArg("path"))
    path = request->arg("path");

  Serial.printf("handleNotFound path: %s Server.uri() %s \n", path.c_str(), request->url().c_str());
#endif

  if (useFS)
  {
#ifndef NEW_WIFI
    if(loadFromFS(path)) return;
#else
    if(loadFromFS(request,path)) return;
#endif
  }
#ifndef NEW_WIFI
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
  //Serial.println(message);
#else
#ifdef SD_FS
  if (useSDCARD)
  {
    if(loadFromFS(SD,path)) return ;
  }
#endif
  // This is an error 
  message = "FS Not Detected\n\n";
  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += request->args();
  message += "\n";
  for (uint8_t i=0; i<request->args(); i++){
    message += " NAME:"+request->arg(i) + "\n VALUE:" + request->arg(i) + "\n";
  }
  request->send(404, "text/plain", message);
  Serial.println(message);
#endif

}

File openFile( const char * path, const char * mode){
   //Serial.printf("Opening file: %s mode %s \n", path,mode);
   char test[]="r";
   File file;
   int rd=strcmp(mode,test);
   //Serial.printf("Opening file: %s mode %s FILE_READ %s rd %d\n", path,mode,FILE_READ,rd);
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
   //Serial.printf("Opening file: %s mode %s \n", path.c_str(),mode);

   File file = _FSYS.open(path,mode);
   if(!file || file.isDirectory()){
       Serial.printf("− failed to open file for mode %s\n",mode);
       return file ;
   }
   return file;
}

String readFile(const char * path){
   //Serial.printf("Reading file: %s\r\n", path);

   File file = _FSYS.open(path);
   if(!file || file.isDirectory()){
       Serial.println("− failed to open file for reading");
       return String();
   }

   Serial.println("− read from file:");
   String fileContent;
   while(file.available()){
      fileContent = (file.readStringUntil('\n'));
   }
   return fileContent;
}
/*
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
*/

void writeFile(const char * path, String message){
   //Serial.printf("Writing file: %s\r\n", path);

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
/*
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
*/
void appendFile( const char * path, String message){
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

#ifndef NEW_WIFI
String getContentType(String filename) {
  if (Server.hasArg("download")) {
    return "application/octet-stream";
#else
String getContentType(AsyncWebServerRequest *request, String filename) {
  if (request->hasArg("download")) {
    return "application/octet-stream";
#endif
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


#ifndef NEW_WIFI
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
#else
void handleFileDelete(AsyncWebServerRequest *request) {
  if (request->args() == 0) {
    return request->send(500, "text/plain", "BAD ARGS");
  }
  String path ;//= request->arg(0);
  if(request->hasParam("FileName"))
    path = request->getParam("FileName")->value();
  
  Serial.println("handleFileDelete: path=" + path);
  if (path == "/") {
    return request->send(500,  "text/plain", "BAD PATH Can't delete rootfs "+ path);
  }
  if (path.length() == 0) {
    return request->send(500,  "text/plain","No File Specified ");
  }
  if (!exists(path)) {
    return request->send(404, "text/plain","FileNotFound");
  }
  deleteFile(path.c_str());
  request->send(200, "text/plain", "File " + path + " Deleted");
  path = String();
#endif
}


#ifndef NEW_WIFI
void handleFileUpload() {
  String UploadFile;
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
#else

void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) 
{
  
  //if (!request->url().equals("/UploadFile")) {
   // // Can't open file send bad response
   // request->send(400, "text/plain", "Request url is wrong! " + request->url() + " Could Not be opened for writing"); // just to make server happy
  //}
  String UploadFile;
  Serial.printf("--- handleFileUpload[%s]: index=%u, len=%u, final=%d\n", filename.c_str(), index, len, final);
  Serial.printf("Content length %d\n",request->header("Content-Length").toInt());
  String path ;//= request->arg(0);
  //if(request->hasParam("FileName"))
    //path = request->getParam("FileName")->value();
  
  Serial.println("handleFileUpload: filename=" + filename);
  if (LittleFS.exists(filename) && index == 0) {
      deleteFile((char *)filename.c_str());
  }
  //Serial.print("Upload: Opening upload.filename: "); 
  if (!index) {
    Serial.print("Upload: : Creating "); Serial.println(filename.c_str());
    
   request->_tempFile = openFile(filename, "w");
    
    

    if (!request->_tempFile) {
          request->send(400, "text/plain", "File not available for writing");
    }
  }   
  
  if (request->_tempFile) {
    Serial.print("Upload: START, filename: "); Serial.println(uploadFile.name());
  }
  else {
    // Can't open file send bad response
    request->send(400, "text/plain", "File " + filename + " Could Not be opened for writing"); // just to make server happy
  }

  
  if (len) {
    request->_tempFile.write(data, len);
  }
  if (final) {
  request->_tempFile.close();
  }
  
  //Serial.println(upload.filename.c_str());
 
 
  request->send(200, "text/plain", "File " + filename + " Might be uploaded"); // just to make server happy
}

#endif

#ifndef NEW_WIFI
void ProcessFileRedirect() {
  String t_state = Server.arg("VALUE");
#else
void ProcessFileRedirect(AsyncWebServerRequest *request) {
  String t_state;
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
#endif
  int i_state=t_state.toInt();
#ifdef FREOPEN
  std::File * rFile;
#endif
  Serial.print("ProcessFileRedirect "); Serial.println(t_state);
  Serial.print("ProcessFileRedirect "); Serial.println(i_state);
  strcpy(buf, "");
  sprintf(buf, "%d redirect state", i_state);
  sprintf(buf, buf);
#ifndef NEW_WIFI
  Server.send(200, "text/plain", buf); //Send web page
#else
  request->send(200, buf_p ,"text/plain");
#endif
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
