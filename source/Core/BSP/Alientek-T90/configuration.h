#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_
#include <stdint.h>
/**
 * Configuration.h
 * Define here your default pre settings for the Alientek T90 (MODEL_T90)
 *
 */

//===========================================================================
//============================= Default Settings ============================
//===========================================================================
/**
 * Default soldering temp is 320.0 C
 * Temperature the iron sleeps at - default 150.0 C
 */

#define SLEEP_TEMP         150 // Default sleep temperature
#define BOOST_TEMP         420 // Default boost temp.
#define BOOST_MODE_ENABLED 1   // 0: Disable 1: Enable

/**
 * OLED Brightness
 *
 */
#define MIN_BRIGHTNESS     1   // Min OLED brightness selectable
#define MAX_BRIGHTNESS     101 // Max OLED brightness selectable
#define BRIGHTNESS_STEP    25  // OLED brightness increment
#define DEFAULT_BRIGHTNESS 25  // default OLED brightness

/**
 * Blink the temperature on the cooling screen when its > 50C
 */
#define COOLING_TEMP_BLINK 0 // 0: Disable 1: Enable

/**
 * How many seconds/minutes we wait until going to sleep/shutdown.
 * Values -> SLEEP_TIME * 10; i.e. 5*10 = 50 Seconds!
 */
#define SLEEP_TIME    5  // x10 Seconds
#define SHUTDOWN_TIME 10 // Minutes

/**
 * Auto start off for safety.
 * Pissible values are:
 *  0 - none
 *  1 - Soldering Temperature
 *  2 - Sleep Temperature
 *  3 - Sleep Off Temperature
 */
#define AUTO_START_MODE 0 // Default to none

/**
 * Locking Mode
 * When in soldering mode a long press on both keys toggle the lock of the buttons
 * Possible values are:
 *  0 - Desactivated
 *  1 - Lock except boost
 *  2 - Full lock
 */
#define LOCKING_MODE 0 // Default to desactivated for safety

/**
 * OLED Orientation
 *
 */
#define ORIENTATION_MODE           0 // 0: Right 1:Left (2:Automatic N/A)
#define MAX_ORIENTATION_MODE       1 // Disable auto mode
#define REVERSE_BUTTON_TEMP_CHANGE 0 // 0:Default 1:Reverse - Reverse the plus and minus button assigment for temperature change

/**
 * Temp change settings
 */
#define TEMP_CHANGE_SHORT_STEP     1  // Default temp change short step +1
#define TEMP_CHANGE_LONG_STEP      10 // Default temp change long step +10
#define TEMP_CHANGE_SHORT_STEP_MAX 50 // Temp change short step MAX value
#define TEMP_CHANGE_LONG_STEP_MAX  90 // Temp change long step MAX value

/* Power pulse for keeping power banks awake*/
#define POWER_PULSE_INCREMENT    1
#define POWER_PULSE_MAX          100 // x10 max watts
#define POWER_PULSE_WAIT_MAX     9   // 9*2.5s = 22.5 seconds
#define POWER_PULSE_DURATION_MAX 9   // 9*250ms = 2.25 seconds

#define POWER_PULSE_DEFAULT          0
#define POWER_PULSE_WAIT_DEFAULT     4 // Default rate of the power pulse: 4*2500 = 10000 ms = 10 s
#define POWER_PULSE_DURATION_DEFAULT 1 // Default duration of the power pulse: 1*250 = 250 ms

/**
 * OLED Orientation Sensitivity on Automatic mode!
 * Motion Sensitivity <0=Off 1=Least Sensitive 9=Most Sensitive>
 */
#define SENSITIVITY 7 // Default 7

/**
 * Detailed soldering screen
 * Detailed idle screen (off for first time users)
 */
#define DETAILED_SOLDERING 0 // 0: Disable 1: Enable - Default 0
#define DETAILED_IDLE      0 // 0: Disable 1: Enable - Default 0

#define CUT_OUT_SETTING          0  // default to no cut-off voltage
#define RECOM_VOL_CELL           33 // Minimum voltage per cell (Recommended 3.3V (33))
#define TEMPERATURE_INF          0  // default to 0
#define DESCRIPTION_SCROLL_SPEED 0  // 0: Slow 1: Fast - default to slow
#define ANIMATION_LOOP           1  // 0: off 1: on
#define ANIMATION_SPEED          settingOffSpeed_t::MEDIUM

// Op-amp gain
// The tip thermocouple is amplified by an external PCB op-amp into PA4 (ADC ch5).
// There is no on-chip PGA in the stock firmware; OP_AMP_GAIN_STAGE is the external gain.

#define ADC_MAX_READING (4096 * 8) // Maximum reading of the adc
#define ADC_VDD_MV      3300       // ADC max reading millivolts

// Deriving the Voltage div:
// Vin_max = (3.3*(r1+r2))/(r2)
// vdiv = (32768*4)/(vin_max*10)

#if defined(MODEL_T90) == 0
#error "No model defined!"
#endif

#define NEEDS_VBUS_PROBE 0

#ifdef MODEL_T90
// Alientek T90: Nations N32L40x (Cortex-M4F), T245 cartridge, GC9-family color SPI LCD,
// CH224Q USB-PD sink over software I2C. Mirrors the Sequre S60 std-periph structure but
// retargets every value to the T90 front-end. Analog constants below are placeholders that
// compile and must be re-fitted on hardware (see "TODO calibrate on hardware").

#define VOLTAGE_DIV        467 // Default divider scaler // TODO calibrate on hardware (Vin ~10:1 divider)
#define CALIBRATION_OFFSET 200 // Default adc offset in uV // TODO calibrate on hardware (per-unit thermocouple zero)
#define PID_POWER_LIMIT    70  // Sets the max pwm power limit
#define POWER_LIMIT        0   // 0 watts default limit
#define MAX_POWER_LIMIT    70
#define POWER_LIMIT_STEPS  5
#define OP_AMP_GAIN_STAGE  536 // External tip amp gain // TODO calibrate on hardware (dominant temperature scale)
#define TEMP_uV_LOOKUP_T245    // Selects the T245 uV->degC curve in ThermoModel.cpp
#define USB_PD_VMAX              20 // Maximum voltage for PD to negotiate (CH224Q supports up to 28V)
#define THERMAL_RUNAWAY_TIME_SEC 20
#define THERMAL_RUNAWAY_TEMP_C   3

#define HARDWARE_MAX_WATTAGE_X10 1000 // x10 watts physical ceiling // TODO calibrate on hardware

#define TIP_THERMAL_MASS    65  // X10 watts to raise 1 deg C in 1 second // TODO calibrate on hardware (T245 cartridge)
#define TIP_THERMAL_INERTIA 128 // Large inertia to smooth the drive to the tip // TODO calibrate on hardware

#define TIP_RESISTANCE 25 // x10 ohms (T245 element ~2.5 ohm, conservative) // TODO calibrate on hardware. Never 0.

// Display: GC9-family RGB565 color LCD over hardware SPI (GC9Display shim, selected in OLED.hpp).
// OLED_128x32 sizes the mono framebuffer the shim expands to RGB565. Do NOT define OLED_I2CBB*/OLED_96x16.
#define OLED_128x32
#define OLED_GC9D01

#define POW_PD_EXT         3 /*External PD via CH224Q (not HUB238=1 / FS2711=2); power_check() drives it*/
#define USB_PD_EPR_WATTAGE 0 /*No EPR*/
// No HAS_POWER_DEBUG_MENU: the CH224Q exposes no PDO/source introspection (only a negotiated
// voltage + status byte), so there is no showPDDebug() implementation for POW_PD_EXT==3.
#define TEMP_NTC       // Cold-junction is an NTC on PA3
#define NO_ACCEL       // No accelerometer/movement sensor on the T90 (PA13/PA14 are a model strap)
#define CH224_SOFT_I2C // Software (bit-bang) I2C for the CH224Q PD chip on PB6/PB7
#define FILTER_DISPLAYED_TIP_TEMP 4 // Filtering for GUI display

// T245 cartridges cap lower than the Core default. Cap the user-selectable setpoint via MAX_TEMP_C/F
// but do NOT define CUSTOM_MAX_TEMP_C: that would also cap TipThermoModel::getTipMaxInC(), which
// isTipDisconnected() uses (max-5) to detect a railed/floating thermocouple. With a 400C cap that
// detection would false-trigger at a 400C setpoint and report "no tip". Let getTipMaxInC() stay at
// the ADC-rail ceiling so disconnect detection works; the 400C setpoint cap is enforced separately.
#define MAX_TEMP_C 400 // Max soldering temp selectable degC (T245) // TODO calibrate on hardware
#define MAX_TEMP_F 750 // Max soldering temp selectable degF (T245) // TODO calibrate on hardware

#define MODEL_HAS_DCDC // No DC/DC but very fast PWM that gets us roughly the same place
#endif                 /* T90 */

// Flash layout: app FLASH is 0x08005000..0x0801F000 (104K). The top two 2K sectors are reserved as
// SEPARATE pages so erasing settings never wipes the logo (N32 erases a whole 2K page at a time):
//   settings page: 0x0801F000..0x0801F800 (2K)  -- flash_save_buffer erases only this page
//   logo page:     0x0801F800..0x08020000 (2K)
#define SETTINGS_START_PAGE (0x08000000 + (124 * 1024)) // 0x0801F000, dedicated 2K settings page
#define FLASH_LOGOADDR      (0x08000000 + (126 * 1024)) // 0x0801F800, dedicated 2K logo page

// Defaults

#ifndef MIN_CALIBRATION_OFFSET
#define MIN_CALIBRATION_OFFSET 100 // Min value for calibration
#endif
#ifndef SOLDERING_TEMP
#define SOLDERING_TEMP 320 // Default soldering temp is 320.0 °C
#endif
#ifndef PID_TIM_HZ
#define PID_TIM_HZ (8) // Tick rate of the PID loop
#endif
#ifndef MAX_TEMP_C
#define MAX_TEMP_C 450 // Max soldering temp selectable °C
#endif
#ifndef MAX_TEMP_F
#define MAX_TEMP_F 850 // Max soldering temp selectable °F
#endif
#ifndef MIN_TEMP_C
#define MIN_TEMP_C 10 // Min soldering temp selectable °C
#endif
#ifndef MIN_TEMP_F
#define MIN_TEMP_F 60 // Min soldering temp selectable °F
#endif
#ifndef MIN_BOOST_TEMP_C
#define MIN_BOOST_TEMP_C 250 // The min settable temp for boost mode °C
#endif
#ifndef MIN_BOOST_TEMP_F
#define MIN_BOOST_TEMP_F 480 // The min settable temp for boost mode °F
#endif

#endif /* CONFIGURATION_H_ */
