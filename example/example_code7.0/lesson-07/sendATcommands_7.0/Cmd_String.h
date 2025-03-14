#ifndef _CMD_STRING_H
#define _CMD_STRING_H

/* AT Command strings. Commands start with AT */
/* General commands */

#define AT_VER                  "+APIVER"            // Version
#define AT_CONFIG               "+CONFIG"       // All current configurations parameter
#define AT_DEFPARAM             "+DEFPARAM"     // All default configurations parameter


#define AT_HARDWAREVER      "+HARDWAREVER"    // Board hardware version 

#define AT_RESTORE            "+RESTORE"      // Restore the current parameters to default values

/* LoRaWAN network management commands */
#define AT_TYPE       "+TYPE"               // LoRaWAN access mode (OTAA or ABP) // 0: none, 1: ABP, 2: OTAA
#define AT_BAND       "+BAND"               // LoRaWAN region
#define AT_CHANNEL    "+CHANNEL"            // LoRaWAN Sub-band for US915&AU915

//OTAA
#define AT_JOINEUI    "+APPEUI"             // LoRaWAN AppEUI
#define AT_DEUI       "+DEUI"             // LoRaWAN DevEUI
#define AT_APPKEY     "+APPKEY"           // LoRaWAN AppKey
//#define AT_NWKKEY     "+NWKKEY"             // LoRaWAN NwkKey

//ABP
#define AT_DADDR      "+DADDR"              // Device Addr
//#define AT_FNWKSKEY   "+FNWKSKEY"          // LoRaWAN FnwksKey     
//#define AT_SNWKSKEY   "+SNWKSKEY"         // LoRaWAN SnwksKey  
#define AT_NWKSKEY    "+NWKSKEY"          // LoRaWAN NwksKey     
#define AT_APPSKEY    "+APPSKEY"          // LoRaWAN AppsKey  

#define AT_DKEY       "+DKEY"           // Device Key
#define AT_DCODE      "+DCODE"            // Device Code

//User commands
#define AT_JOIN       "+JOIN"           //  Lorawan In The Net
#define AT_SEND       "+SEND"            //  Send Data

#define AT_ADR       "+ADR"           //Adaptive Rate
#define AT_DR        "+DR"            //  Data rate
#define AT_DCS       "+DCS"            //  Duty cycle
#define AT_TXP       "+TXP"            //  transmit power
#define AT_RX2DR     "+RX2DR"            //  data rate of the Rx window 2
#endif
