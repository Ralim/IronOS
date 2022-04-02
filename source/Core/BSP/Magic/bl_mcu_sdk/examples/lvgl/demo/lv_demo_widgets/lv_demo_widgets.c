/**
 * @file lv_demo_widgets.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_examples.h"
#include "lv_demo_widgets.h"

#if LV_USE_DEMO_WIDGETS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void controls_create(lv_obj_t *parent);
static void visuals_create(lv_obj_t *parent);
static void selectors_create(lv_obj_t *parent);
static void slider_event_cb(lv_obj_t *slider, lv_event_t e);
static void ta_event_cb(lv_obj_t *ta, lv_event_t e);
static void kb_event_cb(lv_obj_t *ta, lv_event_t e);
static void bar_anim(lv_task_t *t);
static void arc_anim(lv_obj_t *arc, lv_anim_value_t value);
static void linemeter_anim(lv_obj_t *linemeter, lv_anim_value_t value);
static void gauge_anim(lv_obj_t *gauge, lv_anim_value_t value);
static void table_event_cb(lv_obj_t *table, lv_event_t e);
#if LV_USE_THEME_MATERIAL
static void color_chg_event_cb(lv_obj_t *sw, lv_event_t e);
#endif
#if LV_DEMO_WIDGETS_SLIDESHOW
static void tab_content_anim_create(lv_obj_t *parent);
static void tab_changer_task_cb(lv_task_t *task);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *tv;
static lv_obj_t *t1;
static lv_obj_t *t2;
static lv_obj_t *t3;
static lv_obj_t *kb;

static lv_style_t style_box;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_widgets(void)
{
    tv = lv_tabview_create(lv_scr_act(), NULL);
#if LV_USE_THEME_MATERIAL

    if (LV_THEME_DEFAULT_INIT == lv_theme_material_init) {
        lv_disp_size_t disp_size = lv_disp_get_size_category(NULL);

        if (disp_size >= LV_DISP_SIZE_MEDIUM) {
            lv_obj_set_style_local_pad_left(tv, LV_TABVIEW_PART_TAB_BG, LV_STATE_DEFAULT, LV_HOR_RES / 2);
            lv_obj_t *sw = lv_switch_create(lv_scr_act(), NULL);

            if (lv_theme_get_flags() & LV_THEME_MATERIAL_FLAG_DARK) {
                lv_switch_on(sw, LV_ANIM_OFF);
            }

            lv_obj_set_event_cb(sw, color_chg_event_cb);
            lv_obj_set_pos(sw, LV_DPX(10), LV_DPX(10));
            lv_obj_set_style_local_value_str(sw, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, "Dark");
            lv_obj_set_style_local_value_align(sw, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_ALIGN_OUT_RIGHT_MID);
            lv_obj_set_style_local_value_ofs_x(sw, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_DPI / 35);
        }
    }

#endif

    t1 = lv_tabview_add_tab(tv, "Controls");
    t2 = lv_tabview_add_tab(tv, "Visuals");
    t3 = lv_tabview_add_tab(tv, "Selectors");

    lv_style_init(&style_box);
    lv_style_set_value_align(&style_box, LV_STATE_DEFAULT, LV_ALIGN_OUT_TOP_LEFT);
    lv_style_set_value_ofs_y(&style_box, LV_STATE_DEFAULT, -LV_DPX(10));
    lv_style_set_margin_top(&style_box, LV_STATE_DEFAULT, LV_DPX(30));

    controls_create(t1);
    visuals_create(t2);
    selectors_create(t3);

#if LV_DEMO_WIDGETS_SLIDESHOW
    lv_task_create(tab_changer_task_cb, 8000, LV_TASK_PRIO_LOW, NULL);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void controls_create(lv_obj_t *parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY_TOP);

    lv_disp_size_t disp_size = lv_disp_get_size_category(NULL);
    lv_coord_t grid_w = lv_page_get_width_grid(parent, disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1);

#if LV_DEMO_WIDGETS_SLIDESHOW == 0
    static const char *btns[] = { "Cancel", "Ok", "" };

    lv_obj_t *m = lv_msgbox_create(lv_scr_act(), NULL);
    lv_msgbox_add_btns(m, btns);
    lv_obj_t *btnm = lv_msgbox_get_btnmatrix(m);
    lv_btnmatrix_set_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_CHECK_STATE);
#endif

    lv_obj_t *h = lv_cont_create(parent, NULL);
    lv_cont_set_layout(h, LV_LAYOUT_PRETTY_MID);
    lv_obj_add_style(h, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_drag_parent(h, true);

    lv_obj_set_style_local_value_str(h, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "Basics");

    lv_cont_set_fit2(h, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(h, grid_w);
    lv_obj_t *btn = lv_btn_create(h, NULL);
    lv_btn_set_fit2(btn, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn, lv_obj_get_width_grid(h, disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1));
    lv_obj_t *label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Button");

    btn = lv_btn_create(h, btn);
    lv_btn_toggle(btn);
    label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Button");

    lv_switch_create(h, NULL);

    lv_checkbox_create(h, NULL);

    lv_coord_t fit_w = lv_obj_get_width_fit(h);

    lv_obj_t *slider = lv_slider_create(h, NULL);
    lv_slider_set_value(slider, 40, LV_ANIM_OFF);
    lv_obj_set_event_cb(slider, slider_event_cb);
    lv_obj_set_width_margin(slider, fit_w);

    /*Use the knobs style value the display the current value in focused state*/
    lv_obj_set_style_local_margin_top(slider, LV_SLIDER_PART_BG, LV_STATE_DEFAULT, LV_DPX(25));
    lv_obj_set_style_local_value_font(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, lv_theme_get_font_small());
    lv_obj_set_style_local_value_ofs_y(slider, LV_SLIDER_PART_KNOB, LV_STATE_FOCUSED, -LV_DPX(25));
    lv_obj_set_style_local_value_opa(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_value_opa(slider, LV_SLIDER_PART_KNOB, LV_STATE_FOCUSED, LV_OPA_COVER);
    lv_obj_set_style_local_transition_time(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, 300);
    lv_obj_set_style_local_transition_prop_5(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, LV_STYLE_VALUE_OFS_Y);
    lv_obj_set_style_local_transition_prop_6(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, LV_STYLE_VALUE_OPA);

    slider = lv_slider_create(h, slider);
    lv_slider_set_type(slider, LV_SLIDER_TYPE_RANGE);
    lv_slider_set_value(slider, 70, LV_ANIM_OFF);
    lv_slider_set_left_value(slider, 30, LV_ANIM_OFF);
    lv_obj_set_style_local_value_ofs_y(slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, -LV_DPX(25));
    lv_obj_set_style_local_value_font(slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, lv_theme_get_font_small());
    lv_obj_set_style_local_value_opa(slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_event_cb(slider, slider_event_cb);
    lv_event_send(slider, LV_EVENT_VALUE_CHANGED, NULL); /*To refresh the text*/

    if (lv_obj_get_width(slider) > fit_w) {
        lv_obj_set_width(slider, fit_w);
    }

    h = lv_cont_create(parent, h);
    lv_cont_set_fit(h, LV_FIT_NONE);
    lv_obj_set_style_local_value_str(h, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "Text input");

    lv_obj_t *ta = lv_textarea_create(h, NULL);
    lv_cont_set_fit2(ta, LV_FIT_PARENT, LV_FIT_NONE);
    lv_textarea_set_text(ta, "");
    lv_textarea_set_placeholder_text(ta, "E-mail address");
    lv_textarea_set_one_line(ta, true);
    lv_textarea_set_cursor_hidden(ta, true);
    lv_obj_set_event_cb(ta, ta_event_cb);

    ta = lv_textarea_create(h, ta);
    lv_textarea_set_pwd_mode(ta, true);
    lv_textarea_set_placeholder_text(ta, "Password");

    ta = lv_textarea_create(h, NULL);
    lv_cont_set_fit2(ta, LV_FIT_PARENT, LV_FIT_NONE);
    lv_textarea_set_text(ta, "");
    lv_textarea_set_placeholder_text(ta, "Message");
    lv_textarea_set_cursor_hidden(ta, true);
    lv_obj_set_event_cb(ta, ta_event_cb);
    lv_cont_set_fit4(ta, LV_FIT_PARENT, LV_FIT_PARENT, LV_FIT_NONE, LV_FIT_PARENT);

#if LV_DEMO_WIDGETS_SLIDESHOW
    tab_content_anim_create(parent);
#endif
}

static void visuals_create(lv_obj_t *parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY_TOP);

    lv_disp_size_t disp_size = lv_disp_get_size_category(NULL);

    lv_coord_t grid_h_chart = lv_page_get_height_grid(parent, 1, 1);
    lv_coord_t grid_w_chart = lv_page_get_width_grid(parent, disp_size <= LV_DISP_SIZE_LARGE ? 1 : 2, 1);

    lv_obj_t *chart = lv_chart_create(parent, NULL);
    lv_obj_add_style(chart, LV_CHART_PART_BG, &style_box);

    if (disp_size <= LV_DISP_SIZE_SMALL) {
        lv_obj_set_style_local_text_font(chart, LV_CHART_PART_SERIES_BG, LV_STATE_DEFAULT, lv_theme_get_font_small());
    }

    lv_obj_set_drag_parent(chart, true);
    lv_obj_set_style_local_value_str(chart, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "Line chart");
    lv_obj_set_width_margin(chart, grid_w_chart);
    lv_obj_set_height_margin(chart, grid_h_chart);
    lv_chart_set_div_line_count(chart, 3, 0);
    lv_chart_set_point_count(chart, 8);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

    if (disp_size > LV_DISP_SIZE_SMALL) {
        lv_obj_set_style_local_pad_left(chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, 4 * (LV_DPI / 10));
        lv_obj_set_style_local_pad_bottom(chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, 3 * (LV_DPI / 10));
        lv_obj_set_style_local_pad_right(chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, 2 * (LV_DPI / 10));
        lv_obj_set_style_local_pad_top(chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, 2 * (LV_DPI / 10));
        lv_chart_set_y_tick_length(chart, 0, 0);
        lv_chart_set_x_tick_length(chart, 0, 0);
        lv_chart_set_y_tick_texts(chart, "600\n500\n400\n300\n200", 0, LV_CHART_AXIS_DRAW_LAST_TICK);
        lv_chart_set_x_tick_texts(chart, "Jan\nFeb\nMar\nApr\nMay\nJun\nJul\nAug", 0, LV_CHART_AXIS_DRAW_LAST_TICK);
    }

    lv_chart_series_t *s1 = lv_chart_add_series(chart, LV_THEME_DEFAULT_COLOR_PRIMARY);
    lv_chart_series_t *s2 = lv_chart_add_series(chart, LV_THEME_DEFAULT_COLOR_SECONDARY);

    lv_chart_set_next(chart, s1, 10);
    lv_chart_set_next(chart, s1, 90);
    lv_chart_set_next(chart, s1, 30);
    lv_chart_set_next(chart, s1, 60);
    lv_chart_set_next(chart, s1, 10);
    lv_chart_set_next(chart, s1, 90);
    lv_chart_set_next(chart, s1, 30);
    lv_chart_set_next(chart, s1, 60);
    lv_chart_set_next(chart, s1, 10);
    lv_chart_set_next(chart, s1, 90);

    lv_chart_set_next(chart, s2, 32);
    lv_chart_set_next(chart, s2, 66);
    lv_chart_set_next(chart, s2, 5);
    lv_chart_set_next(chart, s2, 47);
    lv_chart_set_next(chart, s2, 32);
    lv_chart_set_next(chart, s2, 32);
    lv_chart_set_next(chart, s2, 66);
    lv_chart_set_next(chart, s2, 5);
    lv_chart_set_next(chart, s2, 47);
    lv_chart_set_next(chart, s2, 66);
    lv_chart_set_next(chart, s2, 5);
    lv_chart_set_next(chart, s2, 47);

    lv_obj_t *chart2 = lv_chart_create(parent, chart);
    lv_chart_set_type(chart2, LV_CHART_TYPE_COLUMN);
    lv_obj_set_style_local_value_str(chart2, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "Column chart");

    s1 = lv_chart_add_series(chart2, LV_THEME_DEFAULT_COLOR_PRIMARY);
    s2 = lv_chart_add_series(chart2, LV_THEME_DEFAULT_COLOR_SECONDARY);

    lv_chart_set_next(chart2, s1, 10);
    lv_chart_set_next(chart2, s1, 90);
    lv_chart_set_next(chart2, s1, 30);
    lv_chart_set_next(chart2, s1, 60);
    lv_chart_set_next(chart2, s1, 10);
    lv_chart_set_next(chart2, s1, 90);
    lv_chart_set_next(chart2, s1, 30);
    lv_chart_set_next(chart2, s1, 60);
    lv_chart_set_next(chart2, s1, 10);
    lv_chart_set_next(chart2, s1, 90);

    lv_chart_set_next(chart2, s2, 32);
    lv_chart_set_next(chart2, s2, 66);
    lv_chart_set_next(chart2, s2, 5);
    lv_chart_set_next(chart2, s2, 47);
    lv_chart_set_next(chart2, s2, 32);
    lv_chart_set_next(chart2, s2, 32);
    lv_chart_set_next(chart2, s2, 66);
    lv_chart_set_next(chart2, s2, 5);
    lv_chart_set_next(chart2, s2, 47);
    lv_chart_set_next(chart2, s2, 66);
    lv_chart_set_next(chart2, s2, 5);
    lv_chart_set_next(chart2, s2, 47);

    lv_coord_t grid_w_meter;

    if (disp_size <= LV_DISP_SIZE_SMALL) {
        grid_w_meter = lv_page_get_width_grid(parent, 1, 1);
    } else if (disp_size <= LV_DISP_SIZE_MEDIUM) {
        grid_w_meter = lv_page_get_width_grid(parent, 2, 1);
    } else {
        grid_w_meter = lv_page_get_width_grid(parent, 3, 1);
    }

    lv_coord_t meter_h = lv_page_get_height_fit(parent);
    lv_coord_t meter_size = LV_MATH_MIN(grid_w_meter, meter_h);

    lv_obj_t *lmeter = lv_linemeter_create(parent, NULL);
    lv_obj_set_drag_parent(lmeter, true);
    lv_linemeter_set_value(lmeter, 50);
    lv_obj_set_size(lmeter, meter_size, meter_size);
    lv_obj_add_style(lmeter, LV_LINEMETER_PART_MAIN, &style_box);
    lv_obj_set_style_local_value_str(lmeter, LV_LINEMETER_PART_MAIN, LV_STATE_DEFAULT, "Line meter");

    lv_obj_t *label = lv_label_create(lmeter, NULL);
    lv_obj_align(label, lmeter, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, lmeter);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)linemeter_anim);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 4000);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);

    lv_obj_t *gauge = lv_gauge_create(parent, NULL);
    lv_obj_set_drag_parent(gauge, true);
    lv_obj_set_size(gauge, meter_size, meter_size);
    lv_obj_add_style(gauge, LV_GAUGE_PART_MAIN, &style_box);
    lv_obj_set_style_local_value_str(gauge, LV_GAUGE_PART_MAIN, LV_STATE_DEFAULT, "Gauge");

    label = lv_label_create(gauge, label);
    lv_obj_align(label, gauge, LV_ALIGN_CENTER, 0, grid_w_meter / 3);

    lv_anim_set_var(&a, gauge);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)gauge_anim);
    lv_anim_start(&a);

    lv_obj_t *arc = lv_arc_create(parent, NULL);
    lv_obj_set_drag_parent(arc, true);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_angles(arc, 0, 0);
    lv_obj_set_size(arc, meter_size, meter_size);
    lv_obj_add_style(arc, LV_ARC_PART_BG, &style_box);
    lv_obj_set_style_local_value_str(arc, LV_ARC_PART_BG, LV_STATE_DEFAULT, "Arc");

    label = lv_label_create(arc, label);
    lv_obj_align(label, arc, LV_ALIGN_CENTER, 0, 0);

    lv_anim_set_var(&a, arc);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)arc_anim);
    lv_anim_set_values(&a, 0, 360);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);

    /*Create a bar and use the backgrounds value style property to display the current value*/
    lv_obj_t *bar_h = lv_cont_create(parent, NULL);
    lv_cont_set_fit2(bar_h, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_add_style(bar_h, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_style_local_value_str(bar_h, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "Bar");

    if (disp_size <= LV_DISP_SIZE_SMALL) {
        lv_obj_set_width(bar_h, lv_page_get_width_grid(parent, 1, 1));
    } else if (disp_size <= LV_DISP_SIZE_MEDIUM) {
        lv_obj_set_width(bar_h, lv_page_get_width_grid(parent, 2, 1));
    } else {
        lv_obj_set_width(bar_h, lv_page_get_width_grid(parent, 3, 2));
    }

    lv_obj_t *bar = lv_bar_create(bar_h, NULL);
    lv_obj_set_width(bar, lv_obj_get_width_fit(bar_h));
    lv_obj_set_style_local_value_font(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, lv_theme_get_font_small());
    lv_obj_set_style_local_value_align(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_ALIGN_OUT_BOTTOM_MID);
    lv_obj_set_style_local_value_ofs_y(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_DPI / 20);
    lv_obj_set_style_local_margin_bottom(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_DPI / 7);
    lv_obj_align(bar, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *led_h = lv_cont_create(parent, NULL);
    lv_cont_set_layout(led_h, LV_LAYOUT_PRETTY_MID);

    if (disp_size <= LV_DISP_SIZE_SMALL) {
        lv_obj_set_width(led_h, lv_page_get_width_grid(parent, 1, 1));
    } else if (disp_size <= LV_DISP_SIZE_MEDIUM) {
        lv_obj_set_width(led_h, lv_page_get_width_grid(parent, 2, 1));
    } else {
        lv_obj_set_width(led_h, lv_page_get_width_grid(parent, 3, 1));
    }

    lv_obj_set_height(led_h, lv_obj_get_height(bar_h));
    lv_obj_add_style(led_h, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_drag_parent(led_h, true);
    lv_obj_set_style_local_value_str(led_h, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "LEDs");

    lv_obj_t *led = lv_led_create(led_h, NULL);
    lv_coord_t led_size = lv_obj_get_height_fit(led_h);
    lv_obj_set_size(led, led_size, led_size);
    lv_led_off(led);

    led = lv_led_create(led_h, led);
    lv_led_set_bright(led, (LV_LED_BRIGHT_MAX - LV_LED_BRIGHT_MIN) / 2 + LV_LED_BRIGHT_MIN);

    led = lv_led_create(led_h, led);
    lv_led_on(led);

    if (disp_size == LV_DISP_SIZE_MEDIUM) {
        lv_obj_add_protect(led_h, LV_PROTECT_POS);
        lv_obj_align(led_h, bar_h, LV_ALIGN_OUT_BOTTOM_MID, 0, lv_obj_get_style_margin_top(led_h, LV_OBJ_PART_MAIN) + lv_obj_get_style_pad_inner(parent, LV_PAGE_PART_SCROLLABLE));
    }

    lv_task_create(bar_anim, 100, LV_TASK_PRIO_LOW, bar);
}

static void selectors_create(lv_obj_t *parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY_MID);

    lv_disp_size_t disp_size = lv_disp_get_size_category(NULL);
    lv_coord_t grid_h = lv_page_get_height_grid(parent, 1, 1);
    lv_coord_t grid_w;

    if (disp_size <= LV_DISP_SIZE_SMALL) {
        grid_w = lv_page_get_width_grid(parent, 1, 1);
    } else {
        grid_w = lv_page_get_width_grid(parent, 2, 1);
    }

    lv_obj_t *cal = lv_calendar_create(parent, NULL);
    lv_obj_set_drag_parent(cal, true);

    if (disp_size > LV_DISP_SIZE_MEDIUM) {
        lv_obj_set_size(cal, LV_MATH_MIN(grid_h, grid_w), LV_MATH_MIN(grid_h, grid_w));
    } else {
        lv_obj_set_size(cal, grid_w, grid_w);

        if (disp_size <= LV_DISP_SIZE_SMALL) {
            lv_obj_set_style_local_text_font(cal, LV_CALENDAR_PART_BG, LV_STATE_DEFAULT, lv_theme_get_font_small());
        }
    }

    static lv_calendar_date_t hl[] = {
        { .year = 2020, .month = 1, .day = 5 },
        { .year = 2020, .month = 1, .day = 9 },
    };

    lv_obj_t *btn;
    lv_obj_t *h = lv_cont_create(parent, NULL);
    lv_obj_set_drag_parent(h, true);

    if (disp_size <= LV_DISP_SIZE_SMALL) {
        lv_cont_set_fit2(h, LV_FIT_NONE, LV_FIT_TIGHT);
        lv_obj_set_width(h, lv_page_get_width_fit(parent));
        lv_cont_set_layout(h, LV_LAYOUT_COLUMN_MID);
    } else if (disp_size <= LV_DISP_SIZE_MEDIUM) {
        lv_obj_set_size(h, lv_obj_get_width(cal), lv_obj_get_height(cal));
        lv_cont_set_layout(h, LV_LAYOUT_PRETTY_TOP);
    } else {
        lv_obj_set_click(h, false);
        lv_obj_set_style_local_bg_opa(h, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
        lv_obj_set_style_local_border_opa(h, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
        lv_obj_set_style_local_pad_left(h, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_obj_set_style_local_pad_right(h, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_obj_set_style_local_pad_top(h, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_obj_set_size(h, LV_MATH_MIN(grid_h, grid_w), LV_MATH_MIN(grid_h, grid_w));
        lv_cont_set_layout(h, LV_LAYOUT_PRETTY_TOP);
    }

    lv_obj_t *roller = lv_roller_create(h, NULL);
    lv_obj_add_style(roller, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_style_local_value_str(roller, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "Roller");
    lv_roller_set_auto_fit(roller, false);
    lv_roller_set_align(roller, LV_LABEL_ALIGN_CENTER);
    lv_roller_set_visible_row_count(roller, 4);
    lv_obj_set_width(roller, lv_obj_get_width_grid(h, disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1));

    lv_obj_t *dd = lv_dropdown_create(h, NULL);
    lv_obj_add_style(dd, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_style_local_value_str(dd, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "Dropdown");
    lv_obj_set_width(dd, lv_obj_get_width_grid(h, disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1));
    lv_dropdown_set_options(dd, "Alpha\nBravo\nCharlie\nDelta\nEcho\nFoxtrot\nGolf\nHotel\nIndia\nJuliette\nKilo\nLima\nMike\nNovember\n"
                                "Oscar\nPapa\nQuebec\nRomeo\nSierra\nTango\nUniform\nVictor\nWhiskey\nXray\nYankee\nZulu");

    lv_obj_t *list = lv_list_create(parent, NULL);
    lv_list_set_scroll_propagation(list, true);
    lv_obj_set_size(list, grid_w, grid_h);

    const char *txts[] = { LV_SYMBOL_SAVE, "Save", LV_SYMBOL_CUT, "Cut", LV_SYMBOL_COPY, "Copy",
                           LV_SYMBOL_OK, "This is a substantially long text to scroll on the list", LV_SYMBOL_EDIT, "Edit", LV_SYMBOL_WIFI, "Wifi",
                           LV_SYMBOL_BLUETOOTH, "Bluetooth", LV_SYMBOL_GPS, "GPS", LV_SYMBOL_USB, "USB",
                           LV_SYMBOL_SD_CARD, "SD card", LV_SYMBOL_CLOSE, "Close", NULL };

    uint32_t i;

    for (i = 0; txts[i] != NULL; i += 2) {
        btn = lv_list_add_btn(list, txts[i], txts[i + 1]);
        lv_btn_set_checkable(btn, true);

        /*Make a button disabled*/
        if (i == 4) {
            lv_btn_set_state(btn, LV_BTN_STATE_DISABLED);
        }
    }

    lv_calendar_set_highlighted_dates(cal, hl, 2);

    static lv_style_t style_cell4;
    lv_style_init(&style_cell4);
    lv_style_set_bg_opa(&style_cell4, LV_STATE_DEFAULT, LV_OPA_50);
    lv_style_set_bg_color(&style_cell4, LV_STATE_DEFAULT, LV_COLOR_GRAY);

    lv_obj_t *page = lv_page_create(parent, NULL);
    lv_obj_set_size(page, grid_w, grid_h);
    lv_coord_t table_w_max = lv_page_get_width_fit(page);
    lv_page_set_scroll_propagation(page, true);

    lv_obj_t *table1 = lv_table_create(page, NULL);
    lv_obj_add_style(table1, LV_TABLE_PART_CELL4, &style_cell4);
    /*Clean the background style of the table because it is placed on a page*/
    lv_obj_clean_style_list(table1, LV_TABLE_PART_BG);
    lv_obj_set_drag_parent(table1, true);
    lv_obj_set_event_cb(table1, table_event_cb);
    lv_table_set_col_cnt(table1, disp_size > LV_DISP_SIZE_SMALL ? 3 : 2);

    if (disp_size > LV_DISP_SIZE_SMALL) {
        lv_table_set_col_width(table1, 0, LV_MATH_MAX(30, 1 * table_w_max / 5));
        lv_table_set_col_width(table1, 1, LV_MATH_MAX(60, 2 * table_w_max / 5));
        lv_table_set_col_width(table1, 2, LV_MATH_MAX(60, 2 * table_w_max / 5));
    } else {
        lv_table_set_col_width(table1, 0, LV_MATH_MAX(30, 1 * table_w_max / 4 - 1));
        lv_table_set_col_width(table1, 1, LV_MATH_MAX(60, 3 * table_w_max / 4 - 1));
    }

    lv_table_set_cell_value(table1, 0, 0, "#");
    lv_table_set_cell_value(table1, 1, 0, "1");
    lv_table_set_cell_value(table1, 2, 0, "2");
    lv_table_set_cell_value(table1, 3, 0, "3");
    lv_table_set_cell_value(table1, 4, 0, "4");
    lv_table_set_cell_value(table1, 5, 0, "5");
    lv_table_set_cell_value(table1, 6, 0, "6");

    lv_table_set_cell_value(table1, 0, 1, "NAME");
    lv_table_set_cell_value(table1, 1, 1, "Mark");
    lv_table_set_cell_value(table1, 2, 1, "Jacob");
    lv_table_set_cell_value(table1, 3, 1, "John");
    lv_table_set_cell_value(table1, 4, 1, "Emily");
    lv_table_set_cell_value(table1, 5, 1, "Ivan");
    lv_table_set_cell_value(table1, 6, 1, "George");

    if (disp_size > LV_DISP_SIZE_SMALL) {
        lv_table_set_cell_value(table1, 0, 2, "CITY");
        lv_table_set_cell_value(table1, 1, 2, "Moscow");
        lv_table_set_cell_value(table1, 2, 2, "New York");
        lv_table_set_cell_value(table1, 3, 2, "Oslo");
        lv_table_set_cell_value(table1, 4, 2, "London");
        lv_table_set_cell_value(table1, 5, 2, "Texas");
        lv_table_set_cell_value(table1, 6, 2, "Athen");
    }
}

static void slider_event_cb(lv_obj_t *slider, lv_event_t e)
{
    if (e == LV_EVENT_VALUE_CHANGED) {
        if (lv_slider_get_type(slider) == LV_SLIDER_TYPE_NORMAL) {
            static char buf[16];
            lv_snprintf(buf, sizeof(buf), "%d", lv_slider_get_value(slider));
            lv_obj_set_style_local_value_str(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, buf);
        } else {
            static char buf[32];
            lv_snprintf(buf, sizeof(buf), "%d-%d", lv_slider_get_left_value(slider), lv_slider_get_value(slider));
            lv_obj_set_style_local_value_str(slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, buf);
        }
    }
}

static void ta_event_cb(lv_obj_t *ta, lv_event_t e)
{
    if (e == LV_EVENT_RELEASED) {
        if (kb == NULL) {
            lv_obj_set_height(tv, LV_VER_RES / 2);
            kb = lv_keyboard_create(lv_scr_act(), NULL);
            lv_obj_set_event_cb(kb, kb_event_cb);

            lv_indev_wait_release(lv_indev_get_act());
        }

        lv_textarea_set_cursor_hidden(ta, false);
        lv_page_focus(t1, lv_textarea_get_label(ta), LV_ANIM_ON);
        lv_keyboard_set_textarea(kb, ta);
    } else if (e == LV_EVENT_DEFOCUSED) {
        lv_textarea_set_cursor_hidden(ta, true);
    }
}

static void kb_event_cb(lv_obj_t *_kb, lv_event_t e)
{
    lv_keyboard_def_event_cb(kb, e);

    if (e == LV_EVENT_CANCEL) {
        if (kb) {
            lv_obj_set_height(tv, LV_VER_RES);
            lv_obj_del(kb);
            kb = NULL;
        }
    }
}

static void bar_anim(lv_task_t *t)
{
    static uint32_t x = 0;
    lv_obj_t *bar = t->user_data;

    static char buf[64];
    lv_snprintf(buf, sizeof(buf), "Copying %d/%d", x, lv_bar_get_max_value(bar));
    lv_obj_set_style_local_value_str(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, buf);

    lv_bar_set_value(bar, x, LV_ANIM_OFF);
    x++;

    if (x > lv_bar_get_max_value(bar)) {
        x = 0;
    }
}

static void arc_anim(lv_obj_t *arc, lv_anim_value_t value)
{
    lv_arc_set_end_angle(arc, value);

    static char buf[64];
    lv_snprintf(buf, sizeof(buf), "%d", value);
    lv_obj_t *label = lv_obj_get_child(arc, NULL);
    lv_label_set_text(label, buf);
    lv_obj_align(label, arc, LV_ALIGN_CENTER, 0, 0);
}

static void linemeter_anim(lv_obj_t *linemeter, lv_anim_value_t value)
{
    lv_linemeter_set_value(linemeter, value);

    static char buf[64];
    lv_snprintf(buf, sizeof(buf), "%d", value);
    lv_obj_t *label = lv_obj_get_child(linemeter, NULL);
    lv_label_set_text(label, buf);
    lv_obj_align(label, linemeter, LV_ALIGN_CENTER, 0, 0);
}

static void gauge_anim(lv_obj_t *gauge, lv_anim_value_t value)
{
    lv_gauge_set_value(gauge, 0, value);

    static char buf[64];
    lv_snprintf(buf, sizeof(buf), "%d", value);
    lv_obj_t *label = lv_obj_get_child(gauge, NULL);
    lv_label_set_text(label, buf);
    lv_obj_align(label, gauge, LV_ALIGN_IN_TOP_MID, 0, lv_obj_get_y(label));
}

static void table_event_cb(lv_obj_t *table, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
        uint16_t row;
        uint16_t col;
        lv_res_t r = lv_table_get_pressed_cell(table, &row, &col);

        if (r == LV_RES_OK && col && row) /*Skip the first row and col*/
        {
            if (lv_table_get_cell_type(table, row, col) == 1) {
                lv_table_set_cell_type(table, row, col, 4);
            } else {
                lv_table_set_cell_type(table, row, col, 1);
            }
        }
    }
}

#if LV_USE_THEME_MATERIAL
static void color_chg_event_cb(lv_obj_t *sw, lv_event_t e)
{
    if (LV_THEME_DEFAULT_INIT != lv_theme_material_init) {
        return;
    }

    if (e == LV_EVENT_VALUE_CHANGED) {
        uint32_t flag = LV_THEME_MATERIAL_FLAG_LIGHT;

        if (lv_switch_get_state(sw)) {
            flag = LV_THEME_MATERIAL_FLAG_DARK;
        }

        LV_THEME_DEFAULT_INIT(lv_theme_get_color_primary(), lv_theme_get_color_secondary(),
                              flag,
                              lv_theme_get_font_small(), lv_theme_get_font_normal(), lv_theme_get_font_subtitle(), lv_theme_get_font_title());
    }
}
#endif

#if LV_DEMO_WIDGETS_SLIDESHOW

static void tab_content_anim_create(lv_obj_t *parent)
{
    lv_anim_t a;
    lv_obj_t *scrl = lv_page_get_scrl(parent);
    lv_coord_t y_start = lv_obj_get_style_pad_top(parent, LV_PAGE_PART_BG);
    lv_coord_t anim_h = lv_obj_get_height(scrl) - lv_obj_get_height_fit(parent);
    uint32_t anim_time = lv_anim_speed_to_time(LV_DPI, 0, anim_h);

    lv_anim_init(&a);
    lv_anim_set_var(&a, scrl);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, y_start, y_start - anim_h);
    lv_anim_set_time(&a, anim_time);
    lv_anim_set_playback_time(&a, anim_time);
    lv_anim_set_playback_delay(&a, 200);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&a, 200);
    lv_anim_start(&a);
}
static void tab_changer_task_cb(lv_task_t *task)
{
    uint16_t act = lv_tabview_get_tab_act(tv);
    act++;

    if (act >= 3) {
        act = 0;
    }

    lv_tabview_set_tab_act(tv, act, LV_ANIM_ON);

    switch (act) {
        case 0:
            tab_content_anim_create(t1);
            break;

        case 1:
            tab_content_anim_create(t2);
            break;

        case 2:
            tab_content_anim_create(t3);
            break;
    }
}
#endif

#endif
