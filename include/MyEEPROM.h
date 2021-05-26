#ifndef __MYEEPROM_H_
#define __MYEEPROM_H_



//#define EPDEBUG

#ifdef EPDEBUG
#define EPDEBUG_PRINT(x) Serial.print(x)
#define EPDEBUG_PRINTLN(x) Serial.println(x)
#else
#define EPDEBUG_PRINT(x) 
#define EPDEBUG_PRINTLN(X)
#endif

void setDefaultEEPROMSettings(uint16_t lastAddress);
float getEEPROMFloatValue(unsigned int address, float default_value);
unsigned long getEEPROMUnsignedLongValue(unsigned int address, unsigned long default_value);
unsigned char getEEPROMByteValue(unsigned int address, unsigned char default_value);
int getEEPROMIntValue(unsigned int address, int default_value);

#endif
