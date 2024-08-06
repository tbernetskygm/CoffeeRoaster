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
//#define WEBSERIAL
//#define PRINT_RESET
//
// These are I/O Defines

#define PIN_SERVO 15
#define PIN_MIXER_DIR 18
#define PIN_MIX_POWER_ENABLE 2 // turn on power to mixer Same as builtin LED
#define PIN_HEATER_POWER_ENABLE 0 // turn on power to heat gun
// Analog In for Temp Sensor Have to use ADC1 Ch 0 because we are using WiFi
#define PIN_TEMP_SENSOR 36
#ifdef THERMOCOUPLE
#define PIN_THERMO_CLK 5 // thermocouple CLK VSPICSO
#define PIN_THERMO_CS 23 // thermocouple CS  VSPID
#define PIN_THERMO_DO 19 // thermocouple DO  VSPIQ
#endif

