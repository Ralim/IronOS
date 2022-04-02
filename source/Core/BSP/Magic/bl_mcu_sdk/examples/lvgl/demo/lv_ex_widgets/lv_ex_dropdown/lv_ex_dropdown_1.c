#include "../../../lv_examples.h"
#include "bflb_platform.h"
#if LV_USE_DROPDOWN

static void event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        //MSG("Option: %s\n", buf);
    }
}

void lv_ex_dropdown_1(void)
{
    /*Create a normal drop down list*/
    lv_obj_t *ddlist = lv_dropdown_create(lv_scr_act(), NULL);
    lv_dropdown_set_options(ddlist, "Apple\n"
                                    "Banana\n"
                                    "Orange\n"
                                    "Melon\n"
                                    "Grape\n"
                                    "Raspberry");

    lv_obj_align(ddlist, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
    lv_obj_set_event_cb(ddlist, event_handler);
}

#endif
