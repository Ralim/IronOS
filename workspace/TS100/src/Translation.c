/*
 * Translation.c
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Translation.h"

#define LANG_EN
#define LANG

#ifndef LANG
#define LANG_EN
#define LANG
#endif

#ifndef LANG
#error NO LANGUAGE DEFINED
#endif
#ifdef LANG_EN
const char* SettingsLongNames[14] = {
/*These are all the help text for all the settings.*/
/*No requirements on spacing or length*/
"Power source. Sets cutoff voltage. <DC 10V> <S 3.3V per cell>",    //
		"Sleep Temperature <C>",    //
		"Sleep Timeout <Minutes>",    //
		"Shutdown Timeout <Minutes>",    //
		"Motion Sensitivity <0.Off 1.least sensitive 9.most sensitive>",    //
		"Temperature Unit <C=Celsius F=Fahrenheit>",    //
		"Display detailed information in a smaller font.",    //
		"Display Orientation <A. Automatic L. Left Handed R. Right Handed>",    //
		"Enable front key enters boost mode 450C mode when soldering",    //
		"Temperature when in \"boost\" mode",    //
		"Automatically starts the iron into soldering on power up. T=Soldering, S= Sleep mode,F=Off",    //
		"Blink the temperature on the cooling screen while the tip is still hot.",    //
		"Calibrate tip offset.",    //
		"Reset all settings",    //
		"VIN Calibration. Buttons adjust, long press to exit",    //
		};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "ZZzz";    // Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";    // <=17 chars
const char* WarningSimpleString = "HOT!";    //Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'V';
const char SettingFalseChar = 'F';
const char SettingSleepChar = 'S';
const char SettingFastChar = 'R';
const char SettingMediumChar = 'M';
const char SettingSlowChar = 'L';
const char SettingRightChar = 'D';
const char SettingLeftChar = 'I';
const char SettingAutoChar = 'A';
const char SettingTempCChar = 'C';
const char SettingTempFChar = 'F';
#endif

#ifdef LANG_ES
const char* SettingsLongNames[14] = {
	/*These are all the help text for all the settings.*/
	/*All must start with 6 spaces so they come on screen nicely.*/
	"Fuente de energía. Ajusta el límite inferior de voltaje. <DC=10V S=3.3V por celda>",    //
	"Temperatura en reposo. <C>",//
	"Tiempo hasta activar reposo. <Minutos>",//
	"Tiempo hasta apagado. <Minutos>",//
	"Sensibilidad del movimiento. <0=Apagado 1=El menos sensible 9=El más sensible>",//
	"Display detailed information in a smaller font.",//
	"Orientación de la pantalla <A=Automático I=Mano izquierda D=Mano derecha>",//
	"Activar el botón \"Boost\" en modo soldadura.",//
	"Temperatura en modo \"Boost\". <C>",//
	"Iniciar modo soldadura en el encendido. <V=Sí S=Modo reposo F=No>",//
	"Parpadea la temperatura en el enfriamiento si la punta sigue caliente.",//
	"Calibrate tip offset.",//
	"Reset all settings",//
};
const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char SettingTrueChar = 'V';
const char SettingFalseChar = 'F';
const char SettingSleepChar = 'S';
const char SettingFastChar = 'R';
const char SettingMediumChar = 'M';
const char SettingSlowChar = 'L';
const char SettingRightChar = 'D';
const char SettingLeftChar = 'I';
const char SettingAutoChar = 'A';
const char SettingTempCChar = 'C';
const char SettingTempFChar = 'F';
#endif

#ifdef LANG_DE
const char* SettingsLongNames[14] = {
	/*These are all the help text for all the settings.*/
	/*All must start with 6 spaces so they come on screen nicely.*/
	"Stromversorgung. Setzt Abschaltspannung <DC=10V S=3.3V pro Zelle>",
	"Ruhetemperatur <C>",
	"Ruhemodus nach <Minuten>",
	"Abschaltzeit <Minuten>",
	"Bewegungsempfindlichkeit <0=Aus 1=Minimal 9=Maximal>",
	"Display detailed information in a smaller font.",    //
	"Anzeigerichtung <A=Auto L=Linkshändig R=Rechtshändig>",
	"Fronttaste für Temperaturboost einschalten",
	"Temperatur im \"boost\"-Modus <C>",
	"Automatischer Start beim Einschalten. <J=Löttemp R=Ruhemodus N=Aus>",
	"Temperatur blinkt beim Abkühlen, solange noch heiß.",
	"Calibrate tip offset.",//s
	"Reset all settings",};
};
const char* UVLOWarningString = "V gering";    //Fixed width 8 chars
const char* CoolingPromptString = "Kalt ";//Fixed width 5 chars
const char SettingTrueChar = 'J';
const char SettingFalseChar = 'N';
const char SettingSleepChar = 'R';
const char SettingFastChar = 'S';
const char SettingMediumChar = 'M';
const char SettingSlowChar = 'L';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingTempCChar = 'C';
const char SettingTempFChar = 'F';
#endif

#ifdef LANG_FR
const char* SettingsLongNames[14] = {
	/*These are all the help text for all the settings.*/
	/*All must start with 6 spaces so they come on screen nicely.*/
	"Type d\'alimentation. Regle la tension de coupure. <DC=10V S=3.3V par cellules>",
	"Temperature en veille. <C>",
	"Temps avant mise en veille. <Minutes>",
	"Temps avant extinction. <Minutes>",
	"Sensibilitee du capteur de mouvement. <0=Inactif 1=Peu sensible 9=Tres sensible>",
	"Display detailed information in a smaller font.",    //
	"Orientation de l\'affichage. <A=Automatique G=Gaucher D=Droitier>",
	"Active le mode \"Boost\" 450C sur le bouton de devant pendant la soudure.",
	"Temperature du mode \"Boost\". <C>",
	"Demarre automatiquement la soudure a l\'allumage. <A=Active, V=Mode Veille, D=Desactive>",
	"Fait clignotter la temperature pendant la phase de refroidissement quand la panne est chaude.",
	"Calibrate tip offset.",//s
	"Reset all settings",};
};
const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";

const char* UVLOWarningString = "Batt Bas";    //Fixed width 8 chars
const char* CoolingPromptString = "Etein";//Fixed width 5 chars
const char SettingTrueChar = 'A';
const char SettingFalseChar = 'D';
const char SettingSleepChar = 'V';
const char SettingFastChar = 'R';
const char SettingMediumChar = 'M';
const char SettingSlowChar = 'L';
const char SettingRightChar = 'D';
const char SettingLeftChar = 'G';
const char SettingAutoChar = 'A';
const char SettingTempCChar = 'C';
const char SettingTempFChar = 'F';
#endif

#ifdef LANG_IT
const char* SettingsLongNames[14] = {
	/*These are all the help text for all the settings.*/
	"Sorgente di alimentazione. Imposta il limite inferiore di tensione. <DC=10V S=3.3V per cella>",
	"Temperatura modalità riposo <C>",
	"Timeout per passaggio a modalità riposo <Minuti>",
	"Timeout spegnimento <Minuti>",
	"Sensibilità al movimento <0=Spento 1=Sensibilità minima 9=Sensibilità massima>",
	"Display detailed information in a smaller font.",    //
	"Orientamento del display <A=Automatico S=Sinistrorso D=Destrorso>",
	"Il tasto anteriore abilita modalità \"boost\" fino a 450C durante la saldatura",
	"Temperatura in modalità \"boost\" <C>",
	"Avvia automaticamente il saldatore quando viene alimentato. <S=Modalità saldatura R=Modalità riposo N=Spento>",
	"Durante lo spegnimento la temperatura lampeggia sul display finché la punta è calda.",
	"Calibrate tip offset.",//s
	"Reset all settings",};
};
const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";

const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* CoolingPromptString = "Cool";//Fixed width 5 chars
const char SettingTrueChar = 'S';
const char SettingFalseChar = 'N';
const char SettingSleepChar = 'R';
const char SettingFastChar = 'V';
const char SettingMediumChar = 'M';
const char SettingSlowChar = 'L';
const char SettingRightChar = 'D';
const char SettingLeftChar = 'S';
const char SettingAutoChar = 'A';
const char SettingTempCChar = 'C';
const char SettingTempFChar = 'F';
#endif

#ifdef LANG_SE
const char* SettingsLongNames[14] = {
	/*These are all the help text for all the settings.*/
	"Stromforsorjning. Satt avstagningsvolt. <VX=10V S=3.3V per cell>",
	"Vilolage Temperatur <C>",
	"Vilolage Timeout <Minuter>",
	"Avstagningstimeout <Minuter>",
	"Rorelsekanslighet <0=Av 1=Minsta kanslighet 9=Hogsta kanslighet>",
	"Display detailed information in a smaller font.",    //
	"Skarmorientation <A=Automatisk V=Vansterhant H=Hogerhant>",
	"Aktivera boost-lage med framre knappen <P=Pa A=Av>",
	"Temperatur i \"boostlage\" <C>",
	"Startar i lodningslage direkt <L=Lodning V=Vilolage A=Av>",
	"Blinka temperaturen medans jarnet fortfarande ar varmt. <P=Pa A=Av>",
	"Calibrate tip offset.",//s
	"Reset all settings",};
};
const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";

const char* UVLOWarningString = "Lag Volt";    //Fixed width 8 chars
const char* CoolingPromptString = "Sval ";//Fixed width 5 chars
const char SettingTrueChar = 'P';
const char SettingFalseChar = 'A';
const char SettingSleepChar = 'V';
const char SettingFastChar = 'S';
const char SettingMediumChar = 'M';
const char SettingSlowChar = 'L';
const char SettingRightChar = 'H';
const char SettingLeftChar = 'V';
const char SettingAutoChar = 'A';
const char SettingTempCChar = 'C';
const char SettingTempFChar = 'F';
#endif

#ifdef LANG_RU
const char* SettingsLongNames[14] = {
//These are all the help text for all the settings./
	"Источник питания. Установка напряжения отключения. <DC 10V> <S 3.3 V на батарею>", "Температура Сна <С>",
	"Переход в режим Сна <Минуты>", "Переходит в режим ожидания <Минуты>",
	"Акселерометр <0. Выкл. 1. мин. чувствительный 9. макс. чувствительный>",
	"Display detailed information in a smaller font.",//
	"Ориентация Дисплея <A. Автоматический L. Левая Рука R. Правая Рука>",
	"Активация кнопки A для Турбо режима до 450С при пайке ", "Установка температуры для Турбо режима",
	"Изменяет стрелки на дисплей питания при пайке",
	"Автоматический запуск паяльника при включении питания. T=Нагрев, S=Режим Сна,F=Выкл.",
	"Мигает температура на экране охлаждения, пока жало остается горячим.", "Calibrate tip offset.",//s
	"Reset all settings",};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";

const char* UVLOWarningString = "Low Volt";    //Fixed width 8 chars
const char* CoolingPromptString = "Выкл. ";//Fixed width 5 chars
const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingSleepChar = 'S';
const char SettingFastChar = 'F';
const char SettingMediumChar = 'M';
const char SettingSlowChar = 'S';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingTempCChar = 'C';
const char SettingTempFChar = 'F';

#endif
const char* SettingsShortNames[14] = { /**/
"PWRSC ",    // Power Source (DC or batt)
		"STMP ", 	// Sleep Temperature
		"STME ", 	// Sleep Timeout
		"SHTME ",    // Shutdown Temperature
		"MSENSE ",    // Motion sensitivity level
		"TMPUNT ",			//Temp in F and C
		"ADVDSP ",    // Advanced display mode enable
		"DSPROT ",    // Display rotation mode
		"BOOST  ",    // Boost enabled
		"BTMP ", 	// Boost temperature
		"ASTART ",    // Automatic Start mode
		"CLBLNK ",    // Cooldown blink
		"TMP CAL?",    // Temperature calibration enter menu
		"RESET? "    // Settings reset command
		};
