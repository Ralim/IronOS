#include "../../../lv_examples.h"
#include "bflb_platform.h"
#if LV_USE_MSGBOX

static void event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED) {
        MSG("Button: %s\n", lv_msgbox_get_active_btn_text(obj));
    }
}

void lv_ex_msgbox_1(void)
{
    static const char *btns[] = { "Apply", "Close", "" };

    lv_obj_t *mbox1 = lv_msgbox_create(lv_scr_act(), NULL);
    lv_msgbox_set_text(mbox1, "A message box with two buttons.");
    lv_msgbox_add_btns(mbox1, btns);
    lv_obj_set_width(mbox1, 200);
    lv_obj_set_event_cb(mbox1, event_handler);
    lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
}

#endif
