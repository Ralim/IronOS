/*
 * Setup.h
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef PINE_SETUP_H_
#define PINE_SETUP_H_
#include <stdint.h>
extern "C" {
#include "bflb_platform.h"
#include "bl702_adc.h"
#include "bl702_common.h"
#include "bl702_glb.h"
#include "bl702_i2c.h"
#include "bl702_pwm.h"
#include "bl702_timer.h"
#include "hal_adc.h"
}
#ifdef __cplusplus
extern "C" {
#endif

uint16_t getADC(uint8_t channel);
void     hardware_init();
uint16_t getADCHandleTemp(uint8_t sample);
uint16_t getADCVin(uint8_t sample);
#ifdef __cplusplus
}
#endif
void                 setupFUSBIRQ();
extern const uint8_t holdoffTicks;
extern const uint8_t tempMeasureTicks;
#endif /* PINE_SETUP_H_ */
