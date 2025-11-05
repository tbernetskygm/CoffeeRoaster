
#include "ProjectDefines.h"
//#include<Arduino.h>
#define _RELEASE 3
#define _VERSION 0
#define _REVISION 0
#include <ArduinoJson.h>
#include <time.h>
#include <Ticker.h>

/*
  CoffeeRoaster.ino, Modified from
  Example for the AutoConnect library.
  Copyright (c) 2018, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect

  This software is released under the MIT License.
  https://opensource.org/licenses/MIT
*/
/* Manually upload data
python3 .arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool.py --chip auto --port /dev/ttyUSB0 --baud 115200 write_flash 0xeb000 /tmp/arduino_build_924397/CoffeeRoaster.spiffs.bin
*/
#if defined(ARDUINO_ARCH_ESP8266)
// #include <ESP8266WiFi.h>
 //#include <ESP8266WebServer.h>
#elif defined(ARDUINO_ARCH_ESP32)
// #include <WiFi.h>
#include <ESPmDNS.h>

#endif


#include "LittleFS.h"


#ifdef NEW_WIFI


  #ifdef WIFI_MANAGER
  #include <WiFiManager.h>
  
  #include <wm_consts_en.h>
  #include <wm_strings_en.h>
  #include <wm_strings_es.h>
  #endif
  #include <ESPAsyncWebServer.h>
  #include <ElegantOTA.h>
  #ifdef ASYNC_WIFI
   //#include <WiFi.h>
   //#include <WiFiClient.h>
   
   //#include <ESPAsync_WiFiManager.h>  
  #endif
#endif

#ifdef PRINT_RESET
#include <rom/rtc.h>
#endif

#include "GlobalStructs.h"
#include "SendXML.h"
#include "RoasterControls.h"
#include "UtilityFunctions.h"
#include "SendXML.h"
#include "UtilTimer.h"
#include "ConfigTemp.h"

#include "ConfigPage.h"
#include "WebPage.h"
#ifdef USE_FSYS

#include "FileSystemFunctions.h"
#endif

#ifdef Tempservo
#include <ESP32Servo.h>

#endif
#ifdef THERMOCOUPLE
#include "max6675.h"
#endif
#if defined(ARDUINO_ARCH_ESP8266)
ESP8266WebServer Server;
#elif defined(ARDUINO_ARCH_ESP32)

  #ifdef AUTO_CONNECT
  WebServer Server;
  #endif

  #ifdef WEBSERIAL
  AsyncWebServer Server(80);
  #endif

  #ifdef NEW_WIFI
  AsyncWebServer Server(80);
  //AsyncEventSource events("/events");
  //AsyncDNSServer dnsServer;
  #endif
#endif

#ifdef Tempservo
ESP32PWM pwm;
Servo TempServo;
#endif
#ifndef BUILTIN_LED
#define BUILTIN_LED  2  // backward compatibility
#endif
// Get time from ntp
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec=0;
const int daylightOffset_sec = 3600;
// Add another GPIO pin for Basket Mixer Relay
// By default it will go in fwd (clockwise)
// During boot 
// GPIO0 low
// GPIO2 must float
// GPIO4 strapping
// GPIO5 Hi
// GPI012 Low
// GPIO15 Hi
// acccording to interwebs WiFi uses ADC2 pins, but not
// all of them...
// GPIO25 ADC2_CH8
// GPIO26 ADC2_CH9
// GPIO27 ADC2_CH7
// GPIO14 ADC2_CH6
// GPIO12 ADC2_CH5
// GPIO13 ADC2_CH4 
// GPIO15 ADC2_CH3 servo 
// GPIO2 ADC2_CH2  mix power
// GPIO0 ADC2_CH1  heater power
// GPIO4 ADC2_CH0

#ifdef AUTO_CONNECT
AutoConnect         Portal(Server);
AutoConnectConfig Config;
#endif

#ifdef NEW_WIFI

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;
bool wm_nonblocking = false; // change to true to use non blocking

WiFiManager wm; // global wm instance
WiFiManagerParameter custom_field; // global param ( for non blocking w params )
#endif

// json stuff
ConfigStruct tempData[180];

const char* host="CoffeeRoaster";

int Release = _RELEASE;
int Version = _VERSION;
int Revision = _REVISION;
  
#ifdef Tempservo
// Published values for SG90 servos; adjust if needed
int minUs = 500;
int maxUs = 2800;//was 2400 and was at 6.5
int servoPosNew=0;
int servoPos=0;
int tempSensorSelect=0;
float setTemp=0;
int servoVal=-1;
int SERVO_MAX_STEPS=180;
#endif
double tempC = 0;
double tempF = 0;
double tempTotalC = 0;
double tempTotalF = 0;
double tempAvgC = 0;
double tempAvgF = 0;
int FanSpeed = 0;
int BeanQuantity=0;
int CoffeeOpt=0;
String CoffeeType="";
bool useFS=false;
bool useSDCARD=false;
bool opened=false;
bool ROAST = false; // start roasting process

bool MIXPWR = false; /// control stirring beans
bool MIXDIR = false; // direction of bean stirrer
bool HEATGUNHIGH = true; // this is the way I have been running it..
//running with heat gun set to high

bool TIMERSTART = false; // Control roasting timer
bool PREHEAT_TIMERSTART = false; // Control UtilTimer timer
bool PREHEAT = false; // Tell if in preheat process
bool CONFIG_TIMERSTART = false; // Control UtilTimer timer
bool CONFIG_DATA_LOADED = false; // Flag used to tell that config data was loaded
bool HEATERPWR = true; // control heat gun power SSR logic reversed
bool TempPref = false; // False is C True is F
bool RESET_TIMER = false;
bool TEMP_CONFIG = false;
bool TimerAdjust = false; // Set true if you adjust roast timer during roast
uint32_t SensorUpdate = 0;
int TempSensorKOhms = 10;
int FanRPM = 0;
int timeoutVal = 500;// 500 msec
int TimerMin0 = 1;
int TimerMin0New = 0;
int TimerAdjustMin = 0;
int TimerSec0 = 1;
int TimerSec0New = 0;
int TimerStartValue = 0;
int TimerValue = 0;// in seconds
int PreTimerMin = 0;
int PreTimerMinNew = 0;
int PreTimerSec = 0;
int PreTimerSecNew = 0;
int PreheatTemp = 120;
int PreheatServoPos = 0;
int FinishTemp = 180;
int FinishServoPos = 0;
int PreheatTimerStartValue = 0;
volatile int PreheatTimerValue = 0;// in seconds
int UtilTimerValue=0;
int ConfigTimerMin =0;
int ConfigTimerMinNew =0 ;
int ConfigTimerSec =0;
int ConfigTimerSecNew =0;
int ConfigTimerStartValue = 0;
volatile int ConfigTimerValue=0;// in seconds
int ConfigElaspedTime=0;// in seconds
int ConfigSteps = 1;
int ConfigStep=0;
int ConfigStepSize=1;
int ConfigStepsNew = 0;
int ConfigMaxSteps = 1;
int ConfigMaxStepsNew = 0;
int DebugNum = 0;
String ConfigFile="/TempConfig.json";
String ConfigHighFile="/TempConfigHigh.json";
String Configuration_Date;
String RoastLogFile="/RoastLog.json";
String RoastLogHighFile="/RoastLogHigh.json";

File uploadFile;
File logFile;	

int ServoPosInc=0;
// Variables for adjusting temp during preheat and roasting
int tempTolerance=3; // 3 degrees ?
int tempSamples=10; // every 10 seconds
volatile SemaphoreHandle_t roastTimerTSemaphore;
//volatile SemaphoreHandle_t utilTimerSemaphore;
volatile SemaphoreHandle_t utilTimerTSemaphore;
//portMUX_TYPE utilTimerMux = portMUX_INITIALIZER_UNLOCKED;
int loopCounter = 0;
// adc values to figure out why it stops giving valid data
int adcValue=0;
double voltageTemp=0;
double Rt=0;
double adcMax=4096.0;
double Vs=3.3;
// timer
//hw_timer_t *RoastTimer = NULL;
//hw_timer_t *UtilTimer = NULL;
Ticker UtilTimerT;
Ticker RoastTimerT;
// the XML array size needs to be bigger that your maximum expected size. 2048 is way too big for this example
// just some buffer holder for char operations
//char buf[64];
//char *buf_p = &buf[0];

#ifdef THERMOCOUPLE
MAX6675 thermocouple(PIN_THERMO_CLK,PIN_THERMO_CS,PIN_THERMO_DO);
#endif
// for UpdateRoasting log
bool first=true;
RoastState  R_State;
RoastState * rState=&R_State;

#ifdef PRINT_RESET
void print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1 : Serial.println ("POWERON_RESET");break;          /**<1, Vbat power on reset*/
    case 3 : Serial.println ("SW_RESET");break;               /**<3, Software reset digital core*/
    case 4 : Serial.println ("OWDT_RESET");break;             /**<4, Legacy watch dog reset digital core*/
    case 5 : Serial.println ("DEEPSLEEP_RESET");break;        /**<5, Deep Sleep reset digital core*/
    case 6 : Serial.println ("SDIO_RESET");break;             /**<6, Reset by SLC module, reset digital core*/
    case 7 : Serial.println ("TG0WDT_SYS_RESET");break;       /**<7, Timer Group0 Watch dog reset digital core*/
    case 8 : Serial.println ("TG1WDT_SYS_RESET");break;       /**<8, Timer Group1 Watch dog reset digital core*/
    case 9 : Serial.println ("RTCWDT_SYS_RESET");break;       /**<9, RTC Watch dog Reset digital core*/
    case 10 : Serial.println ("INTRUSION_RESET");break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : Serial.println ("TGWDT_CPU_RESET");break;       /**<11, Time Group reset CPU*/
    case 12 : Serial.println ("SW_CPU_RESET");break;          /**<12, Software reset CPU*/
    case 13 : Serial.println ("RTCWDT_CPU_RESET");break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : Serial.println ("EXT_CPU_RESET");break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : Serial.println ("RTCWDT_BROWN_OUT_RESET");break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : Serial.println ("RTCWDT_RTC_RESET");break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : Serial.println ("NO_MEAN");
  }
}
#endif

#ifdef WIFI_MANAGER

#endif


#ifdef AUTO_CONNECT
bool atDetect(IPAddress& softapIP) {
  Serial.println("CoffeeRoaster Captive  started, SoftAP IP:" + softapIP.toString());
  return true;
}
#endif

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to get time...");
    return;
  }
  Serial.println(&timeinfo, "printLocalTime : %A, %B %d %Y %H:%M:%S");
}

void returnOK(AsyncWebServerRequest *request) {
  #ifndef NEW_WIFI
  Server.send(200, "text/plain", "");
  #else
  request->send(200,"","text/plain");
  #endif
}

#ifdef WEBSERIAL
void rcvWebMsg(uint8_t *data, size_t len)
{
  WebSerial.println("Received data");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
}
#endif

#ifdef OTA_T
// Functions to handle OTA stuff
void exitOTAStart() {
  Serial.println("OTA started");
}

void exitOTAProgress(unsigned int amount, unsigned int sz) {
  Serial.printf("OTA in progress: received %d bytes, total %d bytes\n", sz, amount);
}

void exitOTAEnd() {
  Serial.println("OTA ended");
}

void exitOTAError(uint8_t err) {
  Serial.printf("OTA error occurred %d\n", err);
}
#endif
#ifdef WIFI_MANAGER

void ResetWiFi(AsyncWebServerRequest *request)
{
  Serial.printf("ResetWiFi\n");
  wm.resetSettings();
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", "Reset WiFi");
  request->send(resp);
  wm.reboot();
}

String getParam(String name){
  //read parameter from server, for customhmtl input
  String value;
  if(wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  return value;
}

void saveParamCallback(){
  Serial.println("[CALLBACK] saveParamCallback fired");
  Serial.println("PARAM customfieldid = " + getParam("customfieldid"));
}
#endif
#ifdef NEW_WIFI

#endif
#ifndef NEW_WIFI
void SetDebug() {
  String t_state = Server.arg("VALUE");
  DebugNum = t_state.toInt();
#else
void SetDebug(AsyncWebServerRequest *request) {
  //Check if GET arg exists
  String t_state;
  char buf[64];
  char * buf_p = &buf[0];
  if(request->hasArg("VALUE"))
    t_state = request->arg("VALUE");
  
#endif

  //Serial.print("SetDebug DebugNum "); Serial.println(DebugNum);
  //Serial.print("ProcessConfigMaxSteps ConfigMaxStepsNew: "); Serial.println(ConfigMaxStepsNew);
  sprintf(buf, "%d", DebugNum);
  #ifndef NEW_WIFI
  Server.send(200, "text/plain", buf); //Send web page
  #else
  AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", String(buf));
  request->send(resp);
  #endif
}

#define FORMAT_FS_IF_FAILED true
void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  rState->utilTimerIntSetup=false;
  Serial.printf("Starting Version %d.%d.%d\n",_RELEASE,_VERSION,_REVISION);

#ifndef NEW_WIFI
  Config.apid = "CoffeeRoaster";
  Config.autoReconnect = true;
  #ifdef OTA_T
  Config.ota = AC_OTA_BUILTIN;
  #endif
  #ifdef OTA_T
  Portal.config(Config);
  Portal.onOTAStart(exitOTAStart);
  Portal.onOTAEnd(exitOTAEnd);
  Portal.onOTAProgress(exitOTAProgress);
  Portal.onOTAError(exitOTAError);
  #endif
#else
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  Serial.setDebugOutput(true);  
  delay(3000);
  Serial.printf("NEW_WIFI starting.\n");
  initLittleFS();

#ifdef ASYNC_WIFI
  #if ( USING_ESP32_S2 || USING_ESP32_C3 )
  ESPAsync_WiFiManager wm(&server, NULL, "AsyncESP32-FSWebServer");
#else
  AsyncDNSServer dnsServer;
  
  ESPAsync_WiFiManager wm(&server, &dnsServer, "AsyncESP32-FSWebServer");
#endif

#endif
  //ssid = readFile(ssidPath);
  //pass = readFile(passPath);
  //ip = readFile(ipPath);
  //gateway = readFile(gwPath);
  #ifdef WIFI_MANAGER
  if(wm_nonblocking) wm.setConfigPortalBlocking(false);

  // add a custom input field
  int customFieldLength = 40;
  // test custom html(radio)
  const char* custom_radio_str = "<br/><label for='customfieldid'>Custom Field Label</label><input type='radio' name='customfieldid' value='1' checked> One<br><input type='radio' name='customfieldid' value='2'> Two<br><input type='radio' name='customfieldid' value='3'> Three";
  new (&custom_field) WiFiManagerParameter(custom_radio_str); // custom html input
  
  wm.addParameter(&custom_field);
  wm.setSaveParamsCallback(saveParamCallback);
  std::vector<const char *> menu = {"wifi","info","param","sep","restart","exit"};
  wm.setMenu(menu);
  //wm.resetSettings(); // wipe settings
  // set dark theme
  wm.setClass("invert");
  wm.setConfigPortalTimeout(90); // auto close configportal after n seconds
  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("CoffeeRoasterAP","123456789"); // password protected ap

  if(!res) {
    Serial.println("Failed to connect or hit timeout");
    // ESP.restart();
  } 
  else {
    //if you get here you have connected to the WiFi    
    Serial.printf("connected...yay :)mode %s status %d\n", wm.getModeString(WiFi.getMode()), WiFi.status());
  }
  #endif
#endif

 

#ifdef WEBSERIAL
  WebSerial.begin(&Server);
  WebSerial.msgCallback(rcvWebMsg);
  Server.begin();
#endif

#ifdef PRINT_RESET
  Serial.println("CPU0 reset reason: ");
  print_reset_reason(rtc_get_reset_reason(0));

  Serial.println("CPU1 reset reason: ");
  print_reset_reason(rtc_get_reset_reason(1));
#endif  
#ifdef USE_FSYS
  if(!formatFS(FORMAT_FS_IF_FAILED)){
    Serial.println("An Error has occurred while mounting FS");
    return;
  }
  
  useFS=true;
  Serial.println("CoffeeRoaster getting directory listing");
  listDir("/", 0);
#endif
  #ifdef Tempservo
  int servoStat =0;
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  TempServo.setPeriodHertz(50);
  servoStat=TempServo.attach(PIN_SERVO,minUs,maxUs);
  Serial.print("Setup servoStat = ");Serial.println(servoStat);
  Serial.print("Setup TempServo Attached = ");Serial.println(TempServo.attached());
  #endif
  pinMode(PIN_MIX_POWER_ENABLE, OUTPUT);
  pinMode(PIN_HEATER_POWER_ENABLE, OUTPUT);
  pinMode(PIN_MIXER_DIR, OUTPUT);
#ifndef NEW_WIFI
  // Put the home location of the web site.
  // But in usually, setting the home uri is not needed cause default location is "/".
  Portal.home("/Roast");
#else
 
  #endif
  // Turn off heater SSR
  digitalWrite(PIN_HEATER_POWER_ENABLE, HEATERPWR);
#ifdef Tempservo
  // Set Temp to 0
  TempServo.write(0);
  delay(15);
#endif
  // Create semaphore to inform us when the timer has fired
  roastTimerTSemaphore = xSemaphoreCreateBinary();
  //utilTimerSemaphore = xSemaphoreCreateBinary();
  utilTimerTSemaphore = xSemaphoreCreateBinary();
  rState->doRoast=false;
  rState->roast=false;
  rState->preheat=false;
  rState->first=true;
  rState->last=false;
  rState->stopit=false;
  rState->heaterpwr=true;
  rState->mixerpwr=false;
  rState->timerStart=false;
  rState->preheatTimerStart=false;
  rState->fileName="";
  // setup task to update roast stuff
  xTaskCreate(UpdateRoastState,"Update Roast State",4096,(void*)rState,tskIDLE_PRIORITY,NULL);
  xTaskCreate(HandleDevices,"Handle Temps and Servo Moving",2000,(void*)DebugNum,1,NULL);
  //UtilTimerValue = 25; // Test
  //UtilTimerT.attach_ms(1000,onUtilTimerT,&UtilTimerValue);
  #ifndef NEW_WIFI
  Server.on("/", handleNewRoot);
  Server.on("/Roast", handleNewRoot);
  Server.on("/style.css", sendStyle);
  Server.on("/Files", handleFilesPage);
  Server.on("/list", printDirectory);
  Server.on("/download",HTTP_GET ,[]() {
      Serial.println("Server.on /download");
    handleNotFound(Server.uri(),false);
  });

  Server.on("/edit", HTTP_POST, []() {
    returnOK();
  }, handleFileUpload);// FileSystemFunctions.cpp

  Server.onNotFound([]() {
      Serial.println("Server.onNotFound");
    handleNotFound(Server.uri(),false);
    });

  Portal.onNotFound([]() {
      Serial.printf("Portal.onNotFound uri %s \n",Server.uri().c_str()));
      #ifndef NEW_WIFI
      for (uint8_t i=0; i<Server.args(); i++){
        
	      Serial.printf(" request NAME: %s  VALUE: %s\n\n",Server.argName(i),Server.arg(i));
        
      }
      #endif
    Serial.println("Portal.onNotFound calling handleNotFound");
    handleNotFound(Server.uri(),true);
    });

  Server.on("/edit", HTTP_DELETE, handleFileDelete);
#else
  if(WiFi.getMode() == WIFI_MODE_STA)
  {
    
  Serial.printf("Setting up / GET -> handleNewRoot Callback\n");
  Server.on("/", HTTP_GET, handleNewRoot);
  Server.on("/Roast", HTTP_GET, handleNewRoot);

  Serial.printf("Setting up /style.css -> sendStyle Callback\n");
  Server.on("/style.css", HTTP_GET, sendStyle);

  Serial.printf("Setting up /Files -> handleFilesPage Callback\n");
  Server.on("/Files", HTTP_GET, handleFilesPage);

  Serial.printf("Setting up /getDir -> printDirectory Callback\n");
  Server.on("/getDir", HTTP_GET, printDirectory); //FileSystemFunctions.cpp
  }
#endif
  // These are in ConfigPage.h
#ifdef CONFIG_PAGE
  #ifndef NEW_WIFI
  Server.on("/SysConfig", handleConfigPage);
  
  #else
  Server.on("/SysConfig", HTTP_GET, handleConfigPage);
  //Server.on("/download",HTTP_GET,handleNotFound);
  #endif
#endif
#ifdef GRAPH_PAGE
 #ifndef NEW_WIFI
 Server.on("/Graphs", handleGraphsPage);
  Server.on("/graphs.js", handleGraphsJsPage);
 Server.on("/charts.js", handleChartsJsPage);
 #else
 
 if(WiFi.getMode() == WIFI_MODE_STA)
 {
   Server.on("/charts.js", HTTP_GET, handleChartsJsPage);
   Server.on("/Graphs", HTTP_GET, handleGraphsPage);
   Server.on("/graphs.js", HTTP_GET, handleGraphsJsPage);
 }
 #endif
#endif
#ifndef NEW_WIFI
  Server.on("/xml", SendXML);
  Server.on("/UPDATE_TEMP_SLIDER", UpdateSlider);
  Server.on("/BUTTON_MIXPWR", ProcessButtonMixPwr);
  Server.on("/BUTTON_MIXDIR", ProcessButtonMixDir);
  Server.on("/BUTTON_TIMER_START", ProcessButtonTimerStart);
  Server.on("/BUTTON_TIMER_ADD", ProcessButtonTimerAdd);
  Server.on("/BUTTON_TIMER_SUB", ProcessButtonTimerSub);
  Server.on("/BUTTON_ROAST_START", ProcessButtonRoastStart);
  Server.on("/BUTTON_HEATER_PWR", ProcessButtonHeaterPwr);
  Server.on("/MIN_BUTTON_0", ProcessMinButton_0);
  Server.on("/SEC_BUTTON_0", ProcessSecButton_0);
  
  Server.on("/PRE_MIN_BUTTON_0", ProcessPreMinButton_0);
  Server.on("/PRE_SEC_BUTTON_0", ProcessPreSecButton_0);
  
  Server.on("/PRE_TEMP_BUTTON", ProcessPreTempButton);
  Server.on("/PREHEAT_TIMER_START", ProcessPreheatTimerStart);
  Server.on("/FINAL_TEMP_BUTTON", ProcessFinalTempButton);
  Server.on("/BEAN_QTY_BUTTON", ProcessBeanQuantityButton);
  Server.on("/COFFEE_TYPE", ProcessCoffeeType);

  Server.on("/SetRoastSettings", ProcessRoastSettingsButton);
#else
  if(WiFi.getMode() == WIFI_MODE_STA)
  {
    Serial.printf("Setting up /xmlUpdate GET -> SendXML Callback\n");
    Server.on("/xmlUpdate", HTTP_GET , SendXML);
    //Server.on("/UPDATE_TEMP_SLIDER", HTTP_POST , UpdateSlider);
    //Serial.printf("Setting up /BUTTON_MIXPWR PUT -> ProcessButtonMixPwr\n");
    Server.on("/BUTTON_MIXPWR", HTTP_POST , ProcessButtonMixPwr);
    Server.on("/BUTTON_MIXDIR", HTTP_POST , ProcessButtonMixDir);
    Server.on("/BUTTON_TIMER_START", HTTP_POST , ProcessButtonTimerStart);
    Server.on("/BUTTON_TIMER_ADD", HTTP_POST , ProcessButtonTimerAdd);
    Server.on("/BUTTON_TIMER_SUB", HTTP_POST , ProcessButtonTimerSub);
    Server.on("/BUTTON_ROAST_START", HTTP_POST , ProcessButtonRoastStart);
    Server.on("/BUTTON_HEATER_PWR", HTTP_POST , ProcessButtonHeaterPwr);
    Server.on("/MIN_BUTTON_0", HTTP_POST , ProcessMinButton_0);
    Server.on("/SEC_BUTTON_0", HTTP_POST , ProcessSecButton_0);
    Server.on("/PRE_MIN_BUTTON_0", HTTP_POST , ProcessPreMinButton_0);
    Server.on("/PRE_SEC_BUTTON_0", HTTP_POST , ProcessPreSecButton_0);
    Server.on("/PRE_TEMP_BUTTON",  HTTP_POST ,ProcessPreTempButton);
    Server.on("/PREHEAT_TIMER_START", HTTP_POST , ProcessPreheatTimerStart);
    Server.on("/FINAL_TEMP_BUTTON",  HTTP_POST ,ProcessFinalTempButton);
    Server.on("/BEAN_QTY_BUTTON",  HTTP_POST ,ProcessBeanQuantityButton);
    Server.on("/COFFEE_TYPE",  HTTP_POST ,ProcessCoffeeType);
    Server.on("/SetRoastSettings", HTTP_ANY , ProcessRoastSettingsButton);
  }
#endif
#ifdef CONFIG_PAGE 
  // Config page 
  #ifndef NEW_WIFI
  Server.on("/TEMP_PREF", ProcessTempPref);

  Server.on("/TEMP_PROBE", ProcessTempProbe);
  Server.on("/REDIRECT", ProcessFileRedirect);
  Server.on("/UPDATE_TEMP_SLIDER", UpdateTempSlider);
  Server.on("/UPDATE_TEMPSENSOR_SLIDER", UpdateTempSensorSlider);
  Server.on("/UPDATE_HEATGUN_SLIDER", UpdateHeatGunSlider);
  
  Server.on("/TEMP_CONFIG_MAX_STEPS", ProcessConfigMaxSteps);
  Server.on("/TEMP_CONFIG_STEP_TIME", ProcessConfigStepTime);
  Server.on("/START_TEMP_CONFIG", ProcessTempConfig);
  Server.on("/TEMP_CONFIG_REQ", SendTempConfigData); //ConfigTemp.cpp
  Server.on("/ROAST_LOG_REQ", SendRoastLogData); //ConfigTemp.cpp
  Server.on("/DEBUG_VAL", SetDebug); //CoffeeRoaster.ino
  #else
  //if(WiFi.getMode() == WIFI_MODE_STA)
  //{
    Server.on("/ROAST_LOG_REQ",HTTP_GET, SendRoastLogData); //ConfigTemp.cpp
    Server.on("/TEMP_PREF", HTTP_POST, ProcessTempPref);

    Server.on("/TEMP_PROBE", HTTP_POST, ProcessTempProbe);
    //Server.on("/REDIRECT", HTTP_GET, ProcessFileRedirect);
    Server.on("/UPDATE_TEMP_SLIDER", HTTP_POST, UpdateTempSlider);
    Server.on("/UPDATE_TEMPSENSOR_SLIDER", HTTP_POST, UpdateTempSensorSlider);
    Server.on("/UPDATE_HEATGUN_SLIDER", HTTP_POST, UpdateHeatGunSlider);
  
    Server.on("/TEMP_CONFIG_MAX_STEPS", HTTP_POST, ProcessConfigMaxSteps);
    Server.on("/TEMP_CONFIG_STEP_TIME", HTTP_POST, ProcessConfigStepTime);
    Server.on("/START_TEMP_CONFIG", HTTP_POST, ProcessTempConfig);
    Server.on("/TEMP_CONFIG_REQ", HTTP_GET, SendTempConfigData); //ConfigTemp.cpp
    Server.on("/ROAST_LOG_REQ",HTTP_GET, SendRoastLogData); //ConfigTemp.cpp
    Server.on("/DeleteFile", HTTP_GET, handleFileDelete);//FileSystemFunctions.cpp
    Server.on("/UploadFile", HTTP_POST, [](AsyncWebServerRequest * request){
      request->send(200);
    },handleFileUpload);//FileSystemFunctions.cpp
   
    Server.on("/DEBUG_VAL",HTTP_POST, SetDebug);
    Server.on("/RESET_WIFI",HTTP_POST, ResetWiFi);
  //}
  #endif

  #ifndef NEW_WIFI
  /*handling uploading file */
  Server.on("/update", HTTP_POST, [](){
    Server.sendHeader("Connection", "close");
  },[](){
    HTTPUpload& upload = Server.upload();
    File root;
    if(opened == false){
      opened = true;
      root = openFile((String("/") + upload.filename).c_str(), FILE_WRITE);  
      if(!root){
        Serial.println("- failed to open file for writing");
        return;
      }
    } 
    if(upload.status == UPLOAD_FILE_WRITE){
      if(root.write(upload.buf, upload.currentSize) != upload.currentSize){
        Serial.println("- failed to write");
        return;
      }
    } else if(upload.status == UPLOAD_FILE_END){
      root.close();
      Serial.println("UPLOAD_FILE_END");
      opened = false;
    }
  });
  #else
  Server.onNotFound([](AsyncWebServerRequest * request) 
    {
      Serial.print(F("NOT_FOUND: "));
      //Serial.printf("method = %d\n",request->method());
      if (request->method() == HTTP_GET)
        Serial.print(F("GET"));
      else if (request->method() == HTTP_POST)
        Serial.print(F("POST"));
      else if (request->method() == HTTP_DELETE)
        Serial.print(F("DELETE"));
      else if (request->method() == HTTP_PUT)
        Serial.print(F("PUT"));
      else if (request->method() == HTTP_PATCH)
        Serial.print(F("PATCH"));
      else if (request->method() == HTTP_HEAD)
        Serial.print(F("HEAD"));
      else if (request->method() == HTTP_OPTIONS)
        Serial.print(F("OPTIONS"));
      else
        Serial.print(F(" UNKNOWN"));
      
      Serial.println(" http://" + request->host() + request->url());

      if (request->contentLength()) 
      {
        Serial.println("_CONTENT_TYPE: " + request->contentType());
        Serial.println("_CONTENT_LENGTH: " + request->contentLength());
      }

      int headers = request->headers();
      int i;
    
      for (i = 0; i < headers; i++) 
      {
        AsyncWebHeader* h = (AsyncWebHeader *)request->getHeader(i);
        Serial.println("_HEADER[" + h->name() + "]: " + h->value());
      }

      int params = request->params();
    
      for (i = 0; i < params; i++) 
      {
        AsyncWebParameter* p = (AsyncWebParameter *)request->getParam(i);
      
        if (p->isFile()) 
        {
          Serial.println("_FILE[" + p->name() + "]: " + p->value() + ", size: " + p->size());
        } 
        else if (p->isPost()) 
        {
          Serial.println("_POST[" + p->name() + "]: " + p->value());
        } 
        else 
        {
          Serial.println("_GET[" + p->name() + "]: " + p->value());
        }
      }

      request->send(404);
    });
  //FIXME
  ElegantOTA.begin(&Server);    // Start ElegantOTA
  if(WiFi.getMode() == WIFI_MODE_STA)
  {
    Serial.println("--- Starting Web Server ---");
    Server.begin();
  }
  #endif

#endif
  // test temp+config
  //temp_config(TempServo, 10,100);
#ifndef NEW_WIFI
  // Starts user web site included the AutoConnect portal.
  Portal.onDetect(atDetect);
  if (Portal.begin()) {
    Serial.println("Started, IP:" + WiFi.localIP().toString());
  }
  else {
    Serial.println("Connection failed.");
    while (true) {
      yield();
    }
  }
#endif
  // Init and get the time
  Serial.println("Setting up ntp time");
  configTime(gmtOffset_sec,daylightOffset_sec,ntpServer);
  printLocalTime();
  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
  }
  Serial.println("End of setup");
}

void loop() {
  
  ElegantOTA.loop();
 
}






