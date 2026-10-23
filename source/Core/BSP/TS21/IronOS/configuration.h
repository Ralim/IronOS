#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#ifndef MODEL_TS21
#error "This configuration is only for the experimental TS21 target"
#endif

#ifdef TS21_HEATER_TEST
#include "ts21.h"
// Reuse the SEQURE S60/S60P C210 controller parameters, not its ADC/PWM calibration.
#define TIP_THERMAL_MASS    10
#define TIP_THERMAL_INERTIA 128
#define BSP_HAS_POWER_READBACK
#else
#define HEATER_DISABLED 1
#define TIP_THERMAL_MASS    0
#define TIP_THERMAL_INERTIA 0
#endif
#define TEMP_DIRECT_ADC 1
#define CUSTOM_MAX_TEMP_C
#define WORKLIGHT_SETTING 1
#define NO_ACCEL 1
#define NO_SLEEP_MODE
#define OLED_128x32
#define OLED_I2C_PER_BYTE_TRANSFERS
#define OLED_128x32_COM_SEQUENTIAL
#define OLED_DISPLAY_OFFSET_QUIRK    1
#define OLED_MEMORY_MODE             2
#define ADC_MAX_READING              4096
#define PID_TIM_HZ                   8
#ifdef TS21_HEATER_TEST
#define SOLDERING_TEMP TS21_DEFAULT_TEMP_C
#define BOOST_TEMP     0
#else
#define SOLDERING_TEMP               320
#define BOOST_TEMP                   420
#endif
#define SLEEP_TEMP                   150
#define AUTO_START_MODE              0
#define LOCKING_MODE                 0
#define ORIENTATION_MODE             0
#define MAX_ORIENTATION_MODE         1
#define REVERSE_BUTTON_TEMP_CHANGE   0
#define MIN_BRIGHTNESS               0
#define MAX_BRIGHTNESS               100
#define BRIGHTNESS_STEP              25
#define DEFAULT_BRIGHTNESS           25
#define TEMP_CHANGE_SHORT_STEP       1
#define TEMP_CHANGE_LONG_STEP        10
#define TEMP_CHANGE_SHORT_STEP_MAX   50
#define TEMP_CHANGE_LONG_STEP_MAX    90
#define POWER_PULSE_DEFAULT          0
#define POWER_PULSE_WAIT_DEFAULT     4
#define POWER_PULSE_DURATION_DEFAULT 1
#define POWER_PULSE_WAIT_MAX         9
#define POWER_PULSE_DURATION_MAX     9
#define SENSITIVITY                  0
#define SLEEP_TIME                   5
#define SHUTDOWN_TIME                10
#define COOLING_TEMP_BLINK           0
#define DETAILED_SOLDERING           0
#define DETAILED_IDLE                0
#define THERMAL_RUNAWAY_TIME_SEC     20
#define THERMAL_RUNAWAY_TEMP_C       3
#define CUT_OUT_SETTING              0
#define RECOM_VOL_CELL               33
#define TEMPERATURE_INF              0
#define DESCRIPTION_SCROLL_SPEED     0
#define ANIMATION_LOOP               1
#define ANIMATION_SPEED              settingOffSpeed_t::MEDIUM
#define VOLTAGE_DIV                  460
#define CALIBRATION_OFFSET           0
#define MIN_CALIBRATION_OFFSET       0
#define PID_POWER_LIMIT              0
#ifdef TS21_HEATER_TEST
#define POWER_LIMIT              (TS21_HEATER_POWER_X10 / 10U)
#define MAX_POWER_LIMIT          POWER_LIMIT
#define HARDWARE_MAX_WATTAGE_X10 TS21_HEATER_POWER_X10
#else
#define POWER_LIMIT                  0
#define MAX_POWER_LIMIT              0
#define HARDWARE_MAX_WATTAGE_X10     0
#endif
#define MAX_TEMP_C                   450
#define MAX_TEMP_F                   850
#define MIN_BOOST_TEMP_C             250
#define MIN_BOOST_TEMP_F             480
#define POWER_LIMIT_STEPS            1
#define ENFORCE_HARDWARE_MAX_WATTAGE 1
#define MIN_TEMP_C                   10
#define MIN_TEMP_F                   50
#define USB_PD_TIMEOUT               0
#define USB_PD_VMAX                  5
#define USB_PD_EPR_WATTAGE           0
#define POW_PD_EXT                   0
#define NEEDS_VBUS_PROBE             0
#define SETTINGS_START_PAGE          0x0800FC00
#define FILTER_DISPLAYED_TIP_TEMP    4

#endif
