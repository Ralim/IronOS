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
 *  0 - Deactivated
 *  1 - Lock except boost
 *  2 - Full lock
 */
#define LOCKING_MODE 0 // Default to desactivated for safety

/**
 * OLED Orientation
 *
 */
#define ORIENTATION_MODE           2 // 0: Right 1:Left 2:Automatic (default: auto-rotate via accel)
#define MAX_ORIENTATION_MODE       2 // Right/Left/Auto (QMA6100P accelerometer is fitted)
#define REVERSE_BUTTON_TEMP_CHANGE 0 // 0:Default 1:Reverse - Reverse the plus and minus button assignment for temperature change

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
// retargets every value to the T90 front-end. The tip/Vin scale constants below are reverse-
// engineered from the stock firmware (VOLTAGE_DIV, OP_AMP_GAIN_STAGE and the uV->degC LUT in
// ThermoModel.cpp); the thermal-model constants are first-cut and want a final fit against a
// reference thermometer on hardware (see "TODO calibrate on hardware").

#define VOLTAGE_DIV        370 // Vin divider scaler (factory uses an ~11:1 divider; RE'd from stock firmware)
#define CALIBRATION_OFFSET 200 // Default tip zero offset in uV; trimmed per-unit by the on-device calibration
#define PID_POWER_LIMIT    70  // Sets the max pwm power limit
#define POWER_LIMIT        0   // 0 watts default limit
#define MAX_POWER_LIMIT    70
#define POWER_LIMIT_STEPS  5
#define OP_AMP_GAIN_STAGE  250 // External tip amp gain (~250, the value the T245 uV->degC LUT in ThermoModel.cpp and the factory RE both assume). 536 was a stale placeholder: dividing by 536 instead of 250 halved the computed tip uV, so a hot tip read ~half temperature and the PID drove full power forever.
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
// The GC9 shim rotates the WHOLE framebuffer in software (GC9Display::Transmit reads getRawRotation()).
// So the UI must draw a single, un-rotated layout: getRotation() returns false to the UI under this
// flag, otherwise the per-screen getRotation() branches (mirrored icons, swapped positions) would
// double-apply on top of the framebuffer rotation and the home-screen icons scatter.
#define FRAMEBUFFER_ROTATION

#define POW_PD_EXT         3 /*External PD via CH224Q (not HUB238=1 / FS2711=2); power_check() drives it*/
#define USB_PD_EPR_WATTAGE 0 /*No EPR*/
// No HAS_POWER_DEBUG_MENU: the CH224Q exposes no PDO/source introspection (only a negotiated
// voltage + status byte), so there is no showPDDebug() implementation for POW_PD_EXT==3.
#define TEMP_NTC       // Cold-junction is an NTC on PA2 (ADC ch3)
#define CH224_SOFT_I2C // Software (bit-bang) I2C for the CH224Q PD chip on PB6/PB7
// QST QMA6100P accelerometer shares the soft-I2C bus (PB6/PB7) with the CH224Q.
// I2CBB2 is the soft-I2C class; the accelerometer framework talks to it via ACCEL_I2CBB2.
#define ACCEL_QMA      // Enable the QMA6100P accelerometer driver
#define ACCEL_I2CBB2   // Accelerometer lives on the I2CBB2 soft-I2C bus
#define I2C_SOFT_BUS_2 // Compile the I2CBB2 soft-I2C class
#define FILTER_DISPLAYED_TIP_TEMP 4 // Filtering for GUI display
// Scroll menu help text in the small font: the wide/short GC9 panel turns the default large font into
// an unreadable smear for long descriptions.
#define MENU_DESCRIPTION_SMALL_FONT

// T245 cartridges cap lower than the Core default. Cap the user-selectable setpoint via MAX_TEMP_C/F
// but do NOT define CUSTOM_MAX_TEMP_C: that would also cap TipThermoModel::getTipMaxInC(), which the
// PID setpoint clamp and the thermal-runaway raw-ADC check rely on staying at the ADC-rail ceiling.
// (Tip-disconnect no longer depends on getTipMaxInC: it uses the idle-temperature test in BSP.cpp.)
#define MAX_TEMP_C 400 // Max soldering temp selectable degC (T245) // TODO calibrate on hardware
#define MAX_TEMP_F 750 // Max soldering temp selectable degF (T245) // TODO calibrate on hardware

#define MODEL_HAS_DCDC // No DC/DC but very fast PWM that gets us roughly the same place
#endif                 /* T90 */

// Flash layout. Two regions of the 128K die are touched by the Alientek HID bootloader on every
// power-up and must be avoided for runtime persistence (both verified on hardware by writing a
// sentinel and reading it back after a power cycle):
//   - the very top (~0x0801C954..0x08020000): bootloader / factory metadata, wiped on power-up.
//   - the single page at 0x08018000 (the first page just past the linker FLASH region): the
//     bootloader uses it as its application-info page and zeroes it on power-up. A settings struct
//     written there reads back fine in-session but comes up mostly 0x00 after a power cycle, which
//     made loadSettings reset to defaults every boot.
// Pages from 0x08018800 up to the metadata region were all verified to retain across power cycles.
// Place the settings + logo pages there, clear of both danger zones. The linker FLASH region is
// 0x08005000..0x08018000 so app code can never reach them.
//   settings page: 0x0801B000..0x0801B800 (2K)  -- flash_save_buffer erases only this page
//   logo page:     0x0801B800..0x0801C000 (2K)
#define SETTINGS_START_PAGE (0x08000000 + (108 * 1024)) // 0x0801B000, dedicated 2K settings page
#define FLASH_LOGOADDR      (0x08000000 + (110 * 1024)) // 0x0801B800, dedicated 2K logo page

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
