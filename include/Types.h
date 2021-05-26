#ifndef __EWHTYPES_H_
#define __EWHTYPES_H_

#define MAX_FEATURES 4


typedef struct
{
    unsigned char ip[4];
    boolean dhcp;
} DeviceIPAddress;

typedef struct 
{
    unsigned char id;
    unsigned char channel;
} DeviceRadio;

typedef struct 
{
    bool heating;
    bool pump;
    bool valve;
    bool forced;
    bool paused;
    bool bypass;
    float temperature_c;
    float flowrate_lpm;
    bool rebootRequired;
    DeviceIPAddress ip;
    DeviceRadio radio;
} WaterHeaterState;

typedef struct 
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

#define RP_STRING_CLEAR     0x01
#define RP_STRING_CENTER    0x02
#define RP_STRING_RIGHT     0x04
typedef struct 
{
    unsigned char row;
    unsigned char column;
    unsigned char hints;
    char msg[20];
} RadioStringPayload;

typedef enum { 
    UNDEFINED,
    INT, 
    FLOAT, 
    UCHAR, 
    BOOL, 
    STR,
    WHS, 
    WHC
} RadioPayloadType;

typedef enum {
    WATERHEATER,
    TEMPSENSOR
} OriginDeviceType;

typedef struct
{
    RadioPayloadType type;
    OriginDeviceType deviceType;
    union 
    {
        WaterHeaterState whstate;
        WaterHeaterControl whcontrol;
        RadioStringPayload message;
    };
    
} RadioPacket;


#endif
