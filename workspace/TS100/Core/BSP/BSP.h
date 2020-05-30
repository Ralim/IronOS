#include "BSP_Flash.h"
#include "BSP_Power.h"
#include "BSP_QC.h"
#include "Defines.h"
#include "UnitSettings.h"
#include "stdint.h"
/*
 * BSP.h -- Board Support
 *
 * This exposes functions that are expected to be implemented to add support for different hardware
 */

#ifndef BSP_BSP_H_
#define BSP_BSP_H_
#ifdef __cplusplus
extern "C" {
#endif

// Called first thing in main() to init the hardware
void preRToSInit();
// Called once the RToS has started for any extra work
void postRToSInit();

// Called to reset the hardware watchdog unit
void resetWatchdog();
// Accepts a output level of 0.. to use to control the tip output PWM
void setTipPWM(uint8_t pulse);
// Returns the Handle temp in C, X10
uint16_t getHandleTemperature();
// Returns the Tip temperature ADC reading in raw units
uint16_t getTipRawTemp(uint8_t refresh);
// Returns the main DC input voltage, using the adjustable divisor + sample flag
uint16_t getInputVoltageX10(uint16_t divisor, uint8_t sample);

// Readers for the two buttons
// !! Returns 1 if held down, 0 if released
uint8_t getButtonA();
uint8_t getButtonB();

// This is a work around that will be called if I2C starts to bug out
// This should toggle the SCL line until SDA goes high to end the current transaction
void unstick_I2C();

// Reboot the IC when things go seriously wrong
void reboot();

// If the user has programmed in a bootup logo, draw it to the screen from flash
// Returns 1 if the logo was printed so that the unit waits for the timeout or button
uint8_t showBootLogoIfavailable();

void delay_ms(uint16_t count);
#ifdef __cplusplus
}
#endif
#endif /* BSP_BSP_H_ */
