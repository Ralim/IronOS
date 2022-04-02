#include "../../../lv_examples.h"
#if LV_USE_KEYBOARD

static lv_obj_t *kb;
static lv_obj_t *ta;

static void kb_event_cb(lv_obj_t *keyboard, lv_event_t e)
{
    lv_keyboard_def_event_cb(kb, e);

    if (e == LV_EVENT_CANCEL) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_del(kb);
        kb = NULL;
    }
}

static void kb_create(void)
{
    kb = lv_keyboard_create(lv_scr_act(), NULL);
    lv_keyboard_set_cursor_manage(kb, true);
    lv_obj_set_event_cb(kb, kb_event_cb);
    lv_keyboard_set_textarea(kb, ta);
}

static void ta_event_cb(lv_obj_t *ta_local, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED && kb == NULL) {
        kb_create();
    }
}

void lv_ex_keyboard_1(void)
{
    /*Create a text area. The keyboard will write here*/
    ta = lv_textarea_create(lv_scr_act(), NULL);
    lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 16);
    lv_obj_set_event_cb(ta, ta_event_cb);
    lv_textarea_set_text(ta, "");
    lv_coord_t max_h = LV_VER_RES / 2 - LV_DPI / 8;

    if (lv_obj_get_height(ta) > max_h) {
        lv_obj_set_height(ta, max_h);
    }

    kb_create();
}
#endif
