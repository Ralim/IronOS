#include "../../../lv_examples.h"
#include "bflb_platform.h"
#if LV_USE_TEXTAREA

lv_obj_t *ta1;

static void event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED) {
        MSG("Value: %s\n", lv_textarea_get_text(obj));
    } else if (event == LV_EVENT_LONG_PRESSED_REPEAT) {
        /*For simple test: Long press the Text are to add the text below*/
        const char *txt = "\n\nYou can scroll it if the text is long enough.\n";
        static uint16_t i = 0;

        if (txt[i] != '\0') {
            lv_textarea_add_char(ta1, txt[i]);
            i++;
        }
    }
}

void lv_ex_textarea_1(void)
{
    ta1 = lv_textarea_create(lv_scr_act(), NULL);
    lv_obj_set_size(ta1, 200, 100);
    lv_obj_align(ta1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_textarea_set_text(ta1, "A text in a Text Area"); /*Set an initial text*/
    lv_obj_set_event_cb(ta1, event_handler);
}

#endif
