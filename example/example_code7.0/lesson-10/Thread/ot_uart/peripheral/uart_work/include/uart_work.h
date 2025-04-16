#ifndef _UART_WORK_H_
#define _UART_WORK_H_

/*————————————————————————————————————————Header file declaration————————————————————————————————————————*/
#include "string.h"
#include "driver/uart.h"
#include "driver/gpio.h"
/*——————————————————————————————————————Header file declaration end——————————————————————————————————————*/

/*——————————————————————————————————————————Variable declaration—————————————————————————————————————————*/
#define UART_TAG "UART"
#define UART_INFO(fmt, ...)  ESP_LOGI(UART_TAG, fmt, ##__VA_ARGS__)
#define UART_DEBUG(fmt, ...) ESP_LOGD(UART_TAG, fmt, ##__VA_ARGS__)
#define UART_ERROR(fmt, ...) ESP_LOGE(UART_TAG, fmt, ##__VA_ARGS__)

#define UART_Baud               CONFIG_UART_Baud
#define RX_BUF_SIZE             CONFIG_RX_BUF_SIZE
#define TX_BUF_SIZE             CONFIG_TX_BUF_SIZE
#define HOST_UART_Num           CONFIG_UART_HOST_Num
#define CLI_UART_Num            CONFIG_UART_CLI_Num
#define HOST_TXD_PIN            CONFIG_HOST_TXD_PIN
#define HOST_RXD_PIN            CONFIG_HOST_RXD_PIN
#define CLI_TXD_PIN             CONFIG_CLI_TXD_PIN
#define CLI_RXD_PIN             CONFIG_CLI_RXD_PIN
/*———————————————————————————————————————Variable declaration end——————————————-—————————————————————————*/

/*——————————————————————————————————————————Function declaration—————————————————————————————————————————*/
void HOST_Uart_Init(void);
void CLI_Uart_Init(void);
/*———————————————————————————————————————Function declaration end——————————————-—————————————————————————*/

#endif