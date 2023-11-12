#ifndef GLOBALS_H
#define GLOBALS_H

#include "GlobalStructs.h"
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#endif

#include <FS.h>
#include <ArduinoJson.h>
#define Tempservo
#ifdef Tempservo
#include <ESP32Servo.h>
extern Servo TempServo;
#endif 

#if defined(ARDUINO_ARCH_ESP8266)
extern ESP8266WebServer Server;
#elif defined(ARDUINO_ARCH_ESP32)
extern WebServer Server;
#endif
#ifndef BUILTIN_LED
//#define BUILTIN_LED  2  // backward compatibility
#endif

extern int Release;
extern int Version;
extern int Revision;

// Add another GPIO pin for Basket Mixer Relay
// By default it will go in fwd (clockwise)

//#define PIN_MIXER_DIR 18
//#define PIN_MIX_POWER_ENABLE 2 // turn on power to mixer Same as builtin LED
//#define PIN_HEATER_POWER_ENABLE 0 // turn on power to heat gun
// Analog In for Temp Sensor Have to use ADC1 Ch 0 because we are using WiFi
//#define PIN_TEMP_SENSOR 36
#ifdef Tempservo
//#define PIN_SERVO 13
extern int servoPos;
extern int servoPosNew;
extern int SERVO_MAX_STEPS;
#endif
//#ifdef THERMOCOUPLE
//extern MAX6675 thermocouple(PIN_THERMO_CLK,PIN_THERMO_CS,PIN_THERMO_DO);
//#endif
extern double tempC;
extern double tempF;
extern double tempTotalC;
extern double tempTotalF;
extern double tempAvgC;
extern double tempAvgF;
extern int FanSpeed;

extern int BeanQuantity;
extern int CoffeeOpt;
extern String CoffeeType;

extern bool useSPIFFS;
extern bool useSDCARD;
extern bool ROAST ; // start roasting process
extern bool MIXPWR; // control stirring beans
extern bool MIXDIR; // direction of bean stirrer
extern bool HEATGUNHIGH ; // if you are running with heat gun set to high
extern bool TIMERSTART ; // Control roasting timer
extern bool PREHEAT_TIMERSTART; // Control preheat timer
extern bool PREHEAT; // Tell if in preheat process
extern bool CONFIG_TIMERSTART; // Temp config process
extern bool CONFIG_DATA_LOADED; // Flag used to tell that config data was loaded
extern bool HEATERPWR; // control heat gun power SSR logic reversed
extern bool TempPref ; // False is C True is F
extern bool RESET_TIMER;
extern bool TEMP_CONFIG;
extern bool TimerAdjust;
extern uint32_t SensorUpdate;
extern int TempSensorKOhms;
extern int FanRPM;
extern int timeoutVal;
extern int TimerMin0 ;
extern int TimerMin0New ;
extern int TimerSec0 ;
extern int TimerSec0New ;
extern int TimerStartValue;
extern int TimerValue ;// in seconds
extern int PreTimerMin ;
extern int PreTimerMinNew ;
extern int PreTimerSec ;
extern int PreTimerSecNew;
extern int PreheatTemp;
extern int PreheatServoPos;
extern int FinishTemp;
extern int FinishServoPos;
extern int PreheatTimerStartValue;
extern int PreheatTimerValue;// in seconds

// Config timer
extern int ConfigTimerMin ;
extern int ConfigTimerMinNew ;
extern int ConfigTimerSec ;
extern int ConfigTimerSecNew;
extern int ConfigTimerStartValue;
extern int ConfigTimerValue;// in seconds
extern int ConfigElaspedTime;// in seconds
extern int ConfigSteps;
extern int ConfigStep;
extern int ConfigStepSize;
extern int ConfigStepsNew;
extern int ConfigMaxSteps;
extern int ConfigMaxStepsNew ;
extern String ConfigFile;
extern String ConfigHighFile;
extern String Configuration_Date;
extern String RoastLogFile;
extern String RoastLogHighFile;

extern String FileMonitorLog;
extern File uploadFile;	
extern File logFile;	
extern int ServoPosInc;
extern int loopCounter;
// Variables for adjusting temp during preheat and roasting
extern int tempTolerance; // 3 degrees ?
extern int tempSamples; // every 10 seconds
// json stuff
extern ConfigStruct tempData[180];
// timer
extern hw_timer_t *RoastTimer ;
extern hw_timer_t *UtilTimer ;
extern volatile SemaphoreHandle_t roastTimerSemaphore;
extern volatile SemaphoreHandle_t utilTimerSemaphore;
// adc values to figure out why it stops giving valid data
extern int adcValue;
extern double voltageTemp;
extern double Rt;
extern double adcMax;
extern double Vs;
// the XML array size needs to be bigger that your maximum expected size. 2048 is way too big for this example
extern char XML[2048];
//extern char tempXML[];
// just some buffer holder for char operations
extern char buf[64];
extern bool first;
void readThermocoupleTemps();

#endif
