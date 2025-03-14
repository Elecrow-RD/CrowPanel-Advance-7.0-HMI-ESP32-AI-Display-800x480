#include "Cmd_List.h"
#include "Cmd_Callback.h"

/**
   @brief  Array corresponding to the description of each possible AT Error
*/
// static const char *const ATError_description[] =
const char *const ATError_description[AT_MAX] =
{
  "\r\nOK\r\n",                      /* AT_OK */
  "\r\nAT_ERROR\r\n",                /* AT_ERROR */
  "\r\nAT_PARAM_ERROR\r\n",          /* AT_PARAM_ERROR */
  "\r\nAT_BUSY_ERROR\r\n",           /* AT_BUSY_ERROR */
  "\r\nAT_TEST_PARAM_OVERFLOW\r\n",  /* AT_TEST_PARAM_OVERFLOW */
  "\r\nAT_NO_NETWORK_JOINED\r\n",    /* AT_NO_NET_JOINED */
  "\r\nAT_RX_ERROR\r\n",             /* AT_RX_ERROR */
  "\r\nAT_NO_CLASS_B_ENABLE\r\n",    /* AT_NO_CLASS_B_ENABLE */
  "\r\nAT_DUTYCYCLE_RESTRICTED\r\n", /* AT_DUTYCYCLE_RESTRICTED */
  "\r\nAT_CRYPTO_ERROR\r\n",         /* AT_CRYPTO_ERROR */
  "\r\nAT_SAVE_FAILED\r\n",          /* AT_SAVE_FAILED */
  "\r\nAT_READ_FAILED\r\n",          /* AT_READ_ERROR */
  "\r\nAT_DELETE_ERROR\r\n",         /* AT_DELETE_ERROR */
  "\r\nerror unknown\r\n",           /* AT_UNKNOWN_ERROR */
};

/**
   @brief  Array of all supported AT Commands
*/
// static const struct ATCommand_s ATCommand[] =
const struct ATCommand_s ATCommand[AT_CMD_MAX] =
{
  /* General commands */
  {
    .string = AT_VER,
    .size_string = sizeof(AT_VER) - 1,
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_version_run,
  },
  {
    .string = AT_CONFIG,
    .size_string = sizeof(AT_CONFIG) - 1,
    .get = AT_Config_get,
    .set = AT_return_error,
    .run = AT_return_error,
  },
  {
    .string = AT_DEFPARAM,
    .size_string = sizeof(AT_DEFPARAM) - 1,
    .get = AT_DefParam_get,
    .set = AT_return_error,
    .run = AT_return_error,
  },
  {
    .string = AT_HARDWAREVER,
    .size_string = sizeof(AT_HARDWAREVER) - 1,
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_HARDWAREVER_run,
  },
  {
    .string = AT_RESTORE,
    .size_string = sizeof(AT_RESTORE) - 1,
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_Parameter_Reset,
  },


  /* LoRaWAN network management commands */
  {
    .string = AT_TYPE,
    .size_string = sizeof(AT_TYPE) - 1,
    .get = AT_ActivationType_get,
    .set = AT_ActivationType_set,
    .run = AT_return_error,
  },
  {
    .string = AT_BAND,
    .size_string = sizeof(AT_BAND) - 1,
    .get = AT_Region_get,
    .set = AT_Region_set,
    .run = AT_return_error,
  },
  {
    .string = AT_CHANNEL,
    .size_string = sizeof(AT_CHANNEL) - 1,
    .get = AT_ChannelGroup_get,
    .set = AT_ChannelGroup_set,
    .run = AT_return_error,
  },

  {
    .string = AT_JOINEUI,
    .size_string = sizeof(AT_JOINEUI) - 1,
    .get = AT_JoinEUI_get,
    .set = AT_JoinEUI_set,
    .run = AT_return_error,
  },
  {
    .string = AT_APPKEY,
    .size_string = sizeof(AT_APPKEY) - 1,
    .get = AT_AppKey_get,
    .set = AT_AppKey_set,
    .run = AT_return_error,
  },
  {
    .string = AT_DEUI,
    .size_string = sizeof(AT_DEUI) - 1,
    .get = AT_DevEUI_get,
    .set = AT_DevEUI_set,
    .run = AT_return_error,
  },
  {
    .string = AT_NWKSKEY,
    .size_string = sizeof(AT_NWKSKEY) - 1,
    .get = AT_NwkSKey_get,
    .set = AT_NwkSKey_set,
    .run = AT_return_error,
  },
  {
    .string = AT_APPSKEY,
    .size_string = sizeof(AT_APPSKEY) - 1,
    .get = AT_AppSKey_get,
    .set = AT_AppSKey_set,
    .run = AT_return_error,
  },
  {
    .string = AT_DKEY,
    .size_string = sizeof(AT_DKEY) - 1,
    .get = AT_DeviceKey_get,
    .set = AT_DeviceKey_set,
    .run = AT_return_error,
  },
  {
    .string = AT_DADDR,
    .size_string = sizeof(AT_DADDR) - 1,
    .get = AT_DevAddr_get,
    .set = AT_DevAddr_set,
    .run = AT_return_error,
  },
  {
    .string = AT_DCODE,
    .size_string = sizeof(AT_DCODE) - 1,
    .get = AT_DevCODE_get,
    .set = AT_DevCODE_set,
    .run = AT_return_error,
  },
  /* User commands */
  {
    .string = AT_JOIN,
    .size_string = sizeof(AT_JOIN) - 1,
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_Join,
  },
  {
    .string = AT_SEND,
    .size_string = sizeof(AT_SEND) - 1,
    .get = AT_return_error,
    .set = AT_Send_set,
    .run = AT_return_error,
  },
  {
    .string = AT_ADR,
    .size_string = sizeof(AT_ADR) - 1,
    .get = AT_ADR_get,
    .set = AT_ADR_set,
    .run = AT_return_error,
  },
  {
    .string = AT_DR,
    .size_string = sizeof(AT_DR) - 1,
    .get = AT_DR_get,
    .set = AT_DR_set,
    .run = AT_return_error,
  },
  {
    .string = AT_DCS,
    .size_string = sizeof(AT_DCS) - 1,
    .get = AT_DCS_get,
    .set = AT_DCS_set,
    .run = AT_return_error,
  },
  {
    .string = AT_TXP,
    .size_string = sizeof(AT_TXP) - 1,
    .get = AT_TXP_get,
    .set = AT_TXP_set,
    .run = AT_return_error,
  },
  {
    .string = AT_RX2DR,
    .size_string = sizeof(AT_RX2DR) - 1,
    .get = AT_RX2DR_get,
    .set = AT_RX2DR_set,
    .run = AT_return_error,
  },
};
