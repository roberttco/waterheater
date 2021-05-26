#include <Arduino.h>
#include <Ethernet.h>
#include <EEPROM.h>

#include "NetworkManagement.h"

#include "Config.h"

unsigned char mac[6] = {0xE3, 0xA8, 0xD7, 0xE2, 0xF3, 0x7C};
unsigned char ip[4] = {0, 0, 0, 0};

// ethernet connection variables
unsigned char connectionStatus = NETWORK_DISCONNECTED;
bool networkInitialized = false;
EthernetLinkStatus lastLinkStatus = Unknown;

EthernetServer server = NULL;

EthernetServer getServer()
{
    return server;
}

EthernetClient getClient()
{
    return server.available();
}

bool networkOk()
{
    return networkInitialized;
}

unsigned char getNetworkConnectionStatus()
{
    return connectionStatus;
}

bool networkConnected()
{
    return (connectionStatus != NETWORK_DISCONNECTED);
}

void getCurrentIpAddress(unsigned char *ip1, unsigned char *ip2, unsigned char *ip3, unsigned char *ip4, bool *dhcp)
{
    *ip1 = ip[0];
    *ip2 = ip[1];
    *ip3 = ip[2];
    *ip4 = ip[3];

    *dhcp = (connectionStatus == NETWORK_DHCP);
}

IPAddress getCurrentIpAddress()
{
    return IPAddress(ip[0], ip[1], ip[2], ip[3]);
}

IPAddress getStoredIpAddress()
{
    unsigned char sip[4];

    for (register unsigned char i = 0; i < 4; i++)
    {
        EEPROM.get(IP_ADDRESS + i,sip[i]);
    }

    return IPAddress(sip[0],sip[1],sip[2],sip[3]);
}

unsigned char getStoredIpAddressOctet(int octet)
{
    unsigned char a;
    
    EEPROM.get(IP_ADDRESS + octet - 1,a);

    return a;
}

bool setIpAddressOctet(int octet, unsigned char value)
{
    if (octet == 4 && (value == 0 || value == 255))
    {
        return false;
    }
    else
    {
        EEPROM.put(IP_ADDRESS + octet - 1, value);  // octet is 1-based
        return true;
    }
}

bool initNetwork()
{
    // first things first - get the address from EEPROM - the single point of truth
    // if the value in EEPROM is bogus, then  write a 0 there.
    NMDEBUG_PRINT(F("Initializing network.\r\nGetting IP from EEPROM - "));

    for (uint8_t i = 0; i < 4; i++)
    {
        EEPROM.get(IP_ADDRESS + i, ip[i]);

        if (isnan(ip[i]))
        {
            ip[i] = 0;
            EEPROM.put(IP_ADDRESS + i, 0);
        }
    }

    NMDEBUG_PRINTLN(getCurrentIpAddress());

    server = EthernetServer(80); // create a server at port 80

    networkInitialized = true;
    return true;
}

bool connectToNetwork(bool reconnect = false)
{
    if ((networkInitialized == false) || ((connectionStatus != NETWORK_DISCONNECTED) && (reconnect == false)))
    {
        return false;
    }
    else
    {
        NMDEBUG_PRINTLN("");
        NMDEBUG_PRINTLN(F("Initializing NIC"));

        if (Ethernet.linkStatus() == LinkON)
        {
            Serial.print(F("Trying "));

            if (ip[0] == 0) // if the first octet is a 0 then use DHCP
            {
    #ifndef ALWAYS_USE_DEFAULT_IP
                if ((Ethernet.linkStatus() == LinkON) && (connectionStatus == NETWORK_DISCONNECTED))
                {
                    Serial.println(F("DHCP"));
                    connectionStatus = (Ethernet.begin(mac) == 1 ? NETWORK_DHCP : NETWORK_DISCONNECTED);
                }
    #endif
                if (connectionStatus == NETWORK_DISCONNECTED)
                {
                    Serial.println(F("DEFAULT"));
                    Ethernet.begin(mac, IPAddress(172, 54, 0, 2));
                    connectionStatus = NETWORK_STATIC;
                }
            }
            else
            {
                Serial.println(F("STATIC IP"));
                Ethernet.begin(mac, getCurrentIpAddress());
                connectionStatus = NETWORK_STATIC;
            }

            Serial.println(F("IP"));
        }
        else
        {
            Serial.println(F("No ETH link"));
            connectionStatus = NETWORK_DISCONNECTED;
        }
    }

    if (connectionStatus == NETWORK_DISCONNECTED)
    {
        Serial.println(F("ETH connect failed"));
    }
    else
    {
        Serial.println(Ethernet.localIP());
        server.begin(); // start to listen for clients
    }

    // update th working IP address after DHCP opr default assignment;
    IPAddress a = Ethernet.localIP();
            
    ip[0] = a[0];
    ip[1] = a[1];
    ip[2] = a[2];
    ip[3] = a[3];

    return networkConnected();
}

void renewDhcpLease()
{
    // renew DHCP lease if needed - https://www.arduino.cc/en/Reference/EthernetMaintain
    if (connectionStatus == NETWORK_DHCP)
    {
        unsigned char renewStatus = Ethernet.maintain();
        if (renewStatus == 1 || renewStatus == 3)
        {
            connectionStatus = NETWORK_DISCONNECTED;
        }
    }
}

// return true if the cable was plugged in
bool checkNetworkCablePlugin()
{
    bool rval = false;

    EthernetLinkStatus linkStatus = Ethernet.linkStatus();

    if (lastLinkStatus == LinkOFF && linkStatus == LinkON) // cable plug-in
    {
        Serial.println(F("Cable plugged in"));
        if (initNetwork())
        {
            connectToNetwork(false);
            rval = true;
        }
    }
    else if (lastLinkStatus == LinkON && linkStatus == LinkOFF) // cable unplug
    {
        Serial.println(F("Cable unplugged"));
        networkInitialized = false;
        connectionStatus = NETWORK_DISCONNECTED;
    }

    lastLinkStatus = linkStatus;
    return rval;
}
