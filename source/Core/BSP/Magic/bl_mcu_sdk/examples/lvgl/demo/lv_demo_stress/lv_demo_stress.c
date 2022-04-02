/**
 * @file lv_demo_stress.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_examples.h"

#if LV_USE_DEMO_STRESS
/*********************
 *      DEFINES
 *********************/
#define TIME_STEP 50

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void auto_del(lv_obj_t *obj, uint32_t delay);
static void obj_test_task_cb(lv_task_t *);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *main_page;
static lv_obj_t *ta;
static lv_style_t style_slow_trans;
static const char *mbox_btns[] = { "Ok", "Cancel", "" };
static uint32_t mem_free_start = 0;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_stress(void)
{
    lv_task_create(obj_test_task_cb, TIME_STEP, LV_TASK_PRIO_MID, NULL);

    lv_style_init(&style_slow_trans);
    lv_style_set_bg_opa(&style_slow_trans, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&style_slow_trans, LV_STATE_DISABLED, LV_COLOR_SILVER);
    lv_style_set_transition_prop_1(&style_slow_trans, LV_STATE_DEFAULT, LV_STYLE_BG_COLOR);
    lv_style_set_transition_time(&style_slow_trans, LV_STATE_DEFAULT, TIME_STEP * 10);

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    mem_free_start = mon.free_size;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void obj_test_task_cb(lv_task_t *param)
{
    (void)param; /*Unused*/
    static int16_t state = -1;

    lv_anim_t a;
    lv_obj_t *obj;

    switch (state) {
        case -1: {
            lv_res_t res = lv_mem_test();

            if (res != LV_RES_OK) {
                LV_LOG_ERROR("Memory integrity error");
            }

            lv_mem_monitor_t mon;
            lv_mem_monitor(&mon);
            LV_LOG_USER("mem leak since start: %d, frag: %3d %%", mem_free_start - mon.free_size, mon.frag_pct);
        } break;

        case 0:
            /* Holder for all object types */
            main_page = lv_page_create(lv_scr_act(), NULL);
            lv_page_set_anim_time(main_page, TIME_STEP);
            lv_obj_set_size(main_page, LV_HOR_RES / 2, LV_VER_RES);
            lv_page_set_scrl_layout(main_page, LV_LAYOUT_PRETTY_MID);

            obj = lv_btn_create(main_page, NULL);
            lv_obj_set_size(obj, 100, 70);
            obj = lv_label_create(obj, NULL);
            lv_label_set_text(obj, "Multi line\n" LV_SYMBOL_OK LV_SYMBOL_CLOSE LV_SYMBOL_WIFI);
            break;

        case 1:
            obj = lv_tabview_create(lv_scr_act(), NULL);
            lv_obj_set_size(obj, LV_HOR_RES / 2, LV_VER_RES / 2);
            lv_obj_align(obj, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
            {
                lv_obj_t *t = lv_tabview_add_tab(obj, "First");

                t = lv_tabview_add_tab(obj, "Second");
                lv_obj_t *c = lv_cpicker_create(t, NULL);
                lv_obj_set_size(c, 150, 150);
                c = lv_led_create(t, NULL);
                lv_obj_set_pos(c, 160, 20);
                t = lv_tabview_add_tab(obj, LV_SYMBOL_EDIT " Edit");
                t = lv_tabview_add_tab(obj, LV_SYMBOL_CLOSE);

                lv_tabview_set_anim_time(obj, TIME_STEP * 2);
                lv_tabview_set_tab_act(obj, 1, LV_ANIM_ON);
            }
            auto_del(obj, TIME_STEP * 5 + 30);
            break;

        case 2:
            obj = lv_btn_create(main_page, NULL);
            lv_obj_set_size(obj, 100, 70);

            /*Move to disabled state very slowly*/
            lv_obj_add_style(obj, LV_BTN_PART_MAIN, &style_slow_trans);
            lv_obj_set_state(obj, LV_STATE_DISABLED);

            /*Add an infinite width change animation*/
            lv_anim_init(&a);
            lv_anim_set_var(&a, obj);
            lv_anim_set_time(&a, TIME_STEP * 2);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_width);
            lv_anim_set_values(&a, 100, 200);
            lv_anim_set_playback_time(&a, TIME_STEP * 2);
            lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
            lv_anim_start(&a);

            /*Delete the object a few sec later*/
            auto_del(obj, TIME_STEP * 10);

            obj = lv_label_create(obj, NULL);
            lv_label_set_text_fmt(obj, "Formatted:\n%d %s", 12, "Volt");
            lv_page_focus(main_page, obj, LV_ANIM_ON);
            break;

        case 3:
            ta = lv_textarea_create(lv_scr_act(), NULL);
            lv_obj_align(ta, main_page, LV_ALIGN_OUT_RIGHT_TOP, 10, 10);
            lv_obj_set_height(ta, LV_VER_RES / 4);
            lv_textarea_set_placeholder_text(ta, "The placeholder");
            break;

        case 4:
            obj = lv_btn_create(main_page, NULL);
            lv_obj_set_size(obj, 100, 70);
            lv_obj_set_style_local_value_str(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "Text from\nstyle");
            lv_obj_del_async(obj); /*Delete on next call of `lv_task_handler` (so not now)*/

            obj = lv_btn_create(main_page, obj);
            lv_btn_set_fit(obj, LV_FIT_TIGHT);
            lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_LEFT);
            lv_obj_set_style_local_pattern_opa(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_50);
            lv_obj_set_style_local_pattern_repeat(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, true);
            lv_page_focus(main_page, obj, LV_ANIM_ON);
            break;

        case 5:
            lv_textarea_set_one_line(ta, true);
            break;

        case 6:
            lv_page_set_scrl_layout(main_page, LV_LAYOUT_COLUMN_LEFT);
            break;

        case 7:
            obj = lv_bar_create(main_page, NULL);
            lv_bar_set_range(obj, -1000, 2000);
            lv_bar_set_anim_time(obj, 1000);
            lv_bar_set_value(obj, 1800, LV_ANIM_ON);
            lv_bar_set_start_value(obj, -500, LV_ANIM_ON);

            auto_del(obj, TIME_STEP * 2 + 70);

            obj = lv_slider_create(main_page, NULL);
            lv_bar_set_anim_time(obj, 100);
            lv_bar_set_value(obj, 5000, LV_ANIM_ON); /*Animate to out of range value*/
            auto_del(obj, TIME_STEP * 5 + 22);

            obj = lv_switch_create(main_page, NULL);
            lv_switch_on(obj, LV_ANIM_ON);

            obj = lv_switch_create(main_page, obj);
            lv_switch_off(obj, LV_ANIM_ON);
            auto_del(obj, 730);
            break;

        case 8:
            obj = lv_win_create(lv_scr_act(), NULL);
            lv_obj_set_size(obj, LV_HOR_RES / 2, LV_VER_RES / 2);
            lv_obj_align(obj, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
            lv_win_set_title(obj, "Window title");
            lv_win_add_btn(obj, LV_SYMBOL_CLOSE);
            lv_win_add_btn(obj, LV_SYMBOL_DOWN);
            auto_del(obj, TIME_STEP * 3 + 5);

            obj = lv_calendar_create(obj, NULL);
            break;

        case 9:
            lv_textarea_set_text(ta, "A very very long text which will should make the text area scrollable"
                                     "Here area some dummy sentences to be sure the text area will be really scrollable.");
            break;

        case 10:
            obj = lv_keyboard_create(lv_scr_act(), NULL);
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_TEXT_UPPER);
            lv_anim_init(&a);
            lv_anim_set_var(&a, obj);
            lv_anim_set_values(&a, LV_VER_RES, LV_VER_RES - lv_obj_get_height(obj));
            lv_anim_set_time(&a, TIME_STEP + 3);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
            lv_anim_start(&a);

            auto_del(obj, TIME_STEP * 2 + 18);
            break;

        case 11:
            obj = lv_dropdown_create(main_page, NULL);
            lv_dropdown_set_options(obj, "Zero\nOne\nTwo\nThree\nFour\nFive\nSix\nSeven\nEight");
            lv_dropdown_open(obj);
            lv_dropdown_set_selected(obj, 2);
            auto_del(obj, TIME_STEP * 3 + 11);
            break;

        case 12:
            obj = lv_roller_create(main_page, NULL);
            lv_roller_set_options(obj, "Zero\nOne\nTwo\nThree\nFour\nFive\nSix\nSeven\nEight", LV_ROLLER_MODE_INFINITE);
            lv_roller_set_anim_time(obj, TIME_STEP);
            lv_roller_set_selected(obj, 2, LV_ANIM_ON);
            auto_del(obj, TIME_STEP * 20 + 22);
            break;

        case 13:
            obj = lv_arc_create(main_page, NULL);
            lv_anim_init(&a);
            lv_anim_set_var(&a, obj);
            lv_anim_set_values(&a, 180, 400);
            lv_anim_set_time(&a, TIME_STEP);
            lv_anim_set_delay(&a, TIME_STEP + 25);
            lv_anim_set_playback_time(&a, TIME_STEP * 2);
            lv_anim_set_repeat_count(&a, 3);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_arc_set_end_angle);
            lv_anim_start(&a);

            obj = lv_linemeter_create(main_page, NULL);
            lv_page_focus(main_page, obj, LV_ANIM_ON);
            lv_linemeter_set_value(obj, 80);
            lv_anim_init(&a);
            lv_anim_set_var(&a, obj);
            lv_anim_set_values(&a, 0, 90);
            lv_anim_set_time(&a, TIME_STEP * 3);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_linemeter_set_angle_offset);
            lv_anim_start(&a);

            auto_del(obj, TIME_STEP * 6 + 30);

            obj = lv_linemeter_create(main_page, obj);
            auto_del(obj, TIME_STEP * 1 + 10);
            break;

        case 14:
            obj = lv_msgbox_create(lv_scr_act(), NULL);
            lv_msgbox_set_text(obj, "Some text on the message box with average length");
            lv_msgbox_add_btns(obj, mbox_btns);
            lv_btnmatrix_set_btn_width(lv_msgbox_get_btnmatrix(obj), 1, 2);
            lv_msgbox_start_auto_close(obj, TIME_STEP * 3);
            lv_obj_align(obj, NULL, LV_ALIGN_IN_RIGHT_MID, -10, 0);
            break;

        case 15:
            lv_textarea_set_one_line(ta, false);
            break;

        case 16: {
            lv_obj_t *tv = lv_tileview_create(lv_scr_act(), NULL);
            static lv_point_t valid_pos[] = { { 0, 0 }, { 0, 1 }, { 1, 1 } };
            lv_tileview_set_valid_positions(tv, valid_pos, 3);
            lv_tileview_set_anim_time(tv, TIME_STEP * 2);
            auto_del(tv, TIME_STEP * 3 + 5);

            obj = lv_label_create(tv, NULL);
            lv_label_set_text(obj, "Tile: 0;0");

            obj = lv_label_create(tv, NULL);
            lv_label_set_text(obj, "Tile: 0;1");
            lv_obj_set_pos(tv, 0, LV_VER_RES);

            obj = lv_label_create(tv, NULL);
            lv_label_set_text(obj, "Tile: 1;1");
            lv_obj_set_pos(tv, LV_HOR_RES, LV_VER_RES);

            lv_tileview_set_tile_act(tv, 1, 1, LV_ANIM_ON);
        } break;

        case 17: {
            static lv_color_t needle_colors[2];
            needle_colors[0] = LV_COLOR_RED;
            needle_colors[1] = LV_COLOR_BLUE;
            obj = lv_gauge_create(main_page, NULL);
            lv_gauge_set_needle_count(obj, 2, needle_colors);
            lv_gauge_set_value(obj, 0, 30);
            lv_gauge_set_value(obj, 1, 60);
            lv_page_focus(main_page, obj, LV_ANIM_ON);
            auto_del(obj, TIME_STEP * 2 + 16);
        } break;

        case 18:
            obj = lv_list_create(main_page, NULL);
            {
                lv_obj_t *b;
                b = lv_list_add_btn(obj, LV_SYMBOL_OK, "1. Some very long text to scroll");
                auto_del(b, 10);
                lv_list_add_btn(obj, LV_SYMBOL_OK, "2. Some very long text to scroll");
                lv_list_add_btn(obj, LV_SYMBOL_OK, "3. Some very long text to scroll");
                b = lv_list_add_btn(obj, LV_SYMBOL_OK, "4. Some very long text to scroll");
                auto_del(b, TIME_STEP);
                b = lv_list_add_btn(obj, LV_SYMBOL_OK, "5. Some very long text to scroll");
                auto_del(b, TIME_STEP + 90);
                b = lv_list_add_btn(obj, LV_SYMBOL_OK, "6. Some very long text to scroll");
                auto_del(b, TIME_STEP + 10);
                lv_list_focus(lv_list_get_prev_btn(obj, NULL), LV_ANIM_ON);
            }
            auto_del(obj, TIME_STEP * 5 + 15);

            obj = lv_list_create(lv_scr_act(), obj);
            lv_list_set_layout(obj, LV_LAYOUT_ROW_MID);
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_width(obj, LV_HOR_RES);
            lv_list_add_btn(obj, LV_SYMBOL_OK, "1. Horizontal list button");
            lv_list_focus(lv_list_get_prev_btn(obj, NULL), LV_ANIM_ON);
            lv_list_add_btn(obj, LV_SYMBOL_OK, "2. Horizontal list button");
            lv_list_add_btn(obj, LV_SYMBOL_OK, "3. Horizontal list button");

            auto_del(obj, TIME_STEP * 3 + 85);
            break;

        case 19:
            lv_textarea_set_cursor_pos(ta, 10);
            lv_textarea_set_text(ta, "__INSERTED TEXT" LV_SYMBOL_EDIT "__");
            break;

        case 20:
            lv_page_set_scrl_layout(main_page, LV_LAYOUT_PRETTY_MID);
            break;

        case 21:
            lv_textarea_set_cursor_pos(ta, 0);
            lv_textarea_add_char(ta, '0');
            lv_textarea_add_char(ta, '1');
            lv_textarea_add_char(ta, '2');
            lv_textarea_add_char(ta, '3');
            lv_textarea_add_char(ta, '4');
            lv_textarea_add_char(ta, '5');
            lv_textarea_add_char(ta, '6');
            lv_textarea_add_char(ta, '7');
            lv_textarea_add_char(ta, '8');
            lv_textarea_add_char(ta, '9');
            lv_textarea_add_char(ta, 'A');
            lv_textarea_add_char(ta, 'B');
            lv_textarea_add_char(ta, 'C');
            lv_textarea_add_char(ta, 'D');
            lv_textarea_add_char(ta, 'E');
            lv_textarea_add_char(ta, 'F');
            lv_textarea_add_text(ta, LV_SYMBOL_OK);
            lv_textarea_add_text(ta, LV_SYMBOL_CLOSE);
            lv_textarea_add_text(ta, LV_SYMBOL_COPY);
            lv_textarea_add_text(ta, LV_SYMBOL_SAVE);
            lv_textarea_add_text(ta, LV_SYMBOL_PASTE);
            break;

        case 22:
            obj = lv_spinbox_create(main_page, NULL);
            lv_spinbox_set_digit_format(obj, 6, 3);
            lv_spinbox_set_value(obj, 5678);
            lv_spinbox_set_step(obj, 10);
            lv_spinbox_increment(obj);
            lv_spinbox_increment(obj);
            lv_spinbox_increment(obj);
            lv_spinbox_set_step(obj, 100);
            lv_spinbox_increment(obj);
            lv_spinbox_increment(obj);
            lv_spinbox_set_step(obj, 1);
            lv_spinbox_increment(obj);
            lv_spinbox_increment(obj);
            auto_del(obj, TIME_STEP * 2 + 15);

            lv_page_scroll_ver(main_page, LV_DPI);

            break;

        case 23:
            obj = lv_chart_create(main_page, NULL);
            {
                lv_chart_series_t *s1 = lv_chart_add_series(obj, LV_COLOR_RED);
                lv_chart_set_next(obj, s1, 36);
                lv_chart_set_next(obj, s1, -29);
                lv_chart_set_next(obj, s1, 51);
                lv_chart_set_next(obj, s1, 107);
                lv_chart_set_next(obj, s1, 70);
                lv_chart_set_next(obj, s1, 36);
                lv_chart_set_next(obj, s1, -2);
                lv_chart_set_next(obj, s1, 63);
                lv_chart_set_next(obj, s1, 48);
                lv_chart_set_next(obj, s1, 72);

                auto_del(obj, TIME_STEP * 3);
            }

            lv_page_scroll_ver(main_page, LV_DPI);
            break;

        case 24:
            obj = lv_checkbox_create(main_page, NULL);
            lv_checkbox_set_text(obj, "An option to select");
            auto_del(obj, TIME_STEP * 2 + 20);

            obj = lv_checkbox_create(main_page, obj);
            lv_checkbox_set_checked(obj, true);

            obj = lv_checkbox_create(main_page, obj);
            lv_checkbox_set_disabled(obj);
            auto_del(obj, TIME_STEP * 1 + 60);

            lv_page_scroll_ver(main_page, LV_DPI);

            break;

        case 25:
            lv_textarea_set_cursor_pos(ta, 20);
            {
                uint16_t i;

                for (i = 0; i < 64; i++) {
                    lv_textarea_del_char_forward(ta);
                }
            }
            break;

        case 26:
            lv_textarea_set_one_line(ta, true);
            break;

        case 29:
            lv_page_clean(main_page);
            lv_obj_del(ta);
            ta = NULL;
            break;

        case 31:
            lv_obj_clean(lv_scr_act());
            main_page = NULL;
            state = -2;
            break;

        default:
            break;
    }

    state++;
}

static void auto_del(lv_obj_t *obj, uint32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 0);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);
    lv_anim_start(&a);
}

#endif /* LV_USE_DEMO_STRESS */
