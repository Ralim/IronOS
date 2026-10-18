/*
 * Setup.h
 *
 * FNIRSI HS-01 (WCH CH571F)
 */

#ifndef HS01_SETUP_H_
#define HS01_SETUP_H_
#include "CH57x_common.h"
#undef min // CH573SFR.h macros, clash with IronOS functions
#undef max
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
void     hardware_init();
void     I2CInit();
uint16_t getADCVin(uint8_t sample);
// Called from the frame timer interrupt in the measurement window, with the heater off
void sampleADC();
// Sets the requested CH224K output voltage, 9/12/15/20 V
void setCH224KVoltage(uint8_t volts);
// True once Power.cpp has found the voltage the charger delivers
bool CH224KVoltageSettled();
void armMovementInterrupt();
#ifdef __cplusplus
}
#endif

extern const uint8_t holdoffTicks;
extern const uint8_t tempMeasureTicks;
// Length of one frame timer tick, in system clock cycles
#define FRAME_TICK_CYCLES 26000
// TMR0 heater PWM period, 60 MHz / 3000 = 20 kHz (same as the stock firmware)
#define HEATER_PWM_PERIOD 3000
#endif /* HS01_SETUP_H_ */
