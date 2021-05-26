#include <Arduino.h>
#include <NRFLite.h>
#include <EEPROM.h>

#include "Radio.h"

#include "Config.h"
#include "Types.h"
#include "MyEEPROM.h"

NRFLite m_radio;
bool m_radioOk = false;

unsigned char radioChannel = RADIO_CHANNEL;
unsigned char radioId = RADIO_ID;

void saveRadioConfig()
{
    EEPROM.put(RADIO_ID_ADDRESS, radioId);
    EEPROM.put(RADIO_CHANNEL_ADDRESS, radioChannel);
}

void setRadioId(unsigned char id)
{
    EEPROM.put(RADIO_ID_ADDRESS, radioId);
}

void setRadioChannel(unsigned char channel)
{
    EEPROM.put(RADIO_CHANNEL_ADDRESS, radioChannel);
}

uint8_t getRadioId()
{
    return radioId;
}

uint8_t getRadioChannel()
{
    return radioChannel;
}

bool initRadio()
{
    radioId = getEEPROMByteValue(RADIO_ID_ADDRESS, RADIO_ID);
    radioChannel = getEEPROMByteValue(RADIO_CHANNEL_ADDRESS, RADIO_ID);

    EEPROM.put(RADIO_ID_ADDRESS, radioId);
    EEPROM.put(RADIO_CHANNEL_ADDRESS, radioChannel);

    if (!m_radio.init(radioId, RADIO_CS_PIN, RADIO_CSN_PIN, NRFLite::BITRATE2MBPS, radioChannel))
    {
        RDEBUG_PRINTLN(F("Cannot communicate with radio"));
        m_radioOk = false;
    }
    else
    {
        m_radioOk = true;
    }

    RDEBUG_PRINT(F("Startup radio ID = "));
    RDEBUG_PRINTLN(radioId);
    RDEBUG_PRINT(F("Startup radio channel = "));
    RDEBUG_PRINTLN(radioChannel);

    return m_radioOk;
}

bool radioOk()
{
    return m_radioOk;
}

bool radioSend(uint8_t destination, WaterHeaterState *arg)
{
    RadioPacket packet;

    packet.type = WHS;
    packet.deviceType = WATERHEATER;

    memcpy(&packet.whstate, arg, sizeof(WaterHeaterState));

    return (m_radio.send(HUB_RADIO_ID, &packet, sizeof(packet))); // Note how '&' must be placed in front of the variable name.
}

bool radioSend(uint8_t destination, RadioStringPayload *arg)
{
    RadioPacket packet;

    packet.type = STR;
    packet.deviceType = WATERHEATER;

    memcpy(&packet.message, arg, sizeof(RadioStringPayload));

    return (m_radio.send(HUB_RADIO_ID, &packet, sizeof(packet))); // Note how '&' must be placed in front of the variable name.
}
