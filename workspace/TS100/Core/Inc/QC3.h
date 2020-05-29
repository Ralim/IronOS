/*
 * QC3.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#ifndef INC_QC3_H_
#define INC_QC3_H_
#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif
void seekQC(int16_t Vx10, uint16_t divisor);
void startQC(uint16_t divisor); // Tries to negotiate QC for highest voltage, must be run after

#ifdef __cplusplus
}
#endif

#endif /* INC_QC3_H_ */
