/*
 * Model_Config.h
 *
 *  Created on: 25 Jul 2020
 *      Author: Ralim
 */

#ifndef BSP_MINIWARE_MODEL_CONFIG_H_
#define BSP_MINIWARE_MODEL_CONFIG_H_
/*
 * Lookup for mapping features <-> Models
 */

#if defined(MODEL_TS100) + defined(MODEL_TS80)+defined(MODEL_TS80P) > 1
#error "Multiple models defined!"
#elif defined(MODEL_TS100) + defined(MODEL_TS80)+ defined(MODEL_TS80P) == 0
#error "No model defined!"
#endif

#ifdef MODEL_TS100
#define POW_DC
#define ACCEL_MMA
#define ACCEL_LIS
#define TEMP_TMP36
#define BATTFILTERDEPTH 32
#endif

#ifdef MODEL_TS80
#define ACCEL_LIS
#define POW_QC
#define TEMP_TMP36
#define ACCEL_ORI_FLIP
#define OLED_FLIP
#define BATTFILTERDEPTH 8
#endif

#ifdef MODEL_TS80P
#define ACCEL_LIS
#define ACCEL_MSA
#define POW_PD
#define POW_QC
#define TEMP_NTC
#define I2C_SOFT
#define ACCEL_ORI_FLIP
#define OLED_FLIP
#define BATTFILTERDEPTH 8
#endif

#endif /* BSP_MINIWARE_MODEL_CONFIG_H_ */
