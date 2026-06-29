/*
 * Power.h
 *
 *  CH224Q USB-PD accessors for the Alientek T90 (Nations N32L40x).
 *  Implemented in Power.cpp; consumed by BSP.cpp (preStartChecks) and preRTOS.cpp.
 */

#ifndef BSP_ALIENTEK_T90_POWER_H_
#define BSP_ALIENTEK_T90_POWER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Request USB_PD_VMAX from the CH224Q. Call after the bit-bang I2C pins are configured.
void ch224q_init(void);

// Negotiated source voltage in volts.
uint16_t ch224q_source_voltage(void);

// Source current capability, in centi-amps (x100).
uint16_t ch224q_source_currentX100(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_ALIENTEK_T90_POWER_H_ */
