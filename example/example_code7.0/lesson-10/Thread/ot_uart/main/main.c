/*————————————————————————————————————————Header file declaration————————————————————————————————————————*/
#include "main.h"
/*——————————————————————————————————————Header file declaration end——————————————————————————————————————*/

/*——————————————————————————————————————————Variable declaration—————————————————————————————————————————*/
TaskHandle_t Host_uart_task_handle;
TaskHandle_t CLI_uart_task_handle;
/*————————————————————————————————————————Variable declaration end———————————————————————————————————————*/

/*—————————————————————————————————————————Functional function———————————————————————————————————————————*/
void Init(void)
{
    HOST_Uart_Init();
    CLI_Uart_Init();
}

static void Host_uart_task(void *arg)
{
    uint8_t* host_data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while(1) 
    {
        int rxBytes = uart_read_bytes(HOST_UART_Num, host_data, RX_BUF_SIZE, 10);
        if (rxBytes > 0) {
            host_data[rxBytes] = 0;
            uart_write_bytes(CLI_UART_Num, (char *)host_data, rxBytes);
        }
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    free(host_data);
}

static void Cli_uart_task(void *arg)
{
    uint8_t* cli_data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while(1) 
    {
        int rxBytes = uart_read_bytes(CLI_UART_Num, cli_data, RX_BUF_SIZE, 10);
        if (rxBytes > 0) {
            cli_data[rxBytes] = 0;
            uart_write_bytes(HOST_UART_Num, (char *)cli_data, rxBytes);
        }
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    free(cli_data);
}

void app_main(void)
{
    MAIN_INFO("----------Demo version----------\r\n");
    Init();
    xTaskCreate(Host_uart_task, "host_uart_work", 2048, NULL, configMAX_PRIORITIES-3, &Host_uart_task_handle);
    xTaskCreate(Cli_uart_task, "cli_uart_work", 2048, NULL, configMAX_PRIORITIES-3, &CLI_uart_task_handle);
}
/*———————————————————————————————————————Functional function end—————————————————————————————————————————*/