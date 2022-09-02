#include "BSP_Flash.h"
#include "BSP_PD.h"
#include "BSP_Power.h"
#include "BSP_QC.h"
#include "Defines.h"
#include "configuration.h"
#include <stdbool.h>
#include <stdint.h>
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

// maximum htim2 PWM value
extern const uint16_t powerPWM;
// htim2.Init.Period, the full PWM cycle
extern uint16_t totalPWM;

// Called first thing in main() to init the hardware
void preRToSInit();
// Called once the RToS has started for any extra work
void postRToSInit();

// Called once from preRToSInit()
void BSPInit(void);

// Called to reset the hardware watchdog unit
void resetWatchdog();
// Accepts a output level of 0.. to use to control the tip output PWM
void setTipPWM(const uint8_t pulse, const bool shouldUseFastModePWM);
// Returns the Handle temp in C, X10
uint16_t getHandleTemperature(uint8_t sample);
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
void showBootLogoIfavailable();
// delay wrapper for delay using the hardware timer (used before RTOS)
void delay_ms(uint16_t count);
// Probe if the Hall sensor is fitted to the unit
bool getHallSensorFitted();
// If the iron has a hall effect sensor in the handle, return an signed count of the reading
// If the sensor is single polarity (or polarity insensitive) just return 0..32768
int16_t getRawHallEffect();

// Returns true if power is from dumb "DC" input rather than "smart" QC or PD
bool getIsPoweredByDCIN();

// Logs the system state to a debug interface if supported
void log_system_state(int32_t PWMWattsx10);

// Returns true if the tip is disconnected
bool isTipDisconnected();

// Return hardware unique ID if possible
uint64_t getDeviceID();

// If device has burned in validation code's, return the code
uint32_t getDeviceValidation();
// If device validation passes returns 0
uint8_t getDeviceValidationStatus();

// Status LED controls

enum StatusLED {
  LED_OFF = 0,           // Turn off status led
  LED_STANDBY,           // unit is in sleep /standby
  LED_HEATING,           // The unit is heating up to temperature
  LED_HOT,               // The unit is at operating temperature
  LED_COOLING_STILL_HOT, // The unit is off and cooling but still hot
  LED_UNKNOWN,           //
};
void setStatusLED(const enum StatusLED state);

// preStartChecks are run until they return 0
// By the PID, after each ADC sample comes in
// For example, on the MHP30 this is used to figure out the resistance of the hotplate
uint8_t preStartChecks();
uint8_t preStartChecksDone();
#ifdef __cplusplus
}
#endif
#endif /* BSP_BSP_H_ */
