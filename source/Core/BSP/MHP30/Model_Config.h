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

#ifndef MODEL_MHP30
#error "No model defined!"
#endif

#ifdef MODEL_MHP30
#define ACCEL_LIS
#define ACCEL_MSA
#define POW_PD
//#define POW_QC // Unsure if we have this
#define TEMP_NTC
#define I2C_SOFT
#define LIS_ORI_FLIP
#define OLED_FLIP
#define BATTFILTERDEPTH 8
#endif

#endif /* BSP_MINIWARE_MODEL_CONFIG_H_ */
