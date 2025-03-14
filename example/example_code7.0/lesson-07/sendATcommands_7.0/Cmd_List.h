#ifndef _CMD_LIST_H
#define _CMD_LIST_H

#include "Cmd_String.h"

#include <stdint.h>

/*
   AT Command Id errors. Note that they are in sync with ATError_description static array
   in command.c
*/
typedef enum eATEerror
{
  AT_OK = 0,
  AT_ERROR,
  AT_PARAM_ERROR,
  AT_BUSY_ERROR,
  AT_TEST_PARAM_OVERFLOW,
  AT_NO_NET_JOINED,
  AT_RX_ERROR,
  AT_NO_CLASS_B_ENABLE,
  AT_DUTYCYCLE_RESTRICTED,
  AT_CRYPTO_ERROR,
  AT_SAVE_FAILED,
  AT_READ_ERROR,
  AT_DELETE_ERROR,
  AT_UNKNOWN_ERROR,
  AT_MAX,
} ATEerror_t;

/*
   AT Command index
*/
typedef enum eATCmd
{
  /* General commands */
  VER_CMD = 0,
  CONFIG_CMD,
  DEFPARAM_CMD,
  HARDWAREVER_CMD,
  RESTORE_CMD,
  /* LoRaWAN network management commands */
  TYPE_CMD,
  BAND_CMD,
  CHANNEL_CMD,
  JOINEUI_CMD,
  APPKEY_CMD,
  DEUI_CMD,
  NWKSKEY_CMD,
  APPSKEY_CMD,
  DKEY_CMD,
  DADDR_CMD,
  DCODE_CMD,
  /* User commands */
  JOIN_CMD,
  SEND_CMD,
  ADR_CMD,
  DR_CMD,
  DCS_CMD,
  TXP_CMD,
  RX2DR_CMD,
  /*Commands end*/
  AT_CMD_MAX,
} eATCmd_t;

/**
   @brief  Structure defining an AT Command
*/
struct ATCommand_s
{
  const char *string;                   /*< command string, after the "AT" */
  const int32_t size_string;            /*< size of the command string, not including the final \0 */
  ATEerror_t (*get)(const char *param); /*< =? after the string to get the current value*/
  ATEerror_t (*set)(const char *param); /*< = (but not =?\0) after the string to set a value */
  ATEerror_t (*run)(const char *param); /*< \0 after the string - run the command */                     /* !NO_HELP */
};

extern const char *const ATError_description[AT_MAX];
extern const struct ATCommand_s ATCommand[AT_CMD_MAX];

#endif
