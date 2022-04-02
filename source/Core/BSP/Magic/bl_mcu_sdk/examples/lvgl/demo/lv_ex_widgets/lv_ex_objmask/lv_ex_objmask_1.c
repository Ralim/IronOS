#include "../../../lv_examples.h"
#if LV_USE_OBJMASK

void lv_ex_objmask_1(void)
{
    /*Set a very visible color for the screen to clearly see what happens*/
    lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex3(0xf33));

    lv_obj_t *om = lv_objmask_create(lv_scr_act(), NULL);
    lv_obj_set_size(om, 200, 200);
    lv_obj_align(om, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t *label = lv_label_create(om, NULL);
    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
    lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(label, 180);
    lv_label_set_text(label, "This label will be masked out. See how it works.");
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

    lv_obj_t *cont = lv_cont_create(om, NULL);
    lv_obj_set_size(cont, 180, 100);
    lv_obj_set_drag(cont, true);
    lv_obj_align(cont, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);

    lv_obj_t *btn = lv_btn_create(cont, NULL);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "Button");
    uint32_t t;

    lv_refr_now(NULL);
    t = lv_tick_get();

    while (lv_tick_elaps(t) < 1000)
        ;

    lv_area_t a;
    lv_draw_mask_radius_param_t r1;

    a.x1 = 10;
    a.y1 = 10;
    a.x2 = 190;
    a.y2 = 190;
    lv_draw_mask_radius_init(&r1, &a, LV_RADIUS_CIRCLE, false);
    lv_objmask_add_mask(om, &r1);

    lv_refr_now(NULL);
    t = lv_tick_get();

    while (lv_tick_elaps(t) < 1000)
        ;

    a.x1 = 100;
    a.y1 = 100;
    a.x2 = 150;
    a.y2 = 150;
    lv_draw_mask_radius_init(&r1, &a, LV_RADIUS_CIRCLE, true);
    lv_objmask_add_mask(om, &r1);

    lv_refr_now(NULL);
    t = lv_tick_get();

    while (lv_tick_elaps(t) < 1000)
        ;

    lv_draw_mask_line_param_t l1;
    lv_draw_mask_line_points_init(&l1, 0, 0, 100, 200, LV_DRAW_MASK_LINE_SIDE_TOP);
    lv_objmask_add_mask(om, &l1);

    lv_refr_now(NULL);
    t = lv_tick_get();

    while (lv_tick_elaps(t) < 1000)
        ;

    lv_draw_mask_fade_param_t f1;
    a.x1 = 100;
    a.y1 = 0;
    a.x2 = 200;
    a.y2 = 200;
    lv_draw_mask_fade_init(&f1, &a, LV_OPA_TRANSP, 0, LV_OPA_COVER, 150);
    lv_objmask_add_mask(om, &f1);
}

#endif
