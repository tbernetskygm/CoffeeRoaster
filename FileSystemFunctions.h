#ifndef FILE_SYSTEM_FUCTIONS_H
#define FILE_SYSTEM_FUCTIONS_H
#include <SPIFFS.h>
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path);
File openFile(fs::FS &fs, const char * path, const char* mode=FILE_READ);
bool loadFromFS(fs::FS &fs,String path);
void handleNotFound(String file, bool portal);
void printDirectory();
String getContentType(String filename);
void handleFileDelete();
void handleFileUpload();
bool exists(String path);
void ProcessFileRedirect();

#endif
