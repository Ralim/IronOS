/**
 * @file lv_demo_music.h
 *
 */

#ifndef LV_DEMO_MUSIC_H
#define LV_DEMO_MUSIC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_examples.h"

#if LV_USE_DEMO_MUSIC

/*********************
 *      DEFINES
 *********************/
#define LV_DEMO_MUSIC_LANDSCAPE   (LV_HOR_RES_MAX > LV_VER_RES_MAX ? 1 : 0)
#define LV_DEMO_LIST_CTRL_OVERLAP 60

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_demo_music(void);
const char *_lv_demo_music_get_title(uint32_t track_id);
const char *_lv_demo_music_get_artist(uint32_t track_id);
const char *_lv_demo_music_get_genre(uint32_t track_id);
uint32_t _lv_demo_music_get_track_length(uint32_t track_id);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_MUSIC*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_MUSIC_H*/
