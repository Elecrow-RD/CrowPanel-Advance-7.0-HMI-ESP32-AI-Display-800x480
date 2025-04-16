/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * OpenThread Border Router Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include <stdio.h>
#include <string.h>

#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_openthread.h"
#include "esp_openthread_border_router.h"
#include "esp_openthread_cli.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_netif_glue.h"
#include "esp_openthread_types.h"
#include "esp_ot_cli_extension.h"
#include "esp_ot_config.h"
#include "esp_ot_wifi_cmd.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_eventfd.h"
#include "esp_wifi.h"
#include "mdns.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/uart_types.h"
#include "openthread/error.h"
#include "openthread/logging.h"
#include "openthread/tasklet.h"

#if CONFIG_OPENTHREAD_STATE_INDICATOR_ENABLE
#include "ot_led_strip.h"
#endif

#if CONFIG_OPENTHREAD_BR_AUTO_START
#include "example_common_private.h"
#include "protocol_examples_common.h"
#endif

#if !CONFIG_OPENTHREAD_BR_AUTO_START && CONFIG_EXAMPLE_CONNECT_ETHERNET
// TZ-1109: Add a menchanism for connecting ETH manually.
#error Currently we do not support a manual way to connect ETH, if you want to use ETH, please enable OPENTHREAD_BR_AUTO_START.
#endif

#define TAG "esp_ot_br"

#if CONFIG_EXTERNAL_COEX_ENABLE
static void ot_br_external_coexist_init(void)
{
    esp_external_coex_gpio_set_t gpio_pin = ESP_OPENTHREAD_DEFAULT_EXTERNAL_COEX_CONFIG();
    esp_external_coex_set_work_mode(EXTERNAL_COEX_LEADER_ROLE);
    ESP_ERROR_CHECK(esp_enable_extern_coex_gpio_pin(CONFIG_EXTERNAL_COEX_WIRE_TYPE, gpio_pin));
}
#endif /* CONFIG_EXTERNAL_COEX_ENABLE */

static void ot_task_worker(void *aContext)
{
    esp_openthread_platform_config_t config = {
        // .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),//The radio configuration（use UART connection to a 15.4 capable radio co-processor (RCP)）
        .radio_config = {
            .radio_mode = RADIO_MODE_UART_RCP,//use UART connection to a 15.4 capable radio co-processor (RCP)
            .radio_uart_config = {
                .port = 1,                                     
                .uart_config = {                                          
                    .baud_rate = 460800,                   
                    .data_bits = UART_DATA_8_BITS,         
                    .parity = UART_PARITY_DISABLE,         
                    .stop_bits = UART_STOP_BITS_1,         
                    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, 
                    .rx_flow_ctrl_thresh = 0,              
                    .source_clk = UART_SCLK_DEFAULT,       
                },                                         
            .rx_pin = GPIO_NUM_19,                                   
            .tx_pin = GPIO_NUM_20,      
            }//connet mode -- Uart Pin
        },
        // .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),//The host connection configuration
        .host_config = {
            .host_connection_mode = HOST_CONNECTION_MODE_CLI_UART,//CLI UART connection to the host(use CLI command)
            .host_uart_config = {                                   
                .port = 0,                                          
                .uart_config =                                      
                    {                                               
                        .baud_rate = 115200,                
                        .data_bits = UART_DATA_8_BITS,              
                        .parity = UART_PARITY_DISABLE,              
                        .stop_bits = UART_STOP_BITS_1,              
                        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,      
                        .rx_flow_ctrl_thresh = 0,                   
                        .source_clk = UART_SCLK_DEFAULT,            
                    },                                              
                .rx_pin = UART_PIN_NO_CHANGE,                       
                .tx_pin = UART_PIN_NO_CHANGE,                       
            },                                 
        },
        // .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),//The port configuration
        .port_config = {
            .storage_partition_name = "nvs",//The partition for storing OpenThread dataset  
            .netif_queue_size = 10,         //The packet queue size for the network interface         
            .task_queue_size = 10,          //The task queue size
        }
    };//The OpenThread platform configuration
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_OPENTHREAD();//netif config(use default)
    esp_netif_t *openthread_netif = esp_netif_new(&cfg);//Creates an instance of new esp-netif object based on provided config
    assert(openthread_netif != NULL);
    ESP_ERROR_CHECK(esp_openthread_init(&config));// Initialize the OpenThread stack
    ESP_ERROR_CHECK(esp_netif_attach(openthread_netif, esp_openthread_netif_glue_init(&config)));//Attach the processing program to the network interface(esp_openthread_netif_glue_init:Create the OpenThread network interface handler)
    esp_openthread_lock_acquire(portMAX_DELAY);//Openthread api lock
    (void)otLoggingSetLevel(CONFIG_LOG_DEFAULT_LEVEL);//Sets the log level
    esp_openthread_cli_init();//This function initializes the OpenThread command line interface(CLI)
    esp_cli_custom_command_init();//Init the custom command
    esp_openthread_cli_create_task();//This function launches an exclusive loop for the OpenThread CLI
    esp_openthread_lock_release();//Openthread api lock release 

    // Run the main loop
    esp_openthread_launch_mainloop();//Launches the OpenThread main loop.

    // Clean up ---mainloop error
    esp_openthread_netif_glue_deinit();
    esp_netif_destroy(openthread_netif);
    esp_vfs_eventfd_unregister();
    vTaskDelete(NULL);
}

void ot_br_init(void *ctx)
{
#if CONFIG_OPENTHREAD_CLI_WIFI
    ESP_ERROR_CHECK(esp_ot_wifi_config_init());//Initalize the nvs for wifi configurations(key:wifi_config)
#endif
#if CONFIG_OPENTHREAD_BR_AUTO_START//Enable the automatic start mode in Thread Border Router
#if CONFIG_EXAMPLE_CONNECT_WIFI || CONFIG_EXAMPLE_CONNECT_ETHERNET
    bool wifi_or_ethernet_connected = false;
#else
#error No backbone netif!
#endif
#if CONFIG_EXAMPLE_CONNECT_WIFI//wifi connect example,use the nvs or kconfig's ssid and password
    char wifi_ssid[32] = "";
    char wifi_password[64] = "";
    if(esp_ot_wifi_config_get_ssid(wifi_ssid) == ESP_OK) 
    {
        ESP_LOGI(TAG, "use the Wi-Fi config from NVS");
        esp_ot_wifi_config_get_password(wifi_password);
    } 
    else
    {
        ESP_LOGI(TAG, "use the Wi-Fi config from Kconfig");
        strcpy(wifi_ssid, CONFIG_EXAMPLE_WIFI_SSID);
        strcpy(wifi_password, CONFIG_EXAMPLE_WIFI_PASSWORD);
    }
    if (esp_ot_wifi_connect(wifi_ssid, wifi_password) == ESP_OK) {
        wifi_or_ethernet_connected = true;
    } else {
        ESP_LOGE(TAG, "Fail to connect to Wi-Fi, please try again manually");
    }
#endif
#if CONFIG_EXAMPLE_CONNECT_ETHERNET
    ESP_ERROR_CHECK(example_ethernet_connect());//User ethernet connect
    wifi_or_ethernet_connected = true;
#endif
#endif // CONFIG_OPENTHREAD_BR_AUTO_START
#if CONFIG_EXTERNAL_COEX_ENABLE
    ot_br_external_coexist_init();
#endif // CONFIG_EXTERNAL_COEX_ENABLE
    ESP_ERROR_CHECK(mdns_init());//Init mdns
    ESP_ERROR_CHECK(mdns_hostname_set("esp-ot-br"));//Device responds to hostname（hostname：esp-ot-br）
    
    esp_openthread_lock_acquire(portMAX_DELAY);//Openthread api lock
#if CONFIG_OPENTHREAD_STATE_INDICATOR_ENABLE
    ESP_ERROR_CHECK(esp_openthread_state_indicator_init(esp_openthread_get_instance()));
#endif
#if CONFIG_OPENTHREAD_BR_AUTO_START
    if (wifi_or_ethernet_connected) {
        esp_openthread_set_backbone_netif(get_example_netif());//Sets the backbone interface used for border routing.(wifi or ethernet)
        ESP_ERROR_CHECK(esp_openthread_border_router_init());//Initializes the border router features of OpenThread.
#if CONFIG_EXAMPLE_CONNECT_WIFI
        esp_ot_wifi_border_router_init_flag_set(true);//Init successful
#endif
        otOperationalDatasetTlvs dataset;
        otError error = otDatasetGetActiveTlvs(esp_openthread_get_instance(), &dataset);
        ESP_ERROR_CHECK(esp_openthread_auto_start((error == OT_ERROR_NONE) ? &dataset : NULL));//Starts the Thread protocol operation and attaches to a Thread network.
    } else {
        ESP_LOGE(TAG, "Auto-start mode failed, please try to start manually");
    }
#endif // CONFIG_OPENTHREAD_BR_AUTO_START
    esp_openthread_lock_release();//Openthread api lock release
    vTaskDelete(NULL);//Init success,delete task
}

void app_main(void)
{
    // Used eventfds:
    // * netif
    // * task queue
    // * border router
    esp_vfs_eventfd_config_t eventfd_config = {
#if CONFIG_OPENTHREAD_RADIO_NATIVE || CONFIG_OPENTHREAD_RADIO_SPINEL_SPI
        // * radio driver (A native radio device needs a eventfd for radio driver.)
        // * SpiSpinelInterface (The Spi Spinel Interface needs a eventfd.)
        // The above will not exist at the same time.
        .max_fds = 4,
#else
        .max_fds = 3,
#endif
    };
    ESP_ERROR_CHECK(esp_vfs_eventfd_register(&eventfd_config));//Registers the event vfs(Used for implementing task notifications in the OpenThread protocol stack)
    ESP_ERROR_CHECK(nvs_flash_init());//nvs partition init
    ESP_ERROR_CHECK(esp_netif_init());//init tcp/ip stack
    ESP_ERROR_CHECK(esp_event_loop_create_default());//create default loop event
    xTaskCreate(ot_task_worker, "ot_br_main", 8192, xTaskGetCurrentTaskHandle(), 5, NULL);
    xTaskCreate(ot_br_init, "ot_br_init", 6144, NULL, 4, NULL);
}
