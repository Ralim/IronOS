#include "../../../lv_examples.h"
#if LV_USE_SPINNER

void lv_ex_spinner_1(void)
{
    /*Create a Preloader object*/
    lv_obj_t *preload = lv_spinner_create(lv_scr_act(), NULL);
    lv_obj_set_size(preload, 100, 100);
    lv_obj_align(preload, NULL, LV_ALIGN_CENTER, 0, 0);
}

#endif
