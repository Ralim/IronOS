/*
 * UnitSettings.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#ifndef BSP_MINIWARE_UNITSETTINGS_H_
#define BSP_MINIWARE_UNITSETTINGS_H_
//On the TS80, the LIS accel is mounted backwards
#if defined(MODEL_TS80)+defined(MODEL_TS80P)>0
#define LIS_ORI_FLIP
#endif




#endif /* BSP_MINIWARE_UNITSETTINGS_H_ */
