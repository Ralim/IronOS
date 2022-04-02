#include "../../../lv_examples.h"
#include "bflb_platform.h"
#if LV_USE_SLIDER

static void event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED) {
        MSG("Value: %d\n", lv_slider_get_value(obj));
    }
}

void lv_ex_slider_1(void)
{
    /*Create a slider*/
    lv_obj_t *slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(slider, event_handler);
}

#endif
