#ifndef __RADIO_H_
#define __RADIO_H_

#include "Types.h"

//#define RDEBUG

#ifdef RDEBUG
#define RDEBUG_PRINT(x) Serial.print(x)
#define RDEBUG_PRINTLN(x) Serial.println(x)
#else
#define RDEBUG_PRINT(x) 
#define RDEBUG_PRINTLN(X)
#endif

bool initRadio();
void saveRadioConfig();
void setRadioId(unsigned char id);
void setRadioChannel(unsigned char id);
uint8_t getRadioId();
uint8_t getRadioChannel();
bool radioOk();
bool radioSend(uint8_t destination, WaterHeaterState *arg);
bool radioSend(uint8_t destination, RadioStringPayload *arg);

#endif
