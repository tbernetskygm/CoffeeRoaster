
#include <SPIFFS.h>

 String get_timer_string(int value);
 String get_temp_string();
 String get_date_string();
 String get_short_date_string();
double CalcAvgTemp(double  temps, int timeval);
 void parseJsonFile(String filename);
 void parseRoastData(String jsonData);
int getServoPos( int temp);
float getTempVal( int pos);
int adjustTemp(int setPoint);
double readTemp(bool F);

void readThermocoupleTemps();

void readThermistorTemps();
void HandleDevices(void * dbg);
