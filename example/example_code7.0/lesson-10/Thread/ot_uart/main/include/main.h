#ifndef _MAIN_H_
#define _MAIN_H_

/*————————————————————————————————————————Header file declaration————————————————————————————————————————*/
#include <stdio.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "uart_work.h"
/*——————————————————————————————————————Header file declaration end——————————————————————————————————————*/
/*——————————————————————————————————————————Variable declaration—————————————————————————————————————————*/
#define MAIN_TAG "MAIN"
#define MAIN_INFO(fmt, ...)   ESP_LOGI(MAIN_TAG, fmt, ##__VA_ARGS__)
#define MAIN_DEBUG(fmt, ...)  ESP_LOGD(MAIN_TAG, fmt, ##__VA_ARGS__)
#define MAIN_ERROR(fmt, ...)  ESP_LOGE(MAIN_TAG, fmt, ##__VA_ARGS__)

/*———————————————————————————————————————Variable declaration end——————————————-—————————————————————————*/
#endif