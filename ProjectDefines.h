// Global defines for project
// These are compiler options
//#define USE_FSYS SPIFFS
#define USE_FSYS 
#define LITTLEFS_FS
//#define SPIFFS_FS
#define CONFIG_PAGE
#define GRAPH_PAGE
#define Tempservo
#define THERMOCOUPLE
#define OTA_T
#define _RELEASE 2
#define _VERSION 0
#define _REVISION 0
#define WHENDEBUG(x) if( (x) & DebugNum )
//#define WEBSERIAL
//#define PRINT_RESET
//
// These are I/O Defines

#define PIN_SERVO 15 // IO15 pin 23
#define PIN_MIXER_DIR 18 //IO18 pin 30
#define PIN_MIX_POWER_ENABLE 2 // turn on power to mixer Same as builtin LED IO2 pin 24
#define PIN_HEATER_POWER_ENABLE 0 // turn on power to heat gun IO0 pin 25
// Analog In for Temp Sensor Have to use ADC1 Ch 0 because we are using WiFi
#define PIN_TEMP_SENSOR 36 // SENSOR_VP pin 5
#ifdef THERMOCOUPLE
#define PIN_THERMO_CLK 5 // thermocouple CLK VSPICSO IO5 pin 29
#define PIN_THERMO_CS 23 // thermocouple CS  VSPID IO23 pin 37
#define PIN_THERMO_DO 19 // thermocouple DO  VSPIQ IO19 pin 31
#endif

