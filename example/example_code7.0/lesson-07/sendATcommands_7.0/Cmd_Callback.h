#ifndef _CMD_CALLBACK_H
#define _CMD_CALLBACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Cmd_List.h"
#include "Param_Var.h"
#include <stdio.h>

extern uint8_t OTAA_Join_Flag;
extern uint8_t ABP_Join_Flag;
extern uint8_t ADR_set_Flag;
extern uint8_t DR_set_Flag;
extern uint8_t DCS_set_Flag;
extern uint8_t TXP_set_Flag;
extern uint8_t RX2DR_set_Flag;
extern uint8_t Band_set_Flag;

extern uint8_t Send_Flag;

extern uint8_t Port_Number;
extern uint8_t Ack_State;
extern uint8_t MsgTemp[128];
extern size_t  MsgTemp_size;
#define AT_PRINTF(...)     do{\
    printf (  __VA_ARGS__ );\
  }while(0)
#define AT_PPRINTF(...)     do{\
    printf (  __VA_ARGS__ );\
  }while(0)


/**
    @brief  Return AT_OK in all cases

    @param  param string of the AT command - unused

    @retval AT_OK
*/
ATEerror_t AT_return_ok(const char *param);

/**
    @brief  Return AT_ERROR in all cases

    @param  param string of the AT command - unused

    @retval AT_ERROR
*/
ATEerror_t AT_return_error(const char *param);

/**
    @brief  Get default DevEui

    @param  param String parameter

    @retval AT_OK
*/
ATEerror_t AT_DefEUI_get(const char *param);

/* --------------- General commands --------------- */


/**
    @brief  Print the version of the AT_Slave FW

    @param  param String parameter

    @retval AT_OK
*/
ATEerror_t AT_version_run(const char *param);

/**
    @brief  Get device current paramenter

    @param  param String parameter

    @retval AT_OK
*/
ATEerror_t AT_Config_get(const char *param);

/**
    @brief  Get device default paramenter

    @param  param String parameter

    @retval AT_OK
*/
ATEerror_t AT_DefParam_get(const char *param);

/**
    @brief  Get hardware version

    @param  param String parameter

    @retval AT_OK
*/
ATEerror_t AT_HARDWAREVER_run(const char *param);

/**
    @brief  Restore the current parameters to default values

    @param  param String parameter

    @retval AT_OK
*/
ATEerror_t AT_Parameter_Reset(const char *param);


/* --------------- LoRaWAN network management commands --------------- */

/**
    @brief  Print actual Active Type

    @param  param string of the AT command

    @retval AT_OK
*/
ATEerror_t AT_ActivationType_get(const char *param);

/**
    @brief  Set Active Type

    @param  param string of the AT command

    @retval AT_OK if OK
*/
ATEerror_t AT_ActivationType_set(const char *param);

/**
    @brief  Print actual Active Region

    @param  param string of the AT command

    @retval AT_OK
*/
ATEerror_t AT_Region_get(const char *param);

/**
    @brief  Set Active Region

    @param  param string of the AT command

    @retval AT_OK if OK
*/
ATEerror_t AT_Region_set(const char *param);

/**
    @brief  Print channel group

    @param  param string of the AT command

    @retval AT_OK
*/
ATEerror_t AT_ChannelGroup_get(const char *param);

/**
    @brief  Set channel group

    @param  param string of the AT command

    @retval AT_OK if OK
*/
ATEerror_t AT_ChannelGroup_set(const char *param);

/**
    @brief  Print Join Eui

    @param  param string of the AT command

    @retval AT_OK if OK, or an appropriate AT_xxx error code
*/
ATEerror_t AT_JoinEUI_get(const char *param);

/**
    @brief  Set Join Eui

    @param  param string of the AT command

    @retval AT_OK if OK, or an appropriate AT_xxx error code
*/
ATEerror_t AT_JoinEUI_set(const char *param);

/**
    @brief  Print Application Key

    @param  param string of the AT command

    @retval AT_OK
*/
ATEerror_t AT_AppKey_get(const char *param);

/**
    @brief  Set Application Key

    @param  param string of the AT command

    @retval AT_OK if OK, or an appropriate AT_xxx error code
*/
ATEerror_t AT_AppKey_set(const char *param);

/**
    @brief  Print Device EUI

    @param  param string of the AT command

    @retval AT_OK
*/
ATEerror_t AT_DevEUI_get(const char *param);

/**
    @brief  Set Device EUI

    @param  param string of the AT command

    @retval AT_OK if OK
*/
ATEerror_t AT_DevEUI_set(const char *param);

/**
    @brief  Print Network Key

    @param  param string of the AT command

    @retval AT_OK
*/
//ATEerror_t AT_NwkKey_get(const char *param);

/**
    @brief  Set Network Key

    @param  param string of the AT command

    @retval AT_OK if OK, or an appropriate AT_xxx error code
*/
//ATEerror_t AT_NwkKey_set(const char *param);

/**
    @brief  Print Network Session Key

    @param  param string of the AT command

    @retval AT_OK
*/
ATEerror_t AT_NwkSKey_get(const char *param);

/**
    @brief  Set Network Session Key

    @param  param String pointing to provided DevAddr

    @retval AT_OK if OK, or an appropriate AT_xxx error code
*/
ATEerror_t AT_NwkSKey_set(const char *param);

/**
    @brief  Print Application Session Key

    @param  param string of the AT command

    @retval AT_OK
*/
ATEerror_t AT_AppSKey_get(const char *param);

/**
    @brief  Set Application Session Key

    @param  param String pointing to provided DevAddr

    @retval AT_OK if OK, or an appropriate AT_xxx error code
*/
ATEerror_t AT_AppSKey_set(const char *param);

/**
    @brief  Print Serving Network Session Integrity Key

    @param  param string of the AT command

    @retval AT_OK
*/
//ATEerror_t AT_SnwksKey_get(const char *param);
/**
    @brief  Set Serving Network Session Integrity Key

    @param  param String pointing to provided DevAddr

    @retval AT_OK if OK, or an appropriate AT_xxx error code
*/
//ATEerror_t AT_SnwksKey_set(const char *param);

/**
    @brief  Print Forwarding Network Session Integrity Key

    @param  param string of the AT command

    @retval AT_OK
*/
//ATEerror_t AT_FnwksKey_get(const char *param);

/**
    @brief  Set Forwarding Network Session Integrity Key

    @param  param String pointing to provided DevAddr

    @retval AT_OK if OK, or an appropriate AT_xxx error code
*/
//ATEerror_t AT_FnwksKey_set(const char *param);

/**
    @brief  Set lora device key

    @param  param String parameter

    @retval AT_OK
*/
ATEerror_t AT_DeviceKey_set(const char *param);

/**
    @brief  Get lora device key

    @param  param String parameter

    @retval AT_OK
*/
ATEerror_t AT_DeviceKey_get(const char *param);

/**
    @brief  Print the DevAddr

    @param  param String pointing to provided DevAddr

    @retval AT_OK
*/
ATEerror_t AT_DevAddr_get(const char *param);

/**
    @brief  Set DevAddr

    @param  param String pointing to provided DevAddr

    @retval AT_OK if OK, or an appropriate AT_xxx error code
*/
ATEerror_t AT_DevAddr_set(const char *param);

/**
    @brief  Set device code

    @param  param String parameter

    @retval AT_OK
*/
ATEerror_t AT_DevCODE_set(const char *param);
/**
    @brief  Get device code

    @param  param String parameter

    @retval AT_OK
*/
ATEerror_t AT_DevCODE_get(const char *param);

/**
    @brief  Lorawan In The Net

    @param  param String parameter

    @retval AT_OK
*/
ATEerror_t AT_Join(const char *param);

/**
    @brief  Send Lorawan Data

    @param  param String parameter

    @retval AT_OK
*/
ATEerror_t AT_Send_set(const char *param);

/**
    @brief  Print ADR Value

    @param  param string of the AT command

    @retval AT_OK
*/
ATEerror_t AT_ADR_get(const char *param);


/**
    @brief  Set ADR Value

    @param  param string of the AT command

    @retval AT_OK if OK
*/
ATEerror_t AT_ADR_set(const char *param);

/**
    @brief  Print DR Value

    @param  param string of the AT command

    @retval AT_OK
*/
ATEerror_t AT_DR_get(const char *param);


/**
    @brief  Set DR Value

    @param  param string of the AT command

    @retval AT_OK if OK
*/
ATEerror_t AT_DR_set(const char *param);


/**
    @brief  Print dutyCycle Value

    @param  param string of the AT command

    @retval AT_OK
*/
ATEerror_t AT_DCS_get(const char *param);


/**
    @brief  Set dutyCycle Value

    @param  param string of the AT command

    @retval AT_OK if OK
*/
ATEerror_t AT_DCS_set(const char *param);

/**
    @brief  Print Transmit Power

    @param  param string of the AT command

    @retval AT_OK
*/
ATEerror_t AT_TXP_get(const char *param);


/**
    @brief  Set Transmit Power

    @param  param string of the AT command

    @retval AT_OK if OK
*/
ATEerror_t AT_TXP_set(const char *param);

/**
    @brief  Print data rate of the Rx window 2

    @param  param string of the AT command

    @retval AT_OK
*/
ATEerror_t AT_RX2DR_get(const char *param);


/**
    @brief  Set data rate of the Rx window 2.

    @param  param string of the AT command

    @retval AT_OK if OK
*/
ATEerror_t AT_RX2DR_set(const char *param);

#ifdef __cplusplus
}
#endif
#endif
