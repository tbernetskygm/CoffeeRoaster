#ifndef GLOBALS_STRUCTS_H
#define GLOBALS_STRUCTS_H
#include <Arduino.h>
typedef struct ConfigStruct  {
	unsigned int ConfigStep;
	unsigned int ServoPosition;
	float tempC;
	float tempF;
} ConfigStruct;

// For Update Roast Log task
typedef struct RoastState {
	bool doRoast;
	bool mixerpwr;
	bool heaterpwr;
	bool preheat;
	bool roast;
	bool first;
	bool last;
	bool stopit;
	bool timerStart;
	bool preheatTimerStart;
	String fileName;
} RoastState;

#endif
