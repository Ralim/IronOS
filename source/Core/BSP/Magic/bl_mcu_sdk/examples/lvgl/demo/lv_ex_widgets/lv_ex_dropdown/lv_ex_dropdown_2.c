#include "../../../lv_examples.h"
#include "bflb_platform.h"
#if LV_USE_DROPDOWN

/**
 * Create a drop LEFT menu
 */
void lv_ex_dropdown_2(void)
{
    /*Create a drop down list*/
    lv_obj_t *ddlist = lv_dropdown_create(lv_scr_act(), NULL);
    lv_dropdown_set_options(ddlist, "Apple\n"
                                    "Banana\n"
                                    "Orange\n"
                                    "Melon\n"
                                    "Grape\n"
                                    "Raspberry");

    lv_dropdown_set_dir(ddlist, LV_DROPDOWN_DIR_LEFT);
    lv_dropdown_set_symbol(ddlist, NULL);
    lv_dropdown_set_show_selected(ddlist, false);
    lv_dropdown_set_text(ddlist, "Fruits");
    /*It will be called automatically when the size changes*/
    lv_obj_align(ddlist, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 20);

    /*Copy the drop LEFT list*/
    ddlist = lv_dropdown_create(lv_scr_act(), ddlist);
    lv_obj_align(ddlist, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 100);
}

#endif
