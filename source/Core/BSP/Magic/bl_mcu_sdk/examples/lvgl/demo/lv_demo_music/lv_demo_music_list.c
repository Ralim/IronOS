/**
 * @file lv_demo_music_list.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_music_list.h"
#if LV_USE_DEMO_MUSIC

#include "lv_demo_music_main.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t *add_list_btn(lv_obj_t *page, uint32_t track_id);
static void btn_event_cb(lv_obj_t *btn, lv_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *list;
static lv_font_t *font_small;
static lv_font_t *font_medium;
static lv_style_t style_btn;
static lv_style_t style_scrollbar;
static lv_style_t style_title;
static lv_style_t style_artist;
static lv_style_t style_time;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *_lv_demo_music_list_create(lv_obj_t *parent)
{
    font_small = &lv_font_montserrat_10;
    font_medium = &lv_font_montserrat_12;

    lv_style_init(&style_scrollbar);
    lv_style_set_size(&style_scrollbar, LV_STATE_DEFAULT, 4);
    lv_style_set_bg_opa(&style_scrollbar, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&style_scrollbar, LV_STATE_DEFAULT, lv_color_hex3(0xeee));
    lv_style_set_radius(&style_scrollbar, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_pad_right(&style_scrollbar, LV_STATE_DEFAULT, 4);

    lv_style_init(&style_btn);
    lv_style_set_bg_opa(&style_btn, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&style_btn, LV_STATE_PRESSED, LV_OPA_COVER);
    lv_style_set_bg_opa(&style_btn, LV_STATE_CHECKED, LV_OPA_COVER);
    lv_style_set_bg_color(&style_btn, LV_STATE_PRESSED, lv_color_hex(0x4c4965));
    lv_style_set_bg_color(&style_btn, LV_STATE_CHECKED, lv_color_hex(0x4c4965));
    lv_style_set_text_opa(&style_btn, LV_STATE_DISABLED, LV_OPA_40);
    lv_style_set_image_opa(&style_btn, LV_STATE_DISABLED, LV_OPA_40);

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, LV_STATE_DEFAULT, &lv_font_montserrat_12);
    lv_style_set_text_color(&style_title, LV_STATE_DEFAULT, lv_color_hex(0xffffff));

    lv_style_init(&style_artist);
    lv_style_set_text_font(&style_artist, LV_STATE_DEFAULT, &lv_font_montserrat_10);
    lv_style_set_text_color(&style_artist, LV_STATE_DEFAULT, lv_color_hex(0xb1b0be));

    lv_style_init(&style_time);
    lv_style_set_text_font(&style_time, LV_STATE_DEFAULT, &lv_font_montserrat_10);
    lv_style_set_text_color(&style_time, LV_STATE_DEFAULT, lv_color_hex(0xffffff));

    /*Create an empty white main container*/
    list = lv_page_create(parent, NULL);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - LV_DEMO_LIST_CTRL_OVERLAP);
    lv_obj_set_y(list, LV_DEMO_LIST_CTRL_OVERLAP);
    lv_obj_clean_style_list(list, LV_PAGE_PART_BG);
    lv_obj_clean_style_list(list, LV_PAGE_PART_SCROLLABLE);
    lv_obj_clean_style_list(list, LV_PAGE_PART_SCROLLBAR);
    lv_obj_add_style(list, LV_PAGE_PART_SCROLLBAR, &style_scrollbar);
    lv_page_set_scrl_layout(list, LV_LAYOUT_COLUMN_MID);

    uint32_t track_id;

    for (track_id = 0; _lv_demo_music_get_title(track_id); track_id++) {
        add_list_btn(list, track_id);
    }

    return list;
}

void _lv_demo_music_list_btn_check(uint32_t track_id, bool state)
{
    lv_obj_t *btn = lv_obj_get_child_back(lv_page_get_scrl(list), NULL);
    uint32_t i = 0;

    while (btn) {
        if (i == track_id) {
            break;
        }

        i++;
        btn = lv_obj_get_child_back(lv_page_get_scrl(list), btn);
    }

    lv_obj_t *icon = lv_obj_get_child_back(btn, NULL);

    if (state) {
        lv_imgbtn_set_state(icon, LV_BTN_STATE_CHECKED_RELEASED);
        lv_obj_add_state(btn, LV_STATE_CHECKED);
    } else {
        lv_imgbtn_set_state(icon, LV_BTN_STATE_RELEASED);
        lv_obj_clear_state(btn, LV_STATE_CHECKED);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t *add_list_btn(lv_obj_t *page, uint32_t track_id)
{
    uint32_t t = _lv_demo_music_get_track_length(track_id);
    char time[32];
    lv_snprintf(time, sizeof(time), "%d:%02d", t / 60, t % 60);
    const char *title = _lv_demo_music_get_title(track_id);
    const char *artist = _lv_demo_music_get_artist(track_id);

    lv_obj_t *btn = lv_obj_create(page, NULL);
    lv_obj_set_size(btn, lv_page_get_width_fit(page), 60);
    lv_obj_clean_style_list(btn, LV_BTN_PART_MAIN);
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &style_btn);
    lv_obj_add_protect(btn, LV_PROTECT_PRESS_LOST);
    lv_obj_set_event_cb(btn, btn_event_cb);
    lv_page_glue_obj(btn, true);

    if (track_id >= 3) {
        lv_obj_add_state(btn, LV_STATE_DISABLED);
    }

    LV_IMG_DECLARE(img_lv_demo_music_btn_list_play);
    LV_IMG_DECLARE(img_lv_demo_music_btn_list_pause);

    lv_obj_t *icon = lv_imgbtn_create(btn, NULL);
    lv_imgbtn_set_src(icon, LV_BTN_STATE_RELEASED, &img_lv_demo_music_btn_list_play);
    lv_imgbtn_set_src(icon, LV_BTN_STATE_CHECKED_RELEASED, &img_lv_demo_music_btn_list_pause);
    lv_obj_align(icon, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);

    lv_obj_t *title_obj = lv_label_create(btn, NULL);
    lv_label_set_text(title_obj, title);
    lv_obj_align(title_obj, icon, LV_ALIGN_OUT_RIGHT_TOP, 5, 13);
    lv_obj_add_style(title_obj, LV_LABEL_PART_MAIN, &style_title);

    lv_obj_t *artist_obj = lv_label_create(btn, NULL);
    lv_label_set_text(artist_obj, artist);
    lv_obj_align(artist_obj, title_obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);
    lv_obj_add_style(artist_obj, LV_LABEL_PART_MAIN, &style_artist);

    lv_obj_t *time_obj = lv_label_create(btn, NULL);
    lv_label_set_text(time_obj, time);
    lv_obj_add_style(time_obj, LV_LABEL_PART_MAIN, &style_time);
    lv_obj_align(time_obj, NULL, LV_ALIGN_IN_RIGHT_MID, -15, 0);

    LV_IMG_DECLARE(img_lv_demo_music_list_border);
    lv_obj_t *border = lv_img_create(btn, NULL);
    lv_img_set_src(border, &img_lv_demo_music_list_border);
    lv_obj_set_width(border, lv_obj_get_width(btn));
    lv_obj_align(border, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    return btn;
}

static void btn_event_cb(lv_obj_t *btn, lv_event_t event)
{
    if (event != LV_EVENT_CLICKED) {
        return;
    }

    lv_obj_t *child = lv_obj_get_child_back(lv_page_get_scrl(list), NULL);
    uint32_t i = 0;

    while (child) {
        if (btn == child) {
            break;
        }

        i++;
        child = lv_obj_get_child_back(lv_page_get_scrl(list), child);
    }

    _lv_demo_music_play(i);
}
#endif /*LV_USE_DEMO_MUSIC*/
