#ifndef __CONFIG_H_
#define __CONFIG_H_

// EEPROM addresses
#define SPL_ADDRESS                 0
#define SPH_ADDRESS                 4
#define PFL_ADDRESS                 8
#define FUDL_ADDRESS                12
#define FPDL_ADDRESS                16
#define IP_ADDRESS                  20
#define RADIO_ID_ADDRESS            24
#define RADIO_CHANNEL_ADDRESS       25
#define LAST_ADDRESS                26

#define FIRST_OCTET                 1
#define SECOND_OCTET                2
#define THIRD_OCTET                 3
#define FOURTH_OCTET                4

// max temp limits
#define MAX_TEMPC                   50 // 122F

// substate intervals
#define WHSM_INVOKE_INTERVAL        500     // go through state machine every 500ms
#define FLOW_CALC_INTERVAL          1000    // calculate the flow rate once per second
#define TEMP_UPDATE_INTERVAL        5000    // update the temperature every 5 seconds
#define RADIO_SEND_INTERVAL         1000    // send a radio update every second
#define NETWORK_RECONNECT_INTERVAL  2000    // see the cable state every 2 seconds


#define RADIO_ID                    254     // Our radio's id - just because
#define HUB_RADIO_ID                0       // Id of the radio we will transmit to.
#define RADIO_CHANNEL               100

// pins - for keyestudio v1.0 Ethernet/SD/UNO dev board
#define FLOW_SENSE_PIN              2   // must be 2 since this is an interrupt pin
#define RESET_PIN                   3
#define SD_CS_PIN                   4   // cannot change but can be used if SD card is not used
#define ERRORLED_PIN                5
#define N_RELAY1_PIN                6     
#define N_RELAY2_PIN                7   
#define BYPASS_RELAY_PIN            8
#define DS_PIN                      9
#define RADIO_CS_PIN                A4
#define RADIO_CSN_PIN               A5
#define W5500_CS_PIN                10 
#define MOSI                        11
#define MISO                        12
#define SCK                         13

// default settings
#define DEFAULT_SPH                 48.8888889
#define DEFAULT_SPL                 47.7777778
#define DEFAULT_PFL                 1.0
#define DEFAULT_FUDL                3000
#define DEFAULT_FPDL                3100

#endif