#include "CRadio.h"
#include "Config.h"

CRadio::CRadio()
{
    m_radiook = false;
}

CRadio::CRadio(unsigned char id, unsigned char channel)
{
    m_radiook = false;

    if (!m_radio.init(id, RADIO_CS_PIN, RADIO_CSN_PIN, NRFLite::BITRATE2MBPS, channel))
    {
        RDEBUG_PRINTLN(F("Cannot communicate with radio"));
        m_radiook = false;
    }
    else
    {
        m_device.id = id;
        m_device.channel = channel;
        m_radiook = true;
    }

    RDEBUG_PRINT(F("Startup radio ID = "));
    RDEBUG_PRINTLN(id);
    RDEBUG_PRINT(F("Startup radio channel = "));
    RDEBUG_PRINTLN(channel);
}

void CRadio::saveConfig()
{
    //EEPROM.put(RADIO_ID_ADDRESS,radioId);
    //EEPROM.put(RADIO_CHANNEL_ADDRESS,radioChannel);
}

bool CRadio::ok()
{
    return m_radiook;
}

unsigned char CRadio::id()
{
    return m_device.id;
}

unsigned char CRadio::channel()
{
    return m_device.channel;
}

bool CRadio::send(uint8_t destination, void *arg, uint8_t arg_length)
{
    if (m_radiook)
    {
        return m_radio.send(DESTINATION_RADIO_ID, arg, arg_length); // Note how '&' must be placed in front of the variable name.
    }
    else
    {
        return false;
    }
}

void CRadio::setId(int id)
{
    m_device.id = id;
}

void CRadio::setChannel(int channel)
{
    m_device.channel = channel;
}
