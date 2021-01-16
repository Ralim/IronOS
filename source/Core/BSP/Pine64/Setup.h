/*
 * Setup.h
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef SETUP_H_
#define SETUP_H_
#include "gd32vf103_libopt.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
uint16_t getADC(uint8_t channel);
void hardware_init();
void setupFUSBIRQ();
#ifdef __cplusplus
}
#endif
extern const uint8_t holdoffTicks;
extern const uint8_t tempMeasureTicks;
#endif /* SETUP_H_ */
