/*
 * Translation.c
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Translation.h"

#ifdef LANG_EN
const char* SettingsLongNames[16] = {
/*These are all the help text for all the settings.*/
/*No requirements on spacing or length*/
"Power source. Sets cutoff voltage. <DC 10V> <S 3.3V per cell>",    //Power Source
		"Sleep Temperature <C>",    //Sleep Temp
		"Sleep Timeout <Minutes/Seconds>",    //Sleep Timeout
		"Shutdown Timeout <Minutes>",    //Shutdown Time
		"Motion Sensitivity <0.Off 1.least sensitive 9.most sensitive>",    //Motion Sensitivity
		"Temperature Unit <C=Celsius F=Fahrenheit>",    //Temp Unit
		"Display detailed information in a smaller font on the idle screen.",    //Detailed Information
		"Display Orientation <A. Automatic L. Left Handed R. Right Handed>",    //Orientation
		"Enable front key enters boost mode 450C mode when soldering",    //Boost enable
		"Temperature when in \"boost\" mode",    //Boost Temp
		"Automatically starts the iron into soldering on power up. T=Soldering, S= Sleep mode,F=Off",    //Auto start
		"Blink the temperature on the cooling screen while the tip is still hot.",    //Cooling Blink
		"Calibrate tip offset.",    //Calibrate Tip
		"Reset all settings",    //Reset Settings
		"VIN Calibration. Buttons adjust, long press to exit",    //VIN Cal
		"Display detailed information while soldering",    //ADV SLD
		};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
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
#endif

#ifdef LANG_RU
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Источник питания. Установка напряжения отключения. <DC 10V> <S 3.3 V на батарею>",    //Cell count
	"Температура режима ожидания <С>",//Sleep Temp
	"Время до перехода в режим ожидания <Минуты>",//Sleep timeout
	"Время до отключения <Минуты>",//Shutdown timeout
	"Акселерометр <0. Выкл. 1. мин. чувствительный 9. макс. чувствительный>",//Sensitivity
	"В чем измерять температуру",//Temp Unit
	"Показывать детальную информацию маленьким шрифтом на домашнем экране",//Detailed Information
	"Ориентация дисплея <A. Автоматический, Л. Левая рука, П. Правая рука>",//Orientation
	"Турбо-режим при удержании кнопки А при пайке ",//Boost enable
	"Температура в турбо-режиме",//Boost temp
	"Автоматический запуск паяльника при включении питания. T=Нагрев, S=Режим ожидания,F=Выкл.",//Auto start
	"Показывать температуру на экране охлаждения, пока жало остается горячим."//Cooling blink
	"Калибровка термодатчика.",//Calibrate Tip
	"Сброс всех настроек.",//Reset Settings
	"Калибровка напряжения входа. Настройка кнопками, нажать и удержать чтобы завершить.",//VIN Cal
	"Показывать детальную информацию при пайке.",//ADV SLD
};

const char* SettingsCalibrationWarning = "Убедитесь, что жало остыло до комнатной температуры, прежде чем продолжать!";
const char* UVLOWarningString = "БАТ РАЗР";    //Fixed width 8 chars
const char* SleepingSimpleString = "Хррр";// Must be <= 4 chars
const char* SleepingAdvancedString = "Ожидание...";// <=17 chars	  
const char* WarningSimpleString = " АЙ!";//Must be <= 4 chars
const char* WarningAdvancedString = "ОСТОРОЖНО! ГОРЯЧО";
							 
const char SettingTrueChar = '+';
const char SettingFalseChar = '-';

const char SettingRightChar = 'П';
const char SettingLeftChar = 'Л';
const char SettingAutoChar = 'A';

const char* SettingsShortNames[16] = { /**/
		"ИстП  ",    // Power Source (DC or batt)
		"Тожд ", 	// Sleep Temperature
		"Вожд ", 	// Sleep Timeout
		"Тоткл ",    // Shutdown Temperature
		"ЧувсДв ",    // Motion sensitivity level
		"ЕдТемп ",			//Temp in F and C
		"ИнфОжд ",    // Advanced idle display mode enable
		"ПовЭкр ",    // Display rotation mode
		"Турбо  ",    // Boost enabled
		"Ттур ", 	// Boost temperature
		"Астарт ",    // Automatic Start mode
		"Охлажд ",    // Cooldown blink
		"КалибрТ?",    // Temperature calibration enter menu
		"СБРОС? ",    // Settings reset command
		"КалибрU?", 
		"ИнфПай ",    //advanced soldering screens
		};

#else

const char* SettingsShortNames[16] = { /**/
		"PWRSC ",    // Power Source (DC or batt)
		"STMP ", 	// Sleep Temperature
		"STME ", 	// Sleep Timeout
		"SHTME ",    // Shutdown Temperature
		"MSENSE ",    // Motion sensitivity level
		"TMPUNT ",			//Temp in F and C
		"ADVIDL ",    // Advanced idle display mode enable
		"DSPROT ",    // Display rotation mode
		"BOOST  ",    // Boost enabled
		"BTMP ", 	// Boost temperature
		"ASTART ",    // Automatic Start mode
		"CLBLNK ",    // Cooldown blink
		"TMP CAL?",    // Temperature calibration enter menu
		"RESET? ",    // Settings reset command
		"CAL VIN?", "ADVSLD ",    //advanced soldering screens
		};

#endif

#endif

#ifdef LANG_ES
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Fuente de energía. Ajusta el límite inferior de voltaje. <DC=10V S=3.3V por celda>",
	"Temperatura en reposo. <C>",
	"Tiempo hasta activar reposo. <Minutos>",
	"Tiempo hasta apagado. <Minutos>",
	"Sensibilidad del movimiento. <0=Apagado 1=El menos sensible 9=El más sensible>",
	"Unidad de temperatura.",
	"Display detailed information in a smaller font on the idle screen.",    //Detailed Information
	"Orientación de la pantalla <A=Automático I=Mano izquierda D=Mano derecha>",
	"Activar el botón \"Boost\" en modo soldadura.",
	"Temperatura en modo \"Boost\". <C>",
	"Iniciar modo soldadura en el encendido. <V=Sí S=Modo reposo F=No>",
	"Parpadea la temperatura en el enfriamiento si la punta sigue caliente."
	"Calibrate tip offset.",//Calibrate Tip
	"Reset all settings",//Reset Settings
	"VIN Calibration. Buttons adjust, long press to exit",//VIN Cal
	"Display detailed information while soldering",//ADV SLD
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
#endif
#ifdef LANG_SE
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Источник питания. Установка напряжения отключения. <DC 10V> <S 3.3 V на батарею>",    //Cell count
	"Температура Сна  <С>",//Sleep Temp
	"Переход в режим Сна <Минуты>",//Sleep timeout
	"Переходит в режим ожидания <Минуты>",//Shutdown timeout
	"Акселерометр <0. Выкл. 1. мин. чувствительный 9. макс. чувствительный>",//Sensitivity
	"В чем измерять температуру",//Temp Unit
	"Display detailed information in a smaller font on the idle screen.",//Detailed Information
	"Ориентация Дисплея <A. Автоматический L. Левая Рука R. Правая Рука>",//Orientation
	"Активация  кнопки A для Турбо режима до  450С при пайке ",//Boost enable
	"Установка температуры для Турбо режима",//Boost temp
	"Автоматический запуск паяльника при включении питания. T=Нагрев, S= Режим Сна,F=Выкл.",//Auto start
	"Мигает температура на экране охлаждения, пока жало остается горячим."//Cooling blink
	"Calibrate tip offset.",//Calibrate Tip
	"Reset all settings",//Reset Settings
	"VIN Calibration. Buttons adjust, long press to exit",//VIN Cal
	"Display detailed information while soldering",//ADV SLD
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


#ifdef LANG_IT
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Sorgente di alimentazione. Imposta il limite inferiore di tensione. <DC=10V S=3.3V per cella>",
	"Temperatura modalità riposo <C>",
	"Timeout per passaggio a modalità riposo <Minuti>",
	"Timeout spegnimento <Minuti>",
	"Sensibilità al movimento <0=Spento 1=Sensibilità minima 9=Sensibilità massima>",
	"Unità di temperatura",
	"Display detailed information in a smaller font on the idle screen.",    //Detailed Information
	"Orientamento del display <A=Automatico S=Sinistrorso D=Destrorso>",
	"Il tasto anteriore abilita modalità \"boost\" fino a 450C durante la saldatura",
	"Temperatura in modalità \"boost\" <C>",
	"Avvia automaticamente il saldatore quando viene alimentato. <S=Modalità saldatura R=Modalità riposo N=Spento>",
	"Durante lo spegnimento la temperatura lampeggia sul display finché la punta è calda."
	"Calibrate tip offset.",//Calibrate Tip
	"Reset all settings",//Reset Settings
	"VIN Calibration. Buttons adjust, long press to exit",//VIN Cal
	"Display detailed information while soldering",//ADV SLD
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
#endif
#ifdef LANG_FR
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Type d\'alimentation. Regle la tension de coupure. <DC=10V S=3.3V par cellules>",
	"Temperature en veille. <C>",
	"Temps avant mise en veille. <Minutes>",
	"Temps avant extinction. <Minutes>",
	"Sensibilitee du capteur de mouvement. <0=Inactif 1=Peu sensible 9=Tres sensible>",
	"Unitee de temperature.",
	"Afficher des informations detaillees en petit lors de la veille",    //Detailed Information
	"Orientation de l\'affichage. <A=Automatique G=Gaucher D=Droitier>",
	"Active le mode \"Boost\" 450C sur le bouton de devant pendant la soudure.",
	"Temperature du mode \"Boost\". <C>",
	"Demarre automatiquement la soudure a l\'allumage. <A=Active, V=Mode Veille, D=Desactive>",
	"Fait clignotter la temperature lors du refroidissement pendant que la panne est chaude."
	"Compenser l\'erreur de la panne",//Calibrate Tip
	"Reinitialiser tout les reglages",//Reset Settings
	"Calibration VIN. Boutons pour ajuster, appui long pour quitter",//VIN Cal
	"Afficher des informations detaillees pendant la soudure",//ADV SLD
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
#endif
#ifdef LANG_DE
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Spannungsquelle (Abschaltspannung) <DC=10V, nS=n*3.3V für n LiIon-Zellen>",
	"Ruhetemperatur (In der eingestellten Einheit)",
	"Ruhemodus nach <Sekunden/Minuten>",
	"Abschaltzeit <Minuten>",
	"Bewegungsempfindlichkeit <0=Aus, 1=Minimal ... 9=Maximal>",
	"Temperatureinheit <C=Celsius, F=Fahrenheit>",
	"Detaillierte Anzeige im Ruhemodus <T=An, F=Aus>",    //Detailed Information
	"Ausrichtung der Anzeige <A=Auto, L=Linkshändig, R=Rechtshändig>",
	"Vordere Taste für Temperaturboost verwenden <T=An, F=Aus>",
	"Temperatur im Boostmodus  (In der eingestellten Einheit)",
	"Automatischer Start des Lötmodus beim Einschalten der Spannungsversorgung. <T=An, F=Aus>",
	"Blinkende Temperaturanzeige beim Abkühlen, solange heiß. <T=An, F=Aus>"
	"Kalibrierung der Lötspitzentemperatur",//Calibrate Tip
	"Alle Einstellungen zurücksetzen",//Reset Settings
	"Kalibrierung der Eingangsspannung. Kurzer Tastendruck zum Einstellen, langer Tastendruck zum Verlassen.",//VIN Cal
	"Detaillierte Anzeige im Lötmodus <T=An, F=Aus>",//ADV SLD
};

const char* SettingsCalibrationWarning = "Vor dem Fortfahren muss die Lötspitze vollständig abgekühlt sein!";
const char* UVLOWarningString = "LOW VOLT";//Fixed width 8 chars
const char* SleepingSimpleString = "Zzz ";// Must be <= 4 chars
const char* SleepingAdvancedString = "Ruhemodus...";// <=17 chars
const char* WarningSimpleString = "HEIß";//Must be <= 4 chars
const char* WarningAdvancedString = "Achtung! Spitze Heiß!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
#endif
//Currently the settings names are not translated

