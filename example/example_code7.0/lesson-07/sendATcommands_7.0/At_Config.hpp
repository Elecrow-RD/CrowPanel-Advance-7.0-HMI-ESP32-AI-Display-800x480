#ifndef _AT_CONFIG_H_
#define _AT_CONFIG_H_

#pragma once

#include <Arduino.h>
#include "Cmd_String.h"
#include "Param_Var.h"
#include "Cmd_List.h"
#include "Cmd_Callback.h"

class AT_Config
{
    private:
        static AT_Config* instance_;

    public:
        AT_Config(void);  

        static AT_Config& getInstance();

        void begin( void );

        /*!
        *  @brief  Parse AT cmd
        *  @param cmd   
        *  @param length 
        */  
        void parseCmd(const char *cmd, uint16_t length);

        /*!
        *  @brief  Receive serial cmd
        *  @param buf   
        *  @param size 
        *  @return bool
        */  
        bool receiveSerialCmd(uint8_t *buf, uint8_t *size);

    protected:

};

extern AT_Config at_config;

#endif
