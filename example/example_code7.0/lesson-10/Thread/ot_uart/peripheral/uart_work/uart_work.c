/*————————————————————————————————————————Header file declaration————————————————————————————————————————*/
#include "uart_work.h"
/*——————————————————————————————————————Header file declaration end——————————————————————————————————————*/

/*——————————————————————————————————————————Variable declaration—————————————————————————————————————————*/
/*————————————————————————————————————————Variable declaration end———————————————————————————————————————*/

/*—————————————————————————————————————————Functional function———————————————————————————————————————————*/
void HOST_Uart_Init(void)
{
    const uart_config_t host_uart_config = {
        .baud_rate = UART_Baud,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_param_config(HOST_UART_Num, &host_uart_config);
    uart_set_pin(HOST_UART_Num, HOST_TXD_PIN, HOST_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(HOST_UART_Num, RX_BUF_SIZE * 2, TX_BUF_SIZE * 2, 0, NULL, 0);
}

void CLI_Uart_Init(void)
{
    const uart_config_t cli_uart_config = {
        .baud_rate = UART_Baud,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_param_config(CLI_UART_Num, &cli_uart_config);
    uart_set_pin(CLI_UART_Num, CLI_TXD_PIN, CLI_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(CLI_UART_Num, RX_BUF_SIZE * 2, TX_BUF_SIZE * 2, 0, NULL, 0);
}
/*———————————————————————————————————————Functional function end—————————————————————————————————————————*/