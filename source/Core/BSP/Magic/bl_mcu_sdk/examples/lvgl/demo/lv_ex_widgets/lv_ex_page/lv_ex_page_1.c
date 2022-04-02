#include "../../../lv_examples.h"
#if LV_USE_PAGE

void lv_ex_page_1(void)
{
    /*Create a page*/
    lv_obj_t *page = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_size(page, 150, 200);
    lv_obj_align(page, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Create a label on the page*/
    lv_obj_t *label = lv_label_create(page, NULL);
    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);   /*Automatically break long lines*/
    lv_obj_set_width(label, lv_page_get_width_fit(page)); /*Set the label width to max value to not show hor. scroll bars*/
    lv_label_set_text(label, "Lorem ipsum dolor sit amet, consectetur adipiscing elit,\n"
                             "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
                             "Ut enim ad minim veniam, quis nostrud exercitation ullamco\n"
                             "laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure\n"
                             "dolor in reprehenderit in voluptate velit esse cillum dolore\n"
                             "eu fugiat nulla pariatur.\n"
                             "Excepteur sint occaecat cupidatat non proident, sunt in culpa\n"
                             "qui officia deserunt mollit anim id est laborum.");
}

#endif
