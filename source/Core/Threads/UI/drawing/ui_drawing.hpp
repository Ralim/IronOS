#ifndef UI_DRAWING_UI_DRAWING_HPP_
#define UI_DRAWING_UI_DRAWING_HPP_
#include "Buttons.hpp"
#include "OLED.hpp"
#include "OperatingModeUtilities.h"
#include "Settings.h"
#include "configuration.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
void ui_draw_warning_undervoltage(void);
void ui_draw_power_source_icon(void);                            // Draw a single character wide power source icon
void ui_draw_tip_temperature(bool symbol, const FontStyle font); // Draw tip temp, aware of conversions
bool warnUser(const char *warning, const ButtonState buttons);   // Print a full screen warning to the user
void ui_draw_cjc_sampling(const uint8_t num_dots);               // Draws the CJC info text and progress dots
void ui_draw_debug_menu(const uint8_t item_number);              // Draws the debug menu state
void ui_draw_homescreen_detailed(TemperatureType_t tipTemp);     // Drawing the home screen -- Detailed mode
void ui_draw_homescreen_simplified(TemperatureType_t tipTemp);   // Drawing the home screen -- Simple mode
void ui_pre_render_assets(void);                                 // If any assets need to be pre-rendered into ram
// Soldering mode
void ui_draw_soldering_power_status(bool boost_mode_on);
void ui_draw_soldering_basic_status(bool boostModeOn);
void ui_draw_soldering_detailed_sleep(TemperatureType_t tipTemp);
void ui_draw_soldering_basic_sleep(TemperatureType_t tipTemp);
void ui_draw_soldering_profile_advanced(TemperatureType_t tipTemp, TemperatureType_t profileCurrentTargetTemp, uint32_t phaseElapsedSeconds, uint32_t phase, const uint32_t phaseTimeGoal);

// Temp change
void ui_draw_temperature_change(void);
// USB-PD debug
void ui_draw_usb_pd_debug_state(const uint16_t vbus_sense_state, const uint8_t stateNumber);
void ui_draw_usb_pd_debug_pdo(const uint8_t entry_num, const uint16_t min_voltage, const uint16_t max_voltage, const uint16_t current_a_x100, const uint16_t wattage);
// Utils
void printVoltage(void);
#endif // UI_DRAWING_UI_DRAWING_HPP_
