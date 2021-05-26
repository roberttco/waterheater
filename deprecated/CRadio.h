#ifndef _CRADIO_H_
#define _CRADIO_H_

#include <NRFLite.h>
#include "Types.h"

//#define RDEBUG

#ifdef RDEBUG
#define RDEBUG_PRINT(x) Serial.print(x)
#define RDEBUG_PRINTLN(x) Serial.println(x)
#else
#define RDEBUG_PRINT(x) 
#define RDEBUG_PRINTLN(X)
#endif


class CRadio
{
    private:
        bool m_radiook;
        NRFLite m_radio;
        DeviceRadio m_device;

    public:
        CRadio();
        CRadio(DeviceRadio device);
        CRadio(unsigned char id, unsigned char channel);

        bool ok();
        bool send(uint8_t destination, void *arg, uint8_t arg_length);
        void setId(int id);
        void setChannel(int channel);
        void saveConfig();
        unsigned char id();
        unsigned char channel();
};

#endif