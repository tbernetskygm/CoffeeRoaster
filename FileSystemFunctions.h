#ifndef FILE_SYSTEM_FUCTIONS_H
#define FILE_SYSTEM_FUCTIONS_H
#include "ProjectDefines.h"
#ifdef SPIFFS_FS
#include <SPIFFS.h>
#elif def LITTLEFS_FS
#include <LittleFS.h>
#endif

void listDir( const char * dirname, uint8_t levels);
void readFile(const char * path);
void writeFile(const char * path, const char * message);
void appendFile(const char * path, const char * message);
void renameFile(const char * path1, const char * path2);
void deleteFile(const char * path);
File openFile(const char * path, const char* mode=FILE_READ);
File openFile(String path, const char* mode=FILE_READ);
bool loadFromFS(String path);
void handleNotFound(String file, bool portal);
void printDirectory();
String getContentType(String filename);
void handleFileDelete();
void handleFileUpload();
bool exists(String path);
void ProcessFileRedirect();
void createDir(const char * path);
void removeDir(const char * path);
bool formatFS(bool flag);
bool ifFileExists(String path);

#endif
