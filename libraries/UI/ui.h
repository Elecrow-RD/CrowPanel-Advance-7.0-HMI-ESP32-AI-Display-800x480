// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.2.0
// LVGL VERSION: 8.3.3
// PROJECT: LVGL35

#ifndef _LVGL35_UI_H
#define _LVGL35_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"



void ui_event_BEGIN(lv_event_t * e);
extern lv_obj_t * ui_BEGIN;
extern lv_obj_t * ui_toppic;
extern lv_obj_t * ui_dowmpic;
extern lv_obj_t * ui_dowmblack;
extern lv_obj_t * ui_MENU;
void ui_event_meun(lv_event_t * e);
extern lv_obj_t * ui_menu;
void ui_event_touch(lv_event_t * e);
extern lv_obj_t * ui_touch;
extern lv_obj_t * ui_Bar;
extern lv_obj_t * ui_TOUCH;
void ui_event_jiaozhun(lv_event_t * e);
extern lv_obj_t * ui_jiaozhun;
void ui_event_ok(lv_event_t * e);
extern lv_obj_t * ui_ok;
extern lv_obj_t * ui_Label2;


extern lv_obj_t * ui_touch_calibrate;
//7.0
LV_IMG_DECLARE(ui_img_1405828474);    // assets\Wizee-ESP32-03-01.png
LV_IMG_DECLARE(ui_img_812486673);    // assets\wizee_logo_03_160x40.png
LV_IMG_DECLARE(CrowPanel_ESP32_Advance_HMI_70);    
LV_IMG_DECLARE(ui_img_626276270);    // assets\007-icon_click_1.png
LV_IMG_DECLARE(ui_img_626270897);    // assets\007-icon_click_2.png
LV_IMG_DECLARE(ui_img_1415402543);    // assets\007-icon_home_1.png
LV_IMG_DECLARE(ui_img_1797835574);    // assets\007-icon_home_2.png
LV_IMG_DECLARE(ui_img_bar_800_01_png);    // assets\bar_800_01.png
LV_IMG_DECLARE(ui_img_bar_800_02_png);    // assets\bar_800_02.png
LV_IMG_DECLARE(ui_img_1086482287);    // assets\007-icon_retuen_1.png
LV_IMG_DECLARE(ui_img_410158353);    // assets\007-icon_retuen_(2).png



void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
