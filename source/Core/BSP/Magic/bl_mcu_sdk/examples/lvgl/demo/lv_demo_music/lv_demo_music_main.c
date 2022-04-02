/**
 * @file lv_demo_music_main.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_music_main.h"
#if LV_USE_DEMO_MUSIC

#include "lv_demo_music_list.h"
#include "assets/spectrum_1.h"
#include "assets/spectrum_2.h"
#include "assets/spectrum_3.h"

/*********************
 *      DEFINES
 *********************/
#define ACTIVE_TRACK_CNT 3
#define INTRO_TIME       2000
#define BAR_COLOR1       lv_color_hex(0xe9dbfc)
#define BAR_COLOR2       lv_color_hex(0x6f8af6)
#define BAR_COLOR3       LV_COLOR_WHITE
#define BAR_COLOR1_STOP  80
#define BAR_COLOR2_STOP  100
#define BAR_COLOR3_STOP  (2 * LV_HOR_RES / 3)
#define BAR_CNT          20
#define DEG_STEP         (180 / BAR_CNT)
#define BAND_CNT         4
#define BAR_PER_BAND_CNT (BAR_CNT / BAND_CNT)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void spectrum_anim_cb(void *a, lv_anim_value_t v);
static void start_anim_cb(void *a, lv_anim_value_t v);
static lv_design_res_t spectrum_design_cb(lv_obj_t *obj, const lv_area_t *mask, lv_design_mode_t mode);
static lv_obj_t *album_img_create(lv_obj_t *parent);
static void album_event_cb(lv_obj_t *img, lv_event_t event);
static void play_event_cb(lv_obj_t *img, lv_event_t event);
static void prev_event_cb(lv_obj_t *img, lv_event_t event);
static void next_event_cb(lv_obj_t *img, lv_event_t event);
static void timer_cb(lv_task_t *t);
static void track_load(uint32_t id);
static lv_res_t main_cont_signal_cb(lv_obj_t *obj, lv_signal_t signal, void *param);
static void stop_start_anim(lv_task_t *task);
static void spectrum_end_cb(lv_anim_t *a);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *main_cont;
static lv_obj_t *spectrum_obj;
static lv_obj_t *title_obj;
static lv_obj_t *artist_obj;
static lv_obj_t *genre_obj;
static lv_obj_t *time_obj;
static lv_obj_t *album_img_obj;
static lv_obj_t *slider_obj;
static uint32_t spectrum_i = 0;
static uint32_t spectrum_i_pause = 0;
static uint32_t bar_ofs = 0;
static uint32_t spectrum_lane_ofs_start = 0;
static uint32_t bar_rot = 0;
static uint32_t time;
static lv_task_t *time_task;
static lv_font_t *font_small;
static lv_font_t *font_medium;
static lv_font_t *font_large;
static uint32_t track_id;
static bool playing;
static bool start_anim;
static uint16_t start_anim_values[40];
static lv_signal_cb_t ancestor_signal_cb;
static lv_obj_t *play_obj;
static const uint16_t (*spectrum)[4];
static uint32_t spectrum_len;
static const uint16_t rnd_array[30] = { 994, 285, 553, 11, 792, 707, 966, 641, 852, 827, 44, 352, 146, 581, 490, 80, 729, 58, 695, 940, 724, 561, 124, 653, 27, 292, 557, 506, 382, 199 };

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *_lv_demo_music_main_create(lv_obj_t *parent)
{
    LV_FONT_DECLARE(lv_demo_music_font_16_bold);
    font_small = &lv_font_montserrat_12;
    font_medium = &lv_font_montserrat_14;
    font_large = &lv_demo_music_font_16_bold;

    /*Create an empty white main container*/
    main_cont = lv_obj_create(parent, NULL);
    lv_obj_set_size(main_cont, LV_HOR_RES, LV_VER_RES + 30);
    lv_obj_set_style_local_bg_color(main_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(main_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 15);
    lv_obj_set_style_local_border_width(main_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_add_protect(main_cont, LV_PROTECT_PRESS_LOST);
    lv_obj_set_drag(main_cont, true);
    lv_obj_set_drag_throw(main_cont, true);
    lv_obj_set_drag_dir(main_cont, LV_DRAG_DIR_VER);
    lv_obj_set_y(main_cont, -15);
    ancestor_signal_cb = lv_obj_get_signal_cb(main_cont);
    lv_obj_set_signal_cb(main_cont, main_cont_signal_cb);

    LV_IMG_DECLARE(img_lv_demo_music_wave_top);
    LV_IMG_DECLARE(img_lv_demo_music_wave_bottom);
    lv_obj_t *wave_top = lv_img_create(main_cont, NULL);
    lv_img_set_src(wave_top, &img_lv_demo_music_wave_top);
    lv_obj_set_width(wave_top, LV_HOR_RES);
    lv_obj_align(wave_top, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    lv_obj_t *wave_bottom = lv_img_create(main_cont, NULL);
    lv_img_set_src(wave_bottom, &img_lv_demo_music_wave_bottom);
    lv_obj_set_width(wave_bottom, LV_HOR_RES);
    lv_obj_align(wave_bottom, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    LV_IMG_DECLARE(img_lv_demo_music_corner_left);
    LV_IMG_DECLARE(img_lv_demo_music_corner_right);
    lv_obj_t *wave_corner = lv_img_create(main_cont, NULL);
    lv_img_set_src(wave_corner, &img_lv_demo_music_corner_left);
    lv_obj_align(wave_corner, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

    wave_corner = lv_img_create(main_cont, NULL);
    lv_img_set_src(wave_corner, &img_lv_demo_music_corner_right);
    lv_obj_align(wave_corner, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);

    /*Create the titles*/
    lv_obj_t *title_box = lv_cont_create(main_cont, NULL);
    lv_obj_clean_style_list(title_box, LV_CONT_PART_MAIN);
    lv_cont_set_layout(title_box, LV_LAYOUT_COLUMN_MID);
    lv_cont_set_fit2(title_box, LV_FIT_NONE, LV_FIT_TIGHT);
#if LV_DEMO_MUSIC_LANDSCAPE
    lv_obj_set_width(title_box, LV_HOR_RES / 2);
    lv_obj_align(title_box, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 23);
#else
    lv_obj_set_width(title_box, LV_HOR_RES - 20);
    lv_obj_align(title_box, NULL, LV_ALIGN_IN_TOP_MID, 0, 15);
#endif

    title_obj = lv_label_create(title_box, NULL);
    lv_obj_set_style_local_text_font(title_obj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_large);
    lv_obj_set_style_local_text_color(title_obj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x504d6d));
    lv_obj_set_style_local_margin_top(title_obj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_label_set_text(title_obj, _lv_demo_music_get_title(track_id));

    artist_obj = lv_label_create(title_box, NULL);
    lv_obj_set_style_local_text_font(artist_obj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_small);
    lv_obj_set_style_local_text_color(artist_obj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x504d6d));
    lv_obj_set_style_local_margin_top(artist_obj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_label_set_text(artist_obj, _lv_demo_music_get_artist(track_id));

    genre_obj = lv_label_create(title_box, NULL);
    lv_obj_set_style_local_text_font(genre_obj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_small);
    lv_obj_set_style_local_text_color(genre_obj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8a86b8));
    lv_label_set_text(genre_obj, _lv_demo_music_get_genre(track_id));

    /*Create the icons*/
    lv_obj_t *icon_box = lv_cont_create(main_cont, NULL);
    lv_obj_clean_style_list(icon_box, LV_CONT_PART_MAIN);
    lv_obj_set_width(icon_box, 176);
    lv_cont_set_fit2(icon_box, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_cont_set_layout(icon_box, LV_LAYOUT_PRETTY_MID);

    lv_obj_t *icon;
    LV_IMG_DECLARE(img_lv_demo_music_icon_1);
    LV_IMG_DECLARE(img_lv_demo_music_icon_2);
    LV_IMG_DECLARE(img_lv_demo_music_icon_3);
    LV_IMG_DECLARE(img_lv_demo_music_icon_4);
    icon = lv_img_create(icon_box, NULL);
    lv_img_set_src(icon, &img_lv_demo_music_icon_1);
    icon = lv_img_create(icon_box, NULL);
    lv_img_set_src(icon, &img_lv_demo_music_icon_2);
    icon = lv_img_create(icon_box, NULL);
    lv_img_set_src(icon, &img_lv_demo_music_icon_3);
    icon = lv_img_create(icon_box, NULL);
    lv_img_set_src(icon, &img_lv_demo_music_icon_4);

#if LV_DEMO_MUSIC_LANDSCAPE
    lv_obj_align(icon_box, title_box, LV_ALIGN_OUT_BOTTOM_MID, 0, 22);
#else
    lv_obj_align(icon_box, NULL, LV_ALIGN_IN_TOP_MID, 0, 100);
#endif

    /*Create the spectrum visualizer*/
    spectrum_obj = lv_obj_create(main_cont, NULL);
    lv_obj_set_design_cb(spectrum_obj, spectrum_design_cb);
    spectrum_obj->ext_draw_pad = 400;
    lv_obj_set_click(spectrum_obj, false);
#if LV_DEMO_MUSIC_LANDSCAPE
    lv_obj_set_size(spectrum_obj, LV_HOR_RES / 2, LV_VER_RES);
    lv_obj_align(spectrum_obj, NULL, LV_ALIGN_IN_RIGHT_MID, 0, -10);
#else
    lv_obj_set_size(spectrum_obj, lv_obj_get_width(lv_scr_act()), 300);
    lv_obj_align(spectrum_obj, NULL, LV_ALIGN_CENTER, 0, -10);
#endif

    album_img_obj = album_img_create(spectrum_obj);

    /*Create the control box*/
    lv_obj_t *ctrl_box = lv_cont_create(main_cont, NULL);
    lv_obj_clean_style_list(ctrl_box, LV_CONT_PART_MAIN);
    lv_obj_set_click(ctrl_box, false);
#if LV_DEMO_MUSIC_LANDSCAPE
    lv_obj_set_size(ctrl_box, (LV_HOR_RES / 2) - 20, 140);
    lv_obj_align(ctrl_box, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 10, -15);
#else
    lv_obj_set_size(ctrl_box, LV_HOR_RES - 10, 140);
    lv_obj_align(ctrl_box, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -15);
#endif

    LV_IMG_DECLARE(img_lv_demo_music_btn_loop);
    LV_IMG_DECLARE(img_lv_demo_music_btn_rnd);
    LV_IMG_DECLARE(img_lv_demo_music_btn_next);
    LV_IMG_DECLARE(img_lv_demo_music_btn_prev);
    LV_IMG_DECLARE(img_lv_demo_music_btn_play);
    LV_IMG_DECLARE(img_lv_demo_music_btn_pause);

    icon = lv_img_create(ctrl_box, NULL);
    lv_img_set_src(icon, &img_lv_demo_music_btn_rnd);
    lv_obj_align(icon, NULL, LV_ALIGN_IN_TOP_MID, -95, 27);

    icon = lv_img_create(ctrl_box, NULL);
    lv_img_set_src(icon, &img_lv_demo_music_btn_loop);
    lv_obj_align(icon, NULL, LV_ALIGN_IN_TOP_MID, 95, 27);

    icon = lv_img_create(ctrl_box, NULL);
    lv_img_set_src(icon, &img_lv_demo_music_btn_prev);
    lv_obj_align(icon, NULL, LV_ALIGN_IN_TOP_MID, -45, 7);
    lv_obj_set_event_cb(icon, prev_event_cb);
    lv_obj_set_click(icon, true);

    play_obj = lv_imgbtn_create(ctrl_box, NULL);
    lv_imgbtn_set_src(play_obj, LV_BTN_STATE_RELEASED, &img_lv_demo_music_btn_play);
    lv_imgbtn_set_src(play_obj, LV_BTN_STATE_CHECKED_RELEASED, &img_lv_demo_music_btn_pause);
    lv_imgbtn_set_checkable(play_obj, true);
    lv_obj_align(play_obj, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_event_cb(play_obj, play_event_cb);

    icon = lv_img_create(ctrl_box, NULL);
    lv_img_set_src(icon, &img_lv_demo_music_btn_next);
    lv_obj_align(icon, NULL, LV_ALIGN_IN_TOP_MID, 45, 7);
    lv_obj_set_event_cb(icon, next_event_cb);
    lv_obj_set_click(icon, true);

    LV_IMG_DECLARE(img_lv_demo_music_slider_knob);
    slider_obj = lv_slider_create(ctrl_box, NULL);
    lv_slider_set_anim_time(slider_obj, 100);
    lv_obj_set_click(slider_obj, false); /*No input from the slider*/

#if LV_DEMO_MUSIC_LANDSCAPE
    lv_obj_set_size(slider_obj, lv_obj_get_width(ctrl_box) - 50, 3);
    lv_obj_align(slider_obj, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 82);
#else
    lv_obj_set_size(slider_obj, 210, 3);
    lv_obj_align(slider_obj, NULL, LV_ALIGN_IN_TOP_MID, -10, 82);
#endif
    lv_obj_set_style_local_pattern_image(slider_obj, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, &img_lv_demo_music_slider_knob);
    lv_obj_set_style_local_bg_opa(slider_obj, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_pad_all(slider_obj, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, 20);
    lv_obj_set_style_local_bg_grad_dir(slider_obj, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, LV_GRAD_DIR_HOR);
    lv_obj_set_style_local_bg_color(slider_obj, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0x569af8));
    lv_obj_set_style_local_bg_grad_color(slider_obj, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0xa666f1));
    lv_obj_set_style_local_outline_width(slider_obj, LV_SLIDER_PART_BG, LV_STATE_DEFAULT, 0);

    time_obj = lv_label_create(ctrl_box, NULL);
    lv_obj_set_style_local_text_font(time_obj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_small);
    lv_obj_set_style_local_text_color(time_obj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8a86b8));
    lv_label_set_text(time_obj, "0:00");
    lv_obj_align(time_obj, NULL, LV_ALIGN_IN_TOP_RIGHT, -1, 75);

    /*A handle to scroll to the track list*/
    lv_obj_t *handle_label = lv_label_create(main_cont, NULL);
    lv_label_set_text(handle_label, "ALL TRACKS");
    lv_obj_set_style_local_text_font(handle_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_small);
    lv_obj_set_style_local_text_color(handle_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8a86b8));
    lv_obj_align(handle_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -35);

    lv_obj_t *handle_rect = lv_obj_create(main_cont, NULL);
    lv_obj_set_size(handle_rect, 20, 2);
    lv_obj_align(handle_rect, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -30);
    lv_obj_set_style_local_bg_color(handle_rect, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8a86b8));
    lv_obj_set_style_local_border_width(handle_rect, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

    time_task = lv_task_create(timer_cb, 1000, LV_TASK_PRIO_OFF, NULL);

    lv_obj_move_foreground(spectrum_obj);

    /*Animate in the content after the intro time*/
    lv_anim_t a;
    lv_anim_path_t path;
    lv_anim_path_init(&path);

    lv_anim_path_set_cb(&path, lv_anim_path_bounce);

    start_anim = true;

    lv_task_t *task = lv_task_create(stop_start_anim, INTRO_TIME + 6000, LV_TASK_PRIO_MID, NULL);
    lv_task_once(task);

    lv_anim_init(&a);
    lv_anim_set_path(&a, &path);

    uint32_t i;
    lv_anim_set_exec_cb(&a, start_anim_cb);

    for (i = 0; i < 20; i++) {
        lv_anim_set_values(&a, LV_HOR_RES, 5);
        lv_anim_set_delay(&a, INTRO_TIME - 200 + rnd_array[i] % 200);
        lv_anim_set_time(&a, 2500 + rnd_array[i] % 500);
        lv_anim_set_var(&a, &start_anim_values[i]);
        lv_anim_start(&a);
    }

    lv_obj_fade_in(wave_top, 1000, INTRO_TIME + 1000);
    lv_obj_fade_in(wave_bottom, 1000, INTRO_TIME + 1000);
    lv_obj_fade_in(title_box, 1000, INTRO_TIME + 1000);
    lv_obj_fade_in(icon_box, 1000, INTRO_TIME + 1000);
    lv_obj_fade_in(ctrl_box, 1000, INTRO_TIME + 1000);
    lv_obj_fade_in(handle_label, 1000, INTRO_TIME + 1000);
    lv_obj_fade_in(handle_rect, 1000, INTRO_TIME + 1000);
    lv_obj_fade_in(album_img_obj, 800, INTRO_TIME + 1000);
    lv_obj_fade_in(spectrum_obj, 0, INTRO_TIME);

    lv_anim_path_set_cb(&path, lv_anim_path_overshoot);
    lv_anim_set_path(&a, &path);

    lv_anim_set_var(&a, album_img_obj);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_delay(&a, INTRO_TIME + 1000);
    lv_anim_set_values(&a, 1, LV_IMG_ZOOM_NONE);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_zoom);
    lv_anim_set_ready_cb(&a, NULL);
    lv_anim_start(&a);

    /* Create an intro from a logo + label */
    LV_IMG_DECLARE(img_lv_demo_music_logo);
    lv_obj_t *logo = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(logo, &img_lv_demo_music_logo);
    lv_obj_align(logo, spectrum_obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_move_foreground(logo);

    lv_obj_t *title = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(title, "LVGL Demo\nMusic player");
    lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_style_local_text_font(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_30);
    lv_obj_set_style_local_text_line_space(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 8);
    lv_obj_align(title, NULL, LV_ALIGN_IN_LEFT_MID, 50, -10);
    lv_obj_fade_out(title, 300, INTRO_TIME);

    lv_anim_path_set_cb(&path, lv_anim_path_ease_in);
    lv_anim_set_path(&a, &path);
    lv_anim_set_var(&a, logo);
    lv_anim_set_time(&a, 400);
    lv_anim_set_delay(&a, INTRO_TIME + 800);
    lv_anim_set_values(&a, LV_IMG_ZOOM_NONE, 10);
    lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);
    lv_anim_start(&a);

    return main_cont;
}

void _lv_demo_music_album_next(bool next)
{
    uint32_t id = track_id;

    if (next) {
        id++;

        if (id >= ACTIVE_TRACK_CNT) {
            id = 0;
        }
    } else {
        if (id == 0) {
            id = ACTIVE_TRACK_CNT - 1;
        } else {
            id--;
        }
    }

    if (playing) {
        _lv_demo_music_play(id);
    } else {
        track_load(id);
    }
}

void _lv_demo_music_play(uint32_t id)
{
    track_load(id);

    _lv_demo_music_resume();
}

void _lv_demo_music_resume(void)
{
    playing = true;
    spectrum_i = spectrum_i_pause;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_values(&a, spectrum_i, spectrum_len - 1);
    lv_anim_set_exec_cb(&a, spectrum_anim_cb);
    lv_anim_set_var(&a, spectrum_obj);
    lv_anim_set_time(&a, ((spectrum_len - spectrum_i) * 1000) / 30);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_set_ready_cb(&a, spectrum_end_cb);
    lv_anim_start(&a);

    lv_task_set_prio(time_task, LV_TASK_PRIO_MID);
    lv_slider_set_range(slider_obj, 0, _lv_demo_music_get_track_length(track_id));

    lv_imgbtn_set_state(play_obj, LV_BTN_STATE_CHECKED_RELEASED);
}

void _lv_demo_music_pause(void)
{
    playing = false;
    spectrum_i_pause = spectrum_i;
    spectrum_i = 0;
    lv_anim_del(spectrum_obj, spectrum_anim_cb);
    lv_obj_invalidate(spectrum_obj);
    lv_img_set_zoom(album_img_obj, LV_IMG_ZOOM_NONE);
    lv_task_set_prio(time_task, LV_TASK_PRIO_OFF);
    lv_imgbtn_set_state(play_obj, LV_BTN_STATE_RELEASED);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void track_load(uint32_t id)
{
    spectrum_i = 0;
    time = 0;
    spectrum_i_pause = 0;
    lv_slider_set_value(slider_obj, 0, LV_ANIM_OFF);
    lv_label_set_text(time_obj, "0:00");

    if (id == track_id) {
        return;
    }

    bool next = false;

    if ((track_id + 1) % ACTIVE_TRACK_CNT == id) {
        next = true;
    }

    _lv_demo_music_list_btn_check(track_id, false);

    track_id = id;

    _lv_demo_music_list_btn_check(id, true);

    lv_label_set_text(title_obj, _lv_demo_music_get_title(track_id));
    lv_label_set_text(artist_obj, _lv_demo_music_get_artist(track_id));
    lv_label_set_text(genre_obj, _lv_demo_music_get_genre(track_id));

    lv_obj_fade_out(album_img_obj, 500, 0);

    lv_anim_path_t path;
    lv_anim_path_init(&path);
    lv_anim_path_set_cb(&path, lv_anim_path_ease_out);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, album_img_obj);
    lv_anim_set_time(&a, 500);
    lv_anim_set_path(&a, &path);
#if LV_DEMO_MUSIC_LANDSCAPE

    if (next) {
        lv_anim_set_values(&a, lv_obj_get_x(album_img_obj), lv_obj_get_x(album_img_obj) - LV_HOR_RES / 7);
    } else {
        lv_anim_set_values(&a, lv_obj_get_x(album_img_obj), lv_obj_get_x(album_img_obj) + LV_HOR_RES / 7);
    }

#else

    if (next) {
        lv_anim_set_values(&a, lv_obj_get_x(album_img_obj), lv_obj_get_x(album_img_obj) - LV_HOR_RES / 2);
    } else {
        lv_anim_set_values(&a, lv_obj_get_x(album_img_obj), lv_obj_get_x(album_img_obj) + LV_HOR_RES / 2);
    }

#endif
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);
    lv_anim_start(&a);

    lv_anim_path_set_cb(&path, lv_anim_path_linear);
    lv_anim_set_path(&a, &path);
    lv_anim_set_var(&a, album_img_obj);
    lv_anim_set_time(&a, 500);
    lv_anim_set_values(&a, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE / 2);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_zoom);
    lv_anim_set_ready_cb(&a, NULL);
    lv_anim_start(&a);

    album_img_obj = album_img_create(spectrum_obj);
    lv_obj_fade_in(album_img_obj, 500, 100);

    lv_anim_path_set_cb(&path, lv_anim_path_overshoot);
    lv_anim_set_path(&a, &path);
    lv_anim_set_var(&a, album_img_obj);
    lv_anim_set_time(&a, 500);
    lv_anim_set_delay(&a, 100);
    lv_anim_set_values(&a, LV_IMG_ZOOM_NONE / 4, LV_IMG_ZOOM_NONE);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_zoom);
    lv_anim_set_ready_cb(&a, NULL);
    lv_anim_start(&a);
}

int32_t get_cos(int32_t deg, int32_t a)
{
    int32_t r = (_lv_trigo_sin(deg + 90) * a);

    r += LV_TRIGO_SIN_MAX / 2;
    return r >> LV_TRIGO_SHIFT;
}

int32_t get_sin(int32_t deg, int32_t a)
{
    int32_t r = _lv_trigo_sin(deg) * a;

    r += LV_TRIGO_SIN_MAX / 2;
    return r >> LV_TRIGO_SHIFT;
}

static lv_design_res_t spectrum_design_cb(lv_obj_t *obj, const lv_area_t *mask, lv_design_mode_t mode)
{
    if (mode == LV_DESIGN_COVER_CHK) {
        return LV_DESIGN_RES_NOT_COVER;
    }

    if (mode == LV_DESIGN_DRAW_POST) {
        lv_opa_t opa = lv_obj_get_style_opa_scale(obj, LV_OBJ_PART_MAIN);

        if (opa < LV_OPA_MIN) {
            return LV_DESIGN_RES_OK;
        }

        lv_point_t poly[4];
        lv_point_t center;
        center.x = obj->coords.x1 + lv_obj_get_width(obj) / 2;
        center.y = obj->coords.y1 + lv_obj_get_height(obj) / 2;

        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);
        draw_dsc.bg_opa = LV_OPA_COVER;

        uint16_t r[64];
        uint32_t i;

        lv_coord_t min_a = 5;
        lv_coord_t r_in = (77 * lv_img_get_zoom(album_img_obj)) >> 8;

        for (i = 0; i < BAR_CNT; i++) {
            r[i] = r_in + min_a;
        }

        uint32_t s;

        for (s = 0; s < 4; s++) {
            uint32_t f;
            uint32_t band_w = 0; /*Real number of bars in this band.*/

            switch (s) {
                case 0:
                    band_w = 20;
                    break;

                case 1:
                    band_w = 8;
                    break;

                case 2:
                    band_w = 4;
                    break;

                case 3:
                    band_w = 2;
                    break;
            }

            /* Add "side bars" with cosine characteristic.*/
            for (f = 0; f < band_w; f++) {
                uint32_t ampl_main = spectrum[spectrum_i][s];
                int32_t ampl_mod = get_cos(f * 360 / band_w + 180, 180) + 180;
                int32_t t = BAR_PER_BAND_CNT * s - band_w / 2 + f;

                if (t < 0) {
                    t = BAR_CNT + t;
                }

                if (t >= BAR_CNT) {
                    t = t - BAR_CNT;
                }

                r[t] += (ampl_main * ampl_mod) >> 9;
            }
        }

        uint32_t amax = 20;
        int32_t animv = spectrum_i - spectrum_lane_ofs_start;

        if (animv > amax) {
            animv = amax;
        }

        for (i = 0; i < BAR_CNT; i++) {
            uint32_t deg_space = 1;
            uint32_t deg = i * DEG_STEP + 90;
            uint32_t j = (i + bar_rot + rnd_array[bar_ofs % 10]) % BAR_CNT;
            uint32_t k = (i + bar_rot + rnd_array[(bar_ofs + 1) % 10]) % BAR_CNT;

            uint32_t v = (r[k] * animv + r[j] * (amax - animv)) / amax;

            if (start_anim) {
                v = r_in + start_anim_values[i];
                deg_space = v >> 7;

                if (deg_space < 1) {
                    deg_space = 1;
                }
            }

            if (v < BAR_COLOR1_STOP) {
                draw_dsc.bg_color = BAR_COLOR1;
            } else if (v > BAR_COLOR3_STOP) {
                draw_dsc.bg_color = BAR_COLOR3;
            } else if (v > BAR_COLOR2_STOP) {
                draw_dsc.bg_color = lv_color_mix(BAR_COLOR3, BAR_COLOR2, ((v - BAR_COLOR2_STOP) * 255) / (BAR_COLOR3_STOP - BAR_COLOR2_STOP));
            } else {
                draw_dsc.bg_color = lv_color_mix(BAR_COLOR2, BAR_COLOR1, ((v - BAR_COLOR1_STOP) * 255) / (BAR_COLOR2_STOP - BAR_COLOR1_STOP));
            }

            uint32_t di = deg + deg_space;

            int32_t x1_out = get_cos(di, v);
            poly[0].x = center.x + x1_out;
            poly[0].y = center.y + get_sin(di, v);

            int32_t x1_in = get_cos(di, r_in);
            poly[1].x = center.x + x1_in;
            poly[1].y = center.y + get_sin(di, r_in);
            di += DEG_STEP - deg_space * 2;

            int32_t x2_in = get_cos(di, r_in);
            poly[2].x = center.x + x2_in;
            poly[2].y = center.y + get_sin(di, r_in);

            int32_t x2_out = get_cos(di, v);
            poly[3].x = center.x + x2_out;
            poly[3].y = center.y + get_sin(di, v);

            lv_draw_polygon(poly, 4, mask, &draw_dsc);

            poly[0].x = center.x - x1_out;
            poly[1].x = center.x - x1_in;
            poly[2].x = center.x - x2_in;
            poly[3].x = center.x - x2_out;
            lv_draw_polygon(poly, 4, mask, &draw_dsc);
        }
    }

    return LV_DESIGN_RES_OK;
}

static void spectrum_anim_cb(void *a, lv_anim_value_t v)
{
    lv_obj_t *obj = a;

    if (start_anim) {
        lv_obj_invalidate(obj);
        return;
    }

    spectrum_i = v;
    lv_obj_invalidate(obj);

    static uint32_t bass_cnt = 0;
    static int32_t last_bass = -1000;
    static int32_t dir = 1;

    if (spectrum[spectrum_i][0] > 12) {
        if (spectrum_i - last_bass > 5) {
            bass_cnt++;
            last_bass = spectrum_i;

            if (bass_cnt >= 2) {
                bass_cnt = 0;
                spectrum_lane_ofs_start = spectrum_i;
                bar_ofs++;
            }
        }
    }

    if (spectrum[spectrum_i][0] < 4) {
        bar_rot += dir;
    }

    lv_img_set_zoom(album_img_obj, LV_IMG_ZOOM_NONE + spectrum[spectrum_i][0]);
}

static void start_anim_cb(void *a, lv_anim_value_t v)
{
    lv_coord_t *av = a;
    *av = v;
    lv_obj_invalidate(spectrum_obj);
}

static lv_obj_t *album_img_create(lv_obj_t *parent)
{
    LV_IMG_DECLARE(img_lv_demo_music_cover_1);
    LV_IMG_DECLARE(img_lv_demo_music_cover_2);
    LV_IMG_DECLARE(img_lv_demo_music_cover_3);

    lv_obj_t *img;
    img = lv_img_create(parent, NULL);

    switch (track_id) {
        case 2:
            lv_img_set_src(img, &img_lv_demo_music_cover_3);
            spectrum = spectrum_3;
            spectrum_len = sizeof(spectrum_1) / sizeof(spectrum_1[0]);
            break;

        case 1:
            lv_img_set_src(img, &img_lv_demo_music_cover_2);
            spectrum = spectrum_2;
            spectrum_len = sizeof(spectrum_2) / sizeof(spectrum_2[0]);
            break;

        case 0:
            lv_img_set_src(img, &img_lv_demo_music_cover_1);
            spectrum = spectrum_1;
            spectrum_len = sizeof(spectrum_3) / sizeof(spectrum_3[0]);
            break;
    }

    lv_img_set_pivot(img, lv_obj_get_width(img) / 2, lv_obj_get_height(img) / 2);
    lv_img_set_antialias(img, false);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(img, album_event_cb);
    lv_obj_set_gesture_parent(img, false);
    lv_obj_set_click(img, true);

    return img;
}

static void album_event_cb(lv_obj_t *img, lv_event_t event)
{
    if (event == LV_EVENT_GESTURE) {
        lv_gesture_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());

        if (dir == LV_GESTURE_DIR_LEFT) {
            _lv_demo_music_album_next(true);
        }

        if (dir == LV_GESTURE_DIR_RIGHT) {
            _lv_demo_music_album_next(false);
        }
    }
}

static void play_event_cb(lv_obj_t *img, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED) {
        if (lv_imgbtn_get_state(img) == LV_BTN_STATE_RELEASED) {
            _lv_demo_music_pause();
        } else {
            _lv_demo_music_resume();
        }
    }
}

static void prev_event_cb(lv_obj_t *img, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED) {
        _lv_demo_music_album_next(false);
    }
}

static void next_event_cb(lv_obj_t *img, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED) {
        _lv_demo_music_album_next(true);
    }
}

static void timer_cb(lv_task_t *t)
{
    time++;
    lv_label_set_text_fmt(time_obj, "%d:%02d", time / 60, time % 60);
    lv_slider_set_value(slider_obj, time, LV_ANIM_ON);
}

static lv_res_t main_cont_signal_cb(lv_obj_t *obj, lv_signal_t signal, void *param)
{
    lv_res_t res;
    res = ancestor_signal_cb(obj, signal, param);

    if (res != LV_RES_OK) {
        return res;
    }

    if (signal == LV_SIGNAL_DRAG_THROW_BEGIN) {
        lv_coord_t y = lv_obj_get_y(obj);
        lv_indev_t *indev = lv_indev_get_act();
        lv_point_t vect;
        lv_indev_get_vect(indev, &vect);
        lv_coord_t y_predict = 0;

        while (vect.y != 0) {
            y_predict += vect.y;
            vect.y = vect.y * (100 - LV_INDEV_DEF_DRAG_THROW) / 100;
        }

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_time(&a, 200);
        lv_indev_finish_drag(lv_indev_get_act());

        if (y + y_predict > -LV_VER_RES / 2) {
            lv_anim_set_values(&a, y, -15);
        } else {
            lv_anim_set_values(&a, y, -lv_obj_get_height(obj) + LV_DEMO_LIST_CTRL_OVERLAP);
        }

        lv_anim_start(&a);
    } else if (signal == LV_SIGNAL_COORD_CHG) {
        lv_coord_t y = lv_obj_get_y(obj);
        lv_coord_t h = lv_obj_get_height(obj);

        if (y > -15) {
            lv_obj_set_y(obj, -15);
        }

        if (obj->coords.y2 < LV_DEMO_LIST_CTRL_OVERLAP) {
            lv_obj_set_y(obj, -h + LV_DEMO_LIST_CTRL_OVERLAP);
        }
    }

    return LV_RES_OK;
}

static void spectrum_end_cb(lv_anim_t *a)
{
    _lv_demo_music_album_next(true);
}

static void stop_start_anim(lv_task_t *task)
{
    start_anim = false;
    spectrum_obj->ext_draw_pad = 0;
}
#endif /*LV_USE_DEMO_MUSIC*/
