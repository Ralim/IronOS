/**
 * @file lv_examples.h
 *
 */

#ifndef LV_EXAMPLES_H
#define LV_EXAMPLES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl.h"
#endif

#if defined(LV_EX_CONF_PATH)
#define __LV_TO_STR_AUX(x) #x
#define __LV_TO_STR(x)     __LV_TO_STR_AUX(x)
#include __LV_TO_STR(LV_EX_CONF_PATH)
#undef __LV_TO_STR_AUX
#undef __LV_TO_STR
#elif defined(LV_EX_CONF_INCLUDE_SIMPLE)
#include "lv_ex_conf.h"
#else
#include "lv_ex_conf.h"
#endif

#include "demo/lv_ex_get_started/lv_ex_get_started.h"
#include "demo/lv_ex_style/lv_ex_style.h"
#include "demo/lv_ex_widgets/lv_ex_widgets.h"
#include "demo/lv_demo_widgets/lv_demo_widgets.h"
#include "demo/lv_demo_benchmark/lv_demo_benchmark.h"
#include "demo/lv_demo_stress/lv_demo_stress.h"
#include "demo/lv_demo_keypad_encoder/lv_demo_keypad_encoder.h"
#include "demo/lv_demo_printer/lv_demo_printer.h"
#include "demo/lv_demo_music/lv_demo_music.h"

/*********************
 *      DEFINES
 *********************/
/*Test  lvgl version*/
#if LV_VERSION_CHECK(7, 8, 1) == 0
#error "lv_examples: Wrong lvgl version"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_EXAMPLES_H*/
