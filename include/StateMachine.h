#ifndef __WHSM_H_
#define __WHSM_H_

#include "Types.h"
#include <arduino-timer.h>

//#define SMDEBUG

#ifdef SMDEBUG
#define SMDEBUG_PRINT(x) Serial.print(x)
#define SMDEBUG_PRINTLN(x) Serial.println(x)
#else
#define SMDEBUG_PRINT(x) 
#define SMDEBUG_PRINTLN(X)
#endif

extern WaterHeaterState whstate;
extern WaterHeaterControl whcontrol;

bool initWaterHeaterSM();
bool doWaterHeaterSM(void *);
bool doCalculateWaterFlow(void *);
bool doWaterHeaterPauseCheck(void *);

#endif
