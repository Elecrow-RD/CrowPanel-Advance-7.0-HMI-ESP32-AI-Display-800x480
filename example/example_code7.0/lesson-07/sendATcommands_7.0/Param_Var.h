#ifndef _PARAM_VAR_H
#define _PARAM_VAR_H


#include <stdint.h>
#include <stdbool.h>

#define LORAWAN_APP_PARAM_ENABLE



typedef enum eLoRaMacRegion
{
  LORAMAC_REGION_TTN_AS923_2 = 0, //TTN AS923_2
  LORAMAC_REGION_AU915,  //Australian band on 915MHz
  LORAMAC_REGION_CN470, //Chinese band on 470MHz
  LORAMAC_REGION_CN779, //Chinese band on 779MHz
  LORAMAC_REGION_EU433, //European band on 433MHz
  LORAMAC_REGION_EU868, //European band on 868MHz
  LORAMAC_REGION_KR920, //South korean band on 920MHz
  LORAMAC_REGION_IN865, //India band on 865MHz
  LORAMAC_REGION_US915, //North american band on 915MHz
  LORAMAC_REGION_RU864, //Russia band on 864MHz

#ifdef LORAWAN_APP_PARAM_ENABLE
  LORAMAC_REGION_AS923_1, //AS band on 923MHz
  LORAMAC_REGION_AS923_2, //AS band on 923MHz
  LORAMAC_REGION_AS923_3, //AS band on 923MHz
  LORAMAC_REGION_AS923_4, //AS band on 923MHz
  LORAMAC_REGION_AS923_1B, //Helium AS923_1B
  LORAMAC_REGION_TTN_AS923_1, //TTN AS923_1
  LORAMAC_REGION_AS_GP_2,
  LORAMAC_REGION_AS_GP_3,
  LORAMAC_REGION_AS_GP_4,
#endif

  LORAMAC_REGION_MAX = 0xFF//none
} LoRaMacRegion_t;

typedef enum
{
  IOT_PLATFORM_SENSECAP_TTN,
  IOT_PLATFORM_OTHER,
  IOT_PLATFORM_HELIUM,
  IOT_PLATFORM_TTN,
  IOT_PLATFORM_SENSECAP_HELIUM,
  IOT_PLATFORM_MAX
} platform_t;

typedef enum eActivationType
{
  ACTIVATION_TYPE_NONE = 0,  //None
  ACTIVATION_TYPE_ABP = 1,  //Activation By Personalization (ACTIVATION_TYPE_ABP)
  ACTIVATION_TYPE_OTAA = 2, //Over-The-Air Activation (ACTIVATION_TYPE_OTAA)
} ActivationType_t;

typedef enum epos_type
{
  pos_gnss_raw = 0x12,
  pos_wifi = 0x14,
  pos_beac = 0x15,
  pos_state = 0x16,
} epos_type_t;



typedef struct lora_info
{
  //LoRaWAN
  uint64_t  DevEui;
  uint64_t  JoinEui;
  uint8_t  DeviceKey[16];
  uint8_t  AppKey[16];

  uint8_t  ActiveRegion;                 //Regional spectrum in LoRaMacRegion_t
  uint8_t  ActivationType;               //access net type 0: none,1: ABP 2: OTAA
  uint8_t  ADR;
  uint8_t  DR;
  uint8_t  dutyCycleEnabled;
  uint16_t  msPerHour;
  uint8_t  txPower;
  uint8_t  rx2dr;
  uint8_t  ChannelGroup;

  uint32_t  DevAddr;
  uint8_t  DeviceCode[8];

  uint8_t  NwkKey[16];
  uint8_t  AppSKey[16];
  uint8_t  NwkSKey[16];
  uint8_t  SnwksKey[16];
  uint8_t  FnwksKey[16];

} lora_info_t;


typedef struct  beac_context
{
  uint8_t beac_mac[6];
  int8_t  cur_rssi;
} beac_context_t;

typedef struct  wifi_context
{
  uint8_t wifi_mac[6];
  int8_t cur_rssi;
} wifi_context_t;

//Byte alignment of variables
#pragma pack(4)
typedef struct app_param
{
  uint8_t param_len;
  lora_info_t lora_info;
  uint8_t crc;
} app_param_t;

typedef struct gnss_group_param
{
  uint32_t   group_id;
} gnss_group_param_t;


typedef struct pos_msg_param
{
  uint8_t   pos_type;
  uint8_t   context_count;
  uint32_t  utc_time;
  uint32_t  pos_status;
  union {
    struct
    {
      uint8_t zone_flag;
      uint16_t group_id;
      uint8_t gnss_len;
      uint8_t gnss_res[46];
    } gps_context;
    struct
    {
      uint8_t len;
      uint8_t sensor_data[49];
    } sensor_context;
    wifi_context_t wifi_context[4];
    beac_context_t beac_context[3];
  } context;
} pos_msg_param_t;

typedef struct pos_msg_file_id
{
  uint8_t   reboot_cnt;
  uint16_t  log_cnt;
  uint16_t  file_id_start;
  uint16_t  file_id_cur;
} pos_msg_file_id_t;

typedef struct various_interval
{
  uint16_t  position_interval;
  uint16_t  sample_interval;
} various_interval_t;


typedef struct append_param
{
  platform_t  Platform;            // 0: SENSECAP_TTN, 1: OTHER, 2: Helium, 3: TTN, 4:SENSECAP_HELIUM
  uint8_t  Retry;
  bool cache_en;                   //cache data enabled/disable
  uint8_t res1;            //Four-byte alignment
  uint16_t  position_interval;
  uint16_t  sample_interval;
  union {
    struct
    {
      uint8_t res2;
      uint8_t res3;
      uint8_t res4;
      uint8_t res5;
    } Reserved_param;
    uint32_t Reserved;
    uint8_t res[4];
  };
} append_param_t;


#pragma pack()

extern app_param_t app_param;

extern pos_msg_file_id_t pos_msg_file_id;
extern pos_msg_param_t pos_msg_param;

extern gnss_group_param_t app_gnss_group_param;
extern append_param_t app_append_param;

extern uint16_t track_gnss_group_id;
#endif
