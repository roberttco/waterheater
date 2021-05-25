#ifndef __EWHTYPES_H_
#define __EWHTYPES_H_

#define MAX_FEATURES  4

typedef struct _DeviceIPAddress
{
    unsigned char ip[4];
    boolean dhcp;
} DeviceIPAddress;

typedef struct _DeviceRadio
{
    unsigned char id;
    unsigned char channel;
} DeviceRadio;

typedef struct _Device
{
    unsigned char family;
    void *features[MAX_FEATURES];
} Device;

typedef struct _WaterHeaterState
{
    bool heating;
    bool pump;
    bool valve;
    bool forced;
    bool paused;
    float temperature_c;
    float flowrate_lpm;
    bool settingsSaved;
    DeviceIPAddress ip;
    DeviceRadio radio;
} WaterHeaterState;

typedef struct _WaterHeaterControl
{
    float sph_c;
    float spl_c;
    float pfl_lpm;
    int unpausedelayms;
    int pausedelayms;
    bool enable;
    bool force;
    DeviceIPAddress ip;
    DeviceRadio radio;
} WaterHeaterControl;

#endif
