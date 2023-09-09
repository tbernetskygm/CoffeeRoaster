#ifndef GLOBALS_STRUCTS_H
#define GLOBALS_STRUCTS_H

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
typedef struct ConfigStruct  {
	unsigned int ConfigStep;
	unsigned int ServoPosition;
	float tempC;
	float tempF;
} ConfigStruct;

#endif
