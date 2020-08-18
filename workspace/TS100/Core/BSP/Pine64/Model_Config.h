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

#if defined(MODEL_Pinecil) > 1
#error "Multiple models defined!"
#elif defined(MODEL_Pinecil) == 0
#error "No model defined!"
#endif

#ifdef MODEL_Pinecil
#define ACCEL_LIS
#define TEMP_TMP36
#define POW_QC
// #define POW_PD
#endif

#endif /* BSP_MINIWARE_MODEL_CONFIG_H_ */
