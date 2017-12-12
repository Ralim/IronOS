/*
 * Translation.c
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Translation.h"

// TEMPLATES for short names - choose one and use it as base for your translation:

//const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
//const char* SettingsShortNames[16][2] = {
//    /* (<= 5) Power source (DC or batt)          */ {"PWRSC"},
//    /* (<= 4) Sleep temperature                  */ {"STMP"},
//    /* (<= 4) Sleep timeout                      */ {"STME"},
//    /* (<= 5) Shutdown timeout                   */ {"SHTME"},
//    /* (<= 6) Motion sensitivity level           */ {"MSENSE"},
//    /* (<= 6) Temperature in F and C             */ {"TMPUNT"},
//    /* (<= 6) Advanced idle display mode enabled */ {"ADVIDL"},
//    /* (<= 6) Display rotation mode              */ {"DSPROT"},
//    /* (<= 6) Boost enabled                      */ {"BOOST"},
//    /* (<= 4) Boost temperature                  */ {"BTMP"},
//    /* (<= 6) Automatic start mode               */ {"ASTART"},
//    /* (<= 6) Cooldown blink                     */ {"CLBLNK"},
//    /* (<= 8) Temperature calibration enter menu */ {"TMP CAL?"},
//    /* (<= 8) Settings reset command             */ {"RESET?"},
//    /* (<= 8) Calibrate input voltage            */ {"CAL VIN?"},
//    /* (<= 6) Advanced soldering screen enabled  */ {"ADVSLD"},
//};

//const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
//const char* SettingsShortNames[16][2] = {
//    /* (<= 11) Power source (DC or batt)          */ {"Power", "source"},
//    /* (<=  9) Sleep temperature                  */ {"Sleep", "temp"},
//    /* (<=  9) Sleep timeout                      */ {"Sleep", "timeout"},
//    /* (<= 11) Shutdown timeout                   */ {"Shutdown", "timeout"},
//    /* (<= 13) Motion sensitivity level           */ {"Motion", "sensitivity"},
//    /* (<= 13) Temperature in F and C             */ {"Temperature", "units"},
//    /* (<= 13) Advanced idle display mode enabled */ {"Detailed", "idle screen"},
//    /* (<= 13) Display rotation mode              */ {"Display", "orientation"},
//    /* (<= 13) Boost enabled                      */ {"Boost mode", "enabled"},
//    /* (<=  9) Boost temperature                  */ {"Boost", "temp"},
//    /* (<= 13) Automatic start mode               */ {"Auto", "start"},
//    /* (<= 13) Cooldown blink                     */ {"Cooldown", "blink"},
//    /* (<= 16) Temperature calibration enter menu */ {"Calibrate", "temperature?"},
//    /* (<= 16) Settings reset command             */ {"Factory", "Reset?"},
//    /* (<= 16) Calibrate input voltage            */ {"Calibrate", "input voltage?"},
//    /* (<= 13) Advanced soldering screen enabled  */ {"Detailed", "solder screen"},
//};

#ifdef LANG_EN
const char* SettingsLongNames[16] =
		{
				// These are all the help text for all the settings.
				// No requirements on spacing or length.
				/* Power source (DC or batt)          */"Power source. Sets cutoff voltage. <DC 10V> <S 3.3V per cell>",
				/* Sleep temperature                  */"Sleep Temperature <C>",
				/* Sleep timeout                      */"Sleep Timeout <Minutes/Seconds>",
				/* Shutdown timeout                   */"Shutdown Timeout <Minutes>",
				/* Motion sensitivity level           */"Motion Sensitivity <0.Off 1.least sensitive 9.most sensitive>",
				/* Temperature in F and C             */"Temperature Unit <C=Celsius F=Fahrenheit>",
				/* Advanced idle display mode enabled */"Display detailed information in a smaller font on the idle screen.",
				/* Display rotation mode              */"Display Orientation <A. Automatic L. Left Handed R. Right Handed>",
				/* Boost enabled                      */"Enable front key enters boost mode 450C mode when soldering",
				/* Boost temperature                  */"Temperature when in \"boost\" mode",
				/* Automatic start mode               */"Automatically starts the iron into soldering on power up. T=Soldering, S= Sleep mode,F=Off",
				/* Cooldown blink                     */"Blink the temperature on the cooling screen while the tip is still hot.",
				/* Temperature calibration enter menu */"Calibrate tip offset.",
				/* Settings reset command             */"Reset all settings",
				/* Calibrate input voltage            */"VIN Calibration. Buttons adjust, long press to exit",
				/* Advanced soldering screen enabled  */"Display detailed information while soldering", };

const char* SettingsCalibrationWarning =
		"Please ensure the tip is at room temperature before continuing!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";    // Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";    // <=17 chars
const char* WarningSimpleString = "HOT!";    //Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[16][2] = {
/* (<= 11) Power source (DC or batt)          */{ "Power", "source" },
/* (<=  9) Sleep temperature                  */{ "Sleep", "temp" },
/* (<=  9) Sleep timeout                      */{ "Sleep", "timeout" },
/* (<= 11) Shutdown timeout                   */{ "Shutdown", "timeout" },
/* (<= 13) Motion sensitivity level           */{ "Motion", "sensitivity" },
/* (<= 13) Temperature in F and C             */{ "Temperature", "units" },
/* (<= 13) Advanced idle display mode enabled */{ "Detailed", "idle screen" },
/* (<= 13) Display rotation mode              */{ "Display", "orientation" },
/* (<= 13) Boost enabled                      */{ "Boost mode", "enabled" },
/* (<=  9) Boost temperature                  */{ "Boost", "temp" },
/* (<= 13) Automatic start mode               */{ "Auto", "start" },
/* (<= 13) Cooldown blink                     */{ "Cooldown", "blink" },
/* (<= 16) Temperature calibration enter menu */{ "Calibrate", "temperature?" },
/* (<= 16) Settings reset command             */{ "Factory", "Reset?" },
		/* (<= 16) Calibrate input voltage            */{ "Calibrate",
				"input voltage?" },
		/* (<= 13) Advanced soldering screen enabled  */{ "Detailed",
				"solder screen" }, };
#endif

#ifdef LANG_RU
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Источник питания. Установка напряжения отключения. <DC 10V> <S 3.3 V на батарею>",
	/* Sleep temperature                  */"Температура Сна  <С>",
	/* Sleep timeout                      */"Переход в режим Сна <Минуты>",
	/* Shutdown timeout                   */"Переходит в режим ожидания <Минуты>",
	/* Motion sensitivity level           */"Акселерометр <0. Выкл. 1. мин. чувствительный 9. макс. чувствительный>",
	/* Temperature in F and C             */"В чем измерять температуру",
	/* Advanced idle display mode enabled */"Display detailed information in a smaller font on the idle screen.",
	/* Display rotation mode              */"Ориентация Дисплея <A. Автоматический L. Левая Рука R. Правая Рука>",
	/* Boost enabled                      */"Активация  кнопки A для Турбо режима до  450С при пайке ",
	/* Boost temperature                  */"Установка температуры для Турбо режима",
	/* Automatic start mode               */"Автоматический запуск паяльника при включении питания. T=Нагрев, S= Режим Сна,F=Выкл.",
	/* Cooldown blink                     */"Мигает температура на экране охлаждения, пока жало остается горячим."
	/* Temperature calibration enter menu */"Calibrate tip offset.",
	/* Settings reset command             */"Reset all settings",
	/* Calibrate input voltage            */"VIN Calibration. Buttons adjust, long press to exit",
	/* Advanced soldering screen enabled  */"Display detailed information while soldering",
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
	/* (<= 5) Power source (DC or batt)          */{"PWRSC"},
	/* (<= 4) Sleep temperature                  */{"STMP"},
	/* (<= 4) Sleep timeout                      */{"STME"},
	/* (<= 5) Shutdown timeout                   */{"SHTME"},
	/* (<= 6) Motion sensitivity level           */{"MSENSE"},
	/* (<= 6) Temperature in F and C             */{"TMPUNT"},
	/* (<= 6) Advanced idle display mode enabled */{"ADVIDL"},
	/* (<= 6) Display rotation mode              */{"DSPROT"},
	/* (<= 6) Boost enabled                      */{"BOOST"},
	/* (<= 4) Boost temperature                  */{"BTMP"},
	/* (<= 6) Automatic start mode               */{"ASTART"},
	/* (<= 6) Cooldown blink                     */{"CLBLNK"},
	/* (<= 8) Temperature calibration enter menu */{"TMP CAL?"},
	/* (<= 8) Settings reset command             */{"RESET?"},
	/* (<= 8) Calibrate input voltage            */{"CAL VIN?"},
	/* (<= 6) Advanced soldering screen enabled  */{"ADVSLD"},
};
#endif

#ifdef LANG_ES
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Fuente de energía. Ajusta el límite inferior de voltaje. <DC=10V S=3.3V por celda>",
	/* Sleep temperature                  */"Temperatura en reposo. <C>",
	/* Sleep timeout                      */"Tiempo hasta activar reposo. <Minutos>",
	/* Shutdown timeout                   */"Tiempo hasta apagado. <Minutos>",
	/* Motion sensitivity level           */"Sensibilidad del movimiento. <0=Apagado 1=El menos sensible 9=El más sensible>",
	/* Temperature in F and C             */"Unidad de temperatura.",
	/* Advanced idle display mode enabled */"Display detailed information in a smaller font on the idle screen.",
	/* Display rotation mode              */"Orientación de la pantalla <A=Automático I=Mano izquierda D=Mano derecha>",
	/* Boost enabled                      */"Activar el botón \"Boost\" en modo soldadura.",
	/* Boost temperature                  */"Temperatura en modo \"Boost\". <C>",
	/* Automatic start mode               */"Iniciar modo soldadura en el encendido. <V=Sí S=Modo reposo F=No>",
	/* Cooldown blink                     */"Parpadea la temperatura en el enfriamiento si la punta sigue caliente."
	/* Temperature calibration enter menu */"Calibrate tip offset.",
	/* Settings reset command             */"Reset all settings",
	/* Calibrate input voltage            */"VIN Calibration. Buttons adjust, long press to exit",
	/* Advanced soldering screen enabled  */"Display detailed information while soldering",
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
	/* (<= 5) Power source (DC or batt)          */{"PWRSC"},
	/* (<= 4) Sleep temperature                  */{"STMP"},
	/* (<= 4) Sleep timeout                      */{"STME"},
	/* (<= 5) Shutdown timeout                   */{"SHTME"},
	/* (<= 6) Motion sensitivity level           */{"MSENSE"},
	/* (<= 6) Temperature in F and C             */{"TMPUNT"},
	/* (<= 6) Advanced idle display mode enabled */{"ADVIDL"},
	/* (<= 6) Display rotation mode              */{"DSPROT"},
	/* (<= 6) Boost enabled                      */{"BOOST"},
	/* (<= 4) Boost temperature                  */{"BTMP"},
	/* (<= 6) Automatic start mode               */{"ASTART"},
	/* (<= 6) Cooldown blink                     */{"CLBLNK"},
	/* (<= 8) Temperature calibration enter menu */{"TMP CAL?"},
	/* (<= 8) Settings reset command             */{"RESET?"},
	/* (<= 8) Calibrate input voltage            */{"CAL VIN?"},
	/* (<= 6) Advanced soldering screen enabled  */{"ADVSLD"},
};
#endif

#ifdef LANG_SE
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Источник питания. Установка напряжения отключения. <DC 10V> <S 3.3 V на батарею>",
	/* Sleep temperature                  */"Температура Сна  <С>",
	/* Sleep timeout                      */"Переход в режим Сна <Минуты>",
	/* Shutdown timeout                   */"Переходит в режим ожидания <Минуты>",
	/* Motion sensitivity level           */"Акселерометр <0. Выкл. 1. мин. чувствительный 9. макс. чувствительный>",
	/* Temperature in F and C             */"В чем измерять температуру",
	/* Advanced idle display mode enabled */"Display detailed information in a smaller font on the idle screen.",
	/* Display rotation mode              */"Ориентация Дисплея <A. Автоматический L. Левая Рука R. Правая Рука>",
	/* Boost enabled                      */"Активация  кнопки A для Турбо режима до  450С при пайке ",
	/* Boost temperature                  */"Установка температуры для Турбо режима",
	/* Automatic start mode               */"Автоматический запуск паяльника при включении питания. T=Нагрев, S= Режим Сна,F=Выкл.",
	/* Cooldown blink                     */"Мигает температура на экране охлаждения, пока жало остается горячим."
	/* Temperature calibration enter menu */"Calibrate tip offset.",
	/* Settings reset command             */"Reset all settings",
	/* Calibrate input voltage            */"VIN Calibration. Buttons adjust, long press to exit",
	/* Advanced soldering screen enabled  */"Display detailed information while soldering",
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
	/* (<= 5) Power source (DC or batt)          */{"PWRSC"},
	/* (<= 4) Sleep temperature                  */{"STMP"},
	/* (<= 4) Sleep timeout                      */{"STME"},
	/* (<= 5) Shutdown timeout                   */{"SHTME"},
	/* (<= 6) Motion sensitivity level           */{"MSENSE"},
	/* (<= 6) Temperature in F and C             */{"TMPUNT"},
	/* (<= 6) Advanced idle display mode enabled */{"ADVIDL"},
	/* (<= 6) Display rotation mode              */{"DSPROT"},
	/* (<= 6) Boost enabled                      */{"BOOST"},
	/* (<= 4) Boost temperature                  */{"BTMP"},
	/* (<= 6) Automatic start mode               */{"ASTART"},
	/* (<= 6) Cooldown blink                     */{"CLBLNK"},
	/* (<= 8) Temperature calibration enter menu */{"TMP CAL?"},
	/* (<= 8) Settings reset command             */{"RESET?"},
	/* (<= 8) Calibrate input voltage            */{"CAL VIN?"},
	/* (<= 6) Advanced soldering screen enabled  */{"ADVSLD"},
};
#endif

#ifdef LANG_IT
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Sorgente di alimentazione; imposta il limite minimo di tensione <DC: 10V; S: 3.3V per cella>",
	/* Sleep temperature                  */"Temperatura standby <°C>",
	/* Sleep timeout                      */"Timeout standby <minuti/secondi>",
	/* Shutdown timeout                   */"Timeout spegnimento <minuti>",
	/* Motion sensitivity level           */"Sensibilità al movimento <0: nessuna; 1: minima; 9: massima>",
	/* Temperature in F and C             */"Unità di misura della temperatura <C: Celsius; F: Farenheit>",
	/* Advanced idle display mode enabled */"Mostra informazioni dettagliate con un carattere più piccolo sulla schermata di inattività",
	/* Display rotation mode              */"Orientamento del display <A: automatico; S: mano sinistra; D: mano destra>",
	/* Boost enabled                      */"Il tasto anteriore attiva la modalità \"boost\" durante la saldatura",
	/* Boost temperature                  */"Temperatura in modalità \"boost\"",
	/* Automatic start mode               */"Attiva automaticamente il saldatore quando viene alimentato <A: saldatura; S: standby; D: disattiva>",
	/* Cooldown blink                     */"Durante il raffreddamento mostra la temperatura sul display se la punta è ancora calda"
	/* Temperature calibration enter menu */"Calibra l'offset della punta",
	/* Settings reset command             */"Ripristina tutte le impostazioni",
	/* Calibrate input voltage            */"Calibra la tensione in entrata; regola con i bottoni, tieni permuto per uscire",
	/* Advanced soldering screen enabled  */"Mostra informazioni dettagliate mentre stai saldando",
};

const char* SettingsCalibrationWarning = "Assicurati che la punta si trovi a temperatura ambiente prima di continuare!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Standby";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "ATTENZIONE! PUNTA CALDA!";

const char SettingTrueChar = 'A';
const char SettingFalseChar = 'D';
const char SettingRightChar = 'D';
const char SettingLeftChar = 'S';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
	/* (<= 5) Power source (DC or batt)          */{"PWRSC"},
	/* (<= 4) Sleep temperature                  */{"STMP"},
	/* (<= 4) Sleep timeout                      */{"STME"},
	/* (<= 5) Shutdown timeout                   */{"SHTME"},
	/* (<= 6) Motion sensitivity level           */{"MSENSE"},
	/* (<= 6) Temperature in F and C             */{"TMPUNT"},
	/* (<= 6) Advanced idle display mode enabled */{"ADVIDL"},
	/* (<= 6) Display rotation mode              */{"DSPROT"},
	/* (<= 6) Boost enabled                      */{"BOOST"},
	/* (<= 4) Boost temperature                  */{"BTMP"},
	/* (<= 6) Automatic start mode               */{"ASTART"},
	/* (<= 6) Cooldown blink                     */{"CLBLNK"},
	/* (<= 8) Temperature calibration enter menu */{"TMP CAL?"},
	/* (<= 8) Settings reset command             */{"RESET?"},
	/* (<= 8) Calibrate input voltage            */{"CAL VIN?"},
	/* (<= 6) Advanced soldering screen enabled  */{"ADVSLD"},
};
#endif

#ifdef LANG_FR
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Type d\'alimentation. Regle la tension de coupure. <DC=10V S=3.3V par cellules>",
	/* Sleep temperature                  */"Temperature en veille. <C>",
	/* Sleep timeout                      */"Temps avant mise en veille. <Minutes>",
	/* Shutdown timeout                   */"Temps avant extinction. <Minutes>",
	/* Motion sensitivity level           */"Sensibilitee du capteur de mouvement. <0=Inactif 1=Peu sensible 9=Tres sensible>",
	/* Temperature in F and C             */"Unitee de temperature.",
	/* Advanced idle display mode enabled */"Afficher des informations detaillees en petit lors de la veille",
	/* Display rotation mode              */"Orientation de l\'affichage. <A=Automatique G=Gaucher D=Droitier>",
	/* Boost enabled                      */"Active le mode \"Boost\" 450C sur le bouton de devant pendant la soudure.",
	/* Boost temperature                  */"Temperature du mode \"Boost\". <C>",
	/* Automatic start mode               */"Demarre automatiquement la soudure a l\'allumage. <A=Active, V=Mode Veille, D=Desactive>",
	/* Cooldown blink                     */"Fait clignotter la temperature lors du refroidissement pendant que la panne est chaude."
	/* Temperature calibration enter menu */"Compenser l\'erreur de la panne",
	/* Settings reset command             */"Reinitialiser tout les reglages",
	/* Calibrate input voltage            */"Calibration VIN. Boutons pour ajuster, appui long pour quitter",
	/* Advanced soldering screen enabled  */"Afficher des informations detaillees pendant la soudure",
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
	/* (<= 5) Power source (DC or batt)          */{"PWRSC"},
	/* (<= 4) Sleep temperature                  */{"STMP"},
	/* (<= 4) Sleep timeout                      */{"STME"},
	/* (<= 5) Shutdown timeout                   */{"SHTME"},
	/* (<= 6) Motion sensitivity level           */{"MSENSE"},
	/* (<= 6) Temperature in F and C             */{"TMPUNT"},
	/* (<= 6) Advanced idle display mode enabled */{"ADVIDL"},
	/* (<= 6) Display rotation mode              */{"DSPROT"},
	/* (<= 6) Boost enabled                      */{"BOOST"},
	/* (<= 4) Boost temperature                  */{"BTMP"},
	/* (<= 6) Automatic start mode               */{"ASTART"},
	/* (<= 6) Cooldown blink                     */{"CLBLNK"},
	/* (<= 8) Temperature calibration enter menu */{"TMP CAL?"},
	/* (<= 8) Settings reset command             */{"RESET?"},
	/* (<= 8) Calibrate input voltage            */{"CAL VIN?"},
	/* (<= 6) Advanced soldering screen enabled  */{"ADVSLD"},
};
#endif

#ifdef LANG_DE
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Spannungsquelle (Abschaltspannung) <DC=10V, nS=n*3.3V für n LiIon-Zellen>",
	/* Sleep temperature                  */"Ruhetemperatur (In der eingestellten Einheit)",
	/* Sleep timeout                      */"Ruhemodus nach <Sekunden/Minuten>",
	/* Shutdown timeout                   */"Abschaltzeit <Minuten>",
	/* Motion sensitivity level           */"Bewegungsempfindlichkeit <0=Aus, 1=Minimal ... 9=Maximal>",
	/* Temperature in F and C             */"Temperatureinheit <C=Celsius, F=Fahrenheit>",
	/* Advanced idle display mode enabled */"Detaillierte Anzeige im Ruhemodus <T=An, F=Aus>",
	/* Display rotation mode              */"Ausrichtung der Anzeige <A=Auto, L=Linkshändig, R=Rechtshändig>",
	/* Boost enabled                      */"Vordere Taste für Temperaturboost verwenden <T=An, F=Aus>",
	/* Boost temperature                  */"Temperatur im Boostmodus  (In der eingestellten Einheit)",
	/* Automatic start mode               */"Automatischer Start des Lötmodus beim Einschalten der Spannungsversorgung. <T=An, F=Aus>",
	/* Cooldown blink                     */"Blinkende Temperaturanzeige beim Abkühlen, solange heiß. <T=An, F=Aus>"
	/* Temperature calibration enter menu */"Kalibrierung der Lötspitzentemperatur",
	/* Settings reset command             */"Alle Einstellungen zurücksetzen",
	/* Calibrate input voltage            */"Kalibrierung der Eingangsspannung. Kurzer Tastendruck zum Einstellen, langer Tastendruck zum Verlassen.",
	/* Advanced soldering screen enabled  */"Detaillierte Anzeige im Lötmodus <T=An, F=Aus>",
};

const char* SettingsCalibrationWarning = "Vor dem Fortfahren muss die Lötspitze vollständig abgekühlt sein!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzz ";// Must be <= 4 chars
const char* SleepingAdvancedString = "Ruhemodus...";// <=17 chars
const char* WarningSimpleString = "HEIß";//Must be <= 4 chars
const char* WarningAdvancedString = "Achtung! Spitze Heiß!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
	/* (<= 5) Power source (DC or batt)          */{"PWRSC"},
	/* (<= 4) Sleep temperature                  */{"STMP"},
	/* (<= 4) Sleep timeout                      */{"STME"},
	/* (<= 5) Shutdown timeout                   */{"SHTME"},
	/* (<= 6) Motion sensitivity level           */{"MSENSE"},
	/* (<= 6) Temperature in F and C             */{"TMPUNT"},
	/* (<= 6) Advanced idle display mode enabled */{"ADVIDL"},
	/* (<= 6) Display rotation mode              */{"DSPROT"},
	/* (<= 6) Boost enabled                      */{"BOOST"},
	/* (<= 4) Boost temperature                  */{"BTMP"},
	/* (<= 6) Automatic start mode               */{"ASTART"},
	/* (<= 6) Cooldown blink                     */{"CLBLNK"},
	/* (<= 8) Temperature calibration enter menu */{"TMP CAL?"},
	/* (<= 8) Settings reset command             */{"RESET?"},
	/* (<= 8) Calibrate input voltage            */{"CAL VIN?"},
	/* (<= 6) Advanced soldering screen enabled  */{"ADVSLD"},
};
#endif

#ifdef LANG_SK
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Zdroj napatia. Nastavit napatie pre vypnutie (cutoff)  <DC=10V, nS=n*3.3V pre LiIon clanky>",
	/* Sleep temperature                  */"Kludova teplota (v nastavenych jednotkach)",
	/* Sleep timeout                      */"Kludovy rezim po <sekundach/minutach>",
	/* Shutdown timeout                   */"Cas na vypnutie <minuty>",
	/* Motion sensitivity level           */"Citlivost detekcie pohybu <0=Vyp, 1=Min ... 9=Max>",
	/* Temperature in F and C             */"Jednotky merania teploty <C=stupne Celzia, F=stupne Fahrenheita>",
	/* Advanced idle display mode enabled */"Zobrazit detailne informacie v kludovom rezime <T=Zap, F=Vyp>",
	/* Display rotation mode              */"Orientacia displeja <A=Auto, L=Lavak, R=Pravak>",
	/* Boost enabled                      */"Povolit tlacidlo pre prudky nahrev <T=Zap, F=Vyp>",
	/* Boost temperature                  */"Cielova teplota pre prudky nahrev (v nastavenych jednotkach)",
	/* Automatic start mode               */"Pri starte spustit rezim spajkovania <T=Zap, F=Vyp, S=Spanok>",
	/* Cooldown blink                     */"Blikanie ukazovatela teploty pocas chladnutia hrotu <T=Zap, F=Vyp>"
	/* Temperature calibration enter menu */"Kalibracia posunu hrotu",
	/* Settings reset command             */"Tovarenske nastavenia",
	/* Calibrate input voltage            */"Kalibracia VIN. Kratke stlacenie meni nastavenie, dlhe stlacenie pre navrat",
	/* Advanced soldering screen enabled  */"Zobrazenie detailov pocas spajkovania <T=Zap, F=Vyp>",
};

const char* SettingsCalibrationWarning = "Najprv sa prosim uistite, ze hrot ma izbovu teplotu!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Chrr";// Must be <= 4 chars
const char* SleepingAdvancedString = "Kludovy rezim...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "Pozor! Hrot je horuci!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
	/* (<= 5) Power source (DC or batt)          */{"PWRSC"},
	/* (<= 4) Sleep temperature                  */{"STMP"},
	/* (<= 4) Sleep timeout                      */{"STME"},
	/* (<= 5) Shutdown timeout                   */{"SHTME"},
	/* (<= 6) Motion sensitivity level           */{"MSENSE"},
	/* (<= 6) Temperature in F and C             */{"TMPUNT"},
	/* (<= 6) Advanced idle display mode enabled */{"ADVIDL"},
	/* (<= 6) Display rotation mode              */{"DSPROT"},
	/* (<= 6) Boost enabled                      */{"BOOST"},
	/* (<= 4) Boost temperature                  */{"BTMP"},
	/* (<= 6) Automatic start mode               */{"ASTART"},
	/* (<= 6) Cooldown blink                     */{"CLBLNK"},
	/* (<= 8) Temperature calibration enter menu */{"TMP CAL?"},
	/* (<= 8) Settings reset command             */{"RESET?"},
	/* (<= 8) Calibrate input voltage            */{"CAL VIN?"},
	/* (<= 6) Advanced soldering screen enabled  */{"ADVSLD"},
};
#endif

#ifdef LANG_TR
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Güç Kaynağı. kesim geriliminı ayarlar. <DC 10V> <S 3.3V hücre başına>",
	/* Sleep temperature                  */"Uyku Sıcaklığı <C>",
	/* Sleep timeout                      */"Uyku Zaman Aşımı <Dakika/Saniye>",
	/* Shutdown timeout                   */"Kapatma Zaman Aşımı <Dakika>",
	/* Motion sensitivity level           */"Hareket Hassasiyeti <0.Kapalı 1.En az duyarlı 9.En duyarlı>",
	/* Temperature in F and C             */"Sıcaklık Ünitesi <C=Celsius F=Fahrenheit>",
	/* Advanced idle display mode enabled */"Boş ekranda ayrıntılı bilgileri daha küçük bir yazı tipi ile göster.",
	/* Display rotation mode              */"Görüntü Yönlendirme <A. Otomatik L. Solak R. Sağlak>",
	/* Boost enabled                      */"Lehimleme yaparken ön tuşa basmak Boost moduna sokar(450C)",
	/* Boost temperature                  */"\"boost\" Modu Derecesi",
	/* Automatic start mode               */"Güç verildiğinde otomatik olarak lehimleme modunda başlat. T=Lehimleme Modu, S= Uyku Modu,F=Kapalı",
	/* Cooldown blink                     */"Soğutma ekranında uç hala sıcakken derece yanıp sönsün.",
	/* Temperature calibration enter menu */"Ucu kalibre et.",
	/* Settings reset command             */"Bütün ayarları sıfırla",
	/* Calibrate input voltage            */"VIN Kalibrasyonu. Düğmeler ayarlar, çıkmak için uzun bas.",
	/* Advanced soldering screen enabled  */"Lehimleme yaparken detaylı bilgi göster",
};

const char* SettingsCalibrationWarning = "Lütfen devam etmeden önce ucun oda sıcaklığında olduğunu garantiye alın!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Uyuyor...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "UYARI! UÇ SICAK!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
	/* (<= 5) Power source (DC or batt)          */{"PWRSC"},
	/* (<= 4) Sleep temperature                  */{"STMP"},
	/* (<= 4) Sleep timeout                      */{"STME"},
	/* (<= 5) Shutdown timeout                   */{"SHTME"},
	/* (<= 6) Motion sensitivity level           */{"MSENSE"},
	/* (<= 6) Temperature in F and C             */{"TMPUNT"},
	/* (<= 6) Advanced idle display mode enabled */{"ADVIDL"},
	/* (<= 6) Display rotation mode              */{"DSPROT"},
	/* (<= 6) Boost enabled                      */{"BOOST"},
	/* (<= 4) Boost temperature                  */{"BTMP"},
	/* (<= 6) Automatic start mode               */{"ASTART"},
	/* (<= 6) Cooldown blink                     */{"CLBLNK"},
	/* (<= 8) Temperature calibration enter menu */{"TMP CAL?"},
	/* (<= 8) Settings reset command             */{"RESET?"},
	/* (<= 8) Calibrate input voltage            */{"CAL VIN?"},
	/* (<= 6) Advanced soldering screen enabled  */{"ADVSLD"},
};
#endif

#ifdef LANG_HR
const char* SettingsLongNames[16] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Izvor napajanja. Postavlja napon iskljucivanja. <DC 10V> <S 3.3V po celiji>",
	/* Sleep temperature                  */"Temperatura spavanja. <C>",
	/* Sleep timeout                      */"Vrijeme spavanja. <Minute/Sekunde>",
	/* Shutdown timeout                   */"Vrijeme gasenja. <Minutes>",
	/* Motion sensitivity level           */"Osjetljivost prepoznavanja pokreta. <0=Ugaseno, 1=Najmanje osjetljivo, 9=Najosjetljivije>",
	/* Temperature in F and C             */"Jedinica temperature. <C=Celzij, F=Fahrenheit>",
	/* Advanced idle display mode enabled */"Prikazivanje detaljnih informacija manjim fontom tijekom cekanja.",
	/* Display rotation mode              */"Orijentacija ekrana. <A=Automatski, L=Ljevoruki, D=Desnoruki>",
	/* Boost enabled                      */"Prednji gumb prilikom lemljenja aktivira pojacani (Boost) nacin.",
	/* Boost temperature                  */"Temperatura u pojacanom (Boost) nacinu.",
	/* Automatic start mode               */"Nacin u kojemu se lemilica pokrece po ukljucivanju napajanja. <+=Lemljenje, S=Spavanje, -=Ugaseno>",
	/* Cooldown blink                     */"Bljeskanje temperature prilikom hladjenja, ako je lemilica vruca.",
	/* Temperature calibration enter menu */"Kalibriranje temperature vrha.",
	/* Settings reset command             */"Vracanje svih postavki.",
	/* Calibrate input voltage            */"Kalibracija ulaznog napona. Podesavanje gumbima, dugacki pritisak za kraj.",
	/* Advanced soldering screen enabled  */"Prikazivanje detaljnih informacija tijekom lemljenja.",
};

const char* SettingsCalibrationWarning = "Provjerite da je vrh ohladjen na sobnu temperaturu prije nego sto nastavite!";
const char* UVLOWarningString = "NAPON!!!";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Spavanje...";// <=17 chars
const char* WarningSimpleString = "VRUC";//Must be <= 4 chars
const char* WarningAdvancedString = "OPREZ! Vrsak je vruc!";

const char SettingTrueChar = '+';
const char SettingFalseChar = '-';
const char SettingRightChar = 'D';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
	/* (<= 5) Power source (DC or batt)          */{"PWRSC"},
	/* (<= 4) Sleep temperature                  */{"STMP"},
	/* (<= 4) Sleep timeout                      */{"STME"},
	/* (<= 5) Shutdown timeout                   */{"SHTME"},
	/* (<= 6) Motion sensitivity level           */{"MSENSE"},
	/* (<= 6) Temperature in F and C             */{"TMPUNT"},
	/* (<= 6) Advanced idle display mode enabled */{"ADVIDL"},
	/* (<= 6) Display rotation mode              */{"DSPROT"},
	/* (<= 6) Boost enabled                      */{"BOOST"},
	/* (<= 4) Boost temperature                  */{"BTMP"},
	/* (<= 6) Automatic start mode               */{"ASTART"},
	/* (<= 6) Cooldown blink                     */{"CLBLNK"},
	/* (<= 8) Temperature calibration enter menu */{"TMP CAL?"},
	/* (<= 8) Settings reset command             */{"RESET?"},
	/* (<= 8) Calibrate input voltage            */{"CAL VIN?"},
	/* (<= 6) Advanced soldering screen enabled  */{"ADVSLD"},
};
#endif

#ifdef LANG_CS_CZ
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Zdroj napajeni. Pri nizsim napeti se odpoji <DC=10V, xS=x*3.3V pro LiPo,LiIon...>", //Power Source
	"Teplota v rezimu spanku",//Sleep Temp
	"Cas do rezimu spanku <Minut/Sekund>",//Sleep Timeout
	"Cas do automatickeho vypnuti <Minut>",//Shutdown Time
	"Citlivost detekce pohybu <0=Vyp, 1=Min, ... 9=Max>",//Motion Sensitivity
	"Jednotky mereni teploty <C=Celsius, F=Fahrenheit>",//Temp Unit
	"Zobrazit podrobnosti na vychozi obrazovce <Z=Zap, V=Vyp>",//Detailed Information
	"Otoceni displaye <A=Auto, L=Levak, P=Pravak>",//Orientation
	"Povolit boost drzenim leveho tlacitka pri pajeni <Z=Zap, V=Vyp>",//Boost enable
	"Teplota pri boostu",//Boost Temp
	"Pri startu ihned nahrivat hrot <Z=Zap, V=Vyp, S=Rezim spanku>",//Auto start
	"Blikani teploty pri chladnuti, dokud je hrot horky <Z=Zap, V=Vyp>",//Cooling Blink
	"Kalibrovat mereni teploty",//Calibrate Tip
	"Obnovit tovarni nastaveni",//Reset Settings
	"Kalibrovat vstupni napeti. Tlacitky upravte, podrzenim potvrdte.",//VIN Cal
	"Zobrazit podrobnosti pri pajeni <Z=Zap, V=Vyp>",//ADV SLD
};

const char* SettingsCalibrationWarning = "Ujistete se, ze hrot ma pokojovou teplotu! "; //ending space needed
const char* UVLOWarningString = "LOW VOLT";//Fixed width 8 chars
const char* SleepingSimpleString = "Zzz ";// Must be <= 4 chars
const char* SleepingAdvancedString = "Rezim spanku...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "!! HORKY HROT !!";// <= 16 chars

const char SettingTrueChar = 'Z';
const char SettingFalseChar = 'V';
const char SettingRightChar = 'P';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
	/* (<= 5) Power source (DC or batt)          */{"PWRSC"},
	/* (<= 4) Sleep temperature                  */{"STMP"},
	/* (<= 4) Sleep timeout                      */{"STME"},
	/* (<= 5) Shutdown timeout                   */{"SHTME"},
	/* (<= 6) Motion sensitivity level           */{"MSENSE"},
	/* (<= 6) Temperature in F and C             */{"TMPUNT"},
	/* (<= 6) Advanced idle display mode enabled */{"ADVIDL"},
	/* (<= 6) Display rotation mode              */{"DSPROT"},
	/* (<= 6) Boost enabled                      */{"BOOST"},
	/* (<= 4) Boost temperature                  */{"BTMP"},
	/* (<= 6) Automatic start mode               */{"ASTART"},
	/* (<= 6) Cooldown blink                     */{"CLBLNK"},
	/* (<= 8) Temperature calibration enter menu */{"TMP CAL?"},
	/* (<= 8) Settings reset command             */{"RESET?"},
	/* (<= 8) Calibrate input voltage            */{"CAL VIN?"},
	/* (<= 6) Advanced soldering screen enabled  */{"ADVSLD"},
};
#endif

#ifdef LANG_HUN
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Áramforrás. Beállítja a lekapcsolási feszültséget. <DC 10V> <S 3.3V cellánként>", //Power Source
	"Alvási hőmérséklet <C>",//Sleep Temp
	"Elalvási időzítő <Perc/Másodperc>",//Sleep Timeout
	"Kikapcsolási időzítő <Minutes>",//Shutdown Time
	"Mozgás érzékenység beállítása. <0.Ki 1.kevésbé érzékeny 9.legérzékenyebb>",//Motion Sensitivity
	"Hőmérsékleti egység <C=Celsius F=Fahrenheit>",//Temp Unit
	"Részletes információ megjelenítése kisebb betűméretben a készenléti képernyőn.",//Detailed Information
	"Megjelenítési tájolás <A. Automatikus L. Balkezes R. Jobbkezes>",//Orientation
	"Elülső gombbal lépjen boost módba, 450C forrasztás közben",//Boost enable
	"Hőmérséklet \"boost\" módban",//Boost Temp
	"Bekapcsolás után automatikusan lépjen forrasztás módba. T=Forrasztás, S=Alvó mód,F=Ki",//Auto start
	"Villogjon a hőmérséklet hűlés közben, amíg a hegy forró.",//Cooling Blink
	"Hegy hőmérsékletének kalibrálása",//Calibrate Tip
	"Beállítások alaphelyzetbe állítása",//Reset Settings
	"A bemeneti feszültség kalibrálása. Röviden megnyomva állítsa be, hosszan nyomja meg a kilépéshez.",//VIN Cal
	"Részletes információk megjelenítése forrasztás közben",//ADV SLD
};

const char* SettingsCalibrationWarning = "Folytatás előtt győződj meg róla, hogy a hegy szobahőmérsékletű!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Alvás...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "FIGYELEM! FORRÓ HEGY!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
	/* (<= 5) Power source (DC or batt)          */{"PWRSC"},
	/* (<= 4) Sleep temperature                  */{"STMP"},
	/* (<= 4) Sleep timeout                      */{"STME"},
	/* (<= 5) Shutdown timeout                   */{"SHTME"},
	/* (<= 6) Motion sensitivity level           */{"MSENSE"},
	/* (<= 6) Temperature in F and C             */{"TMPUNT"},
	/* (<= 6) Advanced idle display mode enabled */{"ADVIDL"},
	/* (<= 6) Display rotation mode              */{"DSPROT"},
	/* (<= 6) Boost enabled                      */{"BOOST"},
	/* (<= 4) Boost temperature                  */{"BTMP"},
	/* (<= 6) Automatic start mode               */{"ASTART"},
	/* (<= 6) Cooldown blink                     */{"CLBLNK"},
	/* (<= 8) Temperature calibration enter menu */{"TMP CAL?"},
	/* (<= 8) Settings reset command             */{"RESET?"},
	/* (<= 8) Calibrate input voltage            */{"CAL VIN?"},
	/* (<= 6) Advanced soldering screen enabled  */{"ADVSLD"},
};
#endif

#ifdef LANG_TR
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	/* Power source (DC or batt)          */"Güç Kaynağı. kesim geriliminı ayarlar. <DC 10V> <S 3.3V hücre başına>", //Power Source
	/* Sleep temperature                  */"Uyku Sıcaklığı <C>",//Sleep Temp
	/* Sleep timeout                      */"Uyku Zaman Aşımı <Dakika/Saniye>",//Sleep Timeout
	/* Shutdown timeout                   */"Kapatma Zaman Aşımı <Dakika>",//Shutdown Time
	/* Motion sensitivity level           */"Hareket Hassasiyeti <0.Kapalı 1.En az duyarlı 9.En duyarlı>",//Motion Sensitivity
	/* Temperature in F and C             */"Sıcaklık Ünitesi <C=Celsius F=Fahrenheit>",//Temp Unit
	/* Advanced idle display mode enabled */"Boş ekranda ayrıntılı bilgileri daha küçük bir yazı tipi ile göster.",//Detailed Information
	/* Display rotation mode              */"Görüntü Yönlendirme <A. Otomatik L. Solak R. Sağlak>",//Orientation
	/* Boost enabled                      */"Lehimleme yaparken ön tuşa basmak Boost moduna sokar(450C)",//Boost enable
	/* Boost temperature                  */"\"boost\" Modu Derecesi",//Boost Temp
	/* Automatic start mode               */"Güç verildiğinde otomatik olarak lehimleme modunda başlat. T=Lehimleme Modu, S= Uyku Modu,F=Kapalı",//Auto start
	/* Cooldown blink                     */"Soğutma ekranında uç hala sıcakken derece yanıp sönsün.",//Cooling Blink
	/* Temperature calibration enter menu */"Ucu kalibre et.",//Calibrate Tip
	/* Settings reset command             */"Bütün ayarları sıfırla",//Reset Settings
	/* Calibrate input voltage            */"VIN Kalibrasyonu. Düğmeler ayarlar, çıkmak için uzun bas.",//VIN Cal
	/* Advanced soldering screen enabled  */"Lehimleme yaparken detaylı bilgi göster",//ADV SLD
};

const char* SettingsCalibrationWarning = "Lütfen devam etmeden önce ucun oda sıcaklığında olduğunu garantiye alın!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Uyuyor...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "UYARI! UÇ SICAK!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[16][2] = {
	/* (<= 5) Power source (DC or batt)          */{"PWRSC"},
	/* (<= 4) Sleep temperature                  */{"STMP"},
	/* (<= 4) Sleep timeout                      */{"STME"},
	/* (<= 5) Shutdown timeout                   */{"SHTME"},
	/* (<= 6) Motion sensitivity level           */{"MSENSE"},
	/* (<= 6) Temperature in F and C             */{"TMPUNT"},
	/* (<= 6) Advanced idle display mode enabled */{"ADVIDL"},
	/* (<= 6) Display rotation mode              */{"DSPROT"},
	/* (<= 6) Boost enabled                      */{"BOOST"},
	/* (<= 4) Boost temperature                  */{"BTMP"},
	/* (<= 6) Automatic start mode               */{"ASTART"},
	/* (<= 6) Cooldown blink                     */{"CLBLNK"},
	/* (<= 8) Temperature calibration enter menu */{"TMP CAL?"},
	/* (<= 8) Settings reset command             */{"RESET?"},
	/* (<= 8) Calibrate input voltage            */{"CAL VIN?"},
	/* (<= 6) Advanced soldering screen enabled  */{"ADVSLD"},
};
#endif

