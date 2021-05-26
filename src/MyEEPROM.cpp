#include <Arduino.h>
#include <EEPROM.h>

#include "MyEEPROM.h"

#include "Config.h"

#include "StateMachine.h"

void setDefaultEEPROMSettings(uint16_t lastAddress)
{
    EPDEBUG_PRINTLN(F("Setting default EEPROM values"));

    // set the default values in the EEPROM
    EEPROM.put(SPL_ADDRESS, (float)DEFAULT_SPL);
    EEPROM.put(SPH_ADDRESS, (float)DEFAULT_SPH);
    EEPROM.put(PFL_ADDRESS, (float)DEFAULT_PFL);
    EEPROM.put(FUDL_ADDRESS, (unsigned int)3000);
    EEPROM.put(FPDL_ADDRESS, (unsigned int)3100);
    EEPROM.put(IP_ADDRESS + 0, (byte)0);
    EEPROM.put(IP_ADDRESS + 1, (byte)0);
    EEPROM.put(IP_ADDRESS + 2, (byte)0);
    EEPROM.put(IP_ADDRESS + 3, (byte)0);
    EEPROM.put(RADIO_ID_ADDRESS, (byte)RADIO_ID);
    EEPROM.put(RADIO_CHANNEL_ADDRESS, (byte)RADIO_CHANNEL);
}

float getEEPROMFloatValue(unsigned int address, float default_value)
{
    EPDEBUG_PRINTLN(F("Getting EEPROM float value"));

    if (address > EEPROM.length())
    {
        return default_value;
    }

    float rval = 0.0;

    EEPROM.get(address, rval);

    if (isnan(rval))
        rval = default_value;

    EPDEBUG_PRINT(F("Got value "));
    EPDEBUG_PRINTLN(rval);

    return rval;
}

unsigned long getEEPROMUnsignedLongValue(unsigned int address, unsigned long default_value)
{
    EPDEBUG_PRINTLN(F("Getting EEPROM unsigned long value"));

    if (address > EEPROM.length())
    {
        return default_value;
    }

    unsigned long rval = 0L;

    EEPROM.get(address, rval);

    if (isnan(rval))
        rval = default_value;

    EPDEBUG_PRINT(F("Got value "));
    EPDEBUG_PRINTLN(rval);

    return rval;
}

unsigned char getEEPROMByteValue(unsigned int address, unsigned char default_value)
{
    EPDEBUG_PRINTLN(F("Getting EEPROM unsigned char value"));

    if (address > EEPROM.length())
    {
        return default_value;
    }

    unsigned char rval = 0;

    EEPROM.get(address, rval);

    if (isnan(rval))
        rval = default_value;

    EPDEBUG_PRINT(F("Got value "));
    EPDEBUG_PRINTLN(rval);

    return rval;
}

int getEEPROMIntValue(unsigned int address, int default_value)
{
    EPDEBUG_PRINTLN(F("Getting EEPROM int value"));

    if (address > EEPROM.length())
    {
        return default_value;
    }

    int rval = 0;

    EEPROM.get(address, rval);

    if (isnan(rval))
        rval = default_value;

    EPDEBUG_PRINT(F("Got value "));
    EPDEBUG_PRINTLN(rval);

    return rval;
}
