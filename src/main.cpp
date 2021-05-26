#include <Arduino.h>
#include <avr/wdt.h>
#include <arduino-timer.h>
//#include <SPI.h>
//#include <PetitFS.h>

#include "Waterheater.h"

#include "Config.h"
#include "MyEEPROM.h"
#include "Types.h"
#include "NetworkManagement.h"
#include "WebServer.h"
#include "TemperatureSensor.h"
#include "StateMachine.h"
#include "Radio.h"

// SD stuff
#ifdef PetitFS_h
FATFS fs;
boolean sdok = false;
#endif

auto timer = timer_create_default(); // create a timer with default settings

WaterHeaterState whstate;
WaterHeaterControl whcontrol;

void flashError(unsigned char errorcode)
{
    DEBUG_PRINT(F("Fatal Error "));
    DEBUG_PRINTLN(errorcode);

    while (1)
    {
        if (errorcode > 0)
        {
            for (unsigned char i = 0; i < errorcode; i++)
            {
                digitalWrite(ERRORLED_PIN, HIGH);
                delay(250);
                digitalWrite(ERRORLED_PIN, LOW);
                delay(250);
            }
            digitalWrite(ERRORLED_PIN, LOW);
            delay(2000);
        }
    }
}

bool sendStateViaRadio(void *arg)
{
    radioSend(0, (WaterHeaterState *)&whstate);
    return true;
}

void setup()
{
    Serial.begin(115200); // for debugging

    pinMode(RESET_PIN, INPUT_PULLUP);
    pinMode(ERRORLED_PIN, OUTPUT);
    pinMode(N_RELAY1_PIN, OUTPUT);
    pinMode(N_RELAY2_PIN, OUTPUT);
    pinMode(BYPASS_RELAY_PIN, OUTPUT);

    // turn off relays
    digitalWrite(N_RELAY1_PIN, HIGH);       // relay is active low
    digitalWrite(N_RELAY2_PIN, HIGH);       // relay is active low
    digitalWrite(BYPASS_RELAY_PIN, HIGH);   // relay is active low

    Serial.println(F("BOOT"));

    // if the reset pin is low then reset everything to defaults
    if (digitalRead(RESET_PIN) == 0)
    {
        Serial.println(F("Reset pin low"));
        setDefaultEEPROMSettings(LAST_ADDRESS);
    }

#ifdef PetitFS_h
    // Initialize SD and file system.
    if (pf_mount(&fs))
    {
        Serial.println(F("SD error"));
        flashError(1);
    }
    else
    {
        Serial.println(F("SD mounted"));
        sdok = true;
    }
#endif

    if (!initRadio())
    {
        DEBUG_PRINTLN(F("Radio init error"));
    }
    else
    {
        whstate.radio.id = getRadioId();
        whstate.radio.channel = getRadioChannel();
    }

    timer.every(RADIO_SEND_INTERVAL, sendStateViaRadio);

    if (!initTemperatureSensor())
    {
        DEBUG_PRINTLN(F("No temperature sensor."));
        flashError(2);
    }
    else
    {
        timer.every(TEMP_UPDATE_INTERVAL, updateTemperature);
    }

    if (!initWaterHeaterSM())
    {
        DEBUG_PRINTLN(F("WHSM init error"));
        flashError(3);
    }
    else
    {
        timer.every(WHSM_INVOKE_INTERVAL, doWaterHeaterSM);
    }

    if (!initNetwork())
    {
        DEBUG_PRINTLN(F("Could not init network."));
    }

    if (!connectToNetwork(false))
    {
        DEBUG_PRINTLN(F("Couldn't connect to network.  Hoping for radio control."));
    }

    // Update the state with the current IP address
    getCurrentIpAddress(&whstate.ip.ip[0], &whstate.ip.ip[1], &whstate.ip.ip[2], &whstate.ip.ip[3], &whstate.ip.dhcp);
    
    // tick the pause timer every 100ms
    timer.every(100, doWaterHeaterPauseCheck);
    timer.every(FLOW_CALC_INTERVAL, doCalculateWaterFlow);

    wdt_enable(WDTO_4S);

    // and... go!
    
    whcontrol.enable = true;
}

unsigned long lastSend = 0;
unsigned long lastNetCheck = 0;
void loop()
{
    timer.tick();

    // Can't do this with timer.every without some rework so do this here.  Check for a
    // new connection (cable plug in) only oif the system is not heating.  This is because
    // a DHCP timeout is very long and may cause heating to go too far.
    if (millis() - lastNetCheck > NETWORK_RECONNECT_INTERVAL)
    {
        if (checkNetworkCablePlugin())
        {
            getCurrentIpAddress(&whstate.ip.ip[0], &whstate.ip.ip[1], &whstate.ip.ip[2], &whstate.ip.ip[3], &whstate.ip.dhcp);
        }

        lastNetCheck = millis();
    }

    if (networkConnected())
    {
        doWebServer();
        renewDhcpLease();
    }

    wdt_reset();
}
