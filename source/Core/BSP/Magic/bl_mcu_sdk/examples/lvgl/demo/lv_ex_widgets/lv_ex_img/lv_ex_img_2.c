#include "../../../lv_examples.h"
#if LV_USE_IMG

#define SLIDER_WIDTH 20

static void create_sliders(void);
static void slider_event_cb(lv_obj_t *slider, lv_event_t event);

static lv_obj_t *red_slider, *green_slider, *blue_slider, *intense_slider;
static lv_obj_t *img1;
LV_IMG_DECLARE(img_cogwheel_argb);

void lv_ex_img_2(void)
{
    /*Create 4 sliders to adjust RGB color and re-color intensity*/
    create_sliders();

    /* Now create the actual image */
    img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img1, &img_cogwheel_argb);
    lv_obj_align(img1, NULL, LV_ALIGN_IN_RIGHT_MID, -20, 0);
}

static void slider_event_cb(lv_obj_t *slider, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED) {
        /* Recolor the image based on the sliders' values */
        lv_color_t color = lv_color_make(lv_slider_get_value(red_slider), lv_slider_get_value(green_slider), lv_slider_get_value(blue_slider));
        lv_opa_t intense = lv_slider_get_value(intense_slider);
        lv_obj_set_style_local_image_recolor_opa(img1, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, intense);
        lv_obj_set_style_local_image_recolor(img1, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, color);
    }
}

static void create_sliders(void)
{
    /* Create a set of RGB sliders */
    /* Use the red one as a base for all the settings */
    red_slider = lv_slider_create(lv_scr_act(), NULL);
    lv_slider_set_range(red_slider, 0, 255);
    lv_obj_set_size(red_slider, SLIDER_WIDTH, 200); /* Be sure it's a vertical slider */
    lv_obj_set_style_local_bg_color(red_slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_obj_set_event_cb(red_slider, slider_event_cb);

    /* Copy it for the other three sliders */
    green_slider = lv_slider_create(lv_scr_act(), red_slider);
    lv_obj_set_style_local_bg_color(green_slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_LIME);

    blue_slider = lv_slider_create(lv_scr_act(), red_slider);
    lv_obj_set_style_local_bg_color(blue_slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE);

    intense_slider = lv_slider_create(lv_scr_act(), red_slider);
    lv_obj_set_style_local_bg_color(intense_slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_slider_set_value(intense_slider, 255, LV_ANIM_OFF);

    lv_obj_align(red_slider, NULL, LV_ALIGN_IN_LEFT_MID, 20, 0);
    lv_obj_align(green_slider, red_slider, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_align(blue_slider, green_slider, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_align(intense_slider, blue_slider, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
}

#endif
