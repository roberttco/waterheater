#ifndef __NETWORKMANAGEMENT_H__
#define __NETWORKMANAGEMENT_H__

#include <Ethernet.h>

//#define NMDEBUG

#ifdef NMDEBUG
#define NMDEBUG_PRINT(x) Serial.print(x)
#define NMDEBUG_PRINTLN(x) Serial.println(x)
#else
#define NMDEBUG_PRINT(x) 
#define NMDEBUG_PRINTLN(X)
#endif

#define MAX_DHCP_RETRIES    3

//#define ALWAYS_USE_DEFAULT_IP

// ethernet connection variables
#define NETWORK_DISCONNECTED 0
#define NETWORK_DHCP 1
#define NETWORK_STATIC 2

void getCurrentIpAddress(unsigned char *ip1, unsigned char *ip2, unsigned char *ip3, unsigned char *ip4, bool *dhcp);
IPAddress getCurrentIpAddress();
IPAddress getStoredIpAddress();
unsigned char getStoredIpAddressOctet(int octet);

bool connectToNetwork(bool reconnect);

bool networkOk();
bool checkNetworkCablePlugin();
bool networkConnected();
unsigned char getNetworkConnectionStatus();

void renewDhcpLease();
void saveIpAddress();
EthernetServer getServer();
EthernetClient getClient();
bool setIpAddressOctet(int octet, unsigned char value);
bool initNetwork();

#endif
