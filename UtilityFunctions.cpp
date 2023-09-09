#include <SPIFFS.h>
#include "Globals.h"
#include<ArduinoJson.h>
#include <time.h>
String get_timer_string(int value)
{
  char  tmpstr[40];

  // divide by 60 to get minutes
  int mins = value / 60;
  int secs = value % 60;
  sprintf(tmpstr, "%02d:%02d", mins, secs);
  return String(tmpstr);
}

// This is from e-tinkers git hub
// the formula for temp in kelvin is
//                 1
// T = ----------------------------
//     1/To + (1/beta) * ln(Rt/Ro)
//
// https://en.wikipedia.org/wiki/Thermistor
double readTemp(bool F)
{
  double R1 = 10000.0;   // voltage divider resistor value
  double Beta = 3950.0;  // Beta value
  double To = 298.15;    // Temperature in Kelvin for 25 degree Celsius
  double Ro = TempSensorKOhms * 1000.0;   // Resistance of Thermistor at 25 degree Celsius
  double T;
  adcValue = analogRead(PIN_TEMP_SENSOR); //Read ADC pin
  voltageTemp = adcValue * Vs/adcMax;
  Rt = R1 * voltageTemp / (Vs - voltageTemp);
  T = 1/(1/To + log(Rt/Ro)/Beta);    // Temperature in Kelvin
  T = T - 273.15;
  if (F)
    T = T * 9 / 5 + 32;

  return T;
}

// this was formula for getting temp from original tutorial
String get_temp_string() {
  // variables for Temp sensor
  if ( TempSensorKOhms == 10) {
    adcValue = analogRead(PIN_TEMP_SENSOR); //Read ADC pin
    voltageTemp = (float)adcValue / 4095.0 * 3.3; // calculate voltage
    //   R1 value 10K
    Rt = 10 * voltageTemp / (3.3 - voltageTemp); // calculate resistance of thermistor
    //                                               vv is thermistor @ 25 deg
    double tempK = 1 / (1 / (273.15 + 25) + log(Rt / TempSensorKOhms) / 3950.0 ); // Temp in Kelvin
    tempC = tempK - 273.15; // Temp converted to Celsius
    tempF = (tempC * 9 / 5) + 32.0;
  }
  else {
  tempC=readTemp(false);
  tempF=readTemp(true);
  }

  if ( tempC < 0)
  {
  // for testing print out data
  Serial.printf("ADC value :%d,\tVoltage : %.2fV, \tTempC : %.2fC \tTempF : %.2fF\n"
       ,adcValue, voltageTemp,tempC, tempF);
  }
  return String(tempC);
}

String get_date_string() {
  static const char *wd[7] = { "Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat" };
  struct tm *tm;
  struct tm  timeinfo;
  time_t  t;
  char    dateTime[40];
  if (!getLocalTime(&timeinfo)) 
  {
    Serial.println("Failed to get local time from NTP!!");
    t = time(NULL);
    tm = localtime(&t);
    
  } else
    tm=	&timeinfo;
  
  sprintf(dateTime, "%04d/%02d/%02d(%s) %02d:%02d:%02d.",
          tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
          wd[tm->tm_wday],
          tm->tm_hour, tm->tm_min, tm->tm_sec);
  Serial.printf("get_date_string date :%s\n", dateTime);
  return String(dateTime);

} 

String get_short_date_string() {
  struct tm *tm;
  struct tm  timeinfo;
  time_t  t;
  char    dateTime[40];
  if (!getLocalTime(&timeinfo)) 
  {
    Serial.println("Failed to get local time from NTP!!");
    t = time(NULL);
    tm = localtime(&t);
  } else
    tm=	&timeinfo;
  
  sprintf(dateTime, "%04d_%02d_%02d-%02d:%02d.",
          tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
          tm->tm_hour, tm->tm_min);
  Serial.printf("get_short_date_string date :%s\n", dateTime);
  return String(dateTime);
} 


double CalcAvgTemp(double temps, int timeval)
{
  double av=0;
  Serial.print("CalcAvgTemp temps "); Serial.println(temps);
  Serial.print("CalcAvgTemp timeval "); Serial.println(timeval);
  av= temps/timeval;
  Serial.print("CalcAvgTemp avg "); Serial.println(av);
  return av;
}


// need to look through config data to figure out 
// steps and temps
int getServoPos( int temp)
{
  int i=0;
  float temp1=0.0;
  float temp2=0.0;
  int pos1;
  int pos2;
  int calcPos=0;
  Serial.printf("getServoPos temp: %d\n",temp);
  // interpolate config data for servo position 
  if(tempData[0].tempC > 0 )
  {
    //Serial.print("getServoPos temp "); Serial.println(temp);
    for ( i = 0; i < SERVO_MAX_STEPS; i++) {
	  //Serial.print(" getServoPos tempC");Serial.println(tempData[i].tempC);
	  if ( tempData[i].tempC >= (float) temp){
		  //Serial.print(" Found higher temp2: ");Serial.println(tempData[i].tempC);
		  temp2=tempData[i].tempC;
		  //Serial.print(" Found higher temp1: ");Serial.println(tempData[i-1].tempC);
		  temp1=tempData[i-1].tempC;
		  //Serial.print(" Found servo pos2: ");Serial.println(tempData[i].ServoPosition);
		  pos2=tempData[i].ServoPosition;
		  //Serial.print(" Found servo pos1: ");Serial.println(tempData[i-1].ServoPosition);
		  pos1=tempData[i-1].ServoPosition;
		  break;
	  }
    }
    // make sure we found a higher temp value
    if (temp2 > 0)
    {
      calcPos =((int)( pos1+(pos2-pos1)/(temp2-temp1)*(temp-temp1)));
      Serial.print(" Calculated servo pos: ");Serial.println(calcPos);
    } else {
      calcPos =0;
      Serial.printf(" Cannot calculate servo pos temp value too high! %d\n",temp);
    }
  } else {
	  Serial.println("Config Data not loaded!!");
  }
	  
  //step1+(step2-step1)/(temp2-temp1)*(temp-temp1)
  return calcPos;
}


// need to look through config data to figure out 
// temp based on position
float getTempVal( int pos)
{
  int i=0;
  float temp1;
  float temp2;
  int pos1;
  int pos2;
  float calcTemp=0.0;
  // interpolate config data for servo position 
  if(tempData[0].tempC > 0 )
  {
    Serial.print("getTempVal pos "); Serial.println(pos);
    for ( i = 0; i < SERVO_MAX_STEPS; i++) {
	  //Serial.print(" getTempVal tempC");Serial.println(tempData[i].tempC);
	  if ( tempData[i].ServoPosition >= pos){
		  Serial.print(" Found higher pos: ");Serial.println(tempData[i].ServoPosition);
		  temp2=tempData[i].tempC;
		  Serial.print(" Found higher temp1: ");Serial.println(tempData[i-1].tempC);
		  temp1=tempData[i-1].tempC;
		  Serial.print(" Found servo pos2: ");Serial.println(tempData[i].ServoPosition);
		  pos2=tempData[i].ServoPosition;
		  Serial.print(" Found servo pos1: ");Serial.println(tempData[i-1].ServoPosition);
		  pos1=tempData[i-1].ServoPosition;
		  break;
	  }
    }
    calcTemp =(( temp1+(temp2-temp1)/(pos2-pos1)*(pos-pos1)));
    Serial.print(" Calculated temp : ");Serial.println(calcTemp);
  } else {
	  Serial.println("Config Data not loaded!!");
  }
	  
  //step1+(step2-step1)/(temp2-temp1)*(temp-temp1)
  return calcTemp;
}


void parseRoastData(String jsonData)
{
  StaticJsonDocument<2048> jdoc;
  int Num;
  double tempC;
  double tempF;
  int servoPos;

  Serial.print("parseRoastData - jsonData : ");Serial.println(jsonData);
  
    // json stuff
    size_t filesize = jsonData.length();
    Serial.print("parseRoastData - file size : ");Serial.println(filesize);
    DeserializationError error;
    error = deserializeJson(jdoc,jsonData);
    if(error) {
      Serial.println("parseRoastLog deserializeJson() failed");
      Serial.println(error.f_str());
      return;
    }
    Serial.println("parseRoastData First deserializeJson() worked!!");
    const char* Roast_Date = jdoc["Roasting Log Date"]; // "1970/01/01(Thr)00:02:01"
    unsigned int cBeanQty = jdoc["Bean Quantity (ounces)"]; // "12"
    unsigned int cPreheatTime = jdoc["Preheat Time (sec)"]; // "12"
    unsigned int cPreheatTemp = jdoc["Preheat Temp degrees C "]; // "12"
    unsigned int cPreheatServoPos = jdoc["Preheat Servo Pos"]; // "12"
    unsigned int cRoastTime = jdoc["Roast Time (sec)"]; // "12"
    unsigned int cFinishTemp = jdoc["Finish Temp degrees C "]; // "12"
    unsigned int cFinishServoPos = jdoc["Finish Servo Pos"]; // "12"
    const char* HeatGunSpeed = jdoc["Heatgun Speed"]; // "12"
    FinishTemp=cFinishTemp;
    Serial.printf("parseRoastData FinishTemp %d\n", FinishTemp);
    Serial.printf("parseRoastData HeatGunSpeed %s\n", HeatGunSpeed);
}

void parseJsonFile(String filename)
{
  StaticJsonDocument<2048> jdoc;
  StaticJsonDocument<2048> jdoc2;
  int Num;
  double tempC;
  double tempF;
  int servoPos;

  Serial.print("parseJsonFile - opening file : ");Serial.println(filename);
  if (SPIFFS.exists(filename)  )
  {
    File file = SPIFFS.open(filename,"r"); 
    if (!file || file.isDirectory()){
      Serial.print("parseJsonFile - failed to open file : ");Serial.println(filename);
      return;
    }
    Serial.print("parseJsonFile - opened file : ");Serial.println(filename);
  
    // json stuff
    size_t filesize = file.size();
    
    DeserializationError error;
    error = deserializeJson(jdoc,file);
    if(error) {
      Serial.println("deserializeJson() failed");
      Serial.println(error.f_str());
      file.close();
      CONFIG_DATA_LOADED = false; // Flag used to tell that config data was loaded
      return;
    }
    Serial.println("First deserializeJson() worked!!");
    file.close();
    const char* Configuration_Date = jdoc["Configuration Date"]; // "1970/01/01(Thr)00:02:01"

for (JsonObject step : jdoc["steps"].as<JsonArray>()) {

  unsigned int step_NUM = step["NUM"]; // "1", "2", "3"
    //Serial.print("steps num : ");Serial.println(step_NUM);
  float step_TEMPAVGC = step["TMPC"]; // "29.22", "29.22", "29.22"
    //Serial.print(": tempc : ");Serial.println(step_TEMPAVGC);
  float step_TEMPAVGF = step["TMPF"]; // " 72.00", " 72.00", " 72.00"
  unsigned int step_SERVOPOS = step["POS"]; // "32", "32", "32"
    //Serial.print(": servo pos : ");Serial.println(step_SERVOPOS);
  // Put in structure of config data
  tempData[step_NUM-1].ConfigStep=step_NUM;
  tempData[step_NUM-1].ServoPosition=step_SERVOPOS;
  tempData[step_NUM-1].tempC=step_TEMPAVGC;
  tempData[step_NUM-1].tempF=step_TEMPAVGF;
}
  // config data has NUM (int) , TEMPAVGF(float), TEMPAVGC(float) , SERVOPOS (int)
    file.close();
    CONFIG_DATA_LOADED = true; // Flag used to tell that config data was loaded
  } else 
  {
    Serial.print("parseJsonFile no file : ");Serial.println(filename);
    CONFIG_DATA_LOADED = false; // Flag used to tell that config data was loaded
  }
}

// the idea is to read the temp value for x number of seconds
// and see if it is higher or lower than the temp we want to be at
// and change the servo position.
int adjustTemp(int setPoint)
{
  int iAvgTemp=0;
  int errVal=0;
  int newPos=0;
  // should be collecting temp values in tempTotalC and tempTotalF
  // and the number of samples is tempSamples
  if (TempPref)
  {
    tempAvgF=CalcAvgTemp(tempTotalF,tempSamples);
    iAvgTemp=(int) tempAvgF;
  }
  else
  {
    tempAvgC=CalcAvgTemp(tempTotalC,tempSamples);
    iAvgTemp=(int) tempAvgC;
  }
    
  errVal = setPoint - iAvgTemp;

  if ( abs(errVal) > tempTolerance)
  {
   Serial.print("adjustTemp out of tolerance: "); Serial.println(errVal);
   newPos=getServoPos(setPoint+errVal);
   Serial.print("adjustTemp out of new position: "); Serial.println(newPos);
  }
  tempTotalF=0;
  tempTotalC=0;
  return newPos;
}
