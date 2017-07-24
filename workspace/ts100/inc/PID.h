/*
 * PID.h
 *
 *  Created on: 20 Sep 2016
 *      Author: ralim
 *
 *      Functions for computing the PID for the iron temp
 */

#ifndef PID_H_
#define PID_H_
#include "Analog.h"
#include "Interrupt.h"

typedef struct {
	uint32_t kp, ki, kd; //PID values
} pidSettingsType;

int32_t computePID(uint16_t setpoint);
void setupPID(void);
#endif /* PID_H_ */
