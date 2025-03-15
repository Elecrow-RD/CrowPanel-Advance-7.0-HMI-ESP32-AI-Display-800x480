#include "Cmd_Callback.h"
#include <Arduino.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define tiny_sscanf sscanf

#define SE_KEY_SIZE 16
#define SE_EUI_SIZE 8

uint8_t OTAA_Join_Flag = 0;
uint8_t ABP_Join_Flag = 0;

uint8_t Send_Flag = 0;

uint8_t ADR_set_Flag = 0;
uint8_t DR_set_Flag = 0;
uint8_t DCS_set_Flag = 0;
uint8_t TXP_set_Flag = 0;
uint8_t RX2DR_set_Flag = 0;
uint8_t Band_set_Flag = 0;

uint8_t Port_Number = 0;
uint8_t Ack_State = 0;
uint8_t MsgTemp[128];
size_t  MsgTemp_size;

int8_t ascii_4bit_to_hex(uint8_t ascii) {
  int8_t result = -1;
  if ((ascii >= '0') && (ascii <= '9')) {
    result = ascii - 0x30;
  } else if ((ascii >= 'a') && (ascii <= 'f')) {
    result = ascii - 'a' + 10;
  } else if ((ascii >= 'A') && (ascii <= 'F')) {
    result = ascii - 'A' + 10;
  }
  return result;
}

/**
   @brief  Print 4 bytes as %02x

   @param  value containing the 4 bytes to print

   @retval
*/
static void print_uint32_as_02x(uint32_t value);

/**
   @brief  Print 8 bytes as %02x

   @param  value containing the 8 bytes to print

   @retval
*/
static void print_uint64_as_02x(uint64_t value);

/**
   @brief  Print 16 bytes as %02X

   @param  pt pointer containing the 16 bytes to print

   @retval
*/
static void print_16_02x(uint8_t *pt);

/**
   @brief  Print 8 bytes as %02X

   @param  pt pointer containing the 8 bytes to print

   @retval
*/
static void print_8_02x(uint8_t *pt);

/**
   @brief  Data analysis

   @param  param

   @param  Buff

   @param  lenth

   @retval  int8_t
*/
static int8_t Data_Analysis(const char *param, uint8_t Buff[], uint8_t lenth) {
  uint8_t param_length = 0;
  int8_t result = 0;
  for (uint8_t j = 0; j < lenth; j++) {
    if (param_length > strlen(param)) {
      return -1;
    }
    result = ascii_4bit_to_hex(param[param_length++]);
    if (result == -1) {
      return -1;
    } else {
      Buff[j] = result << 4;
    }
    result = ascii_4bit_to_hex(param[param_length++]);
    if (result == -1) {
      return -1;
    } else {
      Buff[j] = Buff[j] | result;
    }
    param_length++;  // The byte is a colon
  }
  return 0;
}

static void print_uint32_as_02x(uint32_t value) {
  AT_PRINTF("\"%02X:%02X:%02X:%02X\"",
            (unsigned)((unsigned char *)(&value))[3],
            (unsigned)((unsigned char *)(&value))[2],
            (unsigned)((unsigned char *)(&value))[1],
            (unsigned)((unsigned char *)(&value))[0]);
}

static void print_uint64_as_02x(uint64_t value) {
  AT_PRINTF("\"%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\"",
            (unsigned)((unsigned char *)(&value))[7],
            (unsigned)((unsigned char *)(&value))[6],
            (unsigned)((unsigned char *)(&value))[5],
            (unsigned)((unsigned char *)(&value))[4],
            (unsigned)((unsigned char *)(&value))[3],
            (unsigned)((unsigned char *)(&value))[2],
            (unsigned)((unsigned char *)(&value))[1],
            (unsigned)((unsigned char *)(&value))[0]);
}

static void print_16_02x(uint8_t *pt) {
  AT_PRINTF("\"%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\"",
            pt[0], pt[1], pt[2], pt[3],
            pt[4], pt[5], pt[6], pt[7],
            pt[8], pt[9], pt[10], pt[11],
            pt[12], pt[13], pt[14], pt[15]);
}

static void print_8_02x(uint8_t *pt) {
  AT_PRINTF("\"%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\"",
            pt[0], pt[1], pt[2], pt[3], pt[4], pt[5], pt[6], pt[7]);
}


ATEerror_t AT_return_ok(const char *param) {
  return AT_OK;
}

ATEerror_t AT_return_error(const char *param) {
  return AT_ERROR;
}

ATEerror_t AT_DefEUI_get(const char *param) {
  return AT_OK;
}

/* --------------- General commands --------------- */


/*------------------------AT+VER\r\n-------------------------------------*/

ATEerror_t AT_version_run(const char *param) {
  AT_PRINTF("\r\n");
  AT_PRINTF("Lora_Module_Dev_Boar_AT_Software_V0.1,Support_Lorawan_Version:V1.0.3");
  return AT_OK;
}
/*------------------------AT+VER\r\n-------------------------------------*/

/*------------------------AT+CONFIG=?\r\n -------------------------------------*/
ATEerror_t AT_Config_get(const char *param) {

  return AT_OK;
}
/*------------------------AT+CONFIG=?\r\n -------------------------------------*/

/*------------------------AT+AT_DEFPARAM=?\r\n -------------------------------------*/
ATEerror_t AT_DefParam_get(const char *param) {

  return AT_OK;
}
/*------------------------AT+AT_DEFPARAM=?\r\n -------------------------------------*/

/*------------------------AT+HARDWAREVER\r\n-------------------------------------*/
ATEerror_t AT_HARDWAREVER_run(const char *param) {
  AT_PRINTF("\r\n");
  AT_PRINTF("Lora_Module_Dev_Boar_V0.1");
  return AT_OK;
}
/*------------------------AT+HARDWAREVER\r\n-------------------------------------*/

/*------------------------AT+RESTORE\r\n-------------------------------------*/
ATEerror_t AT_Parameter_Reset(const char *param) {
 
  return AT_OK;
}
/*------------------------AT+RESTORE\r\n-------------------------------------*/



/* ------------------------LoRaWAN network management commands ------------------------*/

/*------------------------AT+TYPE=?\r\n-------------------------------------*/
ATEerror_t AT_ActivationType_get(const char *param) {
  const char *activationTypeStrings[] = { "NONE", "ABP", "OTAA" };

  if (app_param.lora_info.ActivationType > ACTIVATION_TYPE_OTAA) {
    return AT_PARAM_ERROR;
  }
  AT_PRINTF("%d", app_param.lora_info.ActivationType);
  return AT_OK;
}

ATEerror_t AT_ActivationType_set(const char *param) {
  uint16_t activetype;
  if (tiny_sscanf(param, "%hu", &activetype) != 1) {
    return AT_PARAM_ERROR;
  }
  if (activetype > ACTIVATION_TYPE_OTAA) {
    return AT_PARAM_ERROR;
  }
  if (app_param.lora_info.ActivationType != activetype) {
    app_param.lora_info.ActivationType = activetype == ACTIVATION_TYPE_OTAA ? ACTIVATION_TYPE_OTAA : ACTIVATION_TYPE_ABP;
  }
  return AT_OK;
}
/*------------------------AT+TYPE=?\r\n -------------------------------------*/

/*------------------------AT+BAND=?\r\n-------------------------------------*/
ATEerror_t AT_Region_get(const char *param) {
  AT_PRINTF("%d", app_param.lora_info.ActiveRegion);
  return AT_OK;
}

ATEerror_t AT_Region_set(const char *param) {

  uint16_t region;
  Band_set_Flag = 1;
  if (tiny_sscanf(param, "%hu", &region) != 1) {
    return AT_PARAM_ERROR;
  }
  if (region >= 2) {
    return AT_PARAM_ERROR;
  }
  if (app_param.lora_info.ActiveRegion != region) {
    app_param.lora_info.ActiveRegion = region;
  }
  return AT_OK;
}
/*------------------------AT+BAND=?\r\n-------------------------------------*/

/*------------------------AT+CHANNEL=?\r\n -------------------------------------*/
ATEerror_t AT_ChannelGroup_get(const char *param) {
  return AT_OK;
}

ATEerror_t AT_ChannelGroup_set(const char *param) {
  uint16_t channelgroup = 0;
  return AT_OK;
}
/*------------------------AT+CHANNEL=?\r\n -------------------------------------*/

/*------------------------AT+APPEUI=?\r\n-------------------------------------*/
ATEerror_t AT_JoinEUI_get(const char *param) {
  print_uint64_as_02x(app_param.lora_info.JoinEui);
  return AT_OK;
}

ATEerror_t AT_JoinEUI_set(const char *param) {
  uint64_t joineui;
  uint8_t joineuitmp[8];
  if ((Data_Analysis(param, joineuitmp, 8) == -1) || (strlen(param) != 25)) {
    return AT_PARAM_ERROR;
  }
  joineui = (static_cast<uint64_t>(joineuitmp[0]) << 56) | (static_cast<uint64_t>(joineuitmp[1]) << 48) | (static_cast<uint64_t>(joineuitmp[2]) << 40) | (static_cast<uint64_t>(joineuitmp[3]) << 32) | (static_cast<uint64_t>(joineuitmp[4]) << 24) | (static_cast<uint64_t>(joineuitmp[5]) << 16) | (static_cast<uint64_t>(joineuitmp[6]) << 8) | static_cast<uint64_t>(joineuitmp[7]);
  if (app_param.lora_info.JoinEui != joineui) {
    app_param.lora_info.JoinEui = joineui;
  }
  return AT_OK;
}
/*------------------------AT+APPEUI=?\r\n-------------------------------------*/

/*------------------------AT+APPKEY=?\r\n-------------------------------------*/
ATEerror_t AT_AppKey_get(const char *param) {
  print_16_02x(app_param.lora_info.AppKey);
  return AT_OK;
}

ATEerror_t AT_AppKey_set(const char *param) {

  uint8_t appKey[16];
  if ((Data_Analysis(param, appKey, 16) == -1) || (strlen(param) != 49)) {
    return AT_PARAM_ERROR;
  }
  if (memcmp(&app_param.lora_info.AppKey[0], appKey, SE_KEY_SIZE) != 0) {
    memcpy((uint8_t *)app_param.lora_info.AppKey, appKey, SE_KEY_SIZE);
  }
  return AT_OK;
}
/*------------------------AT+APPKEY=?\r\n-------------------------------------*/

/*------------------------AT+DEUI=?\r\n-------------------------------------*/
ATEerror_t AT_DevEUI_get(const char *param) {
  print_uint64_as_02x(app_param.lora_info.DevEui);
  return AT_OK;
}
ATEerror_t AT_DevEUI_set(const char *param) {
  uint64_t devEui;
  uint8_t devEuitmp[8];
  if ((Data_Analysis(param, devEuitmp, 8) == -1) || (strlen(param) != 25)) {
    return AT_PARAM_ERROR;
  }
  devEui = (static_cast<uint64_t>(devEuitmp[0]) << 56) | (static_cast<uint64_t>(devEuitmp[1]) << 48) | (static_cast<uint64_t>(devEuitmp[2]) << 40) | (static_cast<uint64_t>(devEuitmp[3]) << 32) | (static_cast<uint64_t>(devEuitmp[4]) << 24) | (static_cast<uint64_t>(devEuitmp[5]) << 16) | (static_cast<uint64_t>(devEuitmp[6]) << 8) | static_cast<uint64_t>(devEuitmp[7]);
  if (app_param.lora_info.DevEui != devEui) {
    app_param.lora_info.DevEui = devEui;
  }
  return AT_OK;
}
/*------------------------AT+NWKKEY=?\r\n-------------------------------------*/

/*------------------------AT+NWKSKEY=?\r\n-------------------------------------*/
ATEerror_t AT_NwkSKey_get(const char *param) {
  print_16_02x(app_param.lora_info.NwkSKey);
  return AT_OK;
}

ATEerror_t AT_NwkSKey_set(const char *param) {
  uint8_t nwkSKey[16];
  if ((Data_Analysis(param, nwkSKey, 16) == -1) || (strlen(param) != 49)) {
    return AT_PARAM_ERROR;
  }
  if (memcmp(&app_param.lora_info.NwkSKey[0], nwkSKey, 16) != 0) {
    memcpy((uint8_t *)app_param.lora_info.NwkSKey, nwkSKey, 16);
  }
  return AT_OK;
}
/*------------------------AT+NWKSKEY=?\r\n-------------------------------------*/

/*------------------------AT+APPSKEY=?\r\n -------------------------------------*/
ATEerror_t AT_AppSKey_get(const char *param) {
  print_16_02x(app_param.lora_info.AppSKey);
  return AT_OK;
}

ATEerror_t AT_AppSKey_set(const char *param) {
  uint8_t appskey[16];
  if ((Data_Analysis(param, appskey, 16) == -1) || (strlen(param) != 49)) {
    return AT_PARAM_ERROR;
  }
  if (memcmp(&app_param.lora_info.AppSKey[0], appskey, SE_KEY_SIZE) != 0) {
    memcpy((uint8_t *)app_param.lora_info.AppSKey, appskey, SE_KEY_SIZE);
  }
  return AT_OK;
}
/*------------------------AT+SNWKSKEY=?\r\n -------------------------------------*/

/*------------------------AT+DKEY=?\r\n-------------------------------------*/
ATEerror_t AT_DeviceKey_get(const char *param) {
  print_16_02x(app_param.lora_info.DeviceKey);
  return AT_OK;
}

ATEerror_t AT_DeviceKey_set(const char *param) {
  uint8_t deviceKey[16];
  if ((Data_Analysis(param, deviceKey, 16) == -1) || (strlen(param) != 49)) {
    return AT_PARAM_ERROR;
  }
  if (memcmp(&app_param.lora_info.DeviceKey[0], deviceKey, SE_KEY_SIZE) != 0) {
    memcpy((uint8_t *)app_param.lora_info.DeviceKey, deviceKey, SE_KEY_SIZE);
  }
  return AT_OK;
}
/*------------------------AT+DKEY=?\r\n-------------------------------------*/

/*------------------------AT+DADDR=?\r\n -------------------------------------*/
ATEerror_t AT_DevAddr_get(const char *param) {
  print_uint32_as_02x(app_param.lora_info.DevAddr);
  return AT_OK;
}

ATEerror_t AT_DevAddr_set(const char *param) {

  uint32_t devAddr;
  uint8_t devAddrtmp[4];
  if ((Data_Analysis(param, devAddrtmp, 4) == -1) || (strlen(param) != 13)) {
    return AT_PARAM_ERROR;
  }

  devAddr = (devAddrtmp[0] << 24) | (devAddrtmp[1] << 16) | (devAddrtmp[2] << 8) | devAddrtmp[3];
  if (app_param.lora_info.DevAddr != devAddr) {
    app_param.lora_info.DevAddr = devAddr;
  }
  return AT_OK;
}
/*------------------------AT+DADDR=?\r\n -------------------------------------*/

/*------------------------AT+DCODE=?\r\n-------------------------------------*/
ATEerror_t AT_DevCODE_get(const char *param) {
  print_8_02x(app_param.lora_info.DeviceCode);
  return AT_OK;
}

ATEerror_t AT_DevCODE_set(const char *param) {

  uint8_t devCode[8];

  if ((Data_Analysis(param, devCode, 8) == -1) || (strlen(param) != 25)) {
    return AT_PARAM_ERROR;
  }
  if (memcmp(&app_param.lora_info.DeviceCode[0], devCode, SE_EUI_SIZE) != 0) {
    memcpy((uint8_t *)app_param.lora_info.DeviceCode, devCode, SE_EUI_SIZE);
  }

  return AT_OK;
}
/*------------------------AT+DCODE=?\r\n-------------------------------------*/

/*------------------------AT+Join=?\r\n-------------------------------------*/
ATEerror_t AT_Join(const char *param) {
  if (app_param.lora_info.ActivationType == 1) {
    ABP_Join_Flag = 1;
    OTAA_Join_Flag = 0;
  } else if (app_param.lora_info.ActivationType == 2) {
    OTAA_Join_Flag = 1;
    ABP_Join_Flag = 0;
  } else {
    OTAA_Join_Flag = 0;
    ABP_Join_Flag = 0;
    AT_PRINTF("ERROR Join None");
  }
  return AT_OK;
}
/*------------------------AT+Join=?\r\n-------------------------------------*/

void hexStringToUint8Array(const char *hexStr, uint8_t *MsgTemp, size_t *actualSize) {
    size_t length = strlen(hexStr);
    *actualSize = 0; // Initial count

    if (length % 2 != 0) {
        printf("Invalid hex string length.\n");
        return;
    }

    for (size_t i = 0; i < length / 2; i++) {
        char byteStr[3]; // 2 characters + terminator
        byteStr[0] = hexStr[i * 2];
        byteStr[1] = hexStr[i * 2 + 1];
        byteStr[2] = '\0'; // End of symbol

        MsgTemp[i] = (uint8_t)strtol(byteStr, NULL, 16);
        (*actualSize)++; // Increment count
    }
}

/*------------------------AT+Send=?\r\n-------------------------------------*/
ATEerror_t AT_Send_set(const char *param) {
  //  AT_PRINTF(param);
  size_t paramLength = strlen(param) + 1;
  char paramCopy[paramLength];
  strcpy(paramCopy, param);
  char *token;
  char delim[] = ":";
  token = strtok(paramCopy, delim);
  Port_Number = atoi(token);
  Serial.print("Port_Number: ");
  Serial.println(Port_Number);
  token = strtok(NULL, delim);
  Ack_State = atoi(token);
  Serial.print("Ack_State: ");
  Serial.println(Ack_State);
  token = strtok(NULL, delim);
  Serial.print("data_State: ");
  Serial.println(token);
  hexStringToUint8Array(token, MsgTemp, &MsgTemp_size);
  Send_Flag = 1;
  return AT_OK;
}
/*------------------------AT+Send=?\r\n-------------------------------------*/

/*------------------------AT+ADR=?\r\n-------------------------------------*/
ATEerror_t AT_ADR_get(const char *param) {
  AT_PRINTF("AT+ADR=%d", app_param.lora_info.ADR);
  return AT_OK;
}

ATEerror_t AT_ADR_set(const char *param) {
  uint8_t adr;
  if (tiny_sscanf(param, "%hu", &adr) != 1) {
    return AT_PARAM_ERROR;
  }
  if (adr > 1) {
    return AT_PARAM_ERROR;
  }
  ADR_set_Flag = 1;
  app_param.lora_info.ADR = adr;

  return AT_OK;
}
/*------------------------AT+ADR=?\r\n-------------------------------------*/

/*------------------------AT+DR=?\r\n-------------------------------------*/
ATEerror_t AT_DR_get(const char *param) {
  AT_PRINTF("AT+DR=%d", app_param.lora_info.DR);
  return AT_OK;
}

ATEerror_t AT_DR_set(const char *param) {
  uint8_t dr;
  if (tiny_sscanf(param, "%hu", &dr) != 1) {
    return AT_PARAM_ERROR;
  }
  if (dr > 8) {
    return AT_PARAM_ERROR;
  }
  app_param.lora_info.DR = dr;
  DR_set_Flag = 1;
  return AT_OK;
}
/*------------------------AT+DR=?\r\n-------------------------------------*/

/*------------------------AT+DCS=?\r\n-------------------------------------*/
ATEerror_t AT_DCS_get(const char *param) {
  AT_PRINTF("AT+DCS=%d,%d", app_param.lora_info.dutyCycleEnabled, app_param.lora_info.msPerHour);
  return AT_OK;
}

ATEerror_t AT_DCS_set(const char *param) {
  uint8_t DutyCycleEnabled;
  uint16_t MsPerHour;
  if (tiny_sscanf(param, "%d,%u", &DutyCycleEnabled, &MsPerHour) != 2) {
    return AT_PARAM_ERROR;
  }
  if (DutyCycleEnabled > 1) {
    return AT_PARAM_ERROR;
  }
  app_param.lora_info.dutyCycleEnabled = DutyCycleEnabled;
  app_param.lora_info.msPerHour = MsPerHour;
  DCS_set_Flag = 1;
  return AT_OK;
}
/*------------------------AT+DCS=?\r\n-------------------------------------*/


/*------------------------AT+TXP=?\r\n-------------------------------------*/
ATEerror_t AT_TXP_get(const char *param) {
  AT_PRINTF("AT+TXP=%d", app_param.lora_info.txPower);
  return AT_OK;
}

ATEerror_t AT_TXP_set(const char *param) {
  uint8_t TxPower;
  if (tiny_sscanf(param, "%hu", &TxPower) != 1) {
    return AT_PARAM_ERROR;
  }
  app_param.lora_info.txPower = TxPower;
  TXP_set_Flag = 1;
  return AT_OK;
}
/*------------------------AT+TXP=?\r\n-------------------------------------*/


/*------------------------AT+RX2DR=?\r\n-------------------------------------*/
ATEerror_t AT_RX2DR_get(const char *param) {
  AT_PRINTF("AT+RX2DR=%d", app_param.lora_info.rx2dr);
  return AT_OK;
}

ATEerror_t AT_RX2DR_set(const char *param) {
  uint8_t RX2DR;
  if (tiny_sscanf(param, "%hu", &RX2DR) != 1) {
    return AT_PARAM_ERROR;
  }
  app_param.lora_info.rx2dr = RX2DR;
  RX2DR_set_Flag = 1;
  return AT_OK;
}
/*------------------------AT+RX2DR=?\r\n-------------------------------------*/
