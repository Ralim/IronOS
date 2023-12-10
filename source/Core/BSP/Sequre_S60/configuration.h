#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_
#include "Settings.h"
#include <stdint.h>
/**
 * Configuration.h
 * Define here your default pre settings for S60
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
#define MIN_BRIGHTNESS     0   // Min OLED brightness selectable
#define MAX_BRIGHTNESS     100 // Max OLED brightness selectable
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
#define ORIENTATION_MODE           0 // 0: Right 1:Left 2:Automatic - Default Automatic
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
#define THERMAL_RUNAWAY_TEMP_C   10

#define CUT_OUT_SETTING          0  // default to no cut-off voltage
#define RECOM_VOL_CELL           33 // Minimum voltage per cell (Recommended 3.3V (33))
#define TEMPERATURE_INF          0  // default to 0
#define DESCRIPTION_SCROLL_SPEED 0  // 0: Slow 1: Fast - default to slow
#define ANIMATION_LOOP           1  // 0: off 1: on
#define ANIMATION_SPEED          settingOffSpeed_t::MEDIUM

// Op-amp gain
// First stage has a gain of 10.31, followed by gain of 52; so total gain is 536

#define ADC_MAX_READING (4096 * 8) // Maximum reading of the adc
#define ADC_VDD_MV      3300       // ADC max reading millivolts

// Deriving the Voltage div:
// Vin_max = (3.3*(r1+r2))/(r2)
// vdiv = (32768*4)/(vin_max*10)

#if defined(MODEL_S60) == 0
#error "No model defined!"
#endif

#define NEEDS_VBUS_PROBE 0

#define MIN_CALIBRATION_OFFSET 100 // Min value for calibration
#define SOLDERING_TEMP         320 // Default soldering temp is 320.0 °C
#define PID_TIM_HZ             (8) // Tick rate of the PID loop
#define MAX_TEMP_C             450 // Max soldering temp selectable °C
#define MAX_TEMP_F             850 // Max soldering temp selectable °F
#define MIN_TEMP_C             10  // Min soldering temp selectable °C
#define MIN_TEMP_F             60  // Min soldering temp selectable °F
#define MIN_BOOST_TEMP_C       250 // The min settable temp for boost mode °C
#define MIN_BOOST_TEMP_F       480 // The min settable temp for boost mode °F

#ifdef MODEL_S60
#define VOLTAGE_DIV        460 // Default divider scaler
#define CALIBRATION_OFFSET 200 // Default adc offset in uV
#define PID_POWER_LIMIT    70  // Sets the max pwm power limit
#define POWER_LIMIT        0   // 0 watts default limit
#define MAX_POWER_LIMIT    70
#define POWER_LIMIT_STEPS  5
#define OP_AMP_GAIN_STAGE  536
#define TEMP_uV_LOOKUP_S60
#define USB_PD_VMAX 12 // Maximum voltage for PD to negotiate

#define HARDWARE_MAX_WATTAGE_X10 600

#define TIP_THERMAL_MASS    8   // X10 watts to raise 1 deg C in 1 second
#define TIP_THERMAL_INERTIA 128 // We use a large inertia value to smooth out the drive to the tip since its stupidly sensitive

#define TIP_RESISTANCE 20 //(actually 2.5 ish but we need to be more conservative on pwm'ing watt limit) x10 ohms

#define OLED_128x32
#define GPIO_VIBRATION
#define POW_PD_EXT                1
#define USB_PD_EPR_WATTAGE        0 /*No EPR*/
#define DEBUG_POWER_MENU_BUTTON_B 1
#define HAS_POWER_DEBUG_MENU
#define TEMP_NTC
#define I2C_SOFT_BUS_2 // For now we are doing software I2C to get around hardware chip issues
#define OLED_I2CBB2

#define MODEL_HAS_DCDC // We dont have DC/DC but have reallly fast PWM that gets us roughly the same place
#endif                 /* S60 */

#define FLASH_LOGOADDR      (0x08000000 + (62 * 1024))
#define SETTINGS_START_PAGE (0x08000000 + (63 * 1024))

#endif /* CONFIGURATION_H_ */
