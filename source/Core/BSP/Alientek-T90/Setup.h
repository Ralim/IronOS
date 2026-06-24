/*
 * Setup.h
 *
 *  Core low-level bring-up for the Alientek T90 (Nations N32L40x).
 *  Re-implemented against the Nations std-periph driver (no ST HAL).
 */

#ifndef SETUP_H_
#define SETUP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "n32l40x.h"
#include <stdint.h>

// Bring up clocks, GPIO, ADC (injected tip group + regular Vin/NTC/current),
// TIM2 heater PWM carrier, TIM4 ADC-schedule timer, NVIC and the watchdog.
void Setup_HAL(void);

// ADC read helpers (mirror the Sequre Setup.cpp logic on the N32 single ADC).
uint16_t getADCVin(uint8_t sample);        // averaged input-supply reading
uint16_t getADCHandleTemp(uint8_t sample); // averaged cold-junction NTC reading
uint16_t getTipRawTemp(uint8_t refresh);   // summed 4 injected tip ranks, <<1, filtered

#ifdef __cplusplus
}
#endif

#endif /* SETUP_H_ */
