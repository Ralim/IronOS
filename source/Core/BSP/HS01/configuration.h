#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_
#include <stdint.h>
/**
 * Configuration.h
 * Define here your default pre settings for the FNIRSI HS-01
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
#define DEFAULT_BRIGHTNESS 26  // default OLED brightness

/**
 * Blink the temperature on the cooling screen when its > 50C
 */
#define COOLING_TEMP_BLINK 0 // 0: Disable 1: Enable

/**
 * How many seconds/minutes we wait until going to sleep/shutdown.
 * Values 1-5 are x10 seconds, 6+ are (value - 5) minutes
 */
#define SLEEP_TIME    10 // 5 minutes: the ball switch gives motion pulses only while the iron moves
#define SHUTDOWN_TIME 10 // Minutes

/**
 * Auto start off for safety.
 * Possible values are:
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
#define LOCKING_MODE 0 // Default to deactivated for safety

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

#define THERMAL_RUNAWAY_TIME_SEC 20
#define THERMAL_RUNAWAY_TEMP_C   3

#define CUT_OUT_SETTING          0  // default to no cut-off voltage
#define RECOM_VOL_CELL           33 // Minimum voltage per cell (Recommended 3.3V (33))
#define TEMPERATURE_INF          0  // default to 0
#define DESCRIPTION_SCROLL_SPEED 0  // 0: Slow 1: Fast - default to slow
#define ANIMATION_LOOP           1  // 0: off 1: on
#define ANIMATION_SPEED          settingOffSpeed_t::MEDIUM

// 12-bit ADC, 0..2.1 V range, 8 samples summed. Tip "uV" are uV at the ADC pin, see ThermoModel.cpp
#define ADC_MAX_READING (4096 * 8) // Maximum reading of the adc
#define ADC_VDD_MV      2100       // ADC max reading millivolts

#define VOLTAGE_DIV 567 // 1:11 divider: 131072 / (2.1 V * 11 * 10)

#ifndef MODEL_HS01
#error "No model defined!"
#endif

#define SOLDERING_TEMP         320 // Default soldering temp is 320.0 °C
#define CALIBRATION_OFFSET     0   // Default adc offset in uV at the ADC pin
#define MIN_CALIBRATION_OFFSET 0   // Min value for calibration
#define PID_POWER_LIMIT        65  // Sets the max pwm power limit
#define POWER_LIMIT            0   // 0 watts default limit
#define MAX_POWER_LIMIT        65  //
#define POWER_LIMIT_STEPS      5   //
#define OP_AMP_GAIN_STAGE      1   // Op-amp gain is folded into the vendor table
#define TEMP_uV_LOOKUP_HS01        // Vendor calibration table
#define PID_TIM_HZ             (8) // Tick rate of the PID loop
#define MAX_TEMP_C             420 // Max soldering temp selectable °C, same as the stock firmware
#define MAX_TEMP_F             790 // Max soldering temp selectable °F
#define MIN_TEMP_C             10  // Min soldering temp selectable °C
#define MIN_TEMP_F             50  // Min soldering temp selectable °F
#define MIN_BOOST_TEMP_C       250 // The min settable temp for boost mode °C
#define MIN_BOOST_TEMP_F       480 // The min settable temp for boost mode °F

#define OLED_128x32 1
// Panel timings from the stock firmware; the generic values cause flicker on this panel
#define OLED_CHARGE_PUMP 0x15
#define OLED_PRECHARGE   0x22
#define OLED_VCOMH       0x20
#define OLED_INTERNAL_IREF
#define OLED_TRANSITIONS_INSTANT // Bit-banged I2C is too slow for the slide animations
// The CH224K negotiates PD on its own, so to IronOS this is a DC input (see Power.cpp)
#define POW_DC             1
#define DC_MIN_VOLTAGE_X10 75 // Same cut-off as the stock firmware; the 9 V PD level can read less than 9.0 V
#define GPIO_VIBRATION        // Ball/vibration switch, no accelerometer
#define NEEDS_VBUS_PROBE   0
#define USB_PD_TIMEOUT     0 // No PD stack (setting unused)

#define HARDWARE_MAX_WATTAGE_X10 650
#define TIP_THERMAL_MASS         65 // X10 watts to raise 1 deg C in 1 second
// PI control with the stock firmware gains (Kp 35, Ki 1.75 per 100 ms, duty 0..3000 at 64 W)
#define TIP_CONTROL_PID
#define TIP_PID_KP                   7
#define TIP_PID_KI                   373
#define TIP_PID_KD                   0
#define TIP_PID_INTEGRAL_LIMIT_SCALE 100 // The integral can supply full power, as in the stock firmware
#define TIP_PID_INTEGRAL_RESET_C     50  // Clear the integral when the error is more than 50 C, as in the stock firmware
#define TIP_PID_LOW_TEMP_C           300 // Below this set point, use the stock low gains (Kp 17, Ki 0.9); full gains oscillate here
#define TIP_PID_LOW_KP               4
#define TIP_PID_LOW_KI               192
#define TIP_RESISTANCE               62 // x10 ohms, the iron is rated 65 W at 20 V
#define CANT_DIRECT_READ_SETTINGS       // Data flash is not memory mapped

#define FILTER_DISPLAYED_TIP_TEMP 4 // Filtering for GUI display

// Last 4K of the code flash; settings live in the data flash
#define FLASH_LOGOADDR (188 * 1024)

#endif /* CONFIGURATION_H_ */
