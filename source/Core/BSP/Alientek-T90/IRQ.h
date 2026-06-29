/*
 * IRQ.h
 *
 *  Alientek T90 (Nations N32L40x) interrupt wiring.
 */

#ifndef BSP_ALIENTEK_T90_IRQ_H_
#define BSP_ALIENTEK_T90_IRQ_H_

#include "BSP.h"
#include "Setup.h"
#include "n32l40x.h"
#include <stdint.h>

// Heater PWM safety: decremented in the heater-schedule timer update IRQ so the tip
// PWM is forced off if the PID task stops scheduling. Defined in BSP.cpp.
extern volatile uint16_t PWMSafetyTimer;
// Pending heater duty (0..powerPWM) written by setTipPWM, applied in the timer IRQ. BSP.cpp.
extern volatile uint8_t pendingPWM;

// getFUS302IRQLow() is declared in BSP_PD.h (C++ linkage) and defined in IRQ.cpp.

#endif /* BSP_ALIENTEK_T90_IRQ_H_ */
