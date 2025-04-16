/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * OpenThread Radio Co-Processor (RCP) Example
 *
 * This example code is in the Public Domain (or CC0-1.0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include <stdio.h>
#include <unistd.h>

#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_openthread.h"
#include "esp_ot_config.h"
#include "esp_vfs_eventfd.h"
#include "driver/uart.h"

#if CONFIG_EXTERNAL_COEX_ENABLE
#include "esp_coexist.h"
#endif

#if !SOC_IEEE802154_SUPPORTED
#error "RCP is only supported for the SoCs which have IEEE 802.15.4 module"
#endif

#define TAG "ot_esp_rcp"

extern void otAppNcpInit(otInstance *instance);

#if CONFIG_EXTERNAL_COEX_ENABLE
#if SOC_EXTERNAL_COEX_ADVANCE
static void ot_external_coexist_init(void)
{
    esp_external_coex_gpio_set_t gpio_pin = ESP_OPENTHREAD_DEFAULT_EXTERNAL_COEX_CONFIG();
    esp_external_coex_set_work_mode(EXTERNAL_COEX_FOLLOWER_ROLE);
    ESP_ERROR_CHECK(esp_enable_extern_coex_gpio_pin(CONFIG_EXTERNAL_COEX_WIRE_TYPE, gpio_pin));
}
#endif // SOC_EXTERNAL_COEX_ADVANCE
#endif // CONFIG_EXTERNAL_COEX_ENABLE

static void ot_task_worker(void *aContext)
{
    esp_openthread_platform_config_t config = {
        // .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .radio_config = {
            .radio_mode = RADIO_MODE_NATIVE//Use the native 15.4 radio
        },
        // .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .host_config = {
            .host_connection_mode = HOST_CONNECTION_MODE_RCP_UART,//RCP UART connection to the host
            .host_uart_config = {
                .port = 0,                                          
                .uart_config = {                                               
                        .baud_rate =  460800,                       
                        .data_bits = UART_DATA_8_BITS,              
                        .parity = UART_PARITY_DISABLE,              
                        .stop_bits = UART_STOP_BITS_1,              
                        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,      
                        .rx_flow_ctrl_thresh = 0,                   
                        .source_clk = UART_SCLK_DEFAULT,            
                    },                                              
                .rx_pin = OPENTHREAD_RCP_UART_RX_PIN,               
                .tx_pin = OPENTHREAD_RCP_UART_TX_PIN,               
            }
        },
        // .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
        .port_config = {
            .storage_partition_name = "nvs",//The partition for storing OpenThread dataset          
            .netif_queue_size = 10,                 
            .task_queue_size = 10,  
        }
    };
    ESP_ERROR_CHECK(esp_openthread_init(&config));//Initialize the OpenThread stack

#if CONFIG_EXTERNAL_COEX_ENABLE
    ot_external_coexist_init();
#endif // CONFIG_EXTERNAL_COEX_ENABLE
    otAppNcpInit(esp_openthread_get_instance());//Initialize the OpenThread ncp
    esp_openthread_launch_mainloop();//Run the main loop

    // Clean up
    esp_vfs_eventfd_unregister();
    vTaskDelete(NULL);
}

void app_main(void)
{
    // Used eventfds:
    // * ot task queue
    // * radio driver
    esp_vfs_eventfd_config_t eventfd_config = {
        .max_fds = 2,
    };

    ESP_ERROR_CHECK(nvs_flash_init());//nvs partition init
    ESP_ERROR_CHECK(esp_event_loop_create_default());//create default loop event
    ESP_ERROR_CHECK(esp_vfs_eventfd_register(&eventfd_config));//Registers the event vfs(Used for implementing task notifications in the OpenThread protocol stack)
    xTaskCreate(ot_task_worker, "ot_rcp_main", 3072, xTaskGetCurrentTaskHandle(), 5, NULL);
}
