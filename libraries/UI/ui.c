// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.2.0
// LVGL VERSION: 8.3.3
// PROJECT: LVGL35

#include "ui.h"
#include "ui_helpers.h"
int goto_widget_flag=0;
int zero_clean=0;
int bar_flag=0;
///////////////////// VARIABLES ////////////////////
void ui_event_BEGIN(lv_event_t * e);
void ui_event_MEUN(lv_event_t * e);
lv_obj_t * ui_BEGIN;
lv_obj_t * ui_toppic;
lv_obj_t * ui_dowmpic;
lv_obj_t * ui_dowmblack;
lv_obj_t * ui_MENU;
void ui_event_meun(lv_event_t * e);
lv_obj_t * ui_menu;
void ui_event_touch(lv_event_t * e);
lv_obj_t * ui_touch;
lv_obj_t * ui_Bar;
lv_obj_t * ui_TOUCH;
void ui_event_jiaozhun(lv_event_t * e);
lv_obj_t * ui_jiaozhun;
void ui_event_ok(lv_event_t * e);
lv_obj_t * ui_ok;
lv_obj_t * ui_Label2;
// lv_obj_t * ui_JIAOZHUN;
// void ui_event_jiaozhun4(lv_event_t * e);
// lv_obj_t * ui_jiaozhun4;

lv_obj_t * ui_touch_calibrate;//校准界面
///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////
static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}
static void anim_y_cb(void * var, int32_t v)
{
    lv_obj_set_y(var, v);
}
///////////////////// FUNCTIONS ////////////////////
void ui_event_BEGIN(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_SCREEN_LOADED) {
        _ui_screen_change(ui_MENU, LV_SCR_LOAD_ANIM_FADE_ON, 150, 3000);      
    }   
}


void ui_event_MEUN(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_SCREEN_LOADED) {
        bar_flag=6;     
    }   
}


void ui_event_meun(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) {
        _ui_image_set_property(ui_menu, _UI_IMAGE_PROPERTY_IMAGE, & ui_img_1797835574);
    }
    if(event_code == LV_EVENT_RELEASED) {
        _ui_image_set_property(ui_menu, _UI_IMAGE_PROPERTY_IMAGE, & ui_img_1415402543);
        goto_widget_flag=1;
    }
    
}

void ui_event_touch(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) {
        _ui_image_set_property(ui_touch, _UI_IMAGE_PROPERTY_IMAGE, & ui_img_626270897);
    }
    if(event_code == LV_EVENT_RELEASED) {
        _ui_image_set_property(ui_touch, _UI_IMAGE_PROPERTY_IMAGE, & ui_img_626276270);
    }
    if(event_code == LV_EVENT_CLICKED) {        
        _ui_screen_change(ui_TOUCH, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0);     
        goto_widget_flag=3;  
        zero_clean=1;
    }
}

void ui_event_jiaozhun(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) {
        _ui_image_set_property(ui_jiaozhun, _UI_IMAGE_PROPERTY_IMAGE, & ui_img_626270897);
    }
    if(event_code == LV_EVENT_RELEASED) {
        _ui_image_set_property(ui_jiaozhun, _UI_IMAGE_PROPERTY_IMAGE, & ui_img_626276270); 
        goto_widget_flag=5;         
    }
    // if(event_code == LV_EVENT_CLICKED) {        
        // _ui_screen_change(ui_touch_calibrate, LV_SCR_LOAD_ANIM_NONE, 0, 0);  
        // goto_widget_flag=5;              
    // }
}

void ui_event_ok(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) {
        _ui_image_set_property(ui_ok, _UI_IMAGE_PROPERTY_IMAGE, & ui_img_410158353);
    }
    if(event_code == LV_EVENT_RELEASED) {
        _ui_image_set_property(ui_ok, _UI_IMAGE_PROPERTY_IMAGE, & ui_img_1086482287);
    }
    if(event_code == LV_EVENT_CLICKED) {       
        _ui_screen_change(ui_MENU, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0);       
        goto_widget_flag=4;
    }
}

// void ui_event_jiaozhun4(lv_event_t * e)
// {
//     lv_event_code_t event_code = lv_event_get_code(e);
//     lv_obj_t * target = lv_event_get_target(e);
//     if(event_code == LV_EVENT_PRESSED) {
//         _ui_image_set_property(ui_jiaozhun4, _UI_IMAGE_PROPERTY_IMAGE, & ui_img_on_png);
//         goto_widget_flag=2;
//     }
//     if(event_code == LV_EVENT_RELEASED) {
//         _ui_image_set_property(ui_jiaozhun4, _UI_IMAGE_PROPERTY_IMAGE, & ui_img_off_png);       
//     }
//     if(event_code == LV_EVENT_CLICKED) {
//         _ui_flag_modify(ui_jiaozhun4, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
//         goto_widget_flag=2;
//     }
// }

///////////////////// SCREENS ////////////////////
void ui_BEGIN_screen_init(void)
{
    ui_BEGIN = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_BEGIN, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_BEGIN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BEGIN, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //TOP照片
    ui_toppic = lv_img_create(ui_BEGIN);
    lv_img_set_src(ui_toppic, &ui_img_812486673);
    lv_obj_set_width(ui_toppic, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_toppic, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_size(ui_toppic, 240, 90);
    lv_obj_set_pos(ui_toppic, 280, 0);
    lv_obj_add_flag(ui_toppic, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_toppic, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ui_toppic);
    lv_anim_set_values(&a, 0, 190);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_exec_cb(&a, anim_y_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);
    //DOWM照片
    ui_dowmpic = lv_img_create(ui_BEGIN);
    lv_img_set_src(ui_dowmpic, &ui_img_1405828474);
    lv_obj_set_width(ui_dowmpic, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_dowmpic, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_size(ui_dowmpic, 241, 34);
    lv_obj_set_pos(ui_dowmpic, 280, 295);
    lv_obj_add_flag(ui_dowmpic, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_dowmpic, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    //DOWMBLACK照片
    // ui_dowmblack = lv_img_create(ui_BEGIN);
    // lv_img_set_src(ui_dowmblack, &ui_img_1952369156);
    // lv_obj_set_width(ui_dowmblack, LV_SIZE_CONTENT);   /// 1
    // lv_obj_set_height(ui_dowmblack, LV_SIZE_CONTENT);    /// 1
    // lv_obj_set_size(ui_dowmblack, 140, 17);
    // lv_obj_set_pos(ui_dowmblack, 170, 150);
    // lv_obj_add_flag(ui_dowmblack, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    // lv_obj_clear_flag(ui_dowmblack, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    // lv_anim_t b;
    // lv_anim_init(&b);
    // lv_anim_set_var(&b, ui_dowmblack);
    // lv_anim_set_values(&b, 170, 310);
    // lv_anim_set_time(&b, 2000);
    // lv_anim_set_exec_cb(&b, anim_x_cb);
    // lv_anim_set_path_cb(&b, lv_anim_path_linear);
    // lv_anim_start(&b);

    lv_obj_add_event_cb(ui_BEGIN, ui_event_BEGIN, LV_EVENT_ALL, NULL);

}
void ui_MENU_screen_init(void)
{
    ui_MENU = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_MENU, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_MENU, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MENU, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_MENU, &CrowPanel_ESP32_Advance_HMI_70, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_menu = lv_img_create(ui_MENU);
    lv_img_set_src(ui_menu, &ui_img_1415402543);
    lv_obj_set_width(ui_menu, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_menu, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_menu, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_add_flag(ui_menu, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_menu, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_zoom(ui_menu, 300);

    ui_touch = lv_img_create(ui_MENU);
    lv_img_set_src(ui_touch, &ui_img_626276270);
    lv_obj_set_width(ui_touch, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_touch, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_touch, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_add_flag(ui_touch, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_touch, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_zoom(ui_touch, 300);

    //进度条
    // ui_Bar = lv_bar_create(ui_MENU);   
    // lv_obj_add_event_cb(ui_Bar,bar_event_cb,LV_EVENT_VALUE_CHANGED,NULL);//添加回调事件
    // // lv_bar_set_value(ui_Bar, 100, LV_ANIM_ON);
    // lv_obj_set_width(ui_Bar, 150);
    // lv_obj_set_height(ui_Bar, 10);
    // lv_obj_set_x(ui_Bar, 0);
    // lv_obj_set_y(ui_Bar, 70);
    // lv_obj_set_align(ui_Bar, LV_ALIGN_CENTER);
    // lv_obj_set_style_radius(ui_Bar, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_grad_color(ui_Bar, lv_color_hex(0x0BE1E8), LV_PART_MAIN | LV_STATE_DEFAULT);

    // lv_obj_set_style_radius(ui_Bar, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(ui_Bar, lv_color_hex(0x1500F4), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_opa(ui_Bar, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    // lv_obj_set_style_outline_color(ui_Bar, lv_color_hex(0x2D8812), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    // lv_obj_set_style_outline_opa(ui_Bar, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

      
    // lv_anim_t c;                                          //创建动画对象
    // lv_anim_init(&c);                                     //初始化动画
    // lv_anim_set_var(&c,ui_Bar);                              //设置动画对象
    // lv_anim_set_values(&c,0,100);                         //设置动画值
    // lv_anim_set_exec_cb(&c,bar_set_value);                //设置回调
    // lv_anim_set_time(&c,3000);                            //设置时间
    // lv_anim_start(&c);

    lv_obj_add_event_cb(ui_MENU, ui_event_MEUN, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_menu, ui_event_meun, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_touch, ui_event_touch, LV_EVENT_ALL, NULL);

}
void ui_TOUCH_screen_init(void)
{
    ui_TOUCH = lv_obj_create(NULL);
    // lv_obj_clear_flag(ui_TOUCH, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_TOUCH, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TOUCH, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_jiaozhun = lv_img_create(ui_TOUCH);
    lv_img_set_src(ui_jiaozhun, &ui_img_626276270);
    lv_obj_set_width(ui_jiaozhun, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_jiaozhun, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_jiaozhun, 140);
    lv_obj_set_y(ui_jiaozhun, 10);
    lv_obj_add_flag(ui_jiaozhun, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_jiaozhun, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_zoom(ui_jiaozhun, 300);

    ui_ok = lv_img_create(ui_TOUCH);
    lv_img_set_src(ui_ok, &ui_img_1086482287);
    lv_obj_set_width(ui_ok, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_ok, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_ok, -140);
    lv_obj_set_y(ui_ok, 10);
    lv_obj_set_align(ui_ok, LV_ALIGN_TOP_RIGHT);
    lv_obj_add_flag(ui_ok, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_ok, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_zoom(ui_ok, 300);

    lv_obj_add_event_cb(ui_jiaozhun, ui_event_jiaozhun, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_ok, ui_event_ok, LV_EVENT_ALL, NULL);

}
void ui_ui_touch_calibrate_screen_init(void)
{
  ui_touch_calibrate = lv_obj_create(NULL);
//   lv_obj_clear_flag(ui_touch_calibrate, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
  lv_obj_set_style_bg_color(ui_touch_calibrate, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(ui_touch_calibrate, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

  ui_Label2 = lv_label_create(ui_touch_calibrate);//创建ui_Label2
  lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT);    /// 1
  lv_obj_set_x(ui_Label2, 0);
  lv_obj_set_y(ui_Label2, 10);
  lv_obj_set_align(ui_Label2, LV_ALIGN_CENTER);
  lv_label_set_text(ui_Label2, "Touch Adjust");
  lv_obj_set_style_text_color(ui_Label2, lv_color_hex(0x09BEFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(ui_Label2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui_Label2, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

//   lv_obj_add_event_cb(ui_touch_calibrate, ui_event_touch_calibrate, LV_EVENT_ALL, NULL);
}

// void ui_JIAOZHUN_screen_init(void)
// {
//     ui_JIAOZHUN = lv_obj_create(NULL);
//     lv_obj_clear_flag(ui_JIAOZHUN, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
//     lv_obj_set_style_bg_color(ui_JIAOZHUN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_bg_opa(ui_JIAOZHUN, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

//     // ui_Label2 = lv_label_create(ui_JIAOZHUN);
//     // lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);   /// 1
//     // lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT);    /// 1
//     // lv_obj_set_x(ui_Label2, 0);
//     // lv_obj_set_y(ui_Label2, -70);
//     // lv_obj_set_align(ui_Label2, LV_ALIGN_CENTER);
//     // lv_label_set_text(ui_Label2, "Touch Adjust");
//     // lv_obj_set_style_text_color(ui_Label2, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
//     // lv_obj_set_style_text_opa(ui_Label2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
//     // lv_obj_set_style_text_font(ui_Label2, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

//     // ui_jiaozhun1 = lv_img_create(ui_JIAOZHUN);
//     // lv_img_set_src(ui_jiaozhun1, &ui_img_off_png);
//     // lv_obj_set_width(ui_jiaozhun1, LV_SIZE_CONTENT);   /// 1
//     // lv_obj_set_height(ui_jiaozhun1, LV_SIZE_CONTENT);    /// 1
//     // lv_obj_add_flag(ui_jiaozhun1, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
//     // lv_obj_clear_flag(ui_jiaozhun1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
//     // lv_img_set_zoom(ui_jiaozhun1, 300);

//     // ui_jiaozhun2 = lv_img_create(ui_JIAOZHUN);
//     // lv_img_set_src(ui_jiaozhun2, &ui_img_off_png);
//     // lv_obj_set_width(ui_jiaozhun2, LV_SIZE_CONTENT);   /// 1
//     // lv_obj_set_height(ui_jiaozhun2, LV_SIZE_CONTENT);    /// 1
//     // lv_obj_set_align(ui_jiaozhun2, LV_ALIGN_BOTTOM_LEFT);
//     // lv_obj_add_flag(ui_jiaozhun2, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
//     // lv_obj_clear_flag(ui_jiaozhun2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
//     // lv_img_set_zoom(ui_jiaozhun2, 300);

//     // ui_jiaozhun3 = lv_img_create(ui_JIAOZHUN);
//     // lv_img_set_src(ui_jiaozhun3, &ui_img_off_png);
//     // lv_obj_set_width(ui_jiaozhun3, LV_SIZE_CONTENT);   /// 1
//     // lv_obj_set_height(ui_jiaozhun3, LV_SIZE_CONTENT);    /// 1
//     // lv_obj_set_align(ui_jiaozhun3, LV_ALIGN_TOP_RIGHT);
//     // lv_obj_add_flag(ui_jiaozhun3, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
//     // lv_obj_clear_flag(ui_jiaozhun3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
//     // lv_img_set_zoom(ui_jiaozhun3, 300);

//     // ui_jiaozhun4 = lv_img_create(ui_JIAOZHUN);
//     // lv_img_set_src(ui_jiaozhun4, &ui_img_off_png);
//     // lv_obj_set_width(ui_jiaozhun4, LV_SIZE_CONTENT);   /// 1
//     // lv_obj_set_height(ui_jiaozhun4, LV_SIZE_CONTENT);    /// 1
//     // lv_obj_set_align(ui_jiaozhun4, LV_ALIGN_BOTTOM_RIGHT);
//     // lv_obj_add_flag(ui_jiaozhun4, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
//     // lv_obj_clear_flag(ui_jiaozhun4, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
//     // lv_img_set_zoom(ui_jiaozhun4, 300);

//     // lv_obj_add_event_cb(ui_jiaozhun1, ui_event_jiaozhun1, LV_EVENT_ALL, NULL);
//     // lv_obj_add_event_cb(ui_jiaozhun2, ui_event_jiaozhun2, LV_EVENT_ALL, NULL);
//     // lv_obj_add_event_cb(ui_jiaozhun3, ui_event_jiaozhun3, LV_EVENT_ALL, NULL);
//     // lv_obj_add_event_cb(ui_jiaozhun4, ui_event_jiaozhun4, LV_EVENT_ALL, NULL);
//     lv_obj_add_event_cb(ui_JIAOZHUN, ui_event_JIAOZHUN, LV_EVENT_ALL, NULL);

// }

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_BEGIN_screen_init();
    ui_MENU_screen_init();
    ui_ui_touch_calibrate_screen_init();
    ui_TOUCH_screen_init();
    
    // ui_JIAOZHUN_screen_init();
    lv_disp_load_scr(ui_BEGIN);
}
