#ifndef FILE_SYSTEM_FUCTIONS_H
#define FILE_SYSTEM_FUCTIONS_H
#include "ProjectDefines.h"
#ifdef SPIFFS_FS
#include <SPIFFS.h>
#elif def LITTLEFS_FS
#include <LittleFS.h>
#endif

void initLittleFS();
void listDir( const char * dirname, uint8_t levels);
String readFile(const char * path);
void writeFile(const char * path, const char * message);
void writeFile(const char * path, String message);
void appendFile(const char * path, const char * message);
void appendFile(const char * path, String message);
void renameFile(const char * path1, const char * path2);
void deleteFile(const char * path);
File openFile(const char * path, const char* mode=FILE_READ);
File openFile(String path, const char* mode=FILE_READ);
void handleNotFound(String file, bool portal);
void handleFileDelete();
void handleFileUpload();
bool exists(String path);
#ifndef NEW_WIFI
void ProcessFileRedirect();
void printDirectory();
bool loadFromFS(String path);
String getContentType(String filename);
void handleFileDelete();
#else
void ProcessFileRedirect(AsyncWebServerRequest *request);
void printDirectory(AsyncWebServerRequest *request);
bool loadFromFS(AsyncWebServerRequest *request, String path);
String getContentType(AsyncWebServerRequest *request, String filename);
void handleFileDelete(AsyncWebServerRequest *request);
#endif
void createDir(const char * path);
void removeDir(const char * path);
bool formatFS(bool flag);
bool ifFileExists(String path);

#endif
