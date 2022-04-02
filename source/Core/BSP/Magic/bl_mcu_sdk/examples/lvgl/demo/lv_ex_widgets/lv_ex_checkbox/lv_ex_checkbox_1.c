#include "../../../lv_examples.h"
#include "bflb_platform.h"
#if LV_USE_CHECKBOX

static void event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED) {
        //MSG("State: %s\n", lv_checkbox_is_checked(obj) ? "Checked" : "Unchecked");
    }
}

void lv_ex_checkbox_1(void)
{
    lv_obj_t *cb = lv_checkbox_create(lv_scr_act(), NULL);
    lv_checkbox_set_text(cb, "I agree to terms and conditions.");
    lv_obj_align(cb, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(cb, event_handler);
}

#endif
