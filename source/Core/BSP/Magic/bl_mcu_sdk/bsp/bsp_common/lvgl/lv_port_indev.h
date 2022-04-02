
/**
 * @file lv_port_indev_templ.h
 *
 */

/*Copy this file as "lv_port_indev.h" and set this value to "1" to enable content*/
#if 1

#ifndef LV_PORT_INDEV_TEMPL_H
#define LV_PORT_INDEV_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/
#define KEY_ADC_DIFF_MAX 15
#define DIFF(x, y)       (((x) > (y)) ? ((x) - (y)) : ((y) - (x)))

#define KEY_NOISE_NUM_MAX 2

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**********************
 *  STATIC VARIABLES
 **********************/
extern lv_indev_t *indev_touchpad;
//extern lv_indev_t * indev_mouse;
extern lv_indev_t *indev_keypad;
//extern lv_indev_t * indev_encoder;
//extern lv_indev_t * indev_button;

void lv_port_indev_init(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_PORT_INDEV_TEMPL_H*/

#endif /*Disable/Enable content*/
