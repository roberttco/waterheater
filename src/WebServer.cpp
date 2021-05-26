#include <Arduino.h>
#include <avr/wdt.h>
#include <EEPROM.h>

#include "WebServer.h"

#include "Config.h"
#include "Types.h"
#include "Waterheater.h"
#include "NetworkManagement.h"
#include "Parser.h"
#include "StateMachine.h"
#include "Radio.h"

char buf[MAX_BUFLEN];
char path[PATH_BUFLEN];

#ifdef PetitFS_h
extern FATFS fs;
extern boolean sdok;
#define SD_BUFLEN 32
#endif

float floatval = 0;
int intval = 0;
unsigned char byteval = 0;

extern WaterHeaterState whstate;
extern WaterHeaterControl whcontrol;

bool doWebServer()
{
    EthernetClient client = getClient(); // try to get client

    if (client)
    {
        boolean currentLineIsBlank = true;
        bool cmdline = true;
        unsigned char bufptr = 0;
        path[0] = 0;
        unsigned char requestcommand = CMD_NONE;

        bool requestHandled = false;
        bool rebootPending = false;
        
        while (client.connected() && requestHandled == false)
        {
            if (client.available())
            {
                // client data available to read
                char c = client.read(); // read 1 unsigned char (character) from client

                if (bufptr < MAX_BUFLEN)
                {
                    buf[bufptr++] = c;
                }

                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n')
                {
                    if (currentLineIsBlank == true)
                    {
                        whstate.rebootRequired = false;

                        // execute code to handle commands that set variables
                        requestcommand = executeCommand(requestcommand,floatval,intval,byteval);

                        // begin response
                        client.println(F("HTTP/1.1 200 OK"));
                        client.println(F("X-Content-Type-Options: nosniff"));
                        client.println(F("Connection: close"));

                        switch (requestcommand)
                        {
                        case CMD_JSON:
                            client.println(F("Content-Type: application/json; charset=UTF-8"));
                            client.println(F("Cache-control: no-cache"));
                            break;
                        case CMD_FAVICON:
                            client.println(F("Content-Type: image/x-icon"));
                            client.println(F("Cache-control: max-age=31536000, immutable"));
                            break;
                        default:
                            client.println(F("Content-Type: text/html; charset=UTF-8"));
                            client.println(F("Cache-control: no-cache"));
                            break;
                        }

                        // end of response blank line
                        client.println();

                        // add response body
                        switch (requestcommand)
                        {
                        case CMD_JSON:
                            client.print(F("{\"state\":{\"temp_f\":"));
                            client.print(whstate.temperature_c * 1.8 + 32);
                            client.print(F(",\"temp_c\":"));
                            client.print(whstate.temperature_c);
                            client.print(F(",\"flowrate\":"));
                            client.print(whstate.flowrate_lpm);
                            client.print(F(",\"pump\":"));
                            client.print(whstate.pump);
                            client.print(F(",\"valve\":"));
                            client.print(whstate.valve);
                            client.print(F(",\"heating\":"));
                            client.print(whstate.heating);
                            client.print(F(",\"forced_heating\":"));
                            client.print(whstate.forced ? 1 : 0);
                            client.print(F(",\"paused\":"));
                            client.print(whstate.paused ? 1 : 0);
                            client.print(F(",\"bypass\":"));
                            client.print(whstate.bypass ? 1 : 0);
                            client.print(F(",\"reboot_required\":"));
                            client.print(whstate.rebootRequired ? 1 : 0);
                            client.print(F(",\"ip\":\""));
                            client.print(getCurrentIpAddress());
                            client.print(F(",\"dhcp\":\""));
                            client.print(whstate.ip.dhcp ? "1" : "0");
                            client.print(F("\",\"radio_id\":"));
                            client.print(getRadioId());
                            client.print(F(",\"radio_ch\":"));
                            client.print(getRadioChannel());
                            client.print(F("},\"control\":{\"force\":"));
                            client.print(whcontrol.force);
                            client.print(F(",\"sph\":"));
                            client.print(whcontrol.sph_c);
                            client.print(F(",\"spl\":"));
                            client.print(whcontrol.spl_c);
                            client.print(F(",\"pfl\":"));
                            client.print(whcontrol.pfl_lpm);
                            client.print(F(",\"fudl\":"));
                            client.print(whcontrol.unpausedelayms);
                            client.print(F(",\"fpdl\":"));
                            client.print(whcontrol.pausedelayms);
                            client.print(F(",\"enable\":"));
                            client.print(whcontrol.enable ? 1 : 0);
                            client.print(F("},\"ts\":\""));
                            client.print(millis());
                            client.print(F("\"}"));
                            break;
                        case CMD_REBOOT:
                            {
                                unsigned char t = getStoredIpAddressOctet(FIRST_OCTET);
                                if (t == 0)
                                {
                                    client.print(F("Rebooting. IP Address will be DHCP."));         
                                }
                                else
                                {
                                    client.print(F("<head><meta http-equiv='refresh' content='15; url=http://"));
                                    client.print(getStoredIpAddress());
                                    client.print("/'></head><html>Rebooting. Reloading in 15 seconds.</html>"); 
                                }
                            }
                            rebootPending = true;
                            break;
                        case CMD_GET:
                        case CMD_FAVICON:
#ifdef PetitFS_h
                            if (sdok)
                            {
                                WSDEBUG_PRINT("Attempting to send file ");
                                WSDEBUG_PRINTLN(path);
                                FRESULT fr = pf_open(path);
                                if (fr != FR_OK)
                                {
                                    WSDEBUG_PRINT(F("pf_open: "));
                                    WSDEBUG_PRINTLN(fr);
                                }
                                else
                                {
                                    uint8_t sdbuf[SD_BUFLEN];
                                    UINT nr = 0;
                                    do
                                    {
                                        fr = pf_read(buf, SD_BUFLEN, &nr);
                                        if (fr != FR_OK)
                                        {
                                            WSDEBUG_PRINT(F("pf_read: "));
                                            WSDEBUG_PRINTLN(fr);
                                            nr = 0;
                                        }
                                        else if (nr > 0)
                                        {
                                            client.write(buf, nr);
                                        }
                                    } while (nr > 0);
                                }
                                WSDEBUG_PRINTLN(F("... success"));
                            }
                            else
                            {
                                WSDEBUG_PRINTLN(F(" SD not ok."));
                            }
#else
                            WSDEBUG_PRINTLN(F("No SD library"));
#endif
                            break;
                        case CMD_INVALID:
                        default:
                            client.print(F("invalid command"));
                            break;
                        }
                        requestHandled = true;
                    }
                    else
                    {
                        // end of a line that is not blank

                        if (cmdline == true)
                        {
#ifdef WSDEBUG
                            WSDEBUG_PRINT(F("*** command ---> "));
                            for (int i = 0; i < bufptr && i < MAX_BUFLEN; Serial.write(buf[i++]))
                                ;
                            if (requestcommand != CMD_NONE)
                            {
                                WSDEBUG_PRINTLN(path);
                            }
#endif
                            requestcommand = parseCommandLine(buf, path, PATH_BUFLEN, &floatval, &intval, &byteval);
                            bufptr = 0;
                            cmdline = false;
                        }
                    }
                }

                // every line of text received from the client ends with \r\n
                if (c == '\n')
                {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                }
                else if (c != '\r')
                {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }

            } // end if (client.available())
        }     // end while (client.connected())

        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection

        if (rebootPending == true)
        {
            wdt_enable(WDTO_15MS);
            delay(100);
        }
    } // end if (client)

    return true;
}
