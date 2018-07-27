/*
 * Translation.cpp
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Translation.h"
#ifndef LANG
#define LANG_RU
#endif
// TEMPLATES for short names - choose one and use it as base for your
// translation:

//const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
//const char* SettingsShortNames[17][2] = {
//  /* (<= 5) Power source (DC or batt)          */ {"PWRSC"},
//  /* (<= 4) Sleep temperature                  */ {"STMP"},
//  /* (<= 4) Sleep timeout                      */ {"STME"},
//  /* (<= 5) Shutdown timeout                   */ {"SHTME"},
//  /* (<= 6) Motion sensitivity level           */ {"MSENSE"},
//  /* (<= 6) Temperature in F and C             */ {"TMPUNT"},
//  /* (<= 6) Advanced idle display mode enabled */ {"ADVIDL"},
//  /* (<= 6) Display rotation mode              */ {"DSPROT"},
//  /* (<= 6) Boost enabled                      */ {"BOOST"},
//  /* (<= 4) Boost temperature                  */ {"BTMP"},
//  /* (<= 6) Automatic start mode               */ {"ASTART"},
//  /* (<= 6) Cooldown blink                     */ {"CLBLNK"},
//  /* (<= 8) Temperature calibration enter menu */ {"TMP CAL?"},
//  /* (<= 8) Settings reset command             */ {"RESET?"},
//  /* (<= 8) Calibrate input voltage            */ {"CAL VIN?"},
//  /* (<= 6) Advanced soldering screen enabled  */ {"ADVSLD"},
//  /* (<= 6) Message Scroll Speed               */ {"DESCSP"},
//};

//const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
//const char* SettingsShortNames[17][2] = {
//  /* (<= 11) Power source (DC or batt)          */ {"Power", "source"},
//  /* (<=  9) Sleep temperature                  */ {"Sleep", "temp"},
//  /* (<=  9) Sleep timeout                      */ {"Sleep", "timeout"},
//  /* (<= 11) Shutdown timeout                   */ {"Shutdown", "timeout"},
//  /* (<= 13) Motion sensitivity level           */ {"Motion", "sensitivity"},
//  /* (<= 13) Temperature in F and C             */ {"Temperature", "units"},
//  /* (<= 13) Advanced idle display mode enabled */ {"Detailed", "idle screen"},
//  /* (<= 13) Display rotation mode              */ {"Display", "orientation"},
//  /* (<= 13) Boost enabled                      */ {"Boost mode", "enabled"},
//  /* (<=  9) Boost temperature                  */ {"Boost", "temp"},
//  /* (<= 13) Automatic start mode               */ {"Auto", "start"},
//  /* (<= 13) Cooldown blink                     */ {"Cooldown", "blink"},
//  /* (<= 16) Temperature calibration enter menu */ {"Calibrate", "temperature?"},
//  /* (<= 16) Settings reset command             */ {"Factory", "Reset?"},
//  /* (<= 16) Calibrate input voltage            */ {"Calibrate", "input voltage?"},
//  /* (<= 13) Advanced soldering screen enabled  */ {"Detailed", "solder screen"},
//  /* (<= 11) Display Help Text Scroll Speed     */ {"Description","Scroll Speed"},
//};

#ifdef LANG_EN

const char* SettingsDescriptions[17] = {
  // These are all the help text for all the settings.
  // No requirements on spacing or length.
  /* Power source (DC or batt)          */ "Power source. Sets cutoff voltage. <DC 10V> <S 3.3V per cell>",
  /* Sleep temperature                  */ "Sleep Temperature <C>",
  /* Sleep timeout                      */ "Sleep Timeout <Minutes/Seconds>",
  /* Shutdown timeout                   */ "Shutdown Timeout <Minutes>",
  /* Motion sensitivity level           */ "Motion Sensitivity <0.Off 1.least sensitive 9.most sensitive>",
  /* Temperature in F and C             */ "Temperature Unit <C=Celsius F=Fahrenheit>",
  /* Advanced idle display mode enabled */ "Display detailed information in a smaller font on the idle screen.",
  /* Display rotation mode              */ "Display Orientation <A. Automatic L. Left Handed R. Right Handed>",
  /* Boost enabled                      */ "Enable front key enters boost mode 450C mode when soldering",
  /* Boost temperature                  */ "Temperature when in \"boost\" mode",
  /* Automatic start mode               */ "Automatically starts the iron into soldering on power up. T=Soldering, S= Sleep mode,F=Off",
  /* Cooldown blink                     */ "Blink the temperature on the cooling screen while the tip is still hot.",
  /* Temperature calibration enter menu */ "Calibrate tip offset.",
  /* Settings reset command             */ "Reset all settings",
  /* Calibrate input voltage            */ "VIN Calibration. Buttons adjust, long press to exit",
  /* Advanced soldering screen enabled  */ "Display detailed information while soldering",
  /* Description Scroll Speed           */ "Speed this text scrolls past at",

#ifdef PIDSETTINGS

  "PID P term. Inverse values! This acts as a divisor. So Larger numbers == typically smaller in other systems",
  "PID I term. Inverse values! This acts as a divisor. So Larger numbers == typically smaller in other systems",
  "PID D term. Inverse values! This acts as a divisor. So Larger numbers == typically smaller in other systems",

#endif

};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";               // <=8 chars
const char* UndervoltageString = "Undervoltage";        // <=16 chars
const char* InputVoltageString = "Input V: ";           // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";        // <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";                   // <=8 chars
const char* SleepingSimpleString = "Zzzz";              // Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";     // <=16 chars
const char* WarningSimpleString = "HOT!";               // Must be <= 4 chars
const char* WarningAdvancedString = "!!! TIP HOT !!!";  // <=16 chars
const char* SleepingTipAdvancedString = "Tip:";         // <=6 chars
const char* IdleTipString = "Tip:";                     // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";                    // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED"; // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";   // <=12 chars
const char* OffString ="Off";                           // 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Power", "source"},
  /* (<=  9) Sleep temperature                  */ {"Sleep", "temp"},
  /* (<=  9) Sleep timeout                      */ {"Sleep", "timeout"},
  /* (<= 10) Shutdown timeout                   */ {"Shutdown", "timeout"},
  /* (<= 13) Motion sensitivity level           */ {"Motion", "sensitivity"},
  /* (<= 13) Temperature in F and C             */ {"Temperature", "units"},
  /* (<= 13) Advanced idle display mode enabled */ {"Detailed", "idle screen"},
  /* (<= 13) Display rotation mode              */ {"Display", "orientation"},
  /* (<= 13) Boost enabled                      */ {"Boost mode", "enabled"},
  /* (<=  9) Boost temperature                  */ {"Boost", "temp"},
  /* (<= 13) Automatic start mode               */ {"Auto", "start"},
  /* (<= 13) Cooldown blink                     */ {"Cooldown", "blink"},
  /* (<= 16) Temperature calibration enter menu */ {"Calibrate", "temperature?"},
  /* (<= 16) Settings reset command             */ {"Factory", "reset?"},
  /* (<= 16) Calibrate input voltage            */ {"Calibrate", "input voltage?"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Detailed", "solder screen"},
  /* (<= 11) Display Help Text Scroll Speed     */ {"Description","Scroll Speed"},
#ifdef PIDSETTINGS

  {"PID","P"},
  {"PID","I"},
  {"PID","D"},
#endif

};

// SettingsMenuEntries lengths <= 13 per line (\n starts second line)
const char* SettingsMenuEntries[4] = {
  /* Soldering Menu    */ "Soldering\nSettings",
  /* Power Saving Menu */ "Sleep\nModes",
  /* UI Menu           */ "User\nInterface",
  /* Advanced Menu     */ "Advanced\nOptions",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  "Soldering settings",
  "Power Saving Settings",
  "User Interface settings",
  "Advanced options"
};
#endif

#ifdef LANG_BG

const char* SettingsDescriptions[17] = {
  /* Power source (DC or batt)          */ "Ð˜Ð·Ñ‚Ð¾Ñ‡Ð½Ð¸Ðº Ð½Ð° Ð·Ð°Ñ…Ñ€Ð°Ð½Ð²Ð°Ð½Ðµ. ÐœÐ¸Ð½Ð¸Ð¼Ð°Ð»Ð½Ð¾ Ð½Ð°Ð¿Ñ€ÐµÐ¶ÐµÐ½Ð¸Ðµ. <DC 10V> <S 3.3V Ð·Ð° ÐºÐ»ÐµÑ‚ÐºÐ°>",
  /* Sleep temperature                  */ "Ð¢ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð° Ð¿Ñ€Ð¸ Ñ€ÐµÐ¶Ð¸Ð¼ \"Ñ�ÑŠÐ½\" <C>",
  /* Sleep timeout                      */ "Ð’ÐºÐ»ÑŽÑ‡Ð²Ð°Ð½Ðµ Ð² Ñ€ÐµÐ¶Ð¸Ð¼ \"Ñ�ÑŠÐ½\" Ñ�Ð»ÐµÐ´: <ÐœÐ¸Ð½ÑƒÑ‚Ð¸/Ð¡ÐµÐºÑƒÐ½Ð´Ð¸>",
  /* Shutdown timeout                   */ "Ð˜Ð·ÐºÐ»ÑŽÑ‡Ð²Ð°Ð½Ðµ Ñ�Ð»ÐµÐ´ <ÐœÐ¸Ð½ÑƒÑ‚Ð¸>",
  /* Motion sensitivity level           */ "Ð£Ñ�ÐµÑ‰Ð°Ð½Ðµ Ð·Ð° Ð´Ð²Ð¸Ð¶ÐµÐ½Ð¸Ðµ <0.Ð˜Ð·ÐºÐ»ÑŽÑ‡ÐµÐ½Ð¾ 1.Ð¡Ð»Ð°Ð±Ð¾ 9.Ð¡Ð¸Ð»Ð½Ð¾>",
  /* Temperature in F and C             */ "Ð•Ð´Ð¸Ð½Ð¸Ñ†Ð¸ Ð·Ð° Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð° <C=Ð¦ÐµÐ»Ð·Ð¸Ð¸ F=Ð¤Ð°Ñ€ÐµÐ½Ñ…Ð°Ð¹Ñ‚>",
  /* Advanced idle display mode enabled */ "ÐŸÐ¾ÐºÐ°Ð¶Ð¸ Ð´ÐµÑ‚Ð°Ð¹Ð»Ð½Ð° Ð¸Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ð¸Ñ� Ñ�ÑŠÑ� Ñ�Ð¸Ñ‚ÐµÐ½ ÑˆÑ€Ð¸Ñ„Ñ‚ Ð½Ð° ÐµÐºÑ€Ð°Ð½Ð° Ð² Ñ€ÐµÐ¶Ð¸Ð¼ Ð½Ð° Ð¿Ð¾ÐºÐ¾Ð¹.",
  /* Display rotation mode              */ "ÐžÑ€Ð¸ÐµÐ½Ñ‚Ð°Ñ†Ð¸Ñ� Ð½Ð° Ð´Ð¸Ñ�Ð¿Ð»ÐµÑ� <A. Ð�Ð²Ñ‚Ð¾Ð¼Ð°Ñ‚Ð¸Ñ‡Ð½Ð¾ L. Ð›Ñ�Ð²Ð° Ð ÑŠÐºÐ° R. Ð”Ñ�Ñ�Ð½Ð° Ð ÑŠÐºÐ°>",
  /* Boost enabled                      */ "ÐŸÐ¾Ð»Ð·Ð²Ð°Ð¹ Ð¿Ñ€ÐµÐ´Ð½Ð¸Ñ� Ð±ÑƒÑ‚Ð¾Ð½ Ð·Ð° \"Ñ‚ÑƒÑ€Ð±Ð¾\" Ñ€ÐµÐ¶Ð¸Ð¼ Ñ� Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð° Ð´Ð¾ 450C Ð¿Ñ€Ð¸ Ð·Ð°Ð¿Ð¾Ñ�Ð²Ð°Ð½Ðµ",
  /* Boost temperature                  */ "Ð¢ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð° Ð·Ð° \"Ñ‚ÑƒÑ€Ð±Ð¾\" Ñ€ÐµÐ¶Ð¸Ð¼",
  /* Automatic start mode               */ "Ð ÐµÐ¶Ð¸Ð¼ Ð½Ð° Ð¿Ð¾Ñ�Ð»Ð½Ð¸ÐºÐ° Ð¿Ñ€Ð¸ Ð²ÐºÐ»ÑŽÑ‡Ð²Ð°Ð½Ðµ Ð½Ð° Ð·Ð°Ñ…Ñ€Ð°Ð½Ð²Ð°Ð½ÐµÑ‚Ð¾. T=Ð Ð°Ð±Ð¾Ñ‚ÐµÐ½, S=Ð¡ÑŠÐ½, F=Ð˜Ð·ÐºÐ»ÑŽÑ‡ÐµÐ½",
  /* Cooldown blink                     */ "Ð¡Ð»ÐµÐ´ Ð¸Ð·ÐºÐ»ÑŽÑ‡Ð²Ð°Ð½Ðµ Ð¾Ñ‚ Ñ€Ð°Ð±Ð¾Ñ‚ÐµÐ½ Ñ€ÐµÐ¶Ð¸Ð¼, Ð¸Ð½Ð´Ð¸ÐºÐ°Ñ‚Ð¾Ñ€Ð° Ð·Ð° Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð° Ð´Ð° Ð¼Ð¸Ð³Ð° Ð´Ð¾ÐºÐ°Ñ‚Ð¾ Ñ‡Ð¾Ð²ÐºÐ°Ñ‚Ð° Ð½Ð° Ð¿Ð¾Ñ�Ð»Ð½Ð¸ÐºÐ° Ð²Ñ�Ðµ Ð¾Ñ‰Ðµ Ðµ Ñ‚Ð¾Ð¿Ð»Ð°",
  /* Temperature calibration enter menu */ "ÐšÐ°Ð»Ð¸Ð±Ñ€Ð¸Ñ€Ð°Ð½Ðµ Ð½Ð° Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð°Ñ‚Ð°",
  /* Settings reset command             */ "Ð’Ñ€ÑŠÑ‰Ð°Ð½Ðµ Ð½Ð° Ñ„Ð°Ð±Ñ€Ð¸Ñ‡Ð½Ð¸ Ð½Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸",
  /* Calibrate input voltage            */ "ÐšÐ°Ð»Ð¸Ð±Ñ€Ð¸Ñ€Ð°Ð½Ðµ Ð½Ð° Ð²Ñ…Ð¾Ð´Ð½Ð¾Ñ‚Ð¾ Ð½Ð°Ð¿Ñ€ÐµÐ¶ÐµÐ½Ð¸Ðµ (VIN). Ð—Ð°Ð´Ñ€ÑŠÐ¶Ñ‚Ðµ Ð±ÑƒÑ‚Ð¾Ð½a Ð·Ð° Ð¸Ð·Ñ…Ð¾Ð´",
  /* Advanced soldering screen enabled  */ "Ð”ÐµÑ‚Ð°Ð¹Ð»Ð½Ð° Ð¸Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ð¸Ñ� Ð² Ñ€Ð°Ð±Ð¾Ñ‚ÐµÐ½ Ñ€ÐµÐ¶Ð¸Ð¼ Ð¿Ñ€Ð¸ Ð·Ð°Ð¿Ð¾Ñ�Ð²Ð°Ð½Ðµ",
  /* Description Scroll Speed           */ "Ð¡ÐºÐ¾Ñ€Ð¾Ñ�Ñ‚ Ð½Ð° Ð´Ð²Ð¸Ð¶ÐµÐ½Ð¸Ðµ Ð½Ð° Ñ‚Ð¾Ð·Ð¸ Ñ‚ÐµÐºÑ�Ñ‚",
};

const char* SettingsCalibrationWarning = "Ð£Ð²ÐµÑ€ÐµÑ‚Ðµ Ñ�Ðµ, Ñ‡Ðµ Ñ‡Ð¾Ð²ÐºÐ°Ñ‚Ð° Ð½Ð° Ð¿Ð¾Ñ�Ð»Ð½Ð¸ÐºÐ° Ðµ Ñ�ÑŠÑ� Ñ�Ñ‚Ð°Ð¹Ð½Ð° Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð° Ð¿Ñ€ÐµÐ´Ð¸ Ð´Ð° Ð¿Ñ€Ð¾Ð´ÑŠÐ»Ð¶Ð¸Ñ‚Ðµ!";
const char* SettingsResetWarning = "Ð¡Ð¸Ð³ÑƒÑ€Ð½Ð¸ Ð»Ð¸ Ñ�Ñ‚Ðµ, Ñ‡Ðµ Ð¸Ñ�ÐºÐ°Ñ‚Ðµ Ð´Ð° Ð²ÑŠÑ€Ð½ÐµÑ‚Ðµ Ñ„Ð°Ð±Ñ€Ð¸Ñ‡Ð½Ð¸Ñ‚Ðµ Ð½Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸?";
const char* UVLOWarningString = "Ð�Ð¸Ñ�ÐºÐ¾ V!";                // <=8 chars
const char* UndervoltageString = "Ð�Ð¸Ñ�ÐºÐ¾ Ð�Ð°Ð¿Ñ€ÐµÐ¶ÐµÐ½Ð¸Ðµ";       // <=16 chars
const char* InputVoltageString = "Ð’Ñ…Ð¾Ð´Ð½Ð¾ V: ";             // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Ð¢ÐµÐ¼Ð¿.: ";              // <=12 chars, preferably end with a space
const char* BadTipString = "Ð›ÐžÐ¨ Ð’Ð ÐªÐ¥";                     // <=8 chars
const char* SleepingSimpleString = "Ð¡ÑŠÐ½";                  // Must be <= 4 chars
const char* SleepingAdvancedString = "Ð¥ÑŠÑ€ Ð¥ÑŠÑ€ Ð¥ÑŠÑ€...";     // <=16 chars
const char* WarningSimpleString = "ÐžÐ¥!";                   // Must be <= 4 chars
const char* WarningAdvancedString = "Ð’Ð�Ð˜ÐœÐ�Ð�Ð˜Ð•! Ð¢ÐžÐŸÐ›Ðž!";    // <=16 chars
const char* SleepingTipAdvancedString = "Ð’Ñ€ÑŠÑ…:";           // <=6 chars
const char* IdleTipString = "Ð’Ñ€ÑŠÑ…:";                       // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";                       // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "Ð’Ð ÐªÐ¥ Ð›ÐžÐ¨Ð� Ð’Ð ÐªÐ—ÐšÐ�";    // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Ð—Ð°Ñ…Ñ€Ð°Ð½Ð²Ð°Ð½Ðµ: "; // <=12 chars
const char* OffString ="Off";                              // 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Ð˜Ð·Ñ‚Ð¾Ñ‡Ð½Ð¸Ðº", "Ð·Ð°Ñ…Ñ€Ð°Ð½Ð²Ð°Ð½Ðµ"},
  /* (<=  9) Sleep temperature                  */ {"Ð¢ÐµÐ¼Ð¿.", "Ñ�ÑŠÐ½"},
  /* (<=  9) Sleep timeout                      */ {"Ð’Ñ€ÐµÐ¼Ðµ", "Ñ�ÑŠÐ½"},
  /* (<= 10) Shutdown timeout                   */ {"Ð’Ñ€ÐµÐ¼Ðµ", "Ð¸Ð·ÐºÐ»."},
  /* (<= 13) Motion sensitivity level           */ {"Ð£Ñ�ÐµÑ‰Ð°Ð½Ðµ", "Ð·Ð° Ð´Ð²Ð¸Ð¶ÐµÐ½Ð¸Ðµ"},
  /* (<= 13) Temperature in F and C             */ {"Ð•Ð´Ð¸Ð½Ð¸Ñ†Ð¸ Ð·Ð°", "Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð°"},
  /* (<= 13) Advanced idle display mode enabled */ {"Ð”ÐµÑ‚Ð°Ð¹Ð»ÐµÐ½", "ÐµÐºÑ€Ð°Ð½ Ð² Ð¿Ð¾ÐºÐ¾Ð¹"},
  /* (<= 13) Display rotation mode              */ {"ÐžÑ€Ð¸ÐµÐ½Ñ‚Ð°Ñ†Ð¸Ñ�", "Ð½Ð° Ð´Ð¸Ñ�Ð¿Ð»ÐµÑ�"},
  /* (<= 13) Boost enabled                      */ {"Ð¢ÑƒÑ€Ð±Ð¾ Ñ€ÐµÐ¶Ð¸Ð¼", "Ð¿ÑƒÑ�Ð½Ð°Ñ‚"},
  /* (<=  9) Boost temperature                  */ {"Ð¢ÑƒÑ€Ð±Ð¾", "Ñ‚ÐµÐ¼Ð¿."},
  /* (<= 13) Automatic start mode               */ {"Ð�Ð²Ñ‚Ð¾Ð¼Ð°Ñ‚Ð¸Ñ‡ÐµÐ½", "Ñ€Ð°Ð±Ð¾Ñ‚ÐµÐ½ Ñ€ÐµÐ¶Ð¸Ð¼"},
  /* (<= 13) Cooldown blink                     */ {"ÐœÐ¸Ð³Ð°Ð¹ Ð¿Ñ€Ð¸", "Ñ‚Ð¾Ð¿ÑŠÐ» Ð¿Ð¾Ñ�Ð»Ð½Ð¸Ðº"},
  /* (<= 16) Temperature calibration enter menu */ {"ÐšÐ°Ð»Ð¸Ð±Ñ€Ð¸Ñ€Ð°Ð½Ðµ", "Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð°?"},
  /* (<= 16) Settings reset command             */ {"Ð¤Ð°Ð±Ñ€Ð¸Ñ‡Ð½Ð¸", "Ð½Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸?"},
  /* (<= 16) Calibrate input voltage            */ {"ÐšÐ°Ð»Ð¸Ð±Ñ€Ð¸Ñ€Ð°Ð½Ðµ", "Ð½Ð°Ð¿Ñ€ÐµÐ¶ÐµÐ½Ð¸Ðµ?"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Ð”ÐµÑ‚Ð°Ð¹Ð»ÐµÐ½", "Ñ€Ð°Ð±Ð¾Ñ‚ÐµÐ½ ÐµÐºÑ€Ð°Ð½"},
  /* (<= 11) Display Help Text Scroll Speed     */ {"Ð¡ÐºÐ¾Ñ€Ð¾Ñ�Ñ‚","Ð½Ð° Ñ‚ÐµÐºÑ�Ñ‚Ð°"},
};

const char* SettingsMenuEntries[4] = {
	/* Soldering Menu */"ÐŸÐ¾Ñ�Ð»Ð½Ð¸Ðº\nÐ�Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸",
	/* Power Saving Menu */"Ð ÐµÐ¶Ð¸Ð¼Ð¸\nÐ�Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸",
	/* UI Menu */"Ð˜Ð½Ñ‚ÐµÑ€Ñ„ÐµÐ¹Ñ�\nÐ�Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸",
	/* Advanced Menu */"Ð”Ð¾Ð¿ÑŠÐ»Ð½Ð¸Ñ‚ÐµÐ»Ð½Ð¸\nÐ�Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸"
};
const char* SettingsMenuEntriesDescriptions[4] = {
	"Ð�Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸ Ð½Ð° Ð¿Ð¾Ñ�Ð»Ð½Ð¸ÐºÐ°",
	"Ð�Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸ ÐµÐ½ÐµÑ€Ð³Ð¾Ñ�Ð¿ÐµÑ�Ñ‚Ñ�Ð²Ð°Ð½Ðµ",
	"Ð�Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸ Ð½Ð° Ð¸Ð½Ñ‚ÐµÑ€Ñ„ÐµÐ¹Ñ�Ð°",
	"Ð”Ð¾Ð¿ÑŠÐ»Ð½Ð¸Ñ‚ÐµÐ»Ð½Ð¸ Ð½Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸"
};
#endif

#ifdef LANG_RU
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "Ð£Ñ�Ñ‚Ð°Ð½Ð¾Ð²ÐºÐ° Ð¼Ð¸Ð½Ð¸Ð¼Ð°Ð»ÑŒÐ½Ð¾Ð³Ð¾ Ð½Ð°Ð¿Ñ€Ñ�Ð¶ÐµÐ½Ð¸Ñ�. <DC - 10V, 3S - 9.9V, 4S - 13.2V, 5S - 16.5V, 6S - 19.8V>",
	/* Sleep temperature                  */ "Ð¢ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð° Ñ€ÐµÐ¶Ð¸Ð¼Ð° Ð¾Ð¶Ð¸Ð´Ð°Ð½Ð¸Ñ� <CÂ°/FÂ°>.",
	/* Sleep timeout                      */ "Ð’Ñ€ÐµÐ¼Ñ� Ð´Ð¾ Ð¿ÐµÑ€ÐµÑ…Ð¾Ð´Ð° Ð² Ñ€ÐµÐ¶Ð¸Ð¼ Ð¾Ð¶Ð¸Ð´Ð°Ð½Ð¸Ñ� <Ð”Ð¾Ñ�Ñ‚ÑƒÐ¿Ð½Ð¾ Ð¾Ñ‚ÐºÐ»ÑŽÑ‡ÐµÐ½Ð¸Ðµ>.",
	/* Shutdown timeout                   */ "Ð’Ñ€ÐµÐ¼Ñ� Ð´Ð¾ Ð¾Ñ‚ÐºÐ»ÑŽÑ‡ÐµÐ½Ð¸Ñ� <Ð”Ð¾Ñ�Ñ‚ÑƒÐ¿Ð½Ð¾ Ð¾Ñ‚ÐºÐ»ÑŽÑ‡ÐµÐ½Ð¸Ðµ>.",
	/* Motion sensitivity level           */ "Ð�ÐºÑ�ÐµÐ»ÐµÑ€Ð¾Ð¼ÐµÑ‚Ñ€ <0 - Ð’Ñ‹ÐºÐ»., 1 - ÐœÐ¸Ð½. Ñ‡ÑƒÐ²Ñ�Ñ‚Ð²Ð¸Ñ‚ÐµÐ»ÑŒÐ½Ð¾Ñ�Ñ‚ÑŒ, 9 - ÐœÐ°ÐºÑ�. Ñ‡ÑƒÐ²Ñ�Ñ‚Ð²Ð¸Ñ‚ÐµÐ»ÑŒÐ½Ð¾Ñ�Ñ‚ÑŒ>.",
	/* Temperature in F and C             */ "Ð•Ð´Ð¸Ð½Ð¸Ñ†Ð° Ð¸Ð·Ð¼ÐµÑ€ÐµÐ½Ð¸Ñ� Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ñ‹. < C - Ð¦ÐµÐ»ÑŒÑ�Ð¸Ð¹, F - Ð¤Ð°Ñ€ÐµÐ½Ð³ÐµÐ¹Ñ‚ >",
	/* Advanced idle display mode enabled */ "ÐŸÐ¾ÐºÐ°Ð·Ñ‹Ð²Ð°Ñ‚ÑŒ Ð´ÐµÑ‚Ð°Ð»ÑŒÐ½ÑƒÑŽ Ð² Ñ€ÐµÐ¶Ð¸Ð¼Ðµ Ð½Ð°Ñ�Ñ‚Ñ€Ð¾ÐµÐº <Ð’Ð¼ÐµÑ�Ñ‚Ð¾ ÐºÐ°Ñ€Ñ‚Ð¸Ð½ÐºÐ¸>.",
	/* Display rotation mode              */ "ÐžÑ€Ð¸ÐµÐ½Ñ‚Ð°Ñ†Ð¸Ñ� Ð´Ð¸Ñ�Ð¿Ð»ÐµÑ�. <A - Ð�Ð²Ñ‚Ð¾Ð¿Ð¾Ð²Ð¾Ñ€Ð¾Ñ‚, L - Ð›ÐµÐ²ÑˆÐ°, R - ÐŸÑ€Ð°Ð²ÑˆÐ°>",
	/* Boost enabled                      */ "Ð¢ÑƒÑ€Ð±Ð¾-Ñ€ÐµÐ¶Ð¸Ð¼ Ð¿Ñ€Ð¸ ÑƒÐ´ÐµÑ€Ð¶Ð°Ð½Ð¸Ð¸ ÐºÐ½Ð¾Ð¿ÐºÐ¸ Ð� Ð² Ñ€ÐµÐ¶Ð¸Ð¼Ðµ Ð¿Ð°Ð¹ÐºÐ¸.",
	/* Boost temperature                  */ "Ð¢ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð° Ð² Ð¢ÑƒÑ€Ð±Ð¾-Ñ€ÐµÐ¶Ð¸Ð¼Ðµ.",
	/* Automatic start mode               */ "Ð�Ð²Ñ‚Ð¾Ð¼Ð°Ñ‚Ð¸Ñ‡ÐµÑ�ÐºÐ¸Ð¹ Ð¿ÐµÑ€ÐµÑ…Ð¾Ð´ Ð² Ñ€ÐµÐ¶Ð¸Ð¼ Ð¿Ð°Ð¹ÐºÐ¸ Ð¿Ñ€Ð¸ Ð²ÐºÐ»ÑŽÑ‡ÐµÐ½Ð¸Ð¸ Ð¿Ð¸Ñ‚Ð°Ð½Ð¸Ñ�.",
	/* Cooldown blink                     */ "ÐŸÐ¾ÐºÐ°Ð·Ñ‹Ð²Ð°Ñ‚ÑŒ Ð¸Ð·Ð¼ÐµÐ½ÐµÐ½Ð¸Ðµ Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ñ‹ Ð² Ð¿Ñ€Ð¾Ñ†ÐµÑ�Ñ�Ðµ Ð¾Ñ…Ð»Ð°Ð¶Ð´ÐµÐ½Ð¸Ñ�, Ð¼Ð¸Ð³Ð°Ñ� Ñ�ÐºÑ€Ð°Ð½Ð¾Ð¼.",
	/* Temperature calibration enter menu */ "ÐšÐ°Ð»Ð¸Ð±Ñ€Ð¾Ð²ÐºÐ° Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð½Ð¾Ð³Ð¾ Ð´Ð°Ñ‚Ñ‡Ð¸ÐºÐ°.",
	/* Settings reset command             */ "Ð¡Ð±Ñ€Ð¾Ñ� Ð²Ñ�ÐµÑ… Ð½Ð°Ñ�Ñ‚Ñ€Ð¾ÐµÐº Ðº Ð¸Ñ�Ñ…Ð¾Ð´Ð½Ñ‹Ð¼ Ð·Ð½Ð°Ñ‡ÐµÐ½Ð¸Ñ�.",
	/* Calibrate input voltage            */ "ÐšÐ°Ð»Ð¸Ð±Ñ€Ð¾Ð²ÐºÐ° Ð½Ð°Ð¿Ñ€Ñ�Ð¶ÐµÐ½Ð¸Ñ� Ð²Ñ…Ð¾Ð´Ð°. Ð�Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ° ÐºÐ½Ð¾Ð¿ÐºÐ°Ð¼Ð¸, Ð½Ð°Ð¶Ð°Ñ‚ÑŒ Ð¸ ÑƒÐ´ÐµÑ€Ð¶Ð°Ñ‚ÑŒ Ñ‡Ñ‚Ð¾Ð±Ñ‹ Ð·Ð°Ð²ÐµÑ€ÑˆÐ¸Ñ‚ÑŒ.",
	/* Advanced soldering screen enabled  */ "ÐŸÐ¾ÐºÐ°Ð·Ñ‹Ð²Ð°Ñ‚ÑŒ Ð´ÐµÑ‚Ð°Ð»ÑŒÐ½ÑƒÑŽ Ð¸Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ð¸ÑŽ Ð¿Ñ€Ð¸ Ð¿Ð°Ð¹ÐºÐµ.",
	/* Description Scroll Speed           */ "Ð¡ÐºÐ¾Ñ€Ð¾Ñ�Ñ‚ÑŒ Ð¿Ñ€Ð¾ÐºÑ€ÑƒÑ‚ÐºÐ¸ Ñ‚ÐµÐºÑ�Ñ‚Ð°.",
};

const char* SettingsCalibrationWarning = "Ð£Ð±ÐµÐ´Ð¸Ñ‚ÐµÑ�ÑŒ, Ñ‡Ñ‚Ð¾ Ð¶Ð°Ð»Ð¾ Ð¾Ñ�Ñ‚Ñ‹Ð»Ð¾ Ð´Ð¾ ÐºÐ¾Ð¼Ð½Ð°Ñ‚Ð½Ð¾Ð¹ Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ñ‹, Ð¿Ñ€ÐµÐ¶Ð´Ðµ Ñ‡ÐµÐ¼ Ð¿Ñ€Ð¾Ð´Ð¾Ð»Ð¶Ð°Ñ‚ÑŒ!";
const char* SettingsResetWarning = "Ð’Ñ‹ Ð´ÐµÐ¹Ñ�Ñ‚Ð²Ð¸Ñ‚ÐµÐ»ÑŒÐ½Ð¾ Ñ…Ð¾Ñ‚Ð¸Ñ‚Ðµ Ñ�Ð±Ñ€Ð¾Ñ�Ð¸Ñ‚ÑŒ Ð½Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸ Ð´Ð¾ Ð·Ð½Ð°Ñ‡ÐµÐ½Ð¸Ð¹ Ð¿Ð¾ ÑƒÐ¼Ð¾Ð»Ñ‡Ð°Ð½Ð¸ÑŽ?";
const char* UVLOWarningString = "Ð�ÐšÐšÐ£Ðœ--";               // <=8 chars
const char* UndervoltageString = "ÐŸÐ¾Ð´ Ð¿Ð¸Ñ‚Ð°Ð½Ð¸ÐµÐ¼";         // <=16 chars
const char* InputVoltageString = "ÐŸÐ¸Ñ‚Ð°Ð½Ð¸Ðµ(B):";          // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Ð–Ð°Ð»Ð¾ tÂ°: ";          // <=12 chars, preferably end with a space
const char* BadTipString = "Ð–Ð°Ð»Ð¾--";                     // <=8 chars
const char* SleepingSimpleString = "Ð¡Ð¾Ð½ ";                // Must be <= 4 chars
const char* SleepingAdvancedString = "ÐžÐ¶Ð¸Ð´Ð°Ð½Ð¸Ðµ...";      // <=16 chars
const char* WarningSimpleString = " Ð�Ð™!";                // Must be <= 4 chars
const char* WarningAdvancedString = "Ð’Ð�Ð˜ÐœÐ�Ð�Ð˜Ð• Ð“ÐžÐ Ð¯Ð§Ðž!"; // <=16 chars
const char* SleepingTipAdvancedString = "Ð–Ð°Ð»Ð¾:";         // <=6 chars
const char* IdleTipString = "Ð–Ð°Ð»Ð¾:";                     // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " ->";                  // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "Ð–Ð°Ð»Ð¾ Ð¾Ñ‚ÐºÐ»ÑŽÑ‡ÐµÐ½Ð¾!";   // <=16 chars
const char* SolderingAdvancedPowerPrompt = "ÐŸÐ¸Ñ‚Ð°Ð½Ð¸Ðµ: ";  // <=12 chars
const char* OffString ="Off";                            // 3 chars max

/*
 * #TODO change support for multibyte constants here
 const char SettingRightChar = 'ÐŸ';
 const char SettingLeftChar = 'Ð›';
 const char SettingAutoChar = 'A';*/

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = '+';
const char SettingSlowChar = '-';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Ð˜Ñ�Ñ‚Ð¾Ñ‡Ð½Ð¸Ðº","Ð¿Ð¸Ñ‚Ð°Ð½Ð¸Ñ�"},		//8,7
  /* (<=  9) Sleep temperature                  */ {"Ð¢ÐµÐ¼Ð¿ÐµÑ€.","Ñ�Ð½Ð°"},			//7,3
  /* (<=  9) Sleep timeout                      */ {"Ð¢Ð°Ð¹Ð¼Ð°ÑƒÑ‚","Ñ�Ð½Ð°"},			//7,3
  /* (<= 10) Shutdown timeout                   */ {"Ð’Ñ€ÐµÐ¼Ñ� Ð´Ð¾","Ð¾Ñ‚ÐºÐ»ÑŽÑ‡ÐµÐ½Ð¸Ñ�"},		//8,10
  /* (<= 13) Motion sensitivity level           */ {"Ð§ÑƒÐ²Ñ�Ñ‚. Ñ�ÐµÐ½Ñ�Ð¾-","Ñ€Ð° Ð´Ð²Ð¸Ð¶ÐµÐ½Ð¸Ñ�"},	//13,11
  /* (<= 13) Temperature in F and C             */ {"Ð¤Ð¾Ñ€Ð¼Ð°Ñ‚ Ñ‚ÐµÐ¼Ð¿Ðµ-","Ñ€Ð°Ñ‚ÑƒÑ€Ñ‹(CÂ°/FÂ°)"},	//13,13
  /* (<= 13) Advanced idle display mode enabled */ {"ÐŸÐ¾Ð´Ñ€Ð¾Ð±Ð½Ñ‹Ð¹ Ñ€Ðµ-","Ð¶Ð¸Ð¼ Ð¾Ð¶Ð¸Ð´Ð°Ð½Ð¸Ñ�"},	//13,12
  /* (<= 13) Display rotation mode              */ {"Ð�Ð²Ñ‚Ð¾Ð¿Ð¾Ð²Ð¾Ñ€Ð¾Ñ‚","Ñ�ÐºÑ€Ð°Ð½Ð°"},		//11,6
  /* (<= 13) Boost enabled                      */ {"Ð ÐµÐ¶Ð¸Ð¼","Ð¢ÑƒÑ€Ð±Ð¾"},			//5,5
  /* (<=  9) Boost temperature                  */ {"Ð¢ÐµÐ¼Ð¿ÐµÑ€.","Ð¢ÑƒÑ€Ð±Ð¾"},                 //7,5
  /* (<= 13) Automatic start mode               */ {"Ð“Ð¾Ñ€Ñ�Ñ‡Ð¸Ð¹","Ñ�Ñ‚Ð°Ñ€Ñ‚"},			//9,9
  /* (<= 13) Cooldown blink                     */ {"ÐŸÐ¾ÐºÐ°Ð· tÂ° Ð¿Ñ€Ð¸","Ð¾Ñ�Ñ‚Ñ‹Ð²Ð°Ð½Ð¸Ð¸"},	//7,3
  /* (<= 16) Temperature calibration enter menu */ {"ÐšÐ°Ð»Ð¸Ð±Ñ€Ð¾Ð²ÐºÐ°","Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ñ‹"},	//10,11
  /* (<= 16) Settings reset command             */ {"Ð¡Ð±Ñ€Ð¾Ñ�Ð¸Ñ‚ÑŒ Ð²Ñ�Ðµ","Ð½Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸?"},	//12,10
  /* (<= 16) Calibrate input voltage            */ {"ÐšÐ°Ð»Ð¸Ð±Ñ€Ð¾Ð²ÐºÐ°","Ð½Ð°Ð¿Ñ€Ñ�Ð¶ÐµÐ½Ð¸Ñ�"},		//10,10
  /* (<= 13) Advanced soldering screen enabled  */ {"ÐŸÐ¾Ð´Ñ€Ð¾Ð±Ð½Ñ‹Ð¹ Ñ€Ðµ-","Ð¶Ð¸Ð¼ Ð¿Ð°Ð¹ÐºÐ¸"},	//13,9
  /* (<= 11) Message Scroll Speed               */ {"Ð¡ÐºÐ¾Ñ€Ð¾Ñ�Ñ‚ÑŒ","Ñ‚ÐµÐºÑ�Ñ‚Ð°"},		//8,6
};

const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"ÐŸÐ°Ð¹ÐºÐ°",
/* Power Saving Menu*/"Ð¡Ð¾Ð½",
/* UI Menu*/"Ð˜Ð½Ñ‚ÐµÑ€Ñ„ÐµÐ¹Ñ�",
/* Advanced Menu*/"Ð”Ñ€ÑƒÐ³Ð¸Ðµ", };
const char* SettingsMenuEntriesDescriptions[4] ={
"Ð�Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸ Ð´Ð»Ñ� Ñ€ÐµÐ¶Ð¸Ð¼Ð° Ð¿Ð°Ð¹ÐºÐ¸. Ð”ÐµÐ¹Ñ�Ñ‚Ð²ÑƒÑŽÑ‚ Ð¿Ñ€Ð¸ Ð²ÐºÐ»ÑŽÑ‡ÐµÐ½Ð½Ð¾Ð¼ Ð¶Ð°Ð»Ðµ.",
"Ð�Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸ Ð¿Ñ€Ð¸ Ð±ÐµÐ·Ð´ÐµÐ¹Ñ�Ñ‚Ð²Ð¸Ð¸. ÐŸÐ¾Ð»ÐµÐ·Ð½Ð¾ Ñ‡Ñ‚Ð¾Ð±Ñ‹ Ð½Ðµ Ð¾Ð±Ð¶ÐµÑ‡ÑŒÑ�Ñ� Ð¸ Ñ�Ð»ÑƒÑ‡Ð°Ð¹Ð½Ð¾ Ð½Ðµ Ñ�Ð¶ÐµÑ‡ÑŒ Ð¶Ð¸Ð»Ð¸Ñ‰Ðµ.",
"ÐŸÐ¾Ð»ÑŒÐ·Ð¾Ð²Ð°Ñ‚ÐµÐ»ÑŒÑ�ÐºÐ¸Ð¹ Ð¸Ð½Ñ‚ÐµÑ€Ñ„ÐµÐ¹Ñ�.",
"Ð Ð°Ñ�ÑˆÐ¸Ñ€ÐµÐ½Ð½Ñ‹Ðµ Ð½Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ¸. Ð”Ð¾Ð¿Ð¾Ð»Ð½Ð¸Ñ‚ÐµÐ»ÑŒÐ½Ñ‹Ðµ ÑƒÐ´Ð¾Ð±Ñ�Ñ‚Ð²Ð°."
};
#endif

#ifdef LANG_ES
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
  /* Power source (DC or batt)          */ "Fuente de energÃ­a. Ajusta el lÃ­mite inferior de voltaje. <DC=10V S=3.3V por celda>",
  /* Sleep temperature                  */ "Temperatura en reposo. <C>",
  /* Sleep timeout                      */ "Tiempo hasta activar reposo. <Minutos>",
  /* Shutdown timeout                   */ "Tiempo hasta apagado. <Minutos>",
  /* Motion sensitivity level           */ "Sensibilidad del movimiento. <0=Apagado 1=El menos sensible 9=El mÃ¡s sensible>",
  /* Temperature in F and C             */ "Unidad de temperatura.",
  /* Advanced idle display mode enabled */ "Mostrar informaciÃ³n detallada con fuente de menor tamaÃ±o en la pantalla de reposo.",
  /* Display rotation mode              */ "OrientaciÃ³n de la pantalla <A=AutomÃ¡tico I=Mano izquierda D=Mano derecha>",
  /* Boost enabled                      */ "Activar el botÃ³n \"Boost\" en modo soldadura.",
  /* Boost temperature                  */ "Temperatura en modo \"Boost\". <C>",
  /* Automatic start mode               */ "Iniciar modo soldadura en el encendido. <V=SÃ­ S=Modo reposo F=No>",
  /* Cooldown blink                     */ "Parpadea la temperatura en el enfriamiento si la punta sigue caliente.",
  /* Temperature calibration enter menu */ "Calibrar desviaciÃ³n tÃ©rmica de la punta.",
  /* Settings reset command             */ "Volver a estado de fÃ¡brica.",
  /* Calibrate input voltage            */ "CalibraciÃ³n VIN (Voltaje de entrada). Ajuste con ambos botones, pulsaciÃ³n larga para salir.",
  /* Advanced soldering screen enabled  */ "Mostrar informaciÃ³n detallada mientras suelda.",
  /* Description Scroll Speed           */ "Velocidad de desplazamiento del texto.",
};

const char* SettingsCalibrationWarning = "Por favor, Â¡asegÃºrese que la punta estÃ© a temperatura ambiente antes de proceder!";
const char* SettingsResetWarning = "Â¿EstÃ¡ seguro de volver a estado de fÃ¡brica?";
const char* UVLOWarningString = "DC LOW";                // <=8 chars
const char* UndervoltageString = "Undervoltage";         // <=16 chars
const char* InputVoltageString = "Input V: ";            // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";         // <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";                    // <=8 chars
const char* SleepingSimpleString = "Zzzz";               // Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";      // <=16 chars
const char* WarningSimpleString = "HOT!";                // Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!"; // <=16 chars
const char* SleepingTipAdvancedString = "Tip:";          // <=6 chars
const char* IdleTipString = "Tip:";                      // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";                     // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED";  // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";    // <=12 chars
const char* OffString ="Off";                            // 3 chars max

const char SettingRightChar = 'D'; // D is "Derecha" = Right
const char SettingLeftChar = 'I'; // I is "Izquierda" = Left
const char SettingAutoChar = 'A';

const char SettingFastChar = 'R'; // R is "RÃ¡pido" = Fast
const char SettingSlowChar = 'L'; // L is "Lento" = Slow

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 5) Power source (DC or batt)          */ {"PWRSC"},
  /* (<= 4) Sleep temperature                  */ {"STMP"},
  /* (<= 4) Sleep timeout                      */ {"STME"},
  /* (<= 5) Shutdown timeout                   */ {"SHTME"},
  /* (<= 6) Motion sensitivity level           */ {"MSENSE"},
  /* (<= 6) Temperature in F and C             */ {"TMPUNT"},
  /* (<= 6) Advanced idle display mode enabled */ {"ADVIDL"},
  /* (<= 6) Display rotation mode              */ {"DSPROT"},
  /* (<= 6) Boost enabled                      */ {"BOOST"},
  /* (<= 4) Boost temperature                  */ {"BTMP"},
  /* (<= 6) Automatic start mode               */ {"ASTART"},
  /* (<= 6) Cooldown blink                     */ {"CLBLNK"},
  /* (<= 8) Temperature calibration enter menu */ {"TMP CAL?"},
  /* (<= 8) Settings reset command             */ {"RESET?"},
  /* (<= 8) Calibrate input voltage            */ {"CAL VIN?"},
  /* (<= 6) Advanced soldering screen enabled  */ {"ADVSLD"},
  /* (<= 6) Message Scroll Speed               */ {"DESCSP"},
};
const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"Opciones de\nsoldadura",
/* Power Saving Menu*/"Modos de\nreposo",
/* UI Menu*/"Interfaz de\nusuario",
/* Advanced Menu*/"Opciones\navanzadas", };
const char* SettingsMenuEntriesDescriptions[4] ={
"Opciones de soldadura",
"Opciones de ahorro energÃ©tico",
"Opciones de interfaz de usuario",
"Opciones avanzadas"
};
#endif

#ifdef LANG_FI

const char* SettingsDescriptions[17] = {
  // These are all the help text for all the settings.
  // No requirements on spacing or length.
  /* Power source (DC or batt)          */ "KÃ¤ytettÃ¤vÃ¤ virtalÃ¤hde. Asettaa katkaisujÃ¤nniteen. <DC 10V, 3S=9.9V, 4S=13.2V, 5S=16.5V, 6S=19.8V>",
  /* Sleep temperature                  */ "Lepotilan lÃ¤mpÃ¶tila. <C>", //"LepolÃ¤mpÃ¶tila <C>",
  /* Sleep timeout                      */ "Lepotilan viive. <minuuttia/sekuntia>",//"Lepoviive <minuuttia/sekuntia>",
  /* Shutdown timeout                   */ "Automaattisen sammutuksen aikaviive. <minuuttia>",
  /* Motion sensitivity level           */ "Liikkeentunnistuksen herkkyys. <0=pois, 1=epÃ¤herkin, 9=herkin>", //"Liikeherkkyys <0=pois, 1=epÃ¤herkin, 9=herkin>",
  /* Temperature in F and C             */ "LÃ¤mpÃ¶tilan yksikkÃ¶. <C=celsius, F=fahrenheit>",
  /* Advanced idle display mode enabled */ "NÃ¤yttÃ¤Ã¤ yksityiskohtaisemmat tiedot lepotilassa.", //"NÃ¤yttÃ¤Ã¤ tarkemmat tiedot leponÃ¤ytÃ¶ssÃ¤",
  /* Display rotation mode              */ "NÃ¤ytÃ¶n kierto. <A=automaattinen O=oikeakÃ¤tinen V=vasenkÃ¤tinen>", //"NÃ¤ytÃ¶n suunta <A=automaattinen O=oikeakÃ¤tinen V=vasenkÃ¤tinen>",
  /* Boost enabled                      */ "Etupainikeella siirrytÃ¤Ã¤n juotettaessa tehostustilaan.",
  /* Boost temperature                  */ "Tehostustilan lÃ¤mpÃ¶tila.", //"Tehostuksen lÃ¤mpÃ¶tila kun kÃ¤ytetÃ¤Ã¤n tehostustilaa",
  /* Automatic start mode               */ "KÃ¤ynnistÃ¤Ã¤ virrat kytkettÃ¤essÃ¤ juotostilan automaattisesti. T=juotostila, S=Lepotila, F=Ei kÃ¤ytÃ¶ssÃ¤", //"LÃ¤mmitys kÃ¤ynnistyy automaattisesti kun virrat kytketÃ¤Ã¤n. T=juottamiseen, S=lepotilaan, F=ei kÃ¤ytÃ¶ssÃ¤",
  /* Cooldown blink                     */ "Vilkuttaa jÃ¤Ã¤htyessÃ¤ juotoskÃ¤rjen lÃ¤mpÃ¶tilaa sen ollessa vielÃ¤ vaarallisen kuuma.",//"Vilkutetaan nÃ¤ytÃ¶ssÃ¤ lÃ¤mpÃ¶tilaa kun juotoskÃ¤rki jÃ¤Ã¤htyy, mutta on yhÃ¤ kuuma",
  /* Temperature calibration enter menu */ "Kalibroi kÃ¤rjen lÃ¤mpÃ¶tilaeron.", //"Kalibroidaan juotoskÃ¤rjen lÃ¤mpÃ¶tilaero",
  /* Settings reset command             */ "Palauta kaikki asetukset oletusarvoihin.", //"Palautetaan kaikki asetukset alkuperÃ¤isiksi",
  /* Calibrate input voltage            */ "TulojÃ¤nnitten kalibrointi (VIN). Painikkeilla sÃ¤Ã¤detÃ¤Ã¤n ja pitkÃ¤Ã¤n painamalla poistutaan.",
  /* Advanced soldering screen enabled  */ "NÃ¤yttÃ¤Ã¤ yksityiskohtaisemmat tiedot juotostilassa.", //"NÃ¤ytetÃ¤Ã¤n tarkemmat tiedot juottamisen aikana",
  /* Description Scroll Speed           */ "NÃ¤iden selitetekstien vieritysnopeus." ,//"Tekstin vieritysnopeus nÃ¤ytÃ¶llÃ¤",
};

const char* SettingsCalibrationWarning = "Varmista ettÃ¤ kÃ¤rki on huoneenlÃ¤mpÃ¶inen ennen jatkamista!"; //"Ã„lÃ¤ jatka ennen kuin kÃ¤rki on jÃ¤Ã¤htynyt!";
const char* SettingsResetWarning = "Haluatko varmasti palauttaa oletusarvot?"; //"Haluatko palauttaa oletusarvot?";
const char* UVLOWarningString = "DC LOW";               // <=8 chars
const char* UndervoltageString = "AlijÃ¤nnite";        // <=16 chars
const char* InputVoltageString = "JÃ¤nnite: ";           // <=11 chars, preferably end with a space
const char* WarningTipTempString = "LÃ¤mpÃ¶tila: ";        // <=12 chars, preferably end with a space
const char* BadTipString = "VIKATILA";                   // <=8 chars
const char* SleepingSimpleString = "Zzz";              // Must be <= 4 chars
const char* SleepingAdvancedString = "Lepotila...";     // <=16 chars
const char* WarningSimpleString = "HOT";               // Must be <= 4 chars
const char* WarningAdvancedString = " ! KÃ„RKI KUUMA !"; //"KUUMA KÃ„RKI";  // <=16 chars
const char* SleepingTipAdvancedString = "KÃ¤rki:";         // <=6 chars
const char* IdleTipString = "KÃ¤rki:";                     // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Asetus:";                    // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "KÃ„RKI ON IRTI"; // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Teho: ";   // <=12 chars
const char* OffString ="OFF";                           // 3 chars max

const char SettingRightChar = 'O';
const char SettingLeftChar = 'V';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'N';   //'F';
const char SettingSlowChar = 'H';   //'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"VirtalÃ¤hde", "DC"},
  /* (<=  9) Sleep temperature                  */ {"Lepotilan", "lÃ¤mpÃ¶tila"},
  /* (<=  9) Sleep timeout                      */ {"Lepotilan", "viive"},
  /* (<= 10) Shutdown timeout                   */ {"Sammutus", "viive"},
  /* (<= 13) Motion sensitivity level           */ {"Liikkeen", "herkkyys"},
  /* (<= 13) Temperature in F and C             */ {"LÃ¤mpÃ¶tilan", "yksikkÃ¶"},
  /* (<= 13) Advanced idle display mode enabled */ {"Tiedot", "lepotilassa"},
  /* (<= 13) Display rotation mode              */ {"NÃ¤ytÃ¶n", "kierto"},
  /* (<= 13) Boost enabled                      */ {"Tehostus", "kÃ¤ytÃ¶ssÃ¤"},
  /* (<=  9) Boost temperature                  */ {"Tehostus-", "lÃ¤mpÃ¶tila"},
  /* (<= 13) Automatic start mode               */ {"Autom.", "kÃ¤ynnistys"},

  /* (<= 13) Cooldown blink                     */ {"JÃ¤Ã¤hdytyksen", "vilkutus"},
  /* (<= 16) Temperature calibration enter menu */ {"Kalibroi", "lÃ¤mpÃ¶tila?"},
  /* (<= 16) Settings reset command             */ {"Palauta", "tehdasasetukset?"},
  /* (<= 16) Calibrate input voltage            */ {"Kalibroi", "tulojÃ¤nnite?"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Tarkempi", "juotosnÃ¤yttÃ¶"},
  /* (<= 11) Display Help Text Scroll Speed     */ {"Tietojen","nÃ¤yttÃ¶nopeus"},
};

// SettingsMenuEntries lengths <= 13 per line (\n starts second line)
const char* SettingsMenuEntries[4] = {
  /* Soldering Menu    */ "Juotos-\nasetukset" ,//"Juotos\nasetukset",
  /* Power Saving Menu */ "Lepotilan\nasetukset", // "Lepo\ntila",
  /* UI Menu           */ "KÃ¤yttÃ¶-\nliittymÃ¤", //"KÃ¤yttÃ¶\nliittymÃ¤",
  /* Advanced Menu     */ "LisÃ¤-\nasetukset", // "LisÃ¤\nasetukset",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  "Juotosasetukset",
  "VirransÃ¤Ã¤stÃ¶asetukset",
  "KÃ¤yttÃ¶liittymÃ¤n asetukset",
  "LisÃ¤asetukset"
};
#endif

#ifdef LANG_IT
const char* SettingsDescriptions[17] = {
  // These are all the help text for all the settings.
  // No requirements on spacing or length.
  /* Power source (DC or batt)          */"Scegli la sorgente di alimentazione; se a batteria, limita lo scaricamento al valore di soglia <DC: 10V; S: 3.3V per cella>",
  /* Sleep temperature                  */"Imposta la temperatura da mantenere in modalitÃ  Standby <Â°C/Â°F>",
  /* Sleep timeout                      */"Imposta il timer per entrare in modalitÃ  Standby <minuti/secondi>",
  /* Shutdown timeout                   */"Imposta il timer per lo spegnimento <minuti>",
  /* Motion sensitivity level           */"Imposta la sensibilitÃ  al movimento per uscire dalla modalitÃ  Standby <0: nessuna; 1: minima; 9: massima>",
  /* Temperature in F and C             */"Scegli l'unitÃ  di misura per la temperatura <C: grado Celsius; F: grado Farenheit>",
  /* Advanced idle display mode enabled */"Mostra informazioni dettagliate con un carattere piÃ¹ piccolo all'interno della schermata principale",
  /* Display rotation mode              */"Imposta l'orientamento del display <A: automatico; S: mano sinistra; D: mano destra>",
  /* Boost enabled                      */"Il tasto superiore attiva la funzione Â«TurboÂ» durante la modalitÃ  Saldatura",
  /* Boost temperature                  */"Imposta la temperatura della funzione Â«TurboÂ»",
  /* Automatic start mode               */"Attiva automaticamente il saldatore quando viene alimentato <A: saldatura; S: standby; D: disattiva>",
  /* Cooldown blink                     */"Mostra la temperatura durante il raffreddamento se la punta Ã¨ ancora calda",
  /* Temperature calibration enter menu */"Calibra le rilevazioni di temperatura",
  /* Settings reset command             */"Ripristina tutte le impostazioni",
  /* Calibrate input voltage            */"Calibra la tensione in ingresso; regola con entrambi i tasti, tieni premuto il tasto superiore per uscire",
  /* Advanced soldering screen enabled  */"Mostra informazioni dettagliate durante la modalitÃ  Saldatura",
  /* Description Scroll Speed           */"Imposta la velocitÃ  di scorrimento del testo <L: lento; V: veloce>",

};

const char* SettingsCalibrationWarning = "Assicurati che la punta si trovi a temperatura ambiente prima di continuare!";
const char* SettingsResetWarning = "Ripristinare le impostazioni iniziali?";
const char* UVLOWarningString = "DC BASSA";            // <=8 chars
const char* UndervoltageString = "DC INSUFFICIENTE";   // <=16 chars
const char* InputVoltageString = "V ingresso:";        // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Punta:";           // <=12 chars, preferably end with a space
const char* BadTipString = "PUNTA NO";                 // <=8 chars
const char* SleepingSimpleString = "ZZZ ";             // Must be <= 4 chars
const char* SleepingAdvancedString = "Standby";        // <=16 chars
const char* WarningSimpleString = "HOT!";              // Must be <= 4 chars
const char* WarningAdvancedString = "PUNTA CALDA!";    // <=16 chars
const char* SleepingTipAdvancedString = "Punta:";      // <=6 chars
const char* IdleTipString = "Punta:";                  // IdleTipString+IdleSetString <= 10
const char* IdleSetString = "/";                       // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "PUNTA ASSENTE";   // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Potenza:"; // <=12 chars
const char* OffString ="OFF";                          // 3 chars max

const char SettingRightChar = 'D';
const char SettingLeftChar = 'S';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'V';
const char SettingSlowChar = 'L';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
 const char* SettingsShortNames[17][2] = {
    /* (<= 11) Power source (DC or batt)          */ {"Sorgente", "alimentaz"},
    /* (<=  9) Sleep temperature                  */ {"Temp", "standby"},
    /* (<=  9) Sleep timeout                      */ {"Timer", "standby"},
    /* (<= 10) Shutdown timeout                   */ {"Timer di", "arresto"},
    /* (<= 13) Motion sensitivity level           */ {"SensibilitÃ ", "al movimento"},
    /* (<= 13) Temperature in F and C             */ {"UnitÃ  di", "temperatura"},
    /* (<= 13) Advanced idle display mode enabled */ {"Mostra", "dettagli"},
    /* (<= 13) Display rotation mode              */ {"Orientamento", "display"},
    /* (<= 13) Boost enabled                      */ {"Funzione", "Â«TurboÂ»"},
    /* (<=  9) Boost temperature                  */ {"Temp", "Â«TurboÂ»"},
    /* (<= 13) Automatic start mode               */ {"Avvio", "automatico"},
    /* (<= 13) Cooldown blink                     */ {"Avviso", "punta calda"},
    /* (<= 16) Temperature calibration enter menu */ {"Calibrazione", "temperatura"},
    /* (<= 16) Settings reset command             */ {"Ripristino", "impostazioni"},
    /* (<= 16) Calibrate input voltage            */ {"Calibrazione", "tensione"},
    /* (<= 13) Advanced soldering screen enabled  */ {"Dettagli", "saldatura"},
    /* (<= 11) Display Help Text Scroll Speed     */ {"VelocitÃ ", "testo"},
};
 const char* SettingsMenuEntries[4] = {
 /*Soldering Menu*/"Opzioni\nsaldatura",
 /* Power Saving Menu*/"Risparmio\nenergetico",
 /* UI Menu*/"Interfaccia\nutente",
 /* Advanced Menu*/"Opzioni\navanzate", };
 const char* SettingsMenuEntriesDescriptions[4] ={
 "MenÃ¹ d'impostazioni della modalitÃ  Saldatura",
 "MenÃ¹ d'impostazioni per il risparmio energetico",
 "MenÃ¹ d'impostazioni dell'interfaccia utente",
 "MenÃ¹ d'impostazioni avanzate"
 };
#endif

#ifdef LANG_FR

const char* SettingsDescriptions[17] = {
  // These are all the help text for all the settings.
  // No requirements on spacing or length.
  /* Power source (DC or batt)          */ "Source d'alimentation. RÃ¨gle la tension de coupure <DC=10V S=3.3V par cellules>",
  /* Sleep temperature                  */ "TempÃ©rature en veille <C>",
  /* Sleep timeout                      */ "DÃ©lai avant mise en veille <Minutes>",
  /* Shutdown timeout                   */ "DÃ©lai avant extinction <Minutes>",
  /* Motion sensitivity level           */ "SensibilitÃ© du capteur de mouvement <0=Inactif 1=Peu sensible 9=Tres sensible>",
  /* Temperature in F and C             */ "UnitÃ© de tempÃ©rature <C=Celsius F=Fahrenheit>",
  /* Advanced idle display mode enabled */ "Afficher des informations dÃ©taillÃ©es lors de la veille.",
  /* Display rotation mode              */ "Orientation de l'affichage <A=Automatique G=Gaucher D=Droitier>",
  /* Boost enabled                      */ "Activer le mode \"Boost\" en maintenant le bouton de devant pendant la soudure",
  /* Boost temperature                  */ "TempÃ©rature du mode \"Boost\"",
  /* Automatic start mode               */ "DÃ©marrer automatiquement la soudure a l'allumage <A=ActivÃ©, V=Mode Veille, D=DÃ©sactivÃ©>",
  /* Cooldown blink                     */ "Faire clignoter la tempÃ©rature lors du refroidissement tant que la panne est chaude.",
  /* Temperature calibration enter menu */ "Etalonner tempÃ©rature de la panne.",
  /* Settings reset command             */ "RÃ©initialiser tous les rÃ©glages",
  /* Calibrate input voltage            */ "Etalonner tension d'entrÃ©e. Boutons pour ajuster, appui long pour quitter",
  /* Advanced soldering screen enabled  */ "Afficher des informations dÃ©taillÃ©es pendant la soudure",
  /* Description Scroll Speed           */ "Vitesse de dÃ©filement de ce texte en <R=Rapide L=Lent>",
};

const char* SettingsCalibrationWarning = "Assurez-vous que la panne soit Ã  tempÃ©rature ambiante avant de continuer!";
const char* SettingsResetWarning = "Voulez-vous vraiment rÃ©initialiser les paramÃ¨tres aux valeurs d'usine?";
const char* UVLOWarningString = "DC FAIBLE";              // <=8 chars
const char* UndervoltageString = "Sous-tension";          // <=16 chars
const char* InputVoltageString = "V d'entrÃ©e: ";          // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Temp. Panne: ";       // <=12 chars, preferably end with a space
const char* BadTipString = "PANNE HS";                    // <=8 chars
const char* SleepingSimpleString = "Zzzz";                // Must be <= 4 chars
const char* SleepingAdvancedString = "En veille...";      // <=16 chars
const char* WarningSimpleString = "HOT!";                 // Must be <= 4 chars
const char* WarningAdvancedString = "ATTENTION! CHAUD";   // Must be <= 16 chars
const char* SleepingTipAdvancedString = "PANNE:";         // <=6 chars
const char* IdleTipString = "PANNE:";                     // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";                      // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "PANNE DEBRANCH";     // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Puissance: "; // <= 12 chars
const char* OffString ="Off";                             // 3 chars max

const char SettingRightChar = 'D';
const char SettingLeftChar = 'G';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'R';
const char SettingSlowChar = 'L';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Source", "d'alim"},
  /* (<=  9) Sleep temperature                  */ {"Temp.", "veille"},
  /* (<=  9) Sleep timeout                      */ {"DÃ©lai", "veille"},
  /* (<= 10) Shutdown timeout                   */ {"DÃ©lai", "extinction"},
  /* (<= 13) Motion sensitivity level           */ {"SensibilitÃ©", "au mouvement"},
  /* (<= 13) Temperature in F and C             */ {"UnitÃ© de", "tempÃ©rature"},
  /* (<= 13) Advanced idle display mode enabled */ {"Ecran veille", "dÃ©taillÃ©"},
  /* (<= 13) Display rotation mode              */ {"Orientation", "de l'Ã©cran"},
  /* (<= 13) Boost enabled                      */ {"Activation du", "mode Boost"},
  /* (<=  9) Boost temperature                  */ {"Temp.", "Boost"},
  /* (<= 13) Automatic start mode               */ {"DÃ©marrage", "automatique"},
  /* (<= 13) Cooldown blink                     */ {"Refroidir en", "clignottant"},
  /* (<= 16) Temperature calibration enter menu */ {"Etalonner", "tempÃ©rature"},
  /* (<= 16) Settings reset command             */ {"RÃ©initialisation", "d'usine"},
  /* (<= 16) Calibrate input voltage            */ {"Etalonner", "tension d'entrÃ©e"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Ecran soudure", "dÃ©taillÃ©"},
  /* (<= 11) Display Help Text Scroll Speed     */ {"Vitesse de","dÃ©filement"},
};

const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"Soudure\nParamÃ¨tres",
/* Power Saving Menu*/"Mode\nVeille",
/* UI Menu*/"Interface\nUtilisateur",
/* Advanced Menu*/"Options\nAdvancÃ©es", };
const char* SettingsMenuEntriesDescriptions[4] ={
"ParamÃ¨tres de soudage",
"ParamÃ¨tres d'Ã©conomie d'Ã©nergie",
"ParamÃ¨tres de l'interface utilisateur",
"Options avancÃ©es"
};
#endif

#ifdef LANG_DE
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "Spannungsquelle (Abschaltspannung) <DC=10V, nS=n*3.3V fÃ¼r n LiIon-Zellen>",
	/* Sleep temperature                  */ "Ruhetemperatur (In der eingestellten Einheit)",
	/* Sleep timeout                      */ "Ruhemodus nach <Sekunden/Minuten>",
	/* Shutdown timeout                   */ "Abschalten nach <Minuten>",
	/* Motion sensitivity level           */ "Bewegungsempfindlichkeit <0=Aus, 1=Minimal ... 9=Maximal>",
	/* Temperature in F and C             */ "Temperatureinheit <C=Celsius, F=Fahrenheit>",
	/* Advanced idle display mode enabled */ "Detaillierte Anzeige im Ruhemodus <J=An, N=Aus>",
	/* Display rotation mode              */ "Ausrichtung der Anzeige <A=Automatisch, L=LinkshÃ¤ndig, R=RechtshÃ¤ndig>",
	/* Boost enabled                      */ "Vordere Taste fÃ¼r Temperaturboost verwenden <J=An, N=Aus>",
	/* Boost temperature                  */ "Temperatur im Boostmodus  (In der eingestellten Einheit)",
	/* Automatic start mode               */ "Automatischer Start des LÃ¶tmodus beim Einschalten der Spannungsversorgung. <J=An, N=Aus>",
	/* Cooldown blink                     */ "Blinkende Temperaturanzeige beim AbkÃ¼hlen, solange heiÃŸ. <J=An, N=Aus>",
	/* Temperature calibration enter menu */ "Kalibrierung der LÃ¶tspitzentemperatur",
	/* Settings reset command             */ "Alle Einstellungen zurÃ¼cksetzen",
	/* Calibrate input voltage            */ "Kalibrierung der Eingangsspannung. Kurzer Tastendruck zum Einstellen, langer Tastendruck zum Verlassen.",
	/* Advanced soldering screen enabled  */ "Detaillierte Anzeige im LÃ¶tmodus <J=An, N=Aus>",
	/* Description Scroll Speed           */ "Scrollgeschwindigkeit der Texte",
};

const char* SettingsCalibrationWarning = "Vor dem Fortfahren muss die LÃ¶tspitze vollstÃ¤ndig abgekÃ¼hlt sein!";
const char* SettingsResetWarning = "Sind Sie sicher, dass Sie alle Werte ZurÃ¼cksetzen wollen?";
const char* UVLOWarningString = "V niedr.";              // <=8 chars
const char* UndervoltageString = "Unterspannung";        // <=16 chars
const char* InputVoltageString = "V Eingang: ";          // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Temperatur: ";       // <=12 chars, preferably end with a space
const char* BadTipString = "DEFEKT";                     // <=8 chars
const char* SleepingSimpleString = "Zzz ";               // Must be <= 4 chars
const char* SleepingAdvancedString = "Ruhemodus...";     // <=16 chars
const char* WarningSimpleString = "HEIÃŸ";                // Must be <= 4 chars
const char* WarningAdvancedString = "SPITZE HEIÃŸ!";      // <=16 chars
const char* SleepingTipAdvancedString = "Temp:";         // <=6 chars
const char* IdleTipString = "Ist:";                      // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Soll:";                    // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "Spitze fehlt";      // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Leistung: "; // <= 12 chars
const char* OffString ="Off";                            // 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Spannungs-", "quelle"},
  /* (<=  9) Sleep temperature                  */ {"Ruhetemp-", "eratur"},
  /* (<=  9) Sleep timeout                      */ {"Ruhever-", "zÃ¶gerung"},
  /* (<= 10) Shutdown timeout                   */ {"Abschalt-", "zeit"},
  /* (<= 13) Motion sensitivity level           */ {"Bewegungs-", "empfindlichk."},
  /* (<= 13) Temperature in F and C             */ {"Temperatur-", "einheit"},
  /* (<= 13) Advanced idle display mode enabled */ {"Detaillierte", "Ruheansicht"},
  /* (<= 13) Display rotation mode              */ {"Anzeige-", "ausrichtung"},
  /* (<= 13) Boost enabled                      */ {"Boosttaste", "aktiv?"},
  /* (<=  9) Boost temperature                  */ {"Boosttemp-", "eratur"},
  /* (<= 13) Automatic start mode               */ {"Start im", "LÃ¶tmodus?"},
  /* (<= 13) Cooldown blink                     */ {"AbkÃ¼hl-", "blinken?"},
  /* (<= 16) Temperature calibration enter menu */ {"Temperatur", "kalibrieren?"},
  /* (<= 16) Settings reset command             */ {"Einstellungen", "zurÃ¼cksetzen?"},
  /* (<= 16) Calibrate input voltage            */ {"Eingangsspannung", "kalibrieren?"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Detaillierte", "LÃ¶tansicht"},
  /* (<= 11) Display Help Text Scroll Speed     */ {"Scroll-","geschw."},
};
const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"LÃ¶t-\neinstellungen",
/* Power Saving Menu*/"Schlaf-\nmodus",
/* UI Menu*/"MenÃ¼-\neinstellungen",
/* Advanced Menu*/"Erweiterte\nEinstellungen", };
const char* SettingsMenuEntriesDescriptions[4] ={
"LÃ¶teinstellungen",
"Energiespareinstellungen",
"MenÃ¼einstellungen",
"Erweiterte Einstellungen"
};
#endif

#ifdef LANG_SK
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "Zdroj napatia. Nastavit napatie pre vypnutie (cutoff)  <DC=10V, nS=n*3.3V pre LiIon clanky>",
	/* Sleep temperature                  */ "Kludova teplota (v nastavenych jednotkach)",
	/* Sleep timeout                      */ "Kludovy rezim po <sekundach/minutach>",
	/* Shutdown timeout                   */ "Cas na vypnutie <minuty>",
	/* Motion sensitivity level           */ "Citlivost detekcie pohybu <0=Vyp, 1=Min ... 9=Max>",
	/* Temperature in F and C             */ "Jednotky merania teploty <C=stupne Celzia, F=stupne Fahrenheita>",
	/* Advanced idle display mode enabled */ "Zobrazit detailne informacie v kludovom rezime <T=Zap, F=Vyp>",
	/* Display rotation mode              */ "Orientacia displeja <A=Auto, L=Lavak, R=Pravak>",
	/* Boost enabled                      */ "Povolit tlacidlo pre prudky nahrev <T=Zap, F=Vyp>",
	/* Boost temperature                  */ "Cielova teplota pre prudky nahrev (v nastavenych jednotkach)",
	/* Automatic start mode               */ "Pri starte spustit rezim spajkovania <T=Zap, F=Vyp, S=Spanok>",
	/* Cooldown blink                     */ "Blikanie ukazovatela teploty pocas chladnutia hrotu <T=Zap, F=Vyp>",
	/* Temperature calibration enter menu */ "Kalibracia posunu hrotu",
	/* Settings reset command             */ "Tovarenske nastavenia",
	/* Calibrate input voltage            */ "Kalibracia VIN. Kratke stlacenie meni nastavenie, dlhe stlacenie pre navrat",
	/* Advanced soldering screen enabled  */ "Zobrazenie detailov pocas spajkovania <T=Zap, F=Vyp>",
	/* Description Scroll Speed           */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Najprv sa prosim uistite, ze hrot ma izbovu teplotu!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";                // <=8 chars
const char* UndervoltageString = "Undervoltage";         // <=16 chars
const char* InputVoltageString = "Input V: ";            // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";         // <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";                    // <=8 chars
const char* SleepingSimpleString = "Chrr";               // Must be <= 4 chars
const char* SleepingAdvancedString = "Kludovy rezim..."; // <=16 chars
const char* WarningSimpleString = "HOT!";                // Must be <= 4 chars
const char* WarningAdvancedString = "Pozor! Hrot je horuci!";  // <=16 chars
const char* SleepingTipAdvancedString = "Tip:";          // <=6 chars
const char* IdleTipString = "Tip:";                      // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";                     // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED";  // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";    // <=12 chars
const char* OffString ="Off";                            // 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 5) Power source (DC or batt)          */ {"PWRSC"},
  /* (<= 4) Sleep temperature                  */ {"STMP"},
  /* (<= 4) Sleep timeout                      */ {"STME"},
  /* (<= 5) Shutdown timeout                   */ {"SHTME"},
  /* (<= 6) Motion sensitivity level           */ {"MSENSE"},
  /* (<= 6) Temperature in F and C             */ {"TMPUNT"},
  /* (<= 6) Advanced idle display mode enabled */ {"ADVIDL"},
  /* (<= 6) Display rotation mode              */ {"DSPROT"},
  /* (<= 6) Boost enabled                      */ {"BOOST"},
  /* (<= 4) Boost temperature                  */ {"BTMP"},
  /* (<= 6) Automatic start mode               */ {"ASTART"},
  /* (<= 6) Cooldown blink                     */ {"CLBLNK"},
  /* (<= 8) Temperature calibration enter menu */ {"TMP CAL?"},
  /* (<= 8) Settings reset command             */ {"RESET?"},
  /* (<= 8) Calibrate input voltage            */ {"CAL VIN?"},
  /* (<= 6) Advanced soldering screen enabled  */ {"ADVSLD"},
  /* (<= 6) Message Scroll Speed               */ {"DESCSP"},
};
const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"Soldering\nSettings",
/* Power Saving Menu*/"Sleep\nModes",
/* UI Menu*/"User\nInterface",
/* Advanced Menu*/"Advanced\nOptions", };
const char* SettingsMenuEntriesDescriptions[4] ={
"Soldering settings",
"Power Saving Settings",
"User Interface settings",
"Advanced options"
};
#endif

#ifdef LANG_TR
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "GÃ¼Ã§ KaynaÄŸÄ±. kesim geriliminÄ± ayarlar. <DC 10V> <S 3.3V hÃ¼cre baÅŸÄ±na>",
	/* Sleep temperature                  */ "Uyku SÄ±caklÄ±ÄŸÄ± <C>",
	/* Sleep timeout                      */ "Uyku Zaman AÅŸÄ±mÄ± <Dakika/Saniye>",
	/* Shutdown timeout                   */ "Kapatma Zaman AÅŸÄ±mÄ± <Dakika>",
	/* Motion sensitivity level           */ "Hareket Hassasiyeti <0.KapalÄ± 1.En az duyarlÄ± 9.En duyarlÄ±>",
	/* Temperature in F and C             */ "SÄ±caklÄ±k Ãœnitesi <C=Celsius F=Fahrenheit>",
	/* Advanced idle display mode enabled */ "BoÅŸ ekranda ayrÄ±ntÄ±lÄ± bilgileri daha kÃ¼Ã§Ã¼k bir yazÄ± tipi ile gÃ¶ster.",
	/* Display rotation mode              */ "GÃ¶rÃ¼ntÃ¼ YÃ¶nlendirme <A. Otomatik L. Solak R. SaÄŸlak>",
	/* Boost enabled                      */ "Lehimleme yaparken Ã¶n tuÅŸa basmak Boost moduna sokar(450C)",
	/* Boost temperature                  */ "\"boost\" Modu Derecesi",
	/* Automatic start mode               */ "GÃ¼Ã§ verildiÄŸinde otomatik olarak lehimleme modunda baÅŸlat. T=Lehimleme Modu, S= Uyku Modu,F=KapalÄ±",
	/* Cooldown blink                     */ "SoÄŸutma ekranÄ±nda uÃ§ hala sÄ±cakken derece yanÄ±p sÃ¶nsÃ¼n.",
	/* Temperature calibration enter menu */ "Ucu kalibre et.",
	/* Settings reset command             */ "BÃ¼tÃ¼n ayarlarÄ± sÄ±fÄ±rla",
	/* Calibrate input voltage            */ "VIN Kalibrasyonu. DÃ¼ÄŸmeler ayarlar, Ã§Ä±kmak iÃ§in uzun bas.",
	/* Advanced soldering screen enabled  */ "Lehimleme yaparken detaylÄ± bilgi gÃ¶ster",
	/* Description Scroll Speed           */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "LÃ¼tfen devam etmeden Ã¶nce ucun oda sÄ±caklÄ±ÄŸÄ±nda olduÄŸunu garantiye alÄ±n!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";               // <=8 chars
const char* UndervoltageString = "Undervoltage";        // <=16 chars
const char* InputVoltageString = "Input V: ";           // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";        // <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";                   // <=8 chars
const char* SleepingSimpleString = "Zzzz";              // Must be <= 4 chars
const char* SleepingAdvancedString = "Uyuyor...";       // <=16 chars
const char* WarningSimpleString = "HOT!";               // Must be <= 4 chars
const char* WarningAdvancedString = "UYARI! UÃ‡ SICAK!"; // <=16 chars
const char* SleepingTipAdvancedString = "Tip:";         // <=6 chars
const char* IdleTipString = "Tip:";                     // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";                    // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED"; // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";   // <=12 chars
const char* OffString ="Off";                           // 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 5) Power source (DC or batt)          */ {"PWRSC"},
  /* (<= 4) Sleep temperature                  */ {"STMP"},
  /* (<= 4) Sleep timeout                      */ {"STME"},
  /* (<= 5) Shutdown timeout                   */ {"SHTME"},
  /* (<= 6) Motion sensitivity level           */ {"MSENSE"},
  /* (<= 6) Temperature in F and C             */ {"TMPUNT"},
  /* (<= 6) Advanced idle display mode enabled */ {"ADVIDL"},
  /* (<= 6) Display rotation mode              */ {"DSPROT"},
  /* (<= 6) Boost enabled                      */ {"BOOST"},
  /* (<= 4) Boost temperature                  */ {"BTMP"},
  /* (<= 6) Automatic start mode               */ {"ASTART"},
  /* (<= 6) Cooldown blink                     */ {"CLBLNK"},
  /* (<= 8) Temperature calibration enter menu */ {"TMP CAL?"},
  /* (<= 8) Settings reset command             */ {"RESET?"},
  /* (<= 8) Calibrate input voltage            */ {"CAL VIN?"},
  /* (<= 6) Advanced soldering screen enabled  */ {"ADVSLD"},
  /* (<= 6) Message Scroll Speed               */ {"DESCSP"},
};
const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"Soldering\nSettings",
/* Power Saving Menu*/"Sleep\nModes",
/* UI Menu*/"User\nInterface",
/* Advanced Menu*/"Advanced\nOptions", };
const char* SettingsMenuEntriesDescriptions[4] ={
"Soldering settings",
"Power Saving Settings",
"User Interface settings",
"Advanced options"
};
#endif

#ifdef LANG_HR
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "Izvor napajanja. Postavlja napon iskljuÄ�ivanja. <DC 10V> <S 3.3V po Ä‡eliji>",
	/* Sleep temperature                  */ "Temperatura na koju se spuÅ¡ta lemilica nakon odreÄ‘enog vremena mirovanja. <C/F>",
	/* Sleep timeout                      */ "Vrijeme mirovanja nakon kojega lemilica spuÅ¡ta temperaturu. <Minute/Sekunde>",
	/* Shutdown timeout                   */ "Vrijeme mirovanja nakon kojega Ä‡e se lemilica ugasiti. <Minute>",
	/* Motion sensitivity level           */ "Osjetljivost prepoznavanja pokreta. <0=UgaÅ¡eno, 1=Najmanje osjetljivo, 9=Najosjetljivije>",
	/* Temperature in F and C             */ "Jedinica temperature. <C=Celzij, F=Fahrenheit>",
	/* Advanced idle display mode enabled */ "Prikazivanje detaljnih informacija manjim fontom tijekom Ä�ekanja.",
	/* Display rotation mode              */ "Orijentacija ekrana. <A=Automatski, L=Ljevoruki, D=Desnoruki>",
	/* Boost enabled                      */ "DrÅ¾anjem prednjeg gumba prilikom lemljenja aktivira se pojaÄ�ani (Boost) naÄ�in.",
	/* Boost temperature                  */ "Temperatura u pojaÄ�anom (Boost) naÄ�inu.",
	/* Automatic start mode               */ "Ako je aktivno, lemilica po ukljuÄ�ivanju napajanja odmah poÄ�inje grijati.",
	/* Cooldown blink                     */ "Bljeskanje temperature prilikom hlaÄ‘enja, ako je lemilica vruÄ‡a.",
	/* Temperature calibration enter menu */ "Kalibriranje temperature mjeri razliku temperatura vrÅ¡ka i drÅ¡ke, dok je lemilica hladna.",
	/* Settings reset command             */ "VraÄ‡anje svih postavki na tvorniÄ�ke vrijednosti.",
	/* Calibrate input voltage            */ "Kalibracija ulaznog napona. PodeÅ¡avanje gumbima, dugaÄ�ki pritisak za kraj.",
	/* Advanced soldering screen enabled  */ "Prikazivanje detaljnih informacija tijekom lemljenja.",
	/* Description Scroll Speed           */ "Brzina kretanja dugaÄ�kih poruka <B=brzo, S=sporo>",
};

const char* SettingsCalibrationWarning = "Provjerite da je vrÅ¡ak ohlaÄ‘en na sobnu temperaturu prije nego Å¡to nastavite!";
const char* SettingsResetWarning = "Jeste li sigurni da Å¾elite sve postavke vratiti na tvorniÄ�ke vrijednosti?";
const char* UVLOWarningString = "BATERIJA";             // <=8 chars
const char* UndervoltageString = "PRENIZAK NAPON";      // <=16 chars
const char* InputVoltageString = "Napajanje: ";         // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Temp vrha: ";       // <=12 chars, preferably end with a space
const char* BadTipString = "NEISPRAVAN VRH";            // <=8 chars
const char* SleepingSimpleString = "Zzz ";              // Must be <= 4 chars
const char* SleepingAdvancedString = "SPAVANJE...";     // <=16 chars
const char* WarningSimpleString = "VRUÄ†";               // Must be <= 4 chars
const char* WarningAdvancedString = "OPREZ, VRUÄ†E!";    // <=16 chars
const char* SleepingTipAdvancedString = "Vrh: ";        // <=6 chars
const char* IdleTipString = "Vrh: ";                    // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " / ";                      // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "VRH NIJE SPOJEN!"; // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Snaga: ";   // <=12 chars
const char* OffString ="Off";                           // 3 chars max

const char SettingRightChar = 'D';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'B';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Izvor", "napajanja"},
  /* (<=  9) Sleep temperature                  */ {"Temp", "spavanja"},
  /* (<=  9) Sleep timeout                      */ {"Vrijeme", "spavanja"},
  /* (<= 10) Shutdown timeout                   */ {"Vrijeme", "gaÅ¡enja"},
  /* (<= 13) Motion sensitivity level           */ {"Osjetljivost", "pokreta"},
  /* (<= 13) Temperature in F and C             */ {"Jedinica", "temperature"},
  /* (<= 13) Advanced idle display mode enabled */ {"Detalji", "pri Ä�ekanju"},
  /* (<= 13) Display rotation mode              */ {"Rotacija", "ekrana"},
  /* (<= 13) Boost enabled                      */ {"Boost", "naÄ�in"},
  /* (<=  9) Boost temperature                  */ {"Boost", "temp"},
  /* (<= 13) Automatic start mode               */ {"Auto", "start"},
  /* (<= 13) Cooldown blink                     */ {"Upozorenje", "pri hlaÄ‘enju"},
  /* (<= 16) Temperature calibration enter menu */ {"Kalibracija", "temperature"},
  /* (<= 16) Settings reset command             */ {"TvorniÄ�ke", "postavke"},
  /* (<= 16) Calibrate input voltage            */ {"Kalibracija", "napona napajanja"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Detalji", "pri lemljenju"},
  /* (<= 11) Display Help Text Scroll Speed     */ {"Brzina","poruka"},
};
const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"Postavke\nlemljenja",
/* Power Saving Menu*/"UÅ¡teda\nenergije",
/* UI Menu*/"KorisniÄ�ko\nsuÄ�elje",
/* Advanced Menu*/"Napredne\nopcije", };
const char* SettingsMenuEntriesDescriptions[4] ={
"Postavke pri lemljenju",
"Postavke spavanja i Å¡tednje energije",
"Postavke korisniÄ�kog suÄ�elja",
"Upravljanje naprednim opcijama"
};
#endif

#ifdef LANG_CS_CZ
const char* SettingsDescriptions[17] = {
  /* Power source (DC or batt)          */ "PÅ™i niÅ¾Å¡Ã­m napÄ›tÃ­ ukonÄ�Ã­ pÃ¡jenÃ­ <DC=10V, ?S=?x3.3V pro LiPo,LiIon...>",
  /* Sleep temperature                  */ "Teplota v reÅ¾imu spÃ¡nku.",
  /* Sleep timeout                      */ "ÄŒas do reÅ¾imu spÃ¡nku <Minut/Sekund>",
  /* Shutdown timeout                   */ "ÄŒas do automatickÃ©ho vypnutÃ­ <Minut>",
  /* Motion sensitivity level           */ "Citlivost detekce pohybu <0=Vyp, 1=Min, ... 9=Max>",
  /* Temperature in F and C             */ "Jednotky mÄ›Å™enÃ­ teploty <C=Celsius, F=Fahrenheit>",
  /* Advanced idle display mode enabled */ "Zobrazit podrobnosti na vÃ½chozÃ­ obrazovce?",
  /* Display rotation mode              */ "Orientace obrazovky <A=Auto, L=LevÃ¡k, P=PravÃ¡k>",
  /* Boost enabled                      */ "Povolit boost drÅ¾enÃ­m pÅ™ednÃ­ho tlaÄ�Ã­tka pÅ™i pÃ¡jenÃ­?",
  /* Boost temperature                  */ "Teplota v reÅ¾imu boost.",
  /* Automatic start mode               */ "PÅ™i startu ihned nahÅ™Ã¡t hrot?",
  /* Cooldown blink                     */ "BlikÃ¡nÃ­ teploty pÅ™i chladnutÃ­, dokud je hrot horkÃ½?",
  /* Temperature calibration enter menu */ "Kalibrace mÄ›Å™enÃ­ teploty.",
  /* Settings reset command             */ "ObnovenÃ­ tovÃ¡rnÃ­ho nastavenÃ­.",
  /* Calibrate input voltage            */ "Kalibrace vstupnÃ­ho napÄ›tÃ­. TlaÄ�Ã­tky uprav, podrÅ¾enÃ­m potvrÄ�.",
  /* Advanced soldering screen enabled  */ "Zobrazit podrobnosti pÅ™i pÃ¡jenÃ­?",
  /* Description Scroll Speed           */ "Rychlost skrolovÃ¡nÃ­ popiskÅ¯ podobnÃ½ch tomuto <P=Pomalu,R=Rychle>",
};

const char* SettingsCalibrationWarning = "UjistÄ›te se, Å¾e hrot mÃ¡ pokojovou teplotu!";
const char* SettingsResetWarning = "Opravdu chcete resetovat zaÅ™Ã­zenÃ­ do tovÃ¡rnÃ­ho nastavenÃ­?";
const char* UVLOWarningString = "DC LOW";               // <=8 chars
const char* UndervoltageString = "! NÃ­zkÃ© napÄ›tÃ­ !";    // <=16 chars
const char* InputVoltageString = "NapÄ›tÃ­: ";            // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Teplota: ";         // <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";                   // <=8 chars
const char* SleepingSimpleString = "Zzz ";              // Must be <= 4 chars
const char* SleepingAdvancedString = "ReÅ¾im spÃ¡nku..."; // <=16 chars
const char* WarningSimpleString = "HOT!";               // Must be <= 4 chars
const char* WarningAdvancedString = "!! HORKÃ� HROT !!"; // <= 16 chars
const char* SleepingTipAdvancedString = "Hrot:";        // <=6 chars
const char* IdleTipString = "Hrot:";                    // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " CÃ­l:";                    // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "HROT NEPÅ˜IPOJEN";  // <=16 chars
const char* SolderingAdvancedPowerPrompt = "OhÅ™ev: ";   // <=12 chars
const char* OffString ="Off";                           // 3 chars max

const char SettingRightChar = 'P';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'R';
const char SettingSlowChar = 'P';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Zdroj", "napÃ¡jenÃ­"},
  /* (<=  9) Sleep temperature                  */ {"Teplota v", "r. spÃ¡nku"},
  /* (<=  9) Sleep timeout                      */ {"ÄŒas do", "r. spÃ¡nku"},
  /* (<= 10) Shutdown timeout                   */ {"ÄŒas do", "vypnutÃ­"},
  /* (<= 13) Motion sensitivity level           */ {"Citlivost", "det. pohybu"},
  /* (<= 13) Temperature in F and C             */ {"Jednotky", "teploty"},
  /* (<= 13) Advanced idle display mode enabled */ {"Podrobnosti", "na vÃ½ch. obr."},
  /* (<= 13) Display rotation mode              */ {"Orientace", "obrazovky"},
  /* (<= 13) Boost enabled                      */ {"Povolit", "boost"},
  /* (<=  9) Boost temperature                  */ {"Teplota v", "r. boost"},
  /* (<= 13) Automatic start mode               */ {"Auto", "start"},
  /* (<= 13) Cooldown blink                     */ {"BlikÃ¡ni pÅ™i", "chladnutÃ­"},
  /* (<= 16) Temperature calibration enter menu */ {"Kalibrovat", "teplotu?"},
  /* (<= 16) Settings reset command             */ {"TovÃ¡rnÃ­", "nastavenÃ­?"},
  /* (<= 16) Calibrate input voltage            */ {"Kalibrovat", "vstupnÃ­ napÄ›tÃ­?"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Podrobnosti", "pÅ™i pÃ¡jenÃ­"},
  /* (<= 13) Display Help Text Scroll Speed     */ {"Rychlost", "popiskÅ¯"},
};

// SettingsMenuEntries lengths <= 13 per line (\n starts second line)
const char* SettingsMenuEntries[4] = {
  /* Soldering Menu    */ "PÃ¡jecÃ­\nnastavenÃ­",
  /* Power Saving Menu */ "ReÅ¾im\nspÃ¡nku",
  /* UI Menu           */ "UÅ¾ivatelskÃ©\nrozhranÃ­",
  /* Advanced Menu     */ "PokroÄ�ilÃ©\nvolby",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  "NastavenÃ­ pÃ¡jenÃ­ (boost, auto start...)",
  "NastavenÃ­ reÅ¾imu spÃ¡nku, automatickÃ©ho vypnutÃ­...",
  "NastavenÃ­ uÅ¾ivatelskÃ©ho rozhranÃ­.",
  "PokroÄ�ilÃ© volby (detailnÃ­ obrazovky, kalibrace, tovÃ¡rnÃ­ nastavenÃ­...)"
};
#endif

#ifdef LANG_HUN
const char* SettingsDescriptions[17] = {
  // These are all the help text for all the settings.
  // No requirements on spacing or length
  /* Power source (DC or batt)          */ "Ã�ramforrÃ¡s. BeÃ¡llÃ­tja a lekapcsolÃ¡si feszÃ¼ltsÃ©get. <DC 10V> <S 3.3V cellÃ¡nkÃ©nt>",
  /* Sleep temperature                  */ "AlvÃ¡si hÅ‘mÃ©rsÃ©klet <C>",
  /* Sleep timeout                      */ "ElalvÃ¡si idÅ‘zÃ­tÅ‘ <Perc/MÃ¡sodperc>",
  /* Shutdown timeout                   */ "KikapcsolÃ¡si idÅ‘zÃ­tÅ‘ <Minutes>",
  /* Motion sensitivity level           */ "MozgÃ¡s Ã©rzÃ©kenysÃ©g beÃ¡llÃ­tÃ¡sa. <0.Ki 1.kevÃ©sbÃ© Ã©rzÃ©keny 9.legÃ©rzÃ©kenyebb>",
  /* Temperature in F and C             */ "HÅ‘mÃ©rsÃ©kleti egysÃ©g <C=Celsius F=Fahrenheit>",
  /* Advanced idle display mode enabled */ "RÃ©szletes informÃ¡ciÃ³ megjelenÃ­tÃ©se kisebb betÅ±mÃ©retben a kÃ©szenlÃ©ti kÃ©pernyÅ‘n.",
  /* Display rotation mode              */ "MegjelenÃ­tÃ©si tÃ¡jolÃ¡s <A. Automatikus L. Balkezes R. Jobbkezes>",
  /* Boost enabled                      */ "ElÃ¼lsÅ‘ gombbal lÃ©pjen boost mÃ³dba, 450C forrasztÃ¡s kÃ¶zben",
  /* Boost temperature                  */ "HÅ‘mÃ©rsÃ©klet \"boost\" mÃ³dban",
  /* Automatic start mode               */ "BekapcsolÃ¡s utÃ¡n automatikusan lÃ©pjen forrasztÃ¡s mÃ³dba. T=ForrasztÃ¡s, S=AlvÃ³ mÃ³d,F=Ki",
  /* Cooldown blink                     */ "Villogjon a hÅ‘mÃ©rsÃ©klet hÅ±lÃ©s kÃ¶zben, amÃ­g a hegy forrÃ³.",
  /* Temperature calibration enter menu */ "Hegy hÅ‘mÃ©rsÃ©kletÃ©nek kalibrÃ¡lÃ¡sa",
  /* Settings reset command             */ "BeÃ¡llÃ­tÃ¡sok alaphelyzetbe Ã¡llÃ­tÃ¡sa",
  /* Calibrate input voltage            */ "A bemeneti feszÃ¼ltsÃ©g kalibrÃ¡lÃ¡sa. RÃ¶viden megnyomva Ã¡llÃ­tsa be, hosszan nyomja meg a kilÃ©pÃ©shez.",
  /* Advanced soldering screen enabled  */ "RÃ©szletes informÃ¡ciÃ³k megjelenÃ­tÃ©se forrasztÃ¡s kÃ¶zben",
  /* Description Scroll Speed           */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "FolytatÃ¡s elÅ‘tt gyÅ‘zÅ‘dj meg rÃ³la, hogy a hegy szobahÅ‘mÃ©rsÃ©kletÅ±!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";               // <=8 chars
const char* UndervoltageString = "Undervoltage";        // <=16 chars
const char* InputVoltageString = "Input V: ";           // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";        // <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";                   // <=8 chars
const char* SleepingSimpleString = "Zzzz";              // Must be <= 4 chars
const char* SleepingAdvancedString = "AlvÃ¡s...";        // <=16 chars
const char* WarningSimpleString = "HOT!";               // Must be <= 4 chars
const char* WarningAdvancedString = "FIGYELEM! FORRÃ“ HEGY!";  // <=16 chars
const char* SleepingTipAdvancedString = "Tip:";         // <=6 chars
const char* IdleTipString = "Tip:";                     // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";                    // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED"; // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";   // <=12 chars
const char* OffString ="Off";                           // 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 5) Power source (DC or batt)          */ {"PWRSC"},
  /* (<= 4) Sleep temperature                  */ {"STMP"},
  /* (<= 4) Sleep timeout                      */ {"STME"},
  /* (<= 5) Shutdown timeout                   */ {"SHTME"},
  /* (<= 6) Motion sensitivity level           */ {"MSENSE"},
  /* (<= 6) Temperature in F and C             */ {"TMPUNT"},
  /* (<= 6) Advanced idle display mode enabled */ {"ADVIDL"},
  /* (<= 6) Display rotation mode              */ {"DSPROT"},
  /* (<= 6) Boost enabled                      */ {"BOOST"},
  /* (<= 4) Boost temperature                  */ {"BTMP"},
  /* (<= 6) Automatic start mode               */ {"ASTART"},
  /* (<= 6) Cooldown blink                     */ {"CLBLNK"},
  /* (<= 8) Temperature calibration enter menu */ {"TMP CAL?"},
  /* (<= 8) Settings reset command             */ {"RESET?"},
  /* (<= 8) Calibrate input voltage            */ {"CAL VIN?"},
  /* (<= 6) Advanced soldering screen enabled  */ {"ADVSLD"},
  /* (<= 6) Message Scroll Speed               */ {"DESCSP"},
};
const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"Soldering\nSettings",
/* Power Saving Menu*/"Sleep\nModes",
/* UI Menu*/"User\nInterface",
/* Advanced Menu*/"Advanced\nOptions", };
const char* SettingsMenuEntriesDescriptions[4] ={
"Soldering settings",
"Power Saving Settings",
"User Interface settings",
"Advanced options"
};
#endif

#ifdef LANG_DK
const char* SettingsDescriptions[17] = {
  // These are all the help text for all the settings.
  // No requirements on spacing or length
  /* Power source (DC or batt)          */ "StrÃ¸mforsyning. Indstil Cutoff SpÃ¦ndingen. <DC 10V <S 3.3V per cell",
  /* Sleep temperature                  */ "Dvale Temperatur <C",
  /* Sleep timeout                      */ "Dvale Timeout <Minutter/Sekunder",
  /* Shutdown timeout                   */ "sluknings Timeout <Minutter",
  /* Motion sensitivity level           */ "BevÃ¦gelsesfÃ¸lsomhed <0.Slukket 1.Mindst fÃ¸lsom 9.Mest fÃ¸lsom",
  /* Temperature in F and C             */ "Temperatur Enhed <C=Celsius F=Fahrenheit",
  /* Advanced idle display mode enabled */ "Vis detialieret information med en mindre skriftstÃ¸rrelse pÃ¥ standby skÃ¦rmen.",
  /* Display rotation mode              */ "SkÃ¦rm Orientering <A. Automatisk V. Venstre HÃ¥ndet H. HÃ¸jre HÃ¥ndet",
  /* Boost enabled                      */ "Ved tryk pÃ¥ front knap Aktiveres boost-funktionen, 450C tilstand nÃ¥r der loddes",
  /* Boost temperature                  */ "Temperatur i \"boost\" mode",
  /* Automatic start mode               */ "Start automatisk med lodning nÃ¥r strÃ¸m sÃ¦ttes til. L=Lodning, D= Dvale tilstand,S=Slukket",
  /* Cooldown blink                     */ "Blink temperaturen pÃ¥ skÃ¦rmen, mens spidsen stadig er varm.",
  /* Temperature calibration enter menu */ "kalibrere spids temperatur.",
  /* Settings reset command             */ "Gendan alle indstillinger",
  /* Calibrate input voltage            */ "VIN kalibrering. Knapperne justere, Lang tryk for at gÃ¥ ud",
  /* Advanced soldering screen enabled  */ "Vis detialieret information mens der loddes",
  /* Description Scroll Speed           */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "SÃ¸rg for at loddespidsen er ved stuetemperatur, inden du fortsÃ¦tter!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "Lav Volt";             // <=8 chars
const char* UndervoltageString = "Undervoltage";        // <=16 chars
const char* InputVoltageString = "Input V: ";           // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";        // <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";                   // <=8 chars
const char* SleepingSimpleString = "Zzzz";              // Must be <= 4 chars
const char* SleepingAdvancedString = "Dvale...";        // <=16 chars
const char* WarningSimpleString = "Varm";               // Must be <= 4 chars
const char* WarningAdvancedString = "ADVARSEL! VARM LODDESPIDS!"; // <=16 chars
const char* SleepingTipAdvancedString = "Tip:";         // <=6 chars
const char* IdleTipString = "Tip:";                     // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";                    // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED"; // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";   // <=12 chars
const char* OffString ="Off";                           // 3 chars max

const char SettingRightChar = 'H';
const char SettingLeftChar = 'V';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 5) Power source (DC or batt)          */ {"PWRSC"},
  /* (<= 4) Sleep temperature                  */ {"STMP"},
  /* (<= 4) Sleep timeout                      */ {"STME"},
  /* (<= 5) Shutdown timeout                   */ {"SHTME"},
  /* (<= 6) Motion sensitivity level           */ {"MSENSE"},
  /* (<= 6) Temperature in F and C             */ {"TMPUNT"},
  /* (<= 6) Advanced idle display mode enabled */ {"ADVIDL"},
  /* (<= 6) Display rotation mode              */ {"DSPROT"},
  /* (<= 6) Boost enabled                      */ {"BOOST"},
  /* (<= 4) Boost temperature                  */ {"BTMP"},
  /* (<= 6) Automatic start mode               */ {"ASTART"},
  /* (<= 6) Cooldown blink                     */ {"CLBLNK"},
  /* (<= 8) Temperature calibration enter menu */ {"TMP CAL?"},
  /* (<= 8) Settings reset command             */ {"RESET?"},
  /* (<= 8) Calibrate input voltage            */ {"CAL VIN?"},
  /* (<= 6) Advanced soldering screen enabled  */ {"ADVSLD"},
  /* (<= 6) Message Scroll Speed               */ {"DESCSP"},
};
const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"Soldering\nSettings",
/* Power Saving Menu*/"Sleep\nModes",
/* UI Menu*/"User\nInterface",
/* Advanced Menu*/"Advanced\nOptions", };
const char* SettingsMenuEntriesDescriptions[4] ={
"Soldering settings",
"Power Saving Settings",
"User Interface settings",
"Advanced options"
};
#endif

#ifdef LANG_PL
const char* SettingsDescriptions[17] = {
    // These are all the help text for all the settings.
    // No requirements on spacing or length
    /* Power source (DC or batt)          */ "Å¹rÃ³dÅ‚o zasilania. Ustaw napiÄ™cie odciÄ™cia. <DC 10V> <S 3.3V dla ogniw Li>",
    /* Sleep temperature                  */ "Temperatura uÅ›pienia <Â°C>",
    /* Sleep timeout                      */ "Czas uÅ›pienia <Minuty/Sekundy>",
    /* Shutdown timeout                   */ "Czas wyÅ‚Ä…czenia <Minuty>",
    /* Motion sensitivity level           */ "CzuÅ‚oÅ›Ä‡ ruchu <0.WyÅ‚Ä…czona 1.minimalna 9.maksymalna>",
    /* Temperature in F and C             */ "Jednostka temperatury <C=Celsius F=Fahrenheit>",
    /* Advanced idle display mode enabled */ "WyÅ›wietla szczegÃ³Å‚owe informacje za pomocÄ… mniejszej czcionki na ekranie bezczynnoÅ›Ä‡i <T = wÅ‚., N = wyÅ‚.>",
    /* Display rotation mode              */ "Orientacja wyÅ›wietlacza <A. Automatyczna L. LeworÄ™czna P. PraworÄ™czna>",
    /* Boost enabled                      */ "UÅ¼yj przycisku przedniego w celu zwiÄ™kszenia temperatury <T = wÅ‚., N = wyÅ‚.>",
    /* Boost temperature                  */ "Temperatura w trybie \"boost\" ",
    /* Automatic start mode               */ "Automatyczne uruchamianie trybu lutowania po wÅ‚Ä…czeniu zasilania. T=Lutowanie, S= Tryb Uspienia ,N=WyÅ‚Ä…cz",
    /* Cooldown blink                     */ "Temperatura na ekranie miga, gdy grot jest jeszcze gorÄ…cy. <T = wÅ‚., N = wyÅ‚.>",
    /* Temperature calibration enter menu */ "Kalibracja temperatury grota lutownicy",
    /* Settings reset command             */ "Zresetuj wszystkie ustawienia",
    /* Calibrate input voltage            */ "Kalibracja napiÄ™cia wejÅ›ciowego. KrÃ³tkie naciÅ›niÄ™cie, aby ustawiÄ‡, dÅ‚ugie naciÅ›niÄ™cie, aby wyjÅ›Ä‡.",
    /* Advanced soldering screen enabled  */ "WyÅ›wietl szczegÃ³Å‚owe informacje podczas lutowania <T = wÅ‚., N = wyÅ‚.>",
    /* Description Scroll Speed           */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Przed kontynuowaniem upewnij siÄ™, Å¼e koÅ„cÃ³wka osiÄ…gnela temperature pokojowa!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";               // <=8 chars
const char* UndervoltageString = "Undervoltage";        // <=16 chars
const char* InputVoltageString = "Input V: ";           // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";        // <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";                   // <=8 chars
const char* SleepingSimpleString = "Zzz!";              // Must be <= 4 chars
const char* SleepingAdvancedString = "Uspienie...";     // <=16 chars
const char* WarningSimpleString = "HOT!";               // Must be <= 4 chars
const char* WarningAdvancedString = "UWAGA! GORÄ„CA KOÅƒCÃ“WKA!"; // <=16 chars
const char* SleepingTipAdvancedString = "Tip:";         // <=6 chars
const char* IdleTipString = "Tip:";                     // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";                    // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED"; // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";   // <=12 chars
const char* OffString ="Off";                           // 3 chars max

const char SettingRightChar = 'P';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 5) Power source (DC or batt)          */ {"PWRSC"},
  /* (<= 4) Sleep temperature                  */ {"STMP"},
  /* (<= 4) Sleep timeout                      */ {"STME"},
  /* (<= 5) Shutdown timeout                   */ {"SHTME"},
  /* (<= 6) Motion sensitivity level           */ {"MSENSE"},
  /* (<= 6) Temperature in F and C             */ {"TMPUNT"},
  /* (<= 6) Advanced idle display mode enabled */ {"ADVIDL"},
  /* (<= 6) Display rotation mode              */ {"DSPROT"},
  /* (<= 6) Boost enabled                      */ {"BOOST"},
  /* (<= 4) Boost temperature                  */ {"BTMP"},
  /* (<= 6) Automatic start mode               */ {"ASTART"},
  /* (<= 6) Cooldown blink                     */ {"CLBLNK"},
  /* (<= 8) Temperature calibration enter menu */ {"TMP CAL?"},
  /* (<= 8) Settings reset command             */ {"RESET?"},
  /* (<= 8) Calibrate input voltage            */ {"CAL VIN?"},
  /* (<= 6) Advanced soldering screen enabled  */ {"ADVSLD"},
  /* (<= 6) Message Scroll Speed               */ {"DESCSP"},
};
const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"Soldering\nSettings",
/* Power Saving Menu*/"Sleep\nModes",
/* UI Menu*/"User\nInterface",
/* Advanced Menu*/"Advanced\nOptions", };
const char* SettingsMenuEntriesDescriptions[4] ={
"Soldering settings",
"Power Saving Settings",
"User Interface settings",
"Advanced options"
};
#endif

#ifdef LANG_BR
const char* SettingsDescriptions[17] = {
  // These are all the help text for all the settings.
  // No requirements on spacing or length.
  /* Power source (DC or batt)          */ "Fonte de alimentação. Define a tensão de corte. <DC 10V> <S 3.3V por célula>",
  /* Sleep temperature                  */ "Temperatura de repouso <C>",
  /* Sleep timeout                      */ "Tempo para repouso <Minutos/Segundos>",
  /* Shutdown timeout                   */ "Tempo para desligamento <Minutos>",
  /* Motion sensitivity level           */ "Sensibilidade ao movimento <0=Desligado 1=Menor 9=Maior>",
  /* Temperature in F and C             */ "Unidade de temperatura <C=Celsius F=Fahrenheit>",
  /* Advanced idle display mode enabled */ "Exibe informações avançadas quando ocioso",
  /* Display rotation mode              */ "Orientação da tela <A.utomática C.anhoto D.estro>",
  /* Boost enabled                      */ "Tecla frontal ativa modo \"turbo\"",
  /* Boost temperature                  */ "Ajuste de temperatura do modo \"turbo\"",
  /* Automatic start mode               */ "Temperatura de aquecimento ao ligar <T=Trabalho S=Repouso F=Desligado>",
  /* Cooldown blink                     */ "Exibe a temperatura durante o resfriamento",
  /* Temperature calibration enter menu */ "Calibra a temperatura",
  /* Settings reset command             */ "Reverte todos ajustes",
  /* Calibrate input voltage            */ "Calibra a tensão e configura os botões. Mantenha presionado para sair",
  /* Advanced soldering screen enabled  */ "Exibe informações avançadas durante o uso",
  /* Description Scroll Speed           */ "Velocidade que esse texto é exibido",
};

const char* SettingsCalibrationWarning = "A ponta deve estar em temperatura ambiente antes de continuar!";
const char* SettingsResetWarning = "Resetar para ajustes de fábrica?";
const char* UVLOWarningString = "DC BAIXO";           // <=8 chars
const char* UndervoltageString = "Subtensão";         // <=16 chars
const char* InputVoltageString = "Tensão ";           // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Temperatura ";    // <=12 chars, preferably end with a space
const char* BadTipString = "ER PONTA";                // <=8 chars
const char* SleepingSimpleString = "Zzzz";            // Must be <= 4 chars
const char* SleepingAdvancedString = "Repouso...";    // <=16 chars
const char* WarningSimpleString = "TEMP";             // Must be <= 4 chars
const char* WarningAdvancedString = "TEMP ELEVADA!";  // <=16 chars
const char* SleepingTipAdvancedString = "Ponta:";     // <=6 chars
const char* IdleTipString = "Ponta:";                 // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Aj:";                   // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "SEM PONTA";      // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: "; // <=12 chars
const char* OffString ="Off";                         // 3 chars max

const char SettingRightChar = 'D';
const char SettingLeftChar  = 'C';
const char SettingAutoChar  = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 5) Power source (DC or batt)          */ {"FONTE"},
  /* (<= 4) Sleep temperature                  */ {"TMPE"},
  /* (<= 4) Sleep timeout                      */ {"TMPO"},
  /* (<= 5) Shutdown timeout                   */ {"DESLI"},
  /* (<= 6) Motion sensitivity level           */ {"MOVIME"},
  /* (<= 6) Temperature in F and C             */ {"UNIDAD"},
  /* (<= 6) Advanced idle display mode enabled */ {"OCIOSO"},
  /* (<= 6) Display rotation mode              */ {"ORIENT"},
  /* (<= 6) Boost enabled                      */ {"TURBO"},
  /* (<= 4) Boost temperature                  */ {"TTMP"},
  /* (<= 6) Automatic start mode               */ {"MODOAT"},
  /* (<= 6) Cooldown blink                     */ {"RESFRI"},
  /* (<= 8) Temperature calibration enter menu */ {"CAL.TEMP"},
  /* (<= 8) Settings reset command             */ {"RESETAR"},
  /* (<= 8) Calibrate input voltage            */ {"CAL.VOLT"},
  /* (<= 6) Advanced soldering screen enabled  */ {"AVNCAD"},
  /* (<= 6) Message Scroll Speed               */ {"VELTEX"},
};
const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"Configurações\nSolda",
/* Power Saving Menu*/"Modos\nSuspensão",
/* UI Menu*/"Interface\nUsuário",
/* Advanced Menu*/"Menu\nAvançado", };
const char* SettingsMenuEntriesDescriptions[4] ={
"Configurações de solda",
"Configurações de economia de energia",
"Configurações da interface do usuário",
"Opções avançadas"
};
#endif

#ifdef LANG_LT
const char* SettingsDescriptions[17] = {
  // These are all the help text for all the settings.
  // No requirements on spacing or length.
  /* Power source (DC or batt)          */ "IÅ¡jungimo Ä¯tampa. <DC 10V arba celiÅ³ (S) kiekis (3.3V per celÄ™)>",
  /* Sleep temperature                  */ "Miego temperatÅ«ra <C>",
  /* Sleep timeout                      */ "Miego laikas <minutÄ—s/sekundÄ—s>",
  /* Shutdown timeout                   */ "IÅ¡jungimo laikas <minutÄ—s>",
  /* Motion sensitivity level           */ "Jautrumas judesiui <0 - iÅ¡jungta, 1 - maÅ¾iausias, 9 - didÅ¾iausias>",
  /* Temperature in F and C             */ "TemperatÅ«ros vienetai <C - Celsijus, F - Farenheitas>",
  /* Advanced idle display mode enabled */ "Ar rodyti papildomÄ… informacijÄ… maÅ¾esniu Å¡riftu laukimo ekrane",
  /* Display rotation mode              */ "Ekrano orientacija <A - automatinÄ—, K - kairiarankiams, D - deÅ¡iniarankiams>",
  /* Boost enabled                      */ "Ar lituojant virÅ¡utinis mygtukas Ä¯jungia turbo reÅ¾imÄ…",
  /* Boost temperature                  */ "TemperatÅ«ra turbo reÅ¾imu",
  /* Automatic start mode               */ "Ar pradÄ—ti kaitininti iÅ¡ karto Ä¯jungus lituoklÄ¯",
  /* Cooldown blink                     */ "Ar mirksÄ—ti temperatÅ«rÄ… ekrane kol vÄ—stantis antgalis vis dar karÅ¡tas",
  /* Temperature calibration enter menu */ "Antgalio temperatÅ«ros kalibravimas",
  /* Settings reset command             */ "Nustatyti nustatymus iÅ¡ naujo",
  /* Calibrate input voltage            */ "Ä®vesties Ä¯tampos kalibravimas. Trumpai paspauskite, norÄ—dami nustatyti, ilgai paspauskite, kad iÅ¡eitumÄ—te",
  /* Advanced soldering screen enabled  */ "Ar rodyti iÅ¡samiÄ… informacijÄ… lituojant",
  /* Description Scroll Speed           */ "Greitis, kuriuo Å¡is tekstas slenka",
};

const char* SettingsCalibrationWarning = "PrieÅ¡ tÄ™sdami Ä¯sitikinkite, kad antgalis yra kambario temperatÅ«ros!";
const char* SettingsResetWarning = "Ar norite atstatyti nustatymus Ä¯ numatytas reikÅ¡mes?";
const char* UVLOWarningString = "MAÅ½ VOLT";                // <=8 chars
const char* UndervoltageString = "Å½ema Ä¯tampa";            // <=16 chars
const char* InputVoltageString = "Ä®vestis V: ";            // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Antgl Temp: ";         // <=12 chars, preferably end with a space
const char* BadTipString = "BLOG ANT";                     // <=8 chars
const char* SleepingSimpleString = "Zzzz";                 // Must be <= 4 chars
const char* SleepingAdvancedString = "Miegu...";           // <=16 chars
const char* WarningSimpleString = "KRÅ !";                  // Must be <= 4 chars
const char* WarningAdvancedString = "ANTGALIS KARÅ TAS";    // <=16 chars
const char* SleepingTipAdvancedString = "Antgl:";          // <=6 chars
const char* IdleTipString = "Ant:";                        // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Nust:";                      // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "ANTGAL ATJUNGTAS";    // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Maitinimas: "; // <=12 chars
const char* OffString ="IÅ¡j";                              // 3 chars max

const char SettingRightChar = 'D';
const char SettingLeftChar = 'K';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'T';
const char SettingSlowChar = 'N';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Maitinimo", "Å¡altinis"},
  /* (<=  9) Sleep temperature                  */ {"Miego", "temperat."},
  /* (<=  9) Sleep timeout                      */ {"Miego", "laikas"},
  /* (<= 10) Shutdown timeout                   */ {"IÅ¡jungimo", "laikas"},
  /* (<= 13) Motion sensitivity level           */ {"Jautrumas", "judesiui"},
  /* (<= 13) Temperature in F and C             */ {"TemperatÅ«ros", "vienetai"},
  /* (<= 13) Advanced idle display mode enabled */ {"Detalus lauki-", "mo ekranas"},
  /* (<= 13) Display rotation mode              */ {"Ekrano", "orientacija"},
  /* (<= 13) Boost enabled                      */ {"Turbo reÅ¾imas", "Ä¯jungtas"},
  /* (<=  9) Boost temperature                  */ {"Turbo", "temperat."},
  /* (<= 13) Automatic start mode               */ {"Auto", "paleidimas"},
  /* (<= 13) Cooldown blink                     */ {"AtvÄ—simo", "mirksÄ—jimas"},
  /* (<= 16) Temperature calibration enter menu */ {"Kalibruoti", "temperatÅ«rÄ…?"},
  /* (<= 16) Settings reset command             */ {"Atstatyti", "nustatymus?"},
  /* (<= 16) Calibrate input voltage            */ {"Kalibruoti", "Ä¯vesties Ä¯tampÄ…?"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Detalus lita-", "vimo ekranas"},
  /* (<= 11) Display Help Text Scroll Speed     */ {"Greitas apra-","Å¡ymo slinkimas"},
};

const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"Litavimo\nnustatymai",
/* Power Saving Menu*/"Miego\nreÅ¾imai",
/* UI Menu*/"Naudotojo\nsÄ…saja",
/* Advanced Menu*/"IÅ¡plÄ—stiniai\nnustatymai", };
const char* SettingsMenuEntriesDescriptions[4] ={
"Litavimo nustatymai",
"Energijos vartojimo nustatymai",
"Naudotojo sÄ…sajos nustatymai",
"IÅ¡plÄ—stiniai nustatymai"
};
#endif

#ifdef LANG_UA
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "Ð£Ñ�Ñ‚Ð°Ð½Ð¾Ð²ÐºÐ° Ð½Ð°Ð¿Ñ€ÑƒÐ³Ð¸ Ð²Ñ–Ð´ÐºÐ»ÑŽÑ‡ÐµÐ½Ð½Ñ�. <DC - 10V, 3S - 9.9V, 4S - 13.2V, 5S - 16.5V, 6S - 19.8V>",
	/* Sleep temperature                  */ "Ð¢ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð° Ñ€ÐµÐ¶Ð¸Ð¼Ñƒ Ð¾Ñ‡Ñ–ÐºÑƒÐ²Ð°Ð½Ð½Ñ� <CÂ°/FÂ°>",
	/* Sleep timeout                      */ "Ð§Ð°Ñ� Ð´Ð¾ Ð¿ÐµÑ€ÐµÑ…Ð¾Ð´Ñƒ Ð² Ñ€ÐµÐ¶Ð¸Ð¼ Ð¾Ñ‡Ñ–ÐºÑƒÐ²Ð°Ð½Ð½Ñ� <Ð”Ð¾Ñ�Ñ‚ÑƒÐ¿Ð½Ð¾ Ð¾Ñ‚ÐºÐ»ÑŽÑ‡ÐµÐ½Ð¸Ðµ>",
	/* Shutdown timeout                   */ "Ð§Ð°Ñ� Ð´Ð¾ Ð²Ñ–Ð´ÐºÐ»ÑŽÑ‡ÐµÐ½Ð½Ñ� <Ð”Ð¾Ñ�Ñ‚ÑƒÐ¿Ð½Ð¾ Ð²Ñ–Ð´ÐºÐ»ÑŽÑ‡ÐµÐ½Ð½Ñ�>",
	/* Motion sensitivity level           */ "Ð�ÐºÑ�ÐµÐ»ÐµÑ€Ð¾Ð¼ÐµÑ‚Ñ€ <0 - Ð’Ð¸ÐºÐ». 1 Ñ…Ð². Ñ‡ÑƒÑ‚Ð»Ð¸Ð²Ð¾Ñ�Ñ‚Ñ– 9 - Ð¼Ð°ÐºÑ�. Ñ‡ÑƒÑ‚Ð»Ð¸Ð²Ð¾Ñ�Ñ‚Ñ–>",
	/* Temperature in F and C             */ "ÐžÐ´Ð¸Ð½Ð¸Ñ†Ñ� Ð²Ð¸Ð¼Ñ–Ñ€Ñƒ Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð¸ <C - Ð¦ÐµÐ»ÑŒÑ�Ñ–Ð¹, F - Ð¤Ð°Ñ€ÐµÐ½Ð³ÐµÐ¹Ñ‚>",
	/* Advanced idle display mode enabled */ "ÐŸÐ¾ÐºÐ°Ð·ÑƒÐ²Ð°Ñ‚Ð¸ Ð´ÐµÑ‚Ð°Ð»ÑŒÐ½Ñƒ Ð² Ñ€ÐµÐ¶Ð¸Ð¼Ñ– Ð½Ð°Ð»Ð°ÑˆÑ‚ÑƒÐ²Ð°Ð½ÑŒ <Ð—Ð°Ð¼Ñ–Ñ�Ñ‚ÑŒ ÐºÐ°Ñ€Ñ‚Ð¸Ð½ÐºÐ¸>",
	/* Display rotation mode              */ "ÐžÑ€Ñ–Ñ”Ð½Ñ‚Ð°Ñ†Ñ–Ñ� Ð´Ð¸Ñ�Ð¿Ð»ÐµÑ� <A - Ð�Ð²Ñ‚Ð¾Ð¿Ð¾Ð²Ð¾Ñ€Ð¾Ñ‚, L - Ð›Ñ–Ð²ÑˆÐ°, R - ÐŸÑ€Ð°Ð²ÑˆÐ°>",
	/* Boost enabled                      */ "Ð¢ÑƒÑ€Ð±Ð¾-Ñ€ÐµÐ¶Ð¸Ð¼ Ð¿Ñ€Ð¸ ÑƒÑ‚Ñ€Ð¸Ð¼Ð°Ð½Ð½Ñ– ÐºÐ½Ð¾Ð¿ÐºÐ¸ Ð� Ð¿Ñ€Ð¸ Ð¿Ð°Ð¹Ñ†Ñ–",
	/* Boost temperature                  */ "Ð¢ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð° Ð² Ð¢ÑƒÑ€Ð±Ð¾-Ñ€ÐµÐ¶Ð¸Ð¼Ñ–",
	/* Automatic start mode               */ "Ð�Ð²Ñ‚Ð¾Ð¼Ð°Ñ‚Ð¸Ñ‡Ð½Ð¸Ð¹ Ð¿ÐµÑ€ÐµÑ…Ñ–Ð´ Ð² Ñ€ÐµÐ¶Ð¸Ð¼ Ð¿Ð°Ð¹ÐºÐ¸ Ð¿Ñ€Ð¸ Ð²ÐºÐ»ÑŽÑ‡ÐµÐ½Ð½Ñ– Ð¶Ð¸Ð²Ð»ÐµÐ½Ð½Ñ�.",
	/* Cooldown blink                     */ "ÐŸÐ¾ÐºÐ°Ð·ÑƒÐ²Ð°Ñ‚Ð¸ Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ñƒ Ð½Ð° ÐµÐºÑ€Ð°Ð½Ñ– Ð¾Ñ…Ð¾Ð»Ð¾Ð´Ð¶ÐµÐ½Ð½Ñ�, Ð¿Ð¾ÐºÐ¸ Ð¶Ð°Ð»Ð¾ Ð·Ð°Ð»Ð¸ÑˆÐ°Ñ”Ñ‚ÑŒÑ�Ñ� Ð³Ð°Ñ€Ñ�Ñ‡Ð¸Ð¼, Ð¿Ñ€Ð¸ Ñ†ÑŒÐ¾Ð¼Ñƒ ÐµÐºÑ€Ð°Ð½ Ð¼Ð¾Ñ€Ð³Ð°Ñ”",
	/* Temperature calibration enter menu */ "ÐšÐ°Ð»Ñ–Ð±Ñ€ÑƒÐ²Ð°Ð½Ð½Ñ� Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð½Ð¾Ð³Ð¾ Ð´Ð°Ñ‚Ñ‡Ð¸ÐºÐ°.",
	/* Settings reset command             */ "Ð¡ÐºÐ¸Ð´Ð°Ð½Ð½Ñ� Ð²Ñ�Ñ–Ñ… Ð¿Ð°Ñ€Ð°Ð¼ÐµÑ‚Ñ€Ñ–Ð² Ð´Ð¾ Ð²Ð¸Ñ…Ñ–Ð´Ð½Ð¸Ñ… Ð·Ð½Ð°Ñ‡ÐµÐ½ÑŒ.",
	/* Calibrate input voltage            */ "ÐšÐ°Ð»Ñ–Ð±Ñ€ÑƒÐ²Ð°Ð½Ð½Ñ� Ð½Ð°Ð¿Ñ€ÑƒÐ³Ð¸ Ð²Ñ…Ð¾Ð´Ñƒ. Ð�Ð°Ð»Ð°ÑˆÑ‚ÑƒÐ²Ð°Ñ‚Ð¸ ÐºÐ½Ð¾Ð¿ÐºÐ°Ð¼Ð¸, Ð½Ð°Ñ‚Ð¸Ñ�Ð½ÑƒÑ‚Ð¸ Ñ– ÑƒÑ‚Ñ€Ð¸Ð¼Ð°Ñ‚Ð¸ Ñ‰Ð¾Ð± Ð·Ð°Ð²ÐµÑ€ÑˆÐ¸Ñ‚Ð¸.",
	/* Advanced soldering screen enabled  */ "ÐŸÐ¾ÐºÐ°Ð·ÑƒÐ²Ð°Ñ‚Ð¸ Ð´ÐµÑ‚Ð°Ð»ÑŒÐ½Ñƒ Ñ–Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ñ–ÑŽ Ð¿Ñ€Ð¸ Ð¿Ð°Ð¹Ñ†Ñ–.",
	/* Description Scroll Speed           */ "Ð¨Ð²Ð¸Ð´ÐºÑ–Ñ�Ñ‚ÑŒ Ð¿Ñ€Ð¾ÐºÑ€ÑƒÑ‚ÐºÐ¸ Ñ‚ÐµÐºÑ�Ñ‚Ñƒ",
};

const char* SettingsCalibrationWarning = "ÐŸÐµÑ€ÐµÐºÐ¾Ð½Ð°Ð¹Ñ‚ÐµÑ�Ñ�, Ñ‰Ð¾ Ð¶Ð°Ð»Ð¾ Ð¾Ñ…Ð¾Ð»Ð¾Ð»Ð¾ Ð´Ð¾ ÐºÑ–Ð¼Ð½Ð°Ñ‚Ð½Ð¾Ñ— Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð¸, Ð¿ÐµÑ€Ñˆ Ð½Ñ–Ð¶ Ð¿Ñ€Ð¾Ð´Ð¾Ð²Ð¶ÑƒÐ²Ð°Ñ‚Ð¸!";
const char* SettingsResetWarning = "Ð’Ð¸ Ð´Ñ–Ð¹Ñ�Ð½Ð¾ Ñ…Ð¾Ñ‡ÐµÑ‚Ðµ Ñ�ÐºÐ¸Ð½ÑƒÑ‚Ð¸ Ð½Ð°Ð»Ð°ÑˆÑ‚ÑƒÐ²Ð°Ð½Ð½Ñ� Ð´Ð¾ Ð·Ð½Ð°Ñ‡ÐµÐ½ÑŒ Ð·Ð° Ð·Ð°Ð¼Ð¾Ð²Ñ‡ÑƒÐ²Ð°Ð½Ð½Ñ�Ð¼?";
const char* UVLOWarningString = "Ð�ÐšÐšÐ£Ðœ--";              // <=8 chars
const char* UndervoltageString = "ÐŸÑ–Ð´ Ð¶Ð¸Ð²Ð»ÐµÐ½Ð½Ñ�Ð¼";       // <=16 chars
const char* InputVoltageString = "Ð–Ð¸Ð².(B):";		// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Ð–Ð°Ð»Ð¾ tÂ°: ";         // <=12 chars, preferably end with a space
const char* BadTipString = "Ð–Ð°Ð»Ð¾--";                    // <=8 chars
const char* SleepingSimpleString = "Ð¡Ð¾Ð½ ";              // Must be <= 4 chars
const char* SleepingAdvancedString = "ÐžÑ‡Ñ–ÐºÑƒÐ²Ð°Ð½Ð½Ñ�...";   // <=16 chars
const char* WarningSimpleString = " Ð�Ð™!";               // Must be <= 4 chars
const char* WarningAdvancedString = "Ð£Ð’Ð�Ð“Ð� Ð“Ð�Ð Ð¯Ð§Ð•!"; 	// <=16 chars
const char* SleepingTipAdvancedString = "Ð–Ð°Ð»Ð¾:";        // <=6 chars
const char* IdleTipString = "Ð–Ð°Ð»Ð¾:";                    // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " ->";                  	// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "Ð–Ð°Ð»Ð¾ Ð²Ð¸Ð¼ÐºÐ½ÐµÐ½Ð¾!";   // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Ð–Ð¸Ð²Ð»ÐµÐ½Ð½Ñ�: ";// <=12 chars
const char* OffString ="Ð’Ð¸Ð¼";                           // 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = '+';
const char SettingSlowChar = '-';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Ð”Ð¶ÐµÑ€ÐµÐ»Ð¾","Ð¶Ð¸Ð²Ð»ÐµÐ½Ð½Ñ�"},		//7,8
  /* (<=  9) Sleep temperature                  */ {"Ð¢ÐµÐ¼Ð¿ÐµÑ€.","Ñ�Ð½Ñƒ"},			//7,3
  /* (<=  9) Sleep timeout                      */ {"Ð¢Ð°Ð¹Ð¼Ð°ÑƒÑ‚","Ñ�Ð½Ñƒ"},			//7,3
  /* (<= 10) Shutdown timeout                   */ {"Ð§Ð°Ñ�Ñƒ Ð´Ð¾","Ð²Ð¸Ð¼ÐºÐ½ÐµÐ½Ð½Ñ�"},		//7,9
  /* (<= 13) Motion sensitivity level           */ {"Ð§ÑƒÑ‚Ð». Ñ�ÐµÐ½Ñ�Ð¾-","Ñ€Ñƒ Ñ€ÑƒÑ…Ñƒ"},		//12,7
  /* (<= 13) Temperature in F and C             */ {"Ð¤Ð¾Ñ€Ð¼Ð°Ñ‚ Ñ‚ÐµÐ¼Ð¿Ðµ-","Ñ€Ð°Ñ‚ÑƒÑ€Ð¸(CÂ°/FÂ°)"},	//13,13
  /* (<= 13) Advanced idle display mode enabled */ {"Ð”ÐµÑ‚Ð°Ð»ÑŒÐ½Ð¸Ð¹ Ñ€Ðµ-","Ð¶Ð¸Ð¼ Ð¾Ñ‡Ñ–ÐºÑƒÐ²Ð°Ð½."},	//13,13
  /* (<= 13) Display rotation mode              */ {"Ð�Ð²Ñ‚Ð¾Ð¿Ð¾Ð²Ð¾Ñ€Ð¾Ñ‚","ÐµÐºÑ€Ð°Ð½Ñƒ"},		//11,6
  /* (<= 13) Boost enabled                      */ {"Ð ÐµÐ¶Ð¸Ð¼","Ð¢ÑƒÑ€Ð±Ð¾"},			//5,5
  /* (<=  9) Boost temperature                  */ {"Ð¢ÐµÐ¼Ð¿ÐµÑ€.","Ð¢ÑƒÑ€Ð±Ð¾"},                 //7,5
  /* (<= 13) Automatic start mode               */ {"Ð“Ð°Ñ€Ñ�Ñ‡Ð¸Ð¹","Ñ�Ñ‚Ð°Ñ€Ñ‚"},			//7,5
  /* (<= 13) Cooldown blink                     */ {"ÐŸÐ¾ÐºÐ°Ð· tÂ° Ð¿Ñ€Ð¸","Ð¾Ñ…Ð¾Ð»Ð¾Ð´Ð¶."},		//12,8
  /* (<= 16) Temperature calibration enter menu */ {"ÐšÐ°Ð»Ñ–Ð±Ñ€Ð¾Ð²ÐºÐ°","Ñ‚ÐµÐ¼Ð¿ÐµÑ€Ð°Ñ‚ÑƒÑ€Ð¸"},	//10,11
  /* (<= 16) Settings reset command             */ {"Ð¡ÐºÐ¸Ð½ÑƒÑ‚Ð¸ Ð²Ñ�Ñ–","Ð½Ð°Ð»Ð°ÑˆÑ‚ÑƒÐ²Ð°Ð½Ð½Ñ�?"},	//11,13
  /* (<= 16) Calibrate input voltage            */ {"ÐšÐ°Ð»Ñ–Ð±Ñ€Ð¾Ð²ÐºÐ°","Ð½Ð°Ð¿Ñ€ÑƒÐ³Ð°"},		//10,7
  /* (<= 13) Advanced soldering screen enabled  */ {"Ð”ÐµÑ‚Ð°Ð»ÑŒÐ½Ð¸Ð¹ Ñ€Ðµ-","Ð¶Ð¸Ð¼ Ð¿Ð°Ð¹ÐºÐ¸"},	//13,9
  /* (<= 11) Message Scroll Speed               */ {"Ð¨Ð²Ð¸Ð´ÐºÑ–Ñ�Ñ‚ÑŒ","Ñ‚ÐµÐºÑ�Ñ‚Ñƒ"},		//9,6
};

const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"ÐŸÐ°Ð¹ÐºÐ°",
/* Power Saving Menu*/"Ð¡Ð¾Ð½",
/* UI Menu*/"Ð†Ð½Ñ‚ÐµÑ€Ñ„ÐµÐ¹Ñ�",
/* Advanced Menu*/"Ð†Ð½ÑˆÑ–",
};

const char* SettingsMenuEntriesDescriptions[4] ={
"Ð�Ð°Ð»Ð°ÑˆÑ‚ÑƒÐ²Ð°Ð½Ð½Ñ� Ð´Ð»Ñ� Ñ€ÐµÐ¶Ð¸Ð¼Ñƒ Ð¿Ð°Ð¹ÐºÐ¸. Ð”Ñ–ÑŽÑ‚ÑŒ Ð¿Ñ€Ð¸ Ð²ÐºÐ»ÑŽÑ‡ÐµÐ½Ð¾Ð¼Ñƒ Ð¶Ð°Ð»Ñ–.",
"Ð�Ð°Ð»Ð°ÑˆÑ‚ÑƒÐ²Ð°Ð½Ð½Ñ� Ð¿Ñ€Ð¸ Ð±ÐµÐ·Ð´Ñ–Ñ�Ð»ÑŒÐ½Ð¾Ñ�Ñ‚Ñ–. ÐšÐ¾Ñ€Ð¸Ñ�Ð½Ð¾ Ñ‰Ð¾ Ð± Ð½Ðµ Ð¾Ð±Ð¿ÐµÐºÑ‚Ð¸Ñ�Ñ� Ñ– Ð· Ñ‡Ð°Ñ�Ð¾Ð¼ Ð½Ðµ Ñ�Ð¿Ð°Ð»Ð¸Ñ‚Ð¸ Ð¶Ð¸Ñ‚Ð»Ð¾.",
"ÐšÐ¾Ñ€Ð¸Ñ�Ñ‚ÑƒÐ²Ð°Ð»ÑŒÐ½Ð¸Ñ†ÑŒÐºÐ¸Ð¹ Ñ–Ð½Ñ‚ÐµÑ€Ñ„ÐµÐ¹Ñ�.",
"Ð Ð¾Ð·ÑˆÐ¸Ñ€ÐµÐ½Ñ– Ð½Ð°Ð»Ð°ÑˆÑ‚ÑƒÐ²Ð°Ð½Ð½Ñ�. Ð”Ð¾Ð´Ð°Ñ‚ÐºÐ¾Ð²Ñ– Ð·Ñ€ÑƒÑ‡Ð½Ð¾Ñ�Ñ‚Ñ–."
};
#endif

#ifdef LANG_NL
const char* SettingsDescriptions[17] = {
  // These are all the help text for all the settings.
  // No requirements on spacing or length.
  /* Power source (DC or batt)          */ "Spanningsbron. Stelt drempelspanning in. <DC 10V> <S 3.3V per cel>",
  /* Sleep temperature                  */ "Temperatuur in slaapstand <C>",
  /* Sleep timeout                      */ "Slaapstand time-out <Minuten/Seconden>",
  /* Shutdown timeout                   */ "Automatisch afsluiten time-out <Minuten>",
  /* Motion sensitivity level           */ "Bewegingsgevoeligheid <0.uit 1.minst gevoelig 9.meest gevoelig>",
  /* Temperature in F and C             */ "Temperatuureenheid <C=Celsius F=Fahrenheit>",
  /* Advanced idle display mode enabled */ "Gedetailleerde informatie weergeven in een kleiner lettertype op het slaapscherm.",
  /* Display rotation mode              */ "SchermoriÃ«ntatie <A. Automatisch L. Linkshandig R. Rechtshandig>",
  /* Boost enabled                      */ "Soldeerbout gaat naar een hogere boost-temperatuur wanneer de voorste knop ingedrukt is.",
  /* Boost temperature                  */ "Temperatuur in boostmodes",
  /* Automatic start mode               */ "Breng de soldeerbout direct op temperatuur bij het opstarten. T=Soldeertemperatuur, S=Slaapstand-temperatuur, F=Uit",
  /* Cooldown blink                     */ "Temperatuur laten flikkeren in het hoofdmenu als de soldeerpunt aan het afkoelen is.",
  /* Temperature calibration enter menu */ "Temperatuursafwijking van de soldeerpunt calibreren.",
  /* Settings reset command             */ "Alle instellingen terugzetten.",
  /* Calibrate input voltage            */ "VIN Calibreren. Knoppen lang ingedrukt houden om te bevestigen.",
  /* Advanced soldering screen enabled  */ "Gedetailleerde informatie weergeven in een kleiner lettertype op het soldeerscherm.",
  /* Description Scroll Speed           */ "Snelheid waarmee de tekst scrolt.",
};

const char* SettingsCalibrationWarning = "Zorg ervoor dat de soldeerpunt op kamertemperatuur is voor je verdergaat!";
const char* SettingsResetWarning = "Ben je zeker dat je alle instellingen naar de standaardwaardes wilt terugzetten?";
const char* UVLOWarningString = "DC LAAG";               // <=8 chars
const char* UndervoltageString = "Onderspanning";        // <=16 chars
const char* InputVoltageString = "Input V: ";           // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";        // <=12 chars, preferably end with a space
const char* BadTipString = "DEFECT";                   // <=8 chars
const char* SleepingSimpleString = "Zzz ";              // Must be <= 4 chars
const char* SleepingAdvancedString = "Slaapstand...";     // <=16 chars
const char* WarningSimpleString = "HEET!";               // Must be <= 4 chars
const char* WarningAdvancedString = "!!! TIP HEET !!!";  // <=16 chars
const char* SleepingTipAdvancedString = "Tip:";         // <=6 chars
const char* IdleTipString = "Tip:";                     // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";                    // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP LOSGEKOPPELD"; // <=16 chars
const char* SolderingAdvancedPowerPrompt = "vermogen: ";   // <=12 chars
const char* OffString ="Off";                           // 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Spannings-", "bron"},
  /* (<=  9) Sleep temperature                  */ {"Slaap", "temp"},
  /* (<=  9) Sleep timeout                      */ {"Slaap", "time-out"},
  /* (<= 10) Shutdown timeout                   */ {"Uitschakel", "time-out"},
  /* (<= 13) Motion sensitivity level           */ {"Bewegings-", "gevoeligheid"},
  /* (<= 13) Temperature in F and C             */ {"Temperatuur", "eenheid"},
  /* (<= 13) Advanced idle display mode enabled */ {"Gedetailleerd", "slaapscherm"},
  /* (<= 13) Display rotation mode              */ {"Scherm-", "oriÃ«ntatie"},
  /* (<= 13) Boost enabled                      */ {"Boostmodes", "ingeschakeld?"},
  /* (<=  9) Boost temperature                  */ {"Boost", "temp"},
  /* (<= 13) Automatic start mode               */ {"Auto", "start"},
  /* (<= 13) Cooldown blink                     */ {"Afkoel", "flikker"},
  /* (<= 16) Temperature calibration enter menu */ {"Calibreer", "temperatuur?"},
  /* (<= 16) Settings reset command             */ {"Instellingen", "resetten?"},
  /* (<= 16) Calibrate input voltage            */ {"Calibreer", "input-voltage?"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Gedetailleerd", "soldeerscherm"},
  /* (<= 11) Display Help Text Scroll Speed     */ {"Scroll","snelheid"},
};

// SettingsMenuEntries lengths <= 13 per line (\n starts second line)
const char* SettingsMenuEntries[4] = {
  /* Soldering Menu    */ "Soldeer\nInstellingen",
  /* Power Saving Menu */ "Slaap\nModes",
  /* UI Menu           */ "Gebruikers-\nInterface",
  /* Advanced Menu     */ "geavanceerde\nInstellingen",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  "Soldeerinstellingen",
  "Batterijbesparingsinstellingen",
  "Gebruikersinterface Instellingen",
  "geavanceerde Instellingen"
};
#endif

#ifdef LANG_SV
const char* SettingsDescriptions[17] = {
  // These are all the help text for all the settings.
  // No requirements on spacing or length.
  /* Power source (DC or batt)          */ "StrÃ¶mkÃ¤lla. Anger lÃ¤gsta spÃ¤nning. <DC 10V> <S 3.3V per cell>",
  /* Sleep temperature                  */ "Vilotemperatur <C>",
  /* Sleep timeout                      */ "Vilo-timeout <Minuter/Seconder>",
  /* Shutdown timeout                   */ "AvstÃ¤ngnings-timeout <Minuter>",
  /* Motion sensitivity level           */ "RÃ¶relsekÃ¤nslighet <0.Av 1.minst kÃ¤nslig 9.mest kÃ¤nslig>",
  /* Temperature in F and C             */ "Temperaturenhet <C=Celsius F=Fahrenheit>",
  /* Advanced idle display mode enabled */ "Visa detaljerad information i mindre typsnitt nÃ¤r inaktiv.",
  /* Display rotation mode              */ "VisningslÃ¤ge <A. Automatisk V. VÃ¤nsterhÃ¤nt H. HÃ¶gerhÃ¤nt>",

  /* Boost enabled                      */ "Aktivera frÃ¤mre knappen fÃ¶r turbolÃ¤ge (temperaturhÃ¶jning) vid lÃ¶dning",
  /* Boost temperature                  */ "Temperatur i \"turbo\"-lÃ¤ge",
  /* Automatic start mode               */ "Startar automatiskt lÃ¶dpennan vid uppstart. T=LÃ¶dning, S=VilolÃ¤ge, F=Av",
  /* Cooldown blink                     */ "Blinka temperaturen medan spetsen kyls av och fortfarande Ã¤r varm.",
  /* Temperature calibration enter menu */ "Kalibrera spets-kompensation.",
  /* Settings reset command             */ "Ã…terstÃ¤ll alla instÃ¤llningar",
  /* Calibrate input voltage            */ "InspÃ¤nningskalibrering. Knapparna justerar, hÃ¥ll inne fÃ¶r avslut",
  /* Advanced soldering screen enabled  */ "Visa detaljerad information vid lÃ¶dning",
  /* Description Scroll Speed           */ "Hastighet som den hÃ¤r texten rullar i",
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LÃ…G";               // <=8 chars
const char* UndervoltageString = "UnderspÃ¤nning";       // <=16 chars
const char* InputVoltageString = "InspÃ¤n. V: ";         // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Spetstemp: ";       // <=12 chars, preferably end with a space
const char* BadTipString = "SPETSFEL";                  // <=8 chars
const char* SleepingSimpleString = "Zzzz";              // Must be <= 4 chars
const char* SleepingAdvancedString = "VilolÃ¤ge...";     // <=16 chars
const char* WarningSimpleString = "VARM";               // Must be <= 4 chars
const char* WarningAdvancedString = "!! SPETS VARM !!"; // <=16 chars
const char* SleepingTipAdvancedString = "Spets:";       // <=6 chars
const char* IdleTipString = "Spets:";                   // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Ange:";                   // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "SPETS URTAGEN";    // <=16 chars
const char* SolderingAdvancedPowerPrompt = "StrÃ¶m: ";   // <=12 chars
const char* OffString ="Av";                            // 3 chars max

const char SettingRightChar = 'H';
const char SettingLeftChar = 'V';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'S';
const char SettingSlowChar = 'L';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"StrÃ¶m-", "kÃ¤lla"},
  /* (<=  9) Sleep temperature                  */ {"Vilo-", "temp"},
  /* (<=  9) Sleep timeout                      */ {"Vilo-", "timeout"},
  /* (<= 10) Shutdown timeout                   */ {"AvstÃ¤ngn.", "timeout"},
  /* (<= 13) Motion sensitivity level           */ {"RÃ¶relse-", "kÃ¤nslighet"},
  /* (<= 13) Temperature in F and C             */ {"Temperatur-", "enheter"},
  /* (<= 13) Advanced idle display mode enabled */ {"Detaljerad", "vid inaktiv"},
  /* (<= 13) Display rotation mode              */ {"Visnings", "lÃ¤ge"},
  /* (<= 13) Boost enabled                      */ {"TurbolÃ¤ge", "aktiverat"},
  /* (<=  9) Boost temperature                  */ {"Turbo-", "temp"},
  /* (<= 13) Automatic start mode               */ {"Auto", "start"},
  /* (<= 13) Cooldown blink                     */ {"Nedkylnings-", "blink"},
  /* (<= 16) Temperature calibration enter menu */ {"Kalibrera", "temperatur?"},
  /* (<= 16) Settings reset command             */ {"Fabriks-", "instÃ¤ll?"},
  /* (<= 16) Calibrate input voltage            */ {"Kalibrera", "inspÃ¤nning?"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Detaljerad", "lÃ¶dng.skÃ¤rm"},
  /* (<= 11) Display Help Text Scroll Speed     */ {"Beskrivning","rullhast."},
};

// SettingsMenuEntries lengths <= 13 per line (\n starts second line)
const char* SettingsMenuEntries[4] = {
  /* Soldering Menu    */ "LÃ¶dnings-\ninstÃ¤llningar",
  /* Power Saving Menu */ "Vilo-\nlÃ¤gen",
  /* UI Menu           */ "AnvÃ¤ndar-\ngrÃ¤nssnitt",
  /* Advanced Menu     */ "Avancerade\nalternativ",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  "LÃ¶dningsinstÃ¤llningar",
  "VilolÃ¤ges-instÃ¤llningar",
  "AnvÃ¤ndargrÃ¤nssnitts-instÃ¤llningar",
  "Avancerade alternativ"
};
#endif

#ifdef LANG_SR
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "Izvor napajanja. Postavlja napon isključivanja. <DC 10V> <S 3.3V po ćeliji>",
	/* Sleep temperature                  */ "Temperatura na koju se spušta lemilica nakon određenog vremena mirovanja. <C/F>",
	/* Sleep timeout                      */ "Vreme mirovanja nakon koga lemilica spušta temperaturu. <Minute/Sekunde>",
	/* Shutdown timeout                   */ "Vreme mirovanja nakon koga će se lemilica ugasiti. <Minute>",
	/* Motion sensitivity level           */ "Osetljivost prepoznavanja pokreta. <0=Ugašeno, 1=Najmanje osetljivo, 9=Najosjetljivije>",
	/* Temperature in F and C             */ "Jedinica temperature. <C=Celzijus, F=Fahrenheit>",
	/* Advanced idle display mode enabled */ "Prikazivanje detaljnih informacija manjim fontom tokom čekanja.",
	/* Display rotation mode              */ "Orijentacija ekrana. <A=Automatski, L=Levoruki, D=Desnoruki>",
	/* Boost enabled                      */ "Držanjem prednjeg dugmeta prilikom lemljenja aktivira se pojačani (Boost) način.",
	/* Boost temperature                  */ "Temperatura u pojačanom (Boost) načinu.",
	/* Automatic start mode               */ "Ako je aktivno, lemilica po uključivanju napajanja odmah počinje grejati.",
	/* Cooldown blink                     */ "Blinkanje temperature prilikom hlađenja, ako je lemilica vruća.",
	/* Temperature calibration enter menu */ "Kalibrisanje temperature meri razliku temperatura vrha i drške, dok je lemilica hladna.",
	/* Settings reset command             */ "Vraćanje svih podešavanja na fabričke vrednosti.",
	/* Calibrate input voltage            */ "Kalibracija ulaznog napona. Podešavanje tastera, dugački pritisak za kraj.",
	/* Advanced soldering screen enabled  */ "Prikazivanje detaljnih informacija tokom lemljenja.",
	/* Description Scroll Speed           */ "Brzina kretanja dugačkih poruka <B=brzo, S=sporo>",
};

const char* SettingsCalibrationWarning = "Proverite da li je vrh ohlađen na sobnu temperaturu pre nego što nastavite!";
const char* SettingsResetWarning = "Jeste li sigurni da želite da vratite sva podešavanja na fabričke vrednosti?";
const char* UVLOWarningString = "BATERIJA";             // <=8 chars
const char* UndervoltageString = "PRENIZAK NAPON";      // <=16 chars
const char* InputVoltageString = "Napajanje: ";         // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Temp vrha: ";       // <=12 chars, preferably end with a space
const char* BadTipString = "NEISPRAVAN VRH";            // <=8 chars
const char* SleepingSimpleString = "Zzz ";              // Must be <= 4 chars
const char* SleepingAdvancedString = "SPAVANJE...";     // <=16 chars
const char* WarningSimpleString = "VRUĆ";               // Must be <= 4 chars
const char* WarningAdvancedString = "OPREZ, VRUĆE!";    // <=16 chars
const char* SleepingTipAdvancedString = "Vrh: ";        // <=6 chars
const char* IdleTipString = "Vrh: ";                    // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " / ";                      // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "VRH NIJE SPOJEN!"; // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Snaga: ";   // <=12 chars
const char* OffString ="Off";                           // 3 chars max

const char SettingRightChar = 'D';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'B';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Izvor", "napajanja"},
  /* (<=  9) Sleep temperature                  */ {"Temp", "spavanja"},
  /* (<=  9) Sleep timeout                      */ {"Vreme", "spavanja"},
  /* (<= 10) Shutdown timeout                   */ {"Vreme", "gašenja"},
  /* (<= 13) Motion sensitivity level           */ {"Osetljivost", "pokreta"},
  /* (<= 13) Temperature in F and C             */ {"Jedinica", "temperature"},
  /* (<= 13) Advanced idle display mode enabled */ {"Detalji", "pri čekanju"},
  /* (<= 13) Display rotation mode              */ {"Rotacija", "ekrana"},
  /* (<= 13) Boost enabled                      */ {"Boost", "način"},
  /* (<=  9) Boost temperature                  */ {"Boost", "temp"},
  /* (<= 13) Automatic start mode               */ {"Auto", "start"},
  /* (<= 13) Cooldown blink                     */ {"Upozorenje", "pri hlađenju"},
  /* (<= 16) Temperature calibration enter menu */ {"Kalibracija", "temperature"},
  /* (<= 16) Settings reset command             */ {"Fabričke", "postavke"},
  /* (<= 16) Calibrate input voltage            */ {"Kalibracija", "napona napajanja"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Detalji", "pri lemljenju"},
  /* (<= 11) Display Help Text Scroll Speed     */ {"Brzina","poruka"},
};
const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"Postavke\nlemljenja",
/* Power Saving Menu*/"Ušteda\nenergije",
/* UI Menu*/"Korisničke\opcije",
/* Advanced Menu*/"Napredne\nopcije", };
const char* SettingsMenuEntriesDescriptions[4] ={
"Postavke pri lemljenju",
"Postavke spavanja i štednje energije",
"Postavke korisničkih opcija",
"Upravljanje naprednim opcijama"
};
#endif
