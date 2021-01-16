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

#if defined(MODEL_Pinecil) == 0
#error "No model defined!"
#endif

#ifdef MODEL_Pinecil
#define POW_PD
#define POW_QC
#define POW_DC
#define POW_QC_20V
#define ENABLE_QC2
#define TEMP_TMP36
#define ACCEL_BMA
#define ACCEL_SC7
#define HALL_SENSOR
#define HALL_SI7210
#define BATTFILTERDEPTH 32
#endif

#endif /* BSP_MINIWARE_MODEL_CONFIG_H_ */
