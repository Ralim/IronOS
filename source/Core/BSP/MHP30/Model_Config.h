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
#define ACCEL_MSA
#define POW_PD
#define TEMP_NTC
#define I2C_SOFT
#define BATTFILTERDEPTH 8
#define OLED_I2CBB
#define ACCEL_EXITS_ON_MOVEMENT
#endif
#ifdef ACCEL_EXITS_ON_MOVEMENT
#define NO_SLEEP_MODE
#endif
#endif /* BSP_MINIWARE_MODEL_CONFIG_H_ */
