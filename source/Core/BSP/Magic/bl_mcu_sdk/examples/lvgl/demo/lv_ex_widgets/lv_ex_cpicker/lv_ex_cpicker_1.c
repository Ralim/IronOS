#include "../../../lv_examples.h"
#if LV_USE_CPICKER

void lv_ex_cpicker_1(void)
{
    lv_obj_t *cpicker;

    cpicker = lv_cpicker_create(lv_scr_act(), NULL);
    lv_obj_set_size(cpicker, 200, 200);
    lv_obj_align(cpicker, NULL, LV_ALIGN_CENTER, 0, 0);
}

#endif
