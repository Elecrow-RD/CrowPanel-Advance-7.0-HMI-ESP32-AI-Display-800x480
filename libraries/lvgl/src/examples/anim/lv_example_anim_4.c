#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_SWITCH

static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}
static void anim_y_cb(void * var, int32_t v)
{
    lv_obj_set_y(var, v);
}
static void sw_event_cb(lv_event_t * e)
{
    lv_obj_t * sw = lv_event_get_target(e);
    lv_obj_t * label = lv_event_get_user_data(e);

    if(lv_obj_has_state(sw, LV_STATE_CHECKED)) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, label);
        lv_anim_set_values(&a, lv_obj_get_x(label), 100);
        lv_anim_set_time(&a, 500);
        lv_anim_set_exec_cb(&a, anim_x_cb);
        lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
        lv_anim_start(&a);
    }
    else {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, label);
        lv_anim_set_values(&a, lv_obj_get_x(label), -lv_obj_get_width(label));
        lv_anim_set_time(&a, 500);
        lv_anim_set_exec_cb(&a, anim_x_cb);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
        lv_anim_start(&a);
    }

}

/**
 * Start animation on an event
 */
void lv_example_anim_4(void)
{
    LV_IMG_DECLARE(toppic);
    LV_IMG_DECLARE(dowm);
    LV_IMG_DECLARE(dowmblack);
    /*Darken the button when pressed and make it wider*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_img_recolor_opa(&style_pr, LV_OPA_TRANSP);
    //  lv_style_set_img_recolor(&style_pr, lv_color_black());
    //  lv_style_set_transform_width(&style_pr, 5);

    /*Create an image button*/
    // lv_obj_t * imgbtn1 = lv_imgbtn_create(lv_scr_act());
    // lv_imgbtn_set_src(imgbtn1, LV_IMGBTN_STATE_RELEASED, NULL, &toppic, NULL);
    // lv_obj_t * imgbtn2 = lv_imgbtn_create(lv_scr_act());
    // lv_imgbtn_set_src(imgbtn2, LV_IMGBTN_STATE_RELEASED, NULL, &dowm, NULL);
    lv_obj_t * img1 = lv_img_create(lv_scr_act());
    lv_img_set_src(img1, &toppic);
    lv_obj_t * img2 = lv_img_create(lv_scr_act());
    lv_img_set_src(img2, &dowm);
    lv_obj_t * img3 = lv_img_create(lv_scr_act());
    lv_img_set_src(img3, &dowmblack);
    // lv_style_set_bg_opa(&style_pr, LV_OPA_0); //设置背景透明度
    // lv_obj_add_style(imgbtn1, &style_pr, 0);         //将样式添加到文字对象中
    // lv_obj_add_style(imgbtn1, &style_pr, LV_STATE_PRESSED);
  
    lv_obj_set_size(img1, 92, 77);
    lv_obj_set_pos(img1, 194, 40);
    lv_obj_set_size(img2, 153, 64);
    lv_obj_set_pos(img2, 164, 150);
    lv_obj_set_size(img3, 153, 64);
    lv_obj_set_pos(img3, 164, 150);
    // lv_obj_align(imgbtn1, LV_ALIGN_CENTER, 0, 0);

    // lv_obj_t * label = lv_label_create(lv_scr_act());
    // lv_label_set_text(label, "ELECROW");
    // lv_obj_set_pos(label, 210, 10);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, img1);
    lv_anim_set_values(&a, 0, 40);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_exec_cb(&a, anim_y_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);

    lv_anim_t b;
    lv_anim_init(&b);
    lv_anim_set_var(&b, img3);
    lv_anim_set_values(&b, 164, 307);
    lv_anim_set_time(&b, 2000);
    lv_anim_set_exec_cb(&b, anim_x_cb);
    lv_anim_set_path_cb(&b, lv_anim_path_ease_out);
    lv_anim_start(&b);

    // lv_anim_t b;
    // lv_anim_init(&b);
    // lv_anim_set_var(&b, imgbtn1);
    // lv_anim_set_values(&b, 0, 50);
    // lv_anim_set_time(&b, 2000);
    // lv_anim_set_exec_cb(&b, anim_x_cb);
    // lv_anim_set_path_cb(&b, lv_anim_path_ease_out);
    // lv_anim_start(&b);

    // lv_obj_t * sw = lv_switch_create(lv_scr_act());
    // lv_obj_center(sw);
    // lv_obj_add_state(sw, LV_STATE_CHECKED);
    // lv_obj_add_event_cb(sw, sw_event_cb, LV_EVENT_VALUE_CHANGED, label);
    
}


#endif
