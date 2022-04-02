#include "../../../lv_examples.h"
#if LV_USE_LINEMETER

void lv_ex_linemeter_1(void)
{
    /*Create a line meter */
    lv_obj_t *lmeter;
    lmeter = lv_linemeter_create(lv_scr_act(), NULL);
    lv_linemeter_set_range(lmeter, 0, 100);  /*Set the range*/
    lv_linemeter_set_value(lmeter, 80);      /*Set the current value*/
    lv_linemeter_set_scale(lmeter, 240, 21); /*Set the angle and number of lines*/
    lv_obj_set_size(lmeter, 150, 150);
    lv_obj_align(lmeter, NULL, LV_ALIGN_CENTER, 0, 0);
}

#endif
