/*
 * Translation.c
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Translation.h"

// TEMPLATES for short names - choose one and use it as base for your
// translation:

//const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
//const char* SettingsShortNames[16][2] = {
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
//};

//const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
//const char* SettingsShortNames[16][2] = {
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
//};

#ifdef LANG_EN
const char* SettingsLongNames[16] = {
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
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "LOW VOLT";          // Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";           // Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";  // <=17 chars
const char* WarningSimpleString = "HOT!";            // Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[16][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Power", "source"},
  /* (<=  9) Sleep temperature                  */ {"Sleep", "temp"},
  /* (<=  9) Sleep timeout                      */ {"Sleep", "timeout"},
  /* (<= 11) Shutdown timeout                   */ {"Shutdown", "timeout"},
  /* (<= 13) Motion sensitivity level           */ {"Motion", "sensitivity"},
  /* (<= 13) Temperature in F and C             */ {"Temperature", "units"},
  /* (<= 13) Advanced idle display mode enabled */ {"Detailed", "idle screen"},
  /* (<= 13) Display rotation mode              */ {"Display", "orientation"},
  /* (<= 13) Boost enabled                      */ {"Boost mode", "enabled"},
  /* (<=  9) Boost temperature                  */ {"Boost", "temp"},
  /* (<= 13) Automatic start mode               */ {"Auto", "start"},
  /* (<= 13) Cooldown blink                     */ {"Cooldown", "blink"},
  /* (<= 16) Temperature calibration enter menu */ {"Calibrate", "temperature?"},
  /* (<= 16) Settings reset command             */ {"Factory", "Reset?"},
  /* (<= 16) Calibrate input voltage            */ {"Calibrate", "input voltage?"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Detailed", "solder screen"},
};
#endif

#ifdef LANG_RU
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "Источник питания. Установка напряжения отключения. <DC 10V> <S 3.3 V на батарею>",
	/* Sleep temperature                  */ "Температура режима ожидания <С>",
	/* Sleep timeout                      */ "Время до перехода в режим ожидания <Минуты>",
	/* Shutdown timeout                   */ "Время до отключения <Минуты>",
	/* Motion sensitivity level           */ "Акселерометр <0. Выкл. 1. мин. чувствительный 9. макс. чувствительный>",
	/* Temperature in F and C             */ "В чем измерять температуру",
	/* Advanced idle display mode enabled */ "Показывать детальную информацию маленьким шрифтом на домашнем экране",
	/* Display rotation mode              */ "Ориентация дисплея <A. Автоматический, Л. Левая рука, П. Правая рука>",
	/* Boost enabled                      */ "Турбо-режим при удержании кнопки А при пайке ",
	/* Boost temperature                  */ "Температура в турбо-режиме",
	/* Automatic start mode               */ "Автоматический запуск паяльника при включении питания. T=Нагрев, S=Режим ожидания,F=Выкл.",
	/* Cooldown blink                     */ "Показывать температуру на экране охлаждения, пока жало остается горячим.",
	/* Temperature calibration enter menu */ "Калибровка термодатчика.",
	/* Settings reset command             */ "Сброс всех настроек.",
	/* Calibrate input voltage            */ "Калибровка напряжения входа. Настройка кнопками, нажать и удержать чтобы завершить.",
	/* Advanced soldering screen enabled  */ "Показывать детальную информацию при пайке.",
};

const char* SettingsCalibrationWarning = "Убедитесь, что жало остыло до комнатной температуры, прежде чем продолжать!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "БАТ РАЗР";          // Fixed width 8 chars
const char* SleepingSimpleString = "Хррр";           // Must be <= 4 chars
const char* SleepingAdvancedString = "Ожидание...";  // <=17 chars
const char* WarningSimpleString = " АЙ!";            // Must be <= 4 chars
const char* WarningAdvancedString = "ОСТОРОЖНО! ГОРЯЧО";

const char SettingTrueChar = '+';
const char SettingFalseChar = '-';
/*
 * #TODO change support for multibyte constants here
 const char SettingRightChar = 'П';
 const char SettingLeftChar = 'Л';
 const char SettingAutoChar = 'A';*/

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
  /* (<= 5) Power source (DC or batt)          */ {"ИстП  "},
  /* (<= 4) Sleep temperature                  */ {"Тожд"},
  /* (<= 4) Sleep timeout                      */ {"Вожд "},
  /* (<= 5) Shutdown timeout                   */ {"Тоткл "},
  /* (<= 6) Motion sensitivity level           */ {"ЧувсДв "},
  /* (<= 6) Temperature in F and C             */ {"ЕдТемп "},
  /* (<= 6) Advanced idle display mode enabled */ {"ИнфОжд "},
  /* (<= 6) Display rotation mode              */ {"ПовЭкр "},
  /* (<= 6) Boost enabled                      */ {"Турбо  "},
  /* (<= 4) Boost temperature                  */ {"Ттур "},
  /* (<= 6) Automatic start mode               */ {"Астарт"},
  /* (<= 6) Cooldown blink                     */ {"Охлажд "},
  /* (<= 8) Temperature calibration enter menu */ {"КалибрТ"},
  /* (<= 8) Settings reset command             */ {"СБРОС?"},
  /* (<= 8) Calibrate input voltage            */ {"КалибрU?"},
  /* (<= 6) Advanced soldering screen enabled  */ {"ИнфПай "},
};
#endif

#ifdef LANG_ES
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
  /* Power source (DC or batt)          */ "Fuente de energía. Ajusta el límite inferior de voltaje. <DC=10V S=3.3V por celda>",
  /* Sleep temperature                  */ "Temperatura en reposo. <C>",
  /* Sleep timeout                      */ "Tiempo hasta activar reposo. <Minutos>",
  /* Shutdown timeout                   */ "Tiempo hasta apagado. <Minutos>",
  /* Motion sensitivity level           */ "Sensibilidad del movimiento. <0=Apagado 1=El menos sensible 9=El más sensible>",
  /* Temperature in F and C             */ "Unidad de temperatura.",
  /* Advanced idle display mode enabled */ "Display detailed information in a smaller font on the idle screen.",
  /* Display rotation mode              */ "Orientación de la pantalla <A=Automático I=Mano izquierda D=Mano derecha>",
  /* Boost enabled                      */ "Activar el botón \"Boost\" en modo soldadura.",
  /* Boost temperature                  */ "Temperatura en modo \"Boost\". <C>",
  /* Automatic start mode               */ "Iniciar modo soldadura en el encendido. <V=Sí S=Modo reposo F=No>",
  /* Cooldown blink                     */ "Parpadea la temperatura en el enfriamiento si la punta sigue caliente.",
  /* Temperature calibration enter menu */ "Calibrate tip offset.",
  /* Settings reset command             */ "Reset all settings",
  /* Calibrate input voltage            */ "VIN Calibration. Buttons adjust, long press to exit",
  /* Advanced soldering screen enabled  */ "Display detailed information while soldering",
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "LOW VOLT";          // Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";           // Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";  // <=17 chars
const char* WarningSimpleString = "HOT!";            // Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
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
};
#endif

#ifdef LANG_SE
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "Источник питания. Установка напряжения отключения. <DC 10V> <S 3.3 V на батарею>",
	/* Sleep temperature                  */ "Температура Сна  <С>",
	/* Sleep timeout                      */ "Переход в режим Сна <Минуты>",
	/* Shutdown timeout                   */ "Переходит в режим ожидания <Минуты>",
	/* Motion sensitivity level           */ "Акселерометр <0. Выкл. 1. мин. чувствительный 9. макс. чувствительный>",
	/* Temperature in F and C             */ "В чем измерять температуру",
	/* Advanced idle display mode enabled */ "Display detailed information in a smaller font on the idle screen.",
	/* Display rotation mode              */ "Ориентация Дисплея <A. Автоматический L. Левая Рука R. Правая Рука>",
	/* Boost enabled                      */ "Активация  кнопки A для Турбо режима до  450С при пайке ",
	/* Boost temperature                  */ "Установка температуры для Турбо режима",
	/* Automatic start mode               */ "Автоматический запуск паяльника при включении питания. T=Нагрев, S= Режим Сна,F=Выкл.",
	/* Cooldown blink                     */ "Мигает температура на экране охлаждения, пока жало остается горячим.",
	/* Temperature calibration enter menu */ "Calibrate tip offset.",
	/* Settings reset command             */ "Reset all settings",
	/* Calibrate input voltage            */ "VIN Calibration. Buttons adjust, long press to exit",
	/* Advanced soldering screen enabled  */ "Display detailed information while soldering",
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "LOW VOLT";          // Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";           // Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";  // <=17 chars
const char* WarningSimpleString = "HOT!";            // Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
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
};
#endif

#ifdef LANG_IT
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "Sorgente di alimentazione; imposta il limite minimo di tensione <DC: 10V; S: 3.3V per cella>",
	/* Sleep temperature                  */ "Temperatura standby <°C>",
	/* Sleep timeout                      */ "Timeout standby <minuti/secondi>",
	/* Shutdown timeout                   */ "Timeout spegnimento <minuti>",
	/* Motion sensitivity level           */ "Sensibilità al movimento <0: nessuna; 1: minima; 9: massima>",
	/* Temperature in F and C             */ "Unità di misura della temperatura <C: Celsius; F: Farenheit>",
	/* Advanced idle display mode enabled */ "Mostra informazioni dettagliate con un carattere più piccolo sulla schermata di inattività",
	/* Display rotation mode              */ "Orientamento del display <A: automatico; S: mano sinistra; D: mano destra>",
	/* Boost enabled                      */ "Il tasto anteriore attiva la modalità \"boost\" durante la saldatura",
	/* Boost temperature                  */ "Temperatura in modalità \"boost\"",
	/* Automatic start mode               */ "Attiva automaticamente il saldatore quando viene alimentato <A: saldatura; S: standby; D: disattiva>",
	/* Cooldown blink                     */ "Durante il raffreddamento mostra la temperatura sul display se la punta è ancora calda",
	/* Temperature calibration enter menu */ "Calibra l'offset della punta",
	/* Settings reset command             */ "Ripristina tutte le impostazioni",
	/* Calibrate input voltage            */ "Calibra la tensione in entrata; regola con i bottoni, tieni permuto per uscire",
	/* Advanced soldering screen enabled  */ "Mostra informazioni dettagliate mentre stai saldando",
};

const char* SettingsCalibrationWarning = "Assicurati che la punta si trovi a temperatura ambiente prima di continuare!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "LOW VOLT";      // Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";       // Must be <= 4 chars
const char* SleepingAdvancedString = "Standby";  // <=17 chars
const char* WarningSimpleString = "HOT!";        // Must be <= 4 chars
const char* WarningAdvancedString = "ATTENZIONE! PUNTA CALDA!";

const char SettingTrueChar = 'A';
const char SettingFalseChar = 'D';
const char SettingRightChar = 'D';
const char SettingLeftChar = 'S';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
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
};
#endif

#ifdef LANG_FR
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "Source d'alimentation. Règle la tension de coupure <DC=10V S=3.3V par cellules>",
	/* Sleep temperature                  */ "Température en veille <C>",
	/* Sleep timeout                      */ "Délai avant mise en veille <Minutes>",
	/* Shutdown timeout                   */ "Délai avant extinction <Minutes>",
	/* Motion sensitivity level           */ "Sensibilité du capteur de mouvement <0=Inactif 1=Peu sensible 9=Tres sensible>",
	/* Temperature in F and C             */ "Unité de temperature",
	/* Advanced idle display mode enabled */ "Afficher des informations détaillées lors de la veille",
	/* Display rotation mode              */ "Orientation de l'affichage <A=Automatique G=Gaucher D=Droitier>",
	/* Boost enabled                      */ "Activer le mode \"Boost\" en maintenant le bouton de devant pendant la soudure",
	/* Boost temperature                  */ "Température du mode \"Boost\" <C>",
	/* Automatic start mode               */ "Démarrer automatiquement la soudure a l'allumage <A=Activé, V=Mode Veille, D=Désactivé>",
	/* Cooldown blink                     */ "Faire clignoter la température lors du refroidissement tant que la panne est chaude",
	/* Temperature calibration enter menu */ "Etalonner température de la panne",
	/* Settings reset command             */ "Réinitialiser tous les réglages",
	/* Calibrate input voltage            */ "Etalonner tension d'entrée. Boutons pour ajuster, appui long pour quitter",
	/* Advanced soldering screen enabled  */ "Afficher des informations détaillées pendant la soudure",
};

const char* SettingsCalibrationWarning = "Assurez-vous que la panne est à température ambiante avant de continuer!";
const char* SettingsResetWarning = "Voulez-vous vraiment réinitialiser les paramètres aux valeurs d'usine?";
const char* UVLOWarningString = "LOW VOLT";          // Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";           // Must be <= 4 chars
const char* SleepingAdvancedString = "En veille...";  // <=17 chars
const char* WarningSimpleString = "HOT!";            // Must be <= 4 chars
const char* WarningAdvancedString = "ATTENTION! PANNE CHAUDE!";

const char SettingTrueChar = 'V';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'D';
const char SettingLeftChar = 'G';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[16][2] = {
	/* (<= 11) Power source (DC or batt)          */ {"Source", "d'alim"},
	/* (<=  9) Sleep temperature                  */ {"Temp.", "veille"},
	/* (<=  9) Sleep timeout                      */ {"Délai", "veille"},
	/* (<= 11) Shutdown timeout                   */ {"Délai", "extinction"},
	/* (<= 13) Motion sensitivity level           */ {"Sensibilité", "au mouvement"},
	/* (<= 13) Temperature in F and C             */ {"Unité de", "température"},
	/* (<= 13) Advanced idle display mode enabled */ {"Ecran veille", "détaillé"},
	/* (<= 13) Display rotation mode              */ {"Orientation", "de l'écran"},
	/* (<= 13) Boost enabled                      */ {"Activation du", "mode Boost"},
	/* (<=  9) Boost temperature                  */ {"Temp.", "Boost"},
	/* (<= 13) Automatic start mode               */ {"Démarrage", "automatique"},
	/* (<= 13) Cooldown blink                     */ {"Refroidir en", "clignottant"},
	/* (<= 16) Temperature calibration enter menu */ {"Etalonner", "température"},
	/* (<= 16) Settings reset command             */ {"Réinitialisation", "d'usine"},
	/* (<= 16) Calibrate input voltage            */ {"Etalonner", "tension d'entrée"},
	/* (<= 13) Advanced soldering screen enabled  */ {"Ecran soudure", "détaillé"},
};
#endif

#ifdef LANG_DE
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "Spannungsquelle (Abschaltspannung) <DC=10V, nS=n*3.3V für n LiIon-Zellen>",
	/* Sleep temperature                  */ "Ruhetemperatur (In der eingestellten Einheit)",
	/* Sleep timeout                      */ "Ruhemodus nach <Sekunden/Minuten>",
	/* Shutdown timeout                   */ "Abschaltzeit <Minuten>",
	/* Motion sensitivity level           */ "Bewegungsempfindlichkeit <0=Aus, 1=Minimal ... 9=Maximal>",
	/* Temperature in F and C             */ "Temperatureinheit <C=Celsius, F=Fahrenheit>",
	/* Advanced idle display mode enabled */ "Detaillierte Anzeige im Ruhemodus <J=An, N=Aus>",
	/* Display rotation mode              */ "Ausrichtung der Anzeige <A=Automatisch, L=Linkshändig, R=Rechtshändig>",
	/* Boost enabled                      */ "Vordere Taste für Temperaturboost verwenden <J=An, N=Aus>",
	/* Boost temperature                  */ "Temperatur im Boostmodus  (In der eingestellten Einheit)",
	/* Automatic start mode               */ "Automatischer Start des Lötmodus beim Einschalten der Spannungsversorgung. <J=An, N=Aus>",
	/* Cooldown blink                     */ "Blinkende Temperaturanzeige beim Abkühlen, solange heiß. <J=An, N=Aus>",
	/* Temperature calibration enter menu */ "Kalibrierung der Lötspitzentemperatur",
	/* Settings reset command             */ "Alle Einstellungen zurücksetzen",
	/* Calibrate input voltage            */ "Kalibrierung der Eingangsspannung. Kurzer Tastendruck zum Einstellen, langer Tastendruck zum Verlassen.",
	/* Advanced soldering screen enabled  */ "Detaillierte Anzeige im Lötmodus <J=An, N=Aus>",
};

const char* SettingsCalibrationWarning = "Vor dem Fortfahren muss die Lötspitze vollständig abgekühlt sein!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "LOW VOLT";           // Fixed width 8 chars
const char* SleepingSimpleString = "Zzz ";            // Must be <= 4 chars
const char* SleepingAdvancedString = "Ruhemodus...";  // <=17 chars
const char* WarningSimpleString = "HEIß";             // Must be <= 4 chars
const char* WarningAdvancedString = "Achtung! Spitze Heiß!";

const char SettingTrueChar = 'J';
const char SettingFalseChar = 'N';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[16][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Spannungs-", "quelle"},
  /* (<=  9) Sleep temperature                  */ {"Ruhetemp-", "eratur"},
  /* (<=  9) Sleep timeout                      */ {"Ruhever-", "zoegerung"},
  /* (<= 11) Shutdown timeout                   */ {"Abschalt-", "zeit"},
  /* (<= 13) Motion sensitivity level           */ {"Bewegungs-", "empfindlichk."},
  /* (<= 13) Temperature in F and C             */ {"Temperatur-", "einheit"},
  /* (<= 13) Advanced idle display mode enabled */ {"Detaillierte", "Ruheansicht"},
  /* (<= 13) Display rotation mode              */ {"Anzeige-", "ausrichtung"},
  /* (<= 13) Boost enabled                      */ {"Boosttaste", "aktiv?"},
  /* (<=  9) Boost temperature                  */ {"Boosttemp-", "eratur"},
  /* (<= 13) Automatic start mode               */ {"Auto-", "start"},
  /* (<= 13) Cooldown blink                     */ {"Abkuehl-", "blinken"},
  /* (<= 16) Temperature calibration enter menu */ {"Temperatur", "kalibrieren?"},
  /* (<= 16) Settings reset command             */ {"Werks-", "reset?"},
  /* (<= 16) Calibrate input voltage            */ {"Eingangsspannung", "kalibrieren?"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Detaillierte", "Loetansicht"},
};
#endif

#ifdef LANG_SK
const char* SettingsLongNames[16] = {
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
};

const char* SettingsCalibrationWarning = "Najprv sa prosim uistite, ze hrot ma izbovu teplotu!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "LOW VOLT";               // Fixed width 8 chars
const char* SleepingSimpleString = "Chrr";                // Must be <= 4 chars
const char* SleepingAdvancedString = "Kludovy rezim...";  // <=17 chars
const char* WarningSimpleString = "HOT!";                 // Must be <= 4 chars
const char* WarningAdvancedString = "Pozor! Hrot je horuci!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
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
};
#endif

#ifdef LANG_TR
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "Güç Kaynağı. kesim geriliminı ayarlar. <DC 10V> <S 3.3V hücre başına>",
	/* Sleep temperature                  */ "Uyku Sıcaklığı <C>",
	/* Sleep timeout                      */ "Uyku Zaman Aşımı <Dakika/Saniye>",
	/* Shutdown timeout                   */ "Kapatma Zaman Aşımı <Dakika>",
	/* Motion sensitivity level           */ "Hareket Hassasiyeti <0.Kapalı 1.En az duyarlı 9.En duyarlı>",
	/* Temperature in F and C             */ "Sıcaklık Ünitesi <C=Celsius F=Fahrenheit>",
	/* Advanced idle display mode enabled */ "Boş ekranda ayrıntılı bilgileri daha küçük bir yazı tipi ile göster.",
	/* Display rotation mode              */ "Görüntü Yönlendirme <A. Otomatik L. Solak R. Sağlak>",
	/* Boost enabled                      */ "Lehimleme yaparken ön tuşa basmak Boost moduna sokar(450C)",
	/* Boost temperature                  */ "\"boost\" Modu Derecesi",
	/* Automatic start mode               */ "Güç verildiğinde otomatik olarak lehimleme modunda başlat. T=Lehimleme Modu, S= Uyku Modu,F=Kapalı",
	/* Cooldown blink                     */ "Soğutma ekranında uç hala sıcakken derece yanıp sönsün.",
	/* Temperature calibration enter menu */ "Ucu kalibre et.",
	/* Settings reset command             */ "Bütün ayarları sıfırla",
	/* Calibrate input voltage            */ "VIN Kalibrasyonu. Düğmeler ayarlar, çıkmak için uzun bas.",
	/* Advanced soldering screen enabled  */ "Lehimleme yaparken detaylı bilgi göster",
};

const char* SettingsCalibrationWarning = "Lütfen devam etmeden önce ucun oda sıcaklığında olduğunu garantiye alın!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "LOW VOLT";        // Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";         // Must be <= 4 chars
const char* SleepingAdvancedString = "Uyuyor...";  // <=17 chars
const char* WarningSimpleString = "HOT!";          // Must be <= 4 chars
const char* WarningAdvancedString = "UYARI! UÇ SICAK!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
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
};
#endif

#ifdef LANG_HR
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */ "Izvor napajanja. Postavlja napon isključivanja. <DC 10V> <S 3.3V po ćeliji>",
	/* Sleep temperature                  */ "Temperatura spavanja. <C>",
	/* Sleep timeout                      */ "Vrijeme spavanja. <Minute/Sekunde>",
	/* Shutdown timeout                   */ "Vrijeme gašenja. <Minutes>",
	/* Motion sensitivity level           */ "Osjetljivost prepoznavanja pokreta. <0=Ugašeno, 1=Najmanje osjetljivo, 9=Najosjetljivije>",
	/* Temperature in F and C             */ "Jedinica temperature. <C=Celzij, F=Fahrenheit>",
	/* Advanced idle display mode enabled */ "Prikazivanje detaljnih informacija manjim fontom tijekom čekanja.",
	/* Display rotation mode              */ "Orijentacija ekrana. <A=Automatski, L=Ljevoruki, D=Desnoruki>",
	/* Boost enabled                      */ "Držanjem prednjeg gumba prilikom lemljenja aktivira se pojačani (Boost) način.",
	/* Boost temperature                  */ "Temperatura u pojačanom (Boost) načinu.",
	/* Automatic start mode               */ "Početno stanje lemilice po uključivanju napajanja. <+=Lemljenje, S=Spavanje, -=Ugašeno>",
	/* Cooldown blink                     */ "Bljeskanje temperature prilikom hlađenja, ako je lemilica vruća.",
	/* Temperature calibration enter menu */ "Kalibriranje temperature mjeri razliku temperature vška i temperature drške, dok je lemilica hladna.",
	/* Settings reset command             */ "Vraćanje svih postavki.",
	/* Calibrate input voltage            */ "Kalibracija ulaznog napona. Podešavanje gumbima, dugački pritisak za kraj.",
	/* Advanced soldering screen enabled  */ "Prikazivanje detaljnih informacija tijekom lemljenja.",
};

const char* SettingsCalibrationWarning = "Provjerite da je vršak ohlađen na sobnu temperaturu prije nego što nastavite!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "NAPON!!!";          // Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";           // Must be <= 4 chars
const char* SleepingAdvancedString = "Spavanje...";  // <=17 chars
const char* WarningSimpleString = "VRUĆ";            // Must be <= 4 chars
const char* WarningAdvancedString = "OPREZ! Vršak je vruć!";

const char SettingTrueChar = '+';
const char SettingFalseChar = '-';
const char SettingRightChar = 'D';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
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
};
#endif

#ifdef LANG_CS_CZ
const char* SettingsLongNames[16] = {
  // These are all the help text for all the settings.
  // No requirements on spacing or length
  /* Power source (DC or batt)          */ "Pri nizsim napeti se odpoji <DC=10V, xS=x*3.3V pro LiPo,LiIon...>",
  /* Sleep temperature                  */ "Teplota v rezimu spanku",
  /* Sleep timeout                      */ "Cas do rezimu spanku <Minut/Sekund>",
  /* Shutdown timeout                   */ "Cas do automatickeho vypnuti <Minut>",
  /* Motion sensitivity level           */ "Citlivost detekce pohybu <0=Vyp, 1=Min, ... 9=Max>",
  /* Temperature in F and C             */ "Jednotky mereni teploty <C=Celsius, F=Fahrenheit>",
  /* Advanced idle display mode enabled */ "Zobrazit podrobnosti na vychozi obrazovce <Z=Zap, V=Vyp>",
  /* Display rotation mode              */ "Orientace obrazovky <A=Auto, L=Levak, P=Pravak>",
  /* Boost enabled                      */ "Povolit boost drzenim leveho tlacitka pri pajeni <Z=Zap, V=Vyp>",
  /* Boost temperature                  */ "Teplota v rezimu boost",
  /* Automatic start mode               */ "Pri startu ihned nahrivat hrot <Z=Zap, V=Vyp, S=Rezim spanku>",
  /* Cooldown blink                     */ "Blikani teploty pri chladnuti, dokud je hrot horky <Z=Zap, V=Vyp>",
  /* Temperature calibration enter menu */ "Kalibrovat mereni teploty",
  /* Settings reset command             */ "Obnovit tovarni nastaveni",
  /* Calibrate input voltage            */ "Kalibrovat vstupni napeti. Tlacitky upravte, podrzenim potvrdte.",
  /* Advanced soldering screen enabled  */ "Zobrazit podrobnosti pri pajeni <Z=Zap, V=Vyp>",
};

const char* SettingsCalibrationWarning = "Ujistete se, ze hrot ma pokojovou teplotu! ";          // ending space needed
const char* SettingsResetWarning = "Opravdu chcete resetovat zarizeni do tovarniho nastaveni?";
const char* UVLOWarningString = "LOW VOLT";              // Fixed width 8 chars
const char* SleepingSimpleString = "Zzz ";               // Must be <= 4 chars
const char* SleepingAdvancedString = "Rezim spanku...";  // <=17 chars
const char* WarningSimpleString = "HOT!";                // Must be <= 4 chars
const char* WarningAdvancedString = "!! HORKY HROT !!";  // <= 16 chars

const char SettingTrueChar = 'Z';
const char SettingFalseChar = 'V';
const char SettingRightChar = 'P';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[16][2] = {
  /* (<= 11) Power source (DC or batt)          */ {"Zdroj", "napajeni"},
  /* (<=  9) Sleep temperature                  */ {"Teplota v", "r. spanku"},
  /* (<=  9) Sleep timeout                      */ {"Cas do", "r. spanku"},
  /* (<= 11) Shutdown timeout                   */ {"Cas do", "vypnuti"},
  /* (<= 13) Motion sensitivity level           */ {"Citlivost", "det. pohybu"},
  /* (<= 13) Temperature in F and C             */ {"Jednotky", "teploty"},
  /* (<= 13) Advanced idle display mode enabled */ {"Podrobnosti", "na vych. obr."},
  /* (<= 13) Display rotation mode              */ {"Orientace", "obrazovky"},
  /* (<= 13) Boost enabled                      */ {"Povolit", "boost"},
  /* (<=  9) Boost temperature                  */ {"Teplota v", "r. boost"},
  /* (<= 13) Automatic start mode               */ {"Auto", "start"},
  /* (<= 13) Cooldown blink                     */ {"Blikani pri", "chladnuti"},
  /* (<= 16) Temperature calibration enter menu */ {"Kalibrovat", "teplotu?"},
  /* (<= 16) Settings reset command             */ {"Tovarni", "nastaveni?"},
  /* (<= 16) Calibrate input voltage            */ {"Kalibrovat", "vstupni napeti?"},
  /* (<= 13) Advanced soldering screen enabled  */ {"Podrobnosti", "pri pajeni"},
};
#endif

#ifdef LANG_HUN
const char* SettingsLongNames[16] = {
  // These are all the help text for all the settings.
  // No requirements on spacing or length
  /* Power source (DC or batt)          */ "Áramforrás. Beállítja a lekapcsolási feszültséget. <DC 10V> <S 3.3V cellánként>",
  /* Sleep temperature                  */ "Alvási hőmérséklet <C>",
  /* Sleep timeout                      */ "Elalvási időzítő <Perc/Másodperc>",
  /* Shutdown timeout                   */ "Kikapcsolási időzítő <Minutes>",
  /* Motion sensitivity level           */ "Mozgás érzékenység beállítása. <0.Ki 1.kevésbé érzékeny 9.legérzékenyebb>",
  /* Temperature in F and C             */ "Hőmérsékleti egység <C=Celsius F=Fahrenheit>",
  /* Advanced idle display mode enabled */ "Részletes információ megjelenítése kisebb betűméretben a készenléti képernyőn.",
  /* Display rotation mode              */ "Megjelenítési tájolás <A. Automatikus L. Balkezes R. Jobbkezes>",
  /* Boost enabled                      */ "Elülső gombbal lépjen boost módba, 450C forrasztás közben",
  /* Boost temperature                  */ "Hőmérséklet \"boost\" módban",
  /* Automatic start mode               */ "Bekapcsolás után automatikusan lépjen forrasztás módba. T=Forrasztás, S=Alvó mód,F=Ki",
  /* Cooldown blink                     */ "Villogjon a hőmérséklet hűlés közben, amíg a hegy forró.",
  /* Temperature calibration enter menu */ "Hegy hőmérsékletének kalibrálása",
  /* Settings reset command             */ "Beállítások alaphelyzetbe állítása",
  /* Calibrate input voltage            */ "A bemeneti feszültség kalibrálása. Röviden megnyomva állítsa be, hosszan nyomja meg a kilépéshez.",
  /* Advanced soldering screen enabled  */ "Részletes információk megjelenítése forrasztás közben",
};

const char* SettingsCalibrationWarning = "Folytatás előtt győződj meg róla, hogy a hegy szobahőmérsékletű!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "LOW VOLT";       // Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";        // Must be <= 4 chars
const char* SleepingAdvancedString = "Alvás...";  // <=17 chars
const char* WarningSimpleString = "HOT!";         // Must be <= 4 chars
const char* WarningAdvancedString = "FIGYELEM! FORRÓ HEGY!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
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
};
#endif

#ifdef LANG_DK
const char* SettingsLongNames[16] = {
  // These are all the help text for all the settings.
  // No requirements on spacing or length
  /* Power source (DC or batt)          */ "Strømforsyning. Indstil Cutoff Spændingen. <DC 10V <S 3.3V per cell",
  /* Sleep temperature                  */ "Dvale Temperatur <C",
  /* Sleep timeout                      */ "Dvale Timeout <Minutter/Sekunder",
  /* Shutdown timeout                   */ "sluknings Timeout <Minutter",
  /* Motion sensitivity level           */ "Bevægelsesfølsomhed <0.Slukket 1.Mindst følsom 9.Mest følsom",
  /* Temperature in F and C             */ "Temperatur Enhed <C=Celsius F=Fahrenheit",
  /* Advanced idle display mode enabled */ "Vis detialieret information med en mindre skriftstørrelse på standby skærmen.",
  /* Display rotation mode              */ "Skærm Orientering <A. Automatisk V. Venstre Håndet H. Højre Håndet",
  /* Boost enabled                      */ "Ved tryk på front knap Aktiveres boost-funktionen, 450C tilstand når der loddes",
  /* Boost temperature                  */ "Temperatur i \"boost\" mode",
  /* Automatic start mode               */ "Start automatisk med lodning når strøm sættes til. L=Lodning, D= Dvale tilstand,S=Slukket",
  /* Cooldown blink                     */ "Blink temperaturen på skærmen, mens spidsen stadig er varm.",
  /* Temperature calibration enter menu */ "kalibrere spids temperatur.",
  /* Settings reset command             */ "Gendan alle indstillinger",
  /* Calibrate input voltage            */ "VIN kalibrering. Knapperne justere, Lang tryk for at gå ud",
  /* Advanced soldering screen enabled  */ "Vis detialieret information mens der loddes",
};

const char* SettingsCalibrationWarning = "Sørg for at loddespidsen er ved stuetemperatur, inden du fortsætter!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "Lav Volt";       // Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";        // Must be <= 4 chars
const char* SleepingAdvancedString = "Dvale...";  // <=17 chars
const char* WarningSimpleString = "Varm";         // Must be <= 4 chars
const char* WarningAdvancedString = "ADVARSEL! VARM LODDESPIDS!";

const char SettingTrueChar = 'j';
const char SettingFalseChar = 'N';
const char SettingRightChar = 'H';
const char SettingLeftChar = 'V';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
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
};
#endif

#ifdef LANG_PL
const char* SettingsLongNames[16] = {
    // These are all the help text for all the settings.
    // No requirements on spacing or length
    /* Power source (DC or batt)          */ "Źródło zasilania. Ustaw napięcie odcięcia. <DC 10V> <S 3.3V dla ogniw Li>",
    /* Sleep temperature                  */ "Temperatura uśpienia <°C>",
    /* Sleep timeout                      */ "Czas uśpienia <Minuty/Sekundy>",
    /* Shutdown timeout                   */ "Czas wyłączenia <Minuty>",
    /* Motion sensitivity level           */ "Czułość ruchu <0.Wyłączona 1.minimalna 9.maksymalna>",
    /* Temperature in F and C             */ "Jednostka temperatury <C=Celsius F=Fahrenheit>",
    /* Advanced idle display mode enabled */ "Wyświetla szczegółowe informacje za pomocą mniejszej czcionki na ekranie bezczynnośći <T = wł., N = wył.>",
    /* Display rotation mode              */ "Orientacja wyświetlacza <A. Automatyczna L. Leworęczna P. Praworęczna>",
    /* Boost enabled                      */ "Użyj przycisku przedniego w celu zwiększenia temperatury <T = wł., N = wył.>",
    /* Boost temperature                  */ "Temperatura w trybie \"boost\" ",
    /* Automatic start mode               */ "Automatyczne uruchamianie trybu lutowania po włączeniu zasilania. T=Lutowanie, S= Tryb Uspienia ,N=Wyłącz",
    /* Cooldown blink                     */ "Temperatura na ekranie miga, gdy grot jest jeszcze gorący. <T = wł., N = wył.>",
    /* Temperature calibration enter menu */ "Kalibracja temperatury grota lutownicy",
    /* Settings reset command             */ "Zresetuj wszystkie ustawienia",
    /* Calibrate input voltage            */ "Kalibracja napięcia wejściowego. Krótkie naciśnięcie, aby ustawić, długie naciśnięcie, aby wyjść.",
    /* Advanced soldering screen enabled  */ "Wyświetl szczegółowe informacje podczas lutowania <T = wł., N = wył.>",
};

const char* SettingsCalibrationWarning = "Przed kontynuowaniem upewnij się, że końcówka osiągnela temperature pokojowa!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "LOW VOLT";          // Fixed width 8 chars
const char* SleepingSimpleString = "Zzz!";           // Must be <= 4 chars
const char* SleepingAdvancedString = "Uspienie...";  // <=17 chars
const char* WarningSimpleString = "HOT!";            // Must be <= 4 chars
const char* WarningAdvancedString = "UWAGA! GORĄCA KOŃCÓWKA!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'N';
const char SettingRightChar = 'P';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
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
};
#endif
