/*
 * Translation.cpp
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Translation.h"
#ifndef LANG
#define LANG_EN
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
	/* Advanced soldering screen enabled  */"Display detailed information while soldering",
	/* Description Scroll Speed           */"Speed this text scrolls past at",

#ifdef PIDSETTINGS

	"PID P term. Inverse values! This acts as a divisor. So Larger numbers == typically smaller in other systems",
	"PID I term. Inverse values! This acts as a divisor. So Larger numbers == typically smaller in other systems",
	"PID D term. Inverse values! This acts as a divisor. So Larger numbers == typically smaller in other systems",

#endif

};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* SettingsResetWarning = "Are you sure you want to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";               // <=8 chars
const char* UndervoltageString = "Undervoltage";// <=16 chars
const char* InputVoltageString = "Input V: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";// <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";// <=8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";// <=16 chars
const char* WarningSimpleString = "HOT!";// Must be <= 4 chars
const char* WarningAdvancedString = "!!! TIP HOT !!!";// <=16 chars
const char* SleepingTipAdvancedString = "Tip:";// <=6 chars
const char* IdleTipString = "Tip:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";// <=12 chars
const char* OffString ="Off";// 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 11) Power source (DC or batt)          */{"Power", "source"},
	/* (<=  9) Sleep temperature                  */{"Sleep", "temp"},
	/* (<=  9) Sleep timeout                      */{"Sleep", "timeout"},
	/* (<= 10) Shutdown timeout                   */{"Shutdown", "timeout"},
	/* (<= 13) Motion sensitivity level           */{"Motion", "sensitivity"},
	/* (<= 13) Temperature in F and C             */{"Temperature", "units"},
	/* (<= 13) Advanced idle display mode enabled */{"Detailed", "idle screen"},
	/* (<= 13) Display rotation mode              */{"Display", "orientation"},
	/* (<= 13) Boost enabled                      */{"Boost mode", "enabled"},
	/* (<=  9) Boost temperature                  */{"Boost", "temp"},
	/* (<= 13) Automatic start mode               */{"Auto", "start"},
	/* (<= 13) Cooldown blink                     */{"Cooldown", "blink"},
	/* (<= 16) Temperature calibration enter menu */{"Calibrate", "temperature?"},
	/* (<= 16) Settings reset command             */{"Factory", "reset?"},
	/* (<= 16) Calibrate input voltage            */{"Calibrate", "input voltage?"},
	/* (<= 13) Advanced soldering screen enabled  */{"Detailed", "solder screen"},
	/* (<= 11) Display Help Text Scroll Speed     */{"Description","Scroll Speed"},
#ifdef PIDSETTINGS

	{	"PID","P"},
	{	"PID","I"},
	{	"PID","D"},
#endif

};

// SettingsMenuEntries lengths <= 13 per line (\n starts second line)
const char* SettingsMenuEntries[4] = {
	/* Soldering Menu    */"Soldering\nSettings",
	/* Power Saving Menu */"Sleep\nModes",
	/* UI Menu           */"User\nInterface",
	/* Advanced Menu     */"Advanced\nOptions",
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
	/* Power source (DC or batt)          */"Източник на захранване. Минимално напрежение. <DC 10V> <S 3.3V за клетка>",
	/* Sleep temperature                  */"Температура при режим \"сън\" <C>",
	/* Sleep timeout                      */"Включване в режим \"сън\" след: <Минути/Секунди>",
	/* Shutdown timeout                   */"Изключване след <Минути>",
	/* Motion sensitivity level           */"Усещане за движение <0.Изключено 1.Слабо 9.Силно>",
	/* Temperature in F and C             */"Единици за температура <C=Целзии F=Фаренхайт>",
	/* Advanced idle display mode enabled */"Покажи детайлна информация със ситен шрифт на екрана в режим на покой.",
	/* Display rotation mode              */"Ориентация на дисплея <A. Автоматично L. Лява Ръка R. Дясна Ръка>",
	/* Boost enabled                      */"Ползвай предния бутон за \"турбо\" режим с температура до 450C при запояване",
	/* Boost temperature                  */"Температура за \"турбо\" режим",
	/* Automatic start mode               */"Режим на поялника при включване на захранването. T=Работен, S=Сън, F=Изключен",
	/* Cooldown blink                     */"След изключване от работен режим, индикатора за температура да мига докато човката на поялника все още е топла",
	/* Temperature calibration enter menu */"Калибриране на температурата",
	/* Settings reset command             */"Връщане на фабрични настройки",
	/* Calibrate input voltage            */"Калибриране на входното напрежение (VIN). Задръжте бутонa за изход",
	/* Advanced soldering screen enabled  */"Детайлна информация в работен режим при запояване",
	/* Description Scroll Speed           */"Скорост на движение на този текст",
};

const char* SettingsCalibrationWarning = "Уверете се, че човката на поялника е със стайна температура преди да продължите!";
const char* SettingsResetWarning = "Сигурни ли сте, че искате да върнете фабричните настройки?";
const char* UVLOWarningString = "Ниско V!";                // <=8 chars
const char* UndervoltageString = "Ниско Напрежение";// <=16 chars
const char* InputVoltageString = "Входно V: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Темп.: ";// <=12 chars, preferably end with a space
const char* BadTipString = "ЛОШ ВРЪХ";// <=8 chars
const char* SleepingSimpleString = "Сън";// Must be <= 4 chars
const char* SleepingAdvancedString = "Хър Хър Хър...";// <=16 chars
const char* WarningSimpleString = "ОХ!";// Must be <= 4 chars
const char* WarningAdvancedString = "ВНИМАНИЕ! ТОПЛО!";// <=16 chars
const char* SleepingTipAdvancedString = "Връх:";// <=6 chars
const char* IdleTipString = "Връх:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "ВРЪХ ЛОША ВРЪЗКА";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Захранване: ";// <=12 chars
const char* OffString ="Off";// 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 11) Power source (DC or batt)          */{"Източник", "захранване"},
	/* (<=  9) Sleep temperature                  */{"Темп.", "сън"},
	/* (<=  9) Sleep timeout                      */{"Време", "сън"},
	/* (<= 10) Shutdown timeout                   */{"Време", "изкл."},
	/* (<= 13) Motion sensitivity level           */{"Усещане", "за движение"},
	/* (<= 13) Temperature in F and C             */{"Единици за", "температура"},
	/* (<= 13) Advanced idle display mode enabled */{"Детайлен", "екран в покой"},
	/* (<= 13) Display rotation mode              */{"Ориентация", "на дисплея"},
	/* (<= 13) Boost enabled                      */{"Турбо режим", "пуснат"},
	/* (<=  9) Boost temperature                  */{"Турбо", "темп."},
	/* (<= 13) Automatic start mode               */{"Автоматичен", "работен режим"},
	/* (<= 13) Cooldown blink                     */{"Мигай при", "топъл поялник"},
	/* (<= 16) Temperature calibration enter menu */{"Калибриране", "температура?"},
	/* (<= 16) Settings reset command             */{"Фабрични", "настройки?"},
	/* (<= 16) Calibrate input voltage            */{"Калибриране", "напрежение?"},
	/* (<= 13) Advanced soldering screen enabled  */{"Детайлен", "работен екран"},
	/* (<= 11) Display Help Text Scroll Speed     */{"Скорост","на текста"},
};

const char* SettingsMenuEntries[4] = {
	/* Soldering Menu */"Поялник\nНастройки",
	/* Power Saving Menu */"Режими\nНастройки",
	/* UI Menu */"Интерфейс\nНастройки",
	/* Advanced Menu */"Допълнителни\nНастройки"
};
const char* SettingsMenuEntriesDescriptions[4] = {
	"Настройки на поялника",
	"Настройки енергоспестяване",
	"Настройки на интерфейса",
	"Допълнителни настройки"
};
#endif

#ifdef LANG_RU
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Установка минимального напряжения. <DC - 10V, 3S - 9.9V, 4S - 13.2V, 5S - 16.5V, 6S - 19.8V>",
	/* Sleep temperature                  */"Температура режима ожидания <C°/F°>.",
	/* Sleep timeout                      */"Время до перехода в режим ожидания <Доступно отключение>.",
	/* Shutdown timeout                   */"Время до отключения <Доступно отключение>.",
	/* Motion sensitivity level           */"Акселерометр <0 - Выкл., 1 - Мин. чувствительность, 9 - Макс. чувствительность>.",
	/* Temperature in F and C             */"Единица измерения температуры. < C - Цельсий, F - Фаренгейт >",
	/* Advanced idle display mode enabled */"Показывать детальную в режиме настроек <Вместо картинки>.",
	/* Display rotation mode              */"Ориентация дисплея. <A - Автоповорот, L - Левша, R - Правша>",
	/* Boost enabled                      */"Турбо-режим при удержании кнопки А в режиме пайки.",
	/* Boost temperature                  */"Температура в Турбо-режиме.",
	/* Automatic start mode               */"Автоматический переход в режим пайки при включении питания.",
	/* Cooldown blink                     */"Показывать изменение температуры в процессе охлаждения, мигая экраном.",
	/* Temperature calibration enter menu */"Калибровка температурного датчика.",
	/* Settings reset command             */"Сброс всех настроек к исходным значения.",
	/* Calibrate input voltage            */"Калибровка напряжения входа. Настройка кнопками, нажать и удержать чтобы завершить.",
	/* Advanced soldering screen enabled  */"Показывать детальную информацию при пайке.",
	/* Description Scroll Speed           */"Скорость прокрутки текста.",
};

const char* SettingsCalibrationWarning = "Убедитесь, что жало остыло до комнатной температуры, прежде чем продолжать!";
const char* SettingsResetWarning = "Вы действительно хотите сбросить настройки до значений по умолчанию?";
const char* UVLOWarningString = "АККУМ--";               // <=8 chars
const char* UndervoltageString = "Под питанием";// <=16 chars
const char* InputVoltageString = "Питание(B):";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Жало t°: ";// <=12 chars, preferably end with a space
const char* BadTipString = "Жало--";// <=8 chars
const char* SleepingSimpleString = "Сон ";// Must be <= 4 chars
const char* SleepingAdvancedString = "Ожидание...";// <=16 chars
const char* WarningSimpleString = " АЙ!";// Must be <= 4 chars
const char* WarningAdvancedString = "ВНИМАНИЕ ГОРЯЧО!";// <=16 chars
const char* SleepingTipAdvancedString = "Жало:";// <=6 chars
const char* IdleTipString = "Жало:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " ->";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "Жало отключено!";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Питание: ";// <=12 chars
const char* OffString ="Off";// 3 chars max

/*
 * #TODO change support for multibyte constants here
 const char SettingRightChar = 'П';
 const char SettingLeftChar = 'Л';
 const char SettingAutoChar = 'A';*/

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = '+';
const char SettingSlowChar = '-';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 11) Power source (DC or batt)          */{"Источник","питания"},	//8,7
	/* (<=  9) Sleep temperature                  */{"Темпер.","сна"},	//7,3
	/* (<=  9) Sleep timeout                      */{"Таймаут","сна"},	//7,3
	/* (<= 10) Shutdown timeout                   */{"Время до","отключения"},//8,10
	/* (<= 13) Motion sensitivity level           */{"Чувст. сенсо-","ра движения"},//13,11
	/* (<= 13) Temperature in F and C             */{"Формат темпе-","ратуры(C°/F°)"},//13,13
	/* (<= 13) Advanced idle display mode enabled */{"Подробный ре-","жим ожидания"},//13,12
	/* (<= 13) Display rotation mode              */{"Автоповорот","экрана"},//11,6
	/* (<= 13) Boost enabled                      */{"Режим","Турбо"},	//5,5
	/* (<=  9) Boost temperature                  */{"Темпер.","Турбо"},   //7,5
	/* (<= 13) Automatic start mode               */{"Горячий","старт"},   //9,9
	/* (<= 13) Cooldown blink                     */{"Показ t° при","остывании"}, //7,3
	/* (<= 16) Temperature calibration enter menu */{"Калибровка","температуры"}, //10,11
	/* (<= 16) Settings reset command             */{"Сбросить все","настройки?"}, //12,10
	/* (<= 16) Calibrate input voltage            */{"Калибровка","напряжения"}, //10,10
	/* (<= 13) Advanced soldering screen enabled  */{"Подробный ре-","жим пайки"}, //13,9
	/* (<= 11) Message Scroll Speed               */{"Скорость","текста"}, //8,6
};

const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Пайка",
	/* Power Saving Menu*/"Сон",
	/* UI Menu*/"Интерфейс",
	/* Advanced Menu*/"Другие",};
const char* SettingsMenuEntriesDescriptions[4] = {
	"Настройки для режима пайки. Действуют при включенном жале.",
	"Настройки при бездействии. Полезно чтобы не обжечься и случайно не сжечь жилище.",
	"Пользовательский интерфейс.",
	"Расширенные настройки. Дополнительные удобства."
};
#endif

#ifdef LANG_ES
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Fuente de energía. Ajusta el límite inferior de voltaje. <DC=10V S=3.3V por celda>",
	/* Sleep temperature                  */"Temperatura en reposo. <C>",
	/* Sleep timeout                      */"Tiempo hasta activar reposo. <Minutos>",
	/* Shutdown timeout                   */"Tiempo hasta apagado. <Minutos>",
	/* Motion sensitivity level           */"Sensibilidad del movimiento. <0=Apagado 1=El menos sensible 9=El más sensible>",
	/* Temperature in F and C             */"Unidad de temperatura.",
	/* Advanced idle display mode enabled */"Mostrar información detallada con fuente de menor tamaño en la pantalla de reposo.",
	/* Display rotation mode              */"Orientación de la pantalla <A=Automático I=Mano izquierda D=Mano derecha>",
	/* Boost enabled                      */"Activar el botón \"Boost\" en modo soldadura.",
	/* Boost temperature                  */"Temperatura en modo \"Boost\". <C>",
	/* Automatic start mode               */"Iniciar modo soldadura en el encendido. <V=Sí S=Modo reposo F=No>",
	/* Cooldown blink                     */"Parpadea la temperatura en el enfriamiento si la punta sigue caliente.",
	/* Temperature calibration enter menu */"Calibrar desviación térmica de la punta.",
	/* Settings reset command             */"Volver a estado de fábrica.",
	/* Calibrate input voltage            */"Calibración VIN (Voltaje de entrada). Ajuste con ambos botones, pulsación larga para salir.",
	/* Advanced soldering screen enabled  */"Mostrar información detallada mientras suelda.",
	/* Description Scroll Speed           */"Velocidad de desplazamiento del texto.",
};

const char* SettingsCalibrationWarning = "Por favor, ¡asegúrese que la punta esté a temperatura ambiente antes de proceder!";
const char* SettingsResetWarning = "¿Está seguro de volver a estado de fábrica?";
const char* UVLOWarningString = "DC LOW";                // <=8 chars
const char* UndervoltageString = "Undervoltage";// <=16 chars
const char* InputVoltageString = "Input V: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";// <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";// <=8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";// <=16 chars
const char* WarningSimpleString = "HOT!";// Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";// <=16 chars
const char* SleepingTipAdvancedString = "Tip:";// <=6 chars
const char* IdleTipString = "Tip:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";// <=12 chars
const char* OffString ="Off";// 3 chars max

const char SettingRightChar = 'D';// D is "Derecha" = Right
const char SettingLeftChar = 'I';// I is "Izquierda" = Left
const char SettingAutoChar = 'A';

const char SettingFastChar = 'R';// R is "Rápido" = Fast
const char SettingSlowChar = 'L';// L is "Lento" = Slow

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
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
	/* (<= 6) Message Scroll Speed               */{"DESCSP"},
};
const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Opciones de\nsoldadura",
	/* Power Saving Menu*/"Modos de\nreposo",
	/* UI Menu*/"Interfaz de\nusuario",
	/* Advanced Menu*/"Opciones\navanzadas",};
const char* SettingsMenuEntriesDescriptions[4] = {
	"Opciones de soldadura",
	"Opciones de ahorro energético",
	"Opciones de interfaz de usuario",
	"Opciones avanzadas"
};
#endif

#ifdef LANG_FI

const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Käytettävä virtalähde. Asettaa katkaisujänniteen. <DC 10V, 3S=9.9V, 4S=13.2V, 5S=16.5V, 6S=19.8V>",
	/* Sleep temperature                  */"Lepotilan lämpötila. <C>",//"Lepolämpötila <C>",
	/* Sleep timeout                      */"Lepotilan viive. <minuuttia/sekuntia>",//"Lepoviive <minuuttia/sekuntia>",
	/* Shutdown timeout                   */"Automaattisen sammutuksen aikaviive. <minuuttia>",
	/* Motion sensitivity level           */"Liikkeentunnistuksen herkkyys. <0=pois, 1=epäherkin, 9=herkin>",//"Liikeherkkyys <0=pois, 1=epäherkin, 9=herkin>",
	/* Temperature in F and C             */"Lämpötilan yksikkö. <C=celsius, F=fahrenheit>",
	/* Advanced idle display mode enabled */"Näyttää yksityiskohtaisemmat tiedot lepotilassa.",//"Näyttää tarkemmat tiedot leponäytössä",
	/* Display rotation mode              */"Näytön kierto. <A=automaattinen O=oikeakätinen V=vasenkätinen>",//"Näytön suunta <A=automaattinen O=oikeakätinen V=vasenkätinen>",
	/* Boost enabled                      */"Etupainikeella siirrytään juotettaessa tehostustilaan.",
	/* Boost temperature                  */"Tehostustilan lämpötila.",//"Tehostuksen lämpötila kun käytetään tehostustilaa",
	/* Automatic start mode               */"Käynnistää virrat kytkettäessä juotostilan automaattisesti. T=juotostila, S=Lepotila, F=Ei käytössä",//"Lämmitys käynnistyy automaattisesti kun virrat kytketään. T=juottamiseen, S=lepotilaan, F=ei käytössä",
	/* Cooldown blink                     */"Vilkuttaa jäähtyessä juotoskärjen lämpötilaa sen ollessa vielä vaarallisen kuuma.",//"Vilkutetaan näytössä lämpötilaa kun juotoskärki jäähtyy, mutta on yhä kuuma",
	/* Temperature calibration enter menu */"Kalibroi kärjen lämpötilaeron.",//"Kalibroidaan juotoskärjen lämpötilaero",
	/* Settings reset command             */"Palauta kaikki asetukset oletusarvoihin.",//"Palautetaan kaikki asetukset alkuperäisiksi",
	/* Calibrate input voltage            */"Tulojännitten kalibrointi (VIN). Painikkeilla säädetään ja pitkään painamalla poistutaan.",
	/* Advanced soldering screen enabled  */"Näyttää yksityiskohtaisemmat tiedot juotostilassa.",//"Näytetään tarkemmat tiedot juottamisen aikana",
	/* Description Scroll Speed           */"Näiden selitetekstien vieritysnopeus." ,//"Tekstin vieritysnopeus näytöllä",
};

const char* SettingsCalibrationWarning = "Varmista että kärki on huoneenlämpöinen ennen jatkamista!"; //"Älä jatka ennen kuin kärki on jäähtynyt!";
const char* SettingsResetWarning = "Haluatko varmasti palauttaa oletusarvot?";//"Haluatko palauttaa oletusarvot?";
const char* UVLOWarningString = "DC LOW";// <=8 chars
const char* UndervoltageString = "Alijännite";// <=16 chars
const char* InputVoltageString = "Jännite: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Lämpötila: ";// <=12 chars, preferably end with a space
const char* BadTipString = "VIKATILA";// <=8 chars
const char* SleepingSimpleString = "Zzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Lepotila...";// <=16 chars
const char* WarningSimpleString = "HOT";// Must be <= 4 chars
const char* WarningAdvancedString = " ! KÄRKI KUUMA !";//"KUUMA KÄRKI";  // <=16 chars
const char* SleepingTipAdvancedString = "Kärki:";// <=6 chars
const char* IdleTipString = "Kärki:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Asetus:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "KÄRKI ON IRTI";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Teho: ";// <=12 chars
const char* OffString ="OFF";// 3 chars max

const char SettingRightChar = 'O';
const char SettingLeftChar = 'V';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'N';//'F';
const char SettingSlowChar = 'H';//'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 11) Power source (DC or batt)          */{"Virtalähde", "DC"},
	/* (<=  9) Sleep temperature                  */{"Lepotilan", "lämpötila"},
	/* (<=  9) Sleep timeout                      */{"Lepotilan", "viive"},
	/* (<= 10) Shutdown timeout                   */{"Sammutus", "viive"},
	/* (<= 13) Motion sensitivity level           */{"Liikkeen", "herkkyys"},
	/* (<= 13) Temperature in F and C             */{"Lämpötilan", "yksikkö"},
	/* (<= 13) Advanced idle display mode enabled */{"Tiedot", "lepotilassa"},
	/* (<= 13) Display rotation mode              */{"Näytön", "kierto"},
	/* (<= 13) Boost enabled                      */{"Tehostus", "käytössä"},
	/* (<=  9) Boost temperature                  */{"Tehostus-", "lämpötila"},
	/* (<= 13) Automatic start mode               */{"Autom.", "käynnistys"},

	/* (<= 13) Cooldown blink                     */{"Jäähdytyksen", "vilkutus"},
	/* (<= 16) Temperature calibration enter menu */{"Kalibroi", "lämpötila?"},
	/* (<= 16) Settings reset command             */{"Palauta", "tehdasasetukset?"},
	/* (<= 16) Calibrate input voltage            */{"Kalibroi", "tulojännite?"},
	/* (<= 13) Advanced soldering screen enabled  */{"Tarkempi", "juotosnäyttö"},
	/* (<= 11) Display Help Text Scroll Speed     */{"Tietojen","näyttönopeus"},
};

// SettingsMenuEntries lengths <= 13 per line (\n starts second line)
const char* SettingsMenuEntries[4] = {
	/* Soldering Menu    */"Juotos-\nasetukset" ,   //"Juotos\nasetukset",
	/* Power Saving Menu */"Lepotilan\nasetukset",// "Lepo\ntila",
	/* UI Menu           */"Käyttö-\nliittymä",//"Käyttö\nliittymä",
	/* Advanced Menu     */"Lisä-\nasetukset",// "Lisä\nasetukset",
};

const char* SettingsMenuEntriesDescriptions[4] = {
	"Juotosasetukset",
	"Virransäästöasetukset",
	"Käyttöliittymän asetukset",
	"Lisäasetukset"
};
#endif

#ifdef LANG_IT
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Scegli la sorgente di alimentazione; se a batteria, limita lo scaricamento al valore di soglia <DC: 10V; S: 3.3V per cella>",
	/* Sleep temperature                  */"Imposta la temperatura da mantenere in modalità Standby <°C/°F>",
	/* Sleep timeout                      */"Imposta il timer per entrare in modalità Standby <minuti/secondi>",
	/* Shutdown timeout                   */"Imposta il timer per lo spegnimento <minuti>",
	/* Motion sensitivity level           */"Imposta la sensibilità al movimento per uscire dalla modalità Standby <0: nessuna; 1: minima; 9: massima>",
	/* Temperature in F and C             */"Scegli l'unità di misura per la temperatura <C: grado Celsius; F: grado Farenheit>",
	/* Advanced idle display mode enabled */"Mostra informazioni dettagliate con un carattere più piccolo all'interno della schermata principale",
	/* Display rotation mode              */"Imposta l'orientamento del display <A: automatico; S: mano sinistra; D: mano destra>",
	/* Boost enabled                      */"Il tasto superiore attiva la funzione «Turbo» durante la modalità Saldatura",
	/* Boost temperature                  */"Imposta la temperatura della funzione «Turbo»",
	/* Automatic start mode               */"Attiva automaticamente il saldatore quando viene alimentato <A: saldatura; S: standby; D: disattiva>",
	/* Cooldown blink                     */"Mostra la temperatura durante il raffreddamento se la punta è ancora calda",
	/* Temperature calibration enter menu */"Calibra le rilevazioni di temperatura",
	/* Settings reset command             */"Ripristina tutte le impostazioni",
	/* Calibrate input voltage            */"Calibra la tensione in ingresso; regola con entrambi i tasti, tieni premuto il tasto superiore per uscire",
	/* Advanced soldering screen enabled  */"Mostra informazioni dettagliate durante la modalità Saldatura",
	/* Description Scroll Speed           */"Imposta la velocità di scorrimento del testo <L: lento; V: veloce>",

};

const char* SettingsCalibrationWarning = "Assicurati che la punta si trovi a temperatura ambiente prima di continuare!";
const char* SettingsResetWarning = "Ripristinare le impostazioni iniziali?";
const char* UVLOWarningString = "DC BASSA";            // <=8 chars
const char* UndervoltageString = "DC INSUFFICIENTE";// <=16 chars
const char* InputVoltageString = "V ingresso:";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Punta:";// <=12 chars, preferably end with a space
const char* BadTipString = "PUNTA NO";// <=8 chars
const char* SleepingSimpleString = "ZZZ ";// Must be <= 4 chars
const char* SleepingAdvancedString = "Standby";// <=16 chars
const char* WarningSimpleString = "HOT!";// Must be <= 4 chars
const char* WarningAdvancedString = "PUNTA CALDA!";// <=16 chars
const char* SleepingTipAdvancedString = "Punta:";// <=6 chars
const char* IdleTipString = "Punta:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = "/";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "PUNTA ASSENTE";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Potenza:";// <=12 chars
const char* OffString ="OFF";// 3 chars max

const char SettingRightChar = 'D';
const char SettingLeftChar = 'S';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'V';
const char SettingSlowChar = 'L';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 11) Power source (DC or batt)          */{"Sorgente", "alimentaz"},
	/* (<=  9) Sleep temperature                  */{"Temp", "standby"},
	/* (<=  9) Sleep timeout                      */{"Timer", "standby"},
	/* (<= 10) Shutdown timeout                   */{"Timer di", "arresto"},
	/* (<= 13) Motion sensitivity level           */{"Sensibilità", "al movimento"},
	/* (<= 13) Temperature in F and C             */{"Unità di", "temperatura"},
	/* (<= 13) Advanced idle display mode enabled */{"Mostra", "dettagli"},
	/* (<= 13) Display rotation mode              */{"Orientamento", "display"},
	/* (<= 13) Boost enabled                      */{"Funzione", "«Turbo»"},
	/* (<=  9) Boost temperature                  */{"Temp", "«Turbo»"},
	/* (<= 13) Automatic start mode               */{"Avvio", "automatico"},
	/* (<= 13) Cooldown blink                     */{"Avviso", "punta calda"},
	/* (<= 16) Temperature calibration enter menu */{"Calibrazione", "temperatura"},
	/* (<= 16) Settings reset command             */{"Ripristino", "impostazioni"},
	/* (<= 16) Calibrate input voltage            */{"Calibrazione", "tensione"},
	/* (<= 13) Advanced soldering screen enabled  */{"Dettagli", "saldatura"},
	/* (<= 11) Display Help Text Scroll Speed     */{"Velocità", "testo"},
};
const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Opzioni\nsaldatura",
	/* Power Saving Menu*/"Risparmio\nenergetico",
	/* UI Menu*/"Interfaccia\nutente",
	/* Advanced Menu*/"Opzioni\navanzate",};
const char* SettingsMenuEntriesDescriptions[4] = {
	"Menù d'impostazioni della modalità Saldatura",
	"Menù d'impostazioni per il risparmio energetico",
	"Menù d'impostazioni dell'interfaccia utente",
	"Menù d'impostazioni avanzate"
};
#endif

#ifdef LANG_FR

const char* SettingsDescriptions[17] =
{
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Source d'alimentation. Règle la tension de coupure <DC=10V S=3.3V par cellules>",
	/* Sleep temperature                  */"Température en veille <C>",
	/* Sleep timeout                      */"Délai avant mise en veille <Minutes>",
	/* Shutdown timeout                   */"Délai avant extinction <Minutes>",
	/* Motion sensitivity level           */"Sensibilité du capteur de mouvement <0=Inactif 1=Peu sensible 9=Tres sensible>",
	/* Temperature in F and C             */"Unité de température <C=Celsius F=Fahrenheit>",
	/* Advanced idle display mode enabled */"Afficher des informations détaillées lors de la veille.",
	/* Display rotation mode              */"Orientation de l'affichage <A=Automatique G=Gaucher D=Droitier>",
	/* Boost enabled                      */"Activer le mode \"Boost\" en maintenant le bouton de devant pendant la soudure",
	/* Boost temperature                  */"Température du mode \"Boost\"",
	/* Automatic start mode               */"Démarrer automatiquement la soudure a l'allumage <A=Activé, V=Mode Veille, D=Désactivé>",
	/* Cooldown blink                     */"Faire clignoter la température lors du refroidissement tant que la panne est chaude.",
	/* Temperature calibration enter menu */"Etalonner température de la panne.",
	/* Settings reset command             */"Réinitialiser tous les réglages",
	/* Calibrate input voltage            */"Etalonner tension d'entrée. Boutons pour ajuster, appui long pour quitter",
	/* Advanced soldering screen enabled  */"Afficher des informations détaillées pendant la soudure",
	/* Description Scroll Speed           */"Vitesse de défilement de ce texte en <R=Rapide L=Lent>",};

const char* SettingsCalibrationWarning =
"Assurez-vous que la panne soit à température ambiante avant de continuer!";
const char* SettingsResetWarning =
"Voulez-vous vraiment réinitialiser les paramètres aux valeurs d'usine?";
const char* UVLOWarningString = "DC FAIBLE";              // <=8 chars
const char* UndervoltageString = "Sous-tension";// <=16 chars
const char* InputVoltageString = "V d'entrée: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Temp. Panne: ";// <=12 chars, preferably end with a space
const char* BadTipString = "PANNE HS";// <=8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "En veille...";// <=16 chars
const char* WarningSimpleString = "HOT!";// Must be <= 4 chars
const char* WarningAdvancedString = "ATTENTION! CHAUD";// Must be <= 16 chars
const char* SleepingTipAdvancedString = "PANNE:";// <=6 chars
const char* IdleTipString = "PANNE:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "PANNE DEBRANCH";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Puissance: ";// <= 12 chars
const char* OffString = "Off";// 3 chars max

const char SettingRightChar = 'D';
const char SettingLeftChar = 'G';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'R';
const char SettingSlowChar = 'L';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 11) Power source (DC or batt)          */{"Source", "d'alim"},
	/* (<=  9) Sleep temperature                  */{"Temp.", "veille"},
	/* (<=  9) Sleep timeout                      */{"Délai", "veille"},
	/* (<= 10) Shutdown timeout                   */{"Délai", "extinction"},
	/* (<= 13) Motion sensitivity level           */{"Sensibilité",
		"au mouvement"},
	/* (<= 13) Temperature in F and C             */{"Unité de",
		"température"},
	/* (<= 13) Advanced idle display mode enabled */{"Ecran veille",
		"détaillé"},
	/* (<= 13) Display rotation mode              */{"Orientation",
		"de l'écran"},
	/* (<= 13) Boost enabled                      */{"Activation du",
		"mode Boost"},
	/* (<=  9) Boost temperature                  */{"Temp.", "Boost"},
	/* (<= 13) Automatic start mode               */{"Démarrage",
		"automatique"},
	/* (<= 13) Cooldown blink                     */{"Refroidir en",
		"clignottant"},
	/* (<= 16) Temperature calibration enter menu */{"Etalonner",
		"température"},
	/* (<= 16) Settings reset command             */{"Réinitialisation",
		"d'usine"},
	/* (<= 16) Calibrate input voltage            */{"Etalonner",
		"tension d'entrée"},
	/* (<= 13) Advanced soldering screen enabled  */{"Ecran soudure",
		"détaillé"},
	/* (<= 11) Display Help Text Scroll Speed     */{"Vitesse de",
		"défilement"},};

const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Soudure\nParamètres",
	/* Power Saving Menu*/"Mode\nVeille",
	/* UI Menu*/"Interface\nUtilisateur",
	/* Advanced Menu*/"Options\nAdvancées",};
const char* SettingsMenuEntriesDescriptions[4] = {"Paramètres de soudage",
	"Paramètres d'économie d'énergie",
	"Paramètres de l'interface utilisateur", "Options avancées"};
#endif

#ifdef LANG_DE
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Spannungsquelle (Abschaltspannung) <DC=10V, nS=n*3.3V für n LiIon-Zellen>",
	/* Sleep temperature                  */"Ruhetemperatur (In der eingestellten Einheit)",
	/* Sleep timeout                      */"Ruhemodus nach <Sekunden/Minuten>",
	/* Shutdown timeout                   */"Abschalten nach <Minuten>",
	/* Motion sensitivity level           */"Bewegungsempfindlichkeit <0=Aus, 1=Minimal ... 9=Maximal>",
	/* Temperature in F and C             */"Temperatureinheit <C=Celsius, F=Fahrenheit>",
	/* Advanced idle display mode enabled */"Detaillierte Anzeige im Ruhemodus <J=An, N=Aus>",
	/* Display rotation mode              */"Ausrichtung der Anzeige <A=Automatisch, L=Linkshändig, R=Rechtshändig>",
	/* Boost enabled                      */"Vordere Taste für Temperaturboost verwenden <J=An, N=Aus>",
	/* Boost temperature                  */"Temperatur im Boostmodus  (In der eingestellten Einheit)",
	/* Automatic start mode               */"Automatischer Start des Lötmodus beim Einschalten der Spannungsversorgung. <J=An, N=Aus>",
	/* Cooldown blink                     */"Blinkende Temperaturanzeige beim Abkühlen, solange heiß. <J=An, N=Aus>",
	/* Temperature calibration enter menu */"Kalibrierung der Lötspitzentemperatur",
	/* Settings reset command             */"Alle Einstellungen zurücksetzen",
	/* Calibrate input voltage            */"Kalibrierung der Eingangsspannung. Kurzer Tastendruck zum Einstellen, langer Tastendruck zum Verlassen.",
	/* Advanced soldering screen enabled  */"Detaillierte Anzeige im Lötmodus <J=An, N=Aus>",
	/* Description Scroll Speed           */"Scrollgeschwindigkeit der Texte",
};

const char* SettingsCalibrationWarning = "Vor dem Fortfahren muss die Lötspitze vollständig abgekühlt sein!";
const char* SettingsResetWarning = "Sind Sie sicher, dass Sie alle Werte Zurücksetzen wollen?";
const char* UVLOWarningString = "V niedr.";              // <=8 chars
const char* UndervoltageString = "Unterspannung";// <=16 chars
const char* InputVoltageString = "V Eingang: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Temperatur: ";// <=12 chars, preferably end with a space
const char* BadTipString = "DEFEKT";// <=8 chars
const char* SleepingSimpleString = "Zzz ";// Must be <= 4 chars
const char* SleepingAdvancedString = "Ruhemodus...";// <=16 chars
const char* WarningSimpleString = "HEIß";// Must be <= 4 chars
const char* WarningAdvancedString = "SPITZE HEIß!";// <=16 chars
const char* SleepingTipAdvancedString = "Temp:";// <=6 chars
const char* IdleTipString = "Ist:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Soll:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "Spitze fehlt";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Leistung: ";// <= 12 chars
const char* OffString ="Off";// 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 11) Power source (DC or batt)          */{"Spannungs-", "quelle"},
	/* (<=  9) Sleep temperature                  */{"Ruhetemp-", "eratur"},
	/* (<=  9) Sleep timeout                      */{"Ruhever-", "zögerung"},
	/* (<= 10) Shutdown timeout                   */{"Abschalt-", "zeit"},
	/* (<= 13) Motion sensitivity level           */{"Bewegungs-", "empfindlichk."},
	/* (<= 13) Temperature in F and C             */{"Temperatur-", "einheit"},
	/* (<= 13) Advanced idle display mode enabled */{"Detaillierte", "Ruheansicht"},
	/* (<= 13) Display rotation mode              */{"Anzeige-", "ausrichtung"},
	/* (<= 13) Boost enabled                      */{"Boosttaste", "aktiv?"},
	/* (<=  9) Boost temperature                  */{"Boosttemp-", "eratur"},
	/* (<= 13) Automatic start mode               */{"Start im", "Lötmodus?"},
	/* (<= 13) Cooldown blink                     */{"Abkühl-", "blinken?"},
	/* (<= 16) Temperature calibration enter menu */{"Temperatur", "kalibrieren?"},
	/* (<= 16) Settings reset command             */{"Einstellungen", "zurücksetzen?"},
	/* (<= 16) Calibrate input voltage            */{"Eingangsspannung", "kalibrieren?"},
	/* (<= 13) Advanced soldering screen enabled  */{"Detaillierte", "Lötansicht"},
	/* (<= 11) Display Help Text Scroll Speed     */{"Scroll-","geschw."},
};
const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Löt-\neinstellungen",
	/* Power Saving Menu*/"Schlaf-\nmodus",
	/* UI Menu*/"Menü-\neinstellungen",
	/* Advanced Menu*/"Erweiterte\nEinstellungen",};
const char* SettingsMenuEntriesDescriptions[4] = {
	"Löteinstellungen",
	"Energiespareinstellungen",
	"Menüeinstellungen",
	"Erweiterte Einstellungen"
};
#endif

#ifdef LANG_SK
const char* SettingsDescriptions[17] = {
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
	/* Cooldown blink                     */"Blikanie ukazovatela teploty pocas chladnutia hrotu <T=Zap, F=Vyp>",
	/* Temperature calibration enter menu */"Kalibracia posunu hrotu",
	/* Settings reset command             */"Tovarenske nastavenia",
	/* Calibrate input voltage            */"Kalibracia VIN. Kratke stlacenie meni nastavenie, dlhe stlacenie pre navrat",
	/* Advanced soldering screen enabled  */"Zobrazenie detailov pocas spajkovania <T=Zap, F=Vyp>",
	/* Description Scroll Speed           */"Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Najprv sa prosim uistite, ze hrot ma izbovu teplotu!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";                // <=8 chars
const char* UndervoltageString = "Undervoltage";// <=16 chars
const char* InputVoltageString = "Input V: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";// <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";// <=8 chars
const char* SleepingSimpleString = "Chrr";// Must be <= 4 chars
const char* SleepingAdvancedString = "Kludovy rezim...";// <=16 chars
const char* WarningSimpleString = "HOT!";// Must be <= 4 chars
const char* WarningAdvancedString = "Pozor! Hrot je horuci!";// <=16 chars
const char* SleepingTipAdvancedString = "Tip:";// <=6 chars
const char* IdleTipString = "Tip:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";// <=12 chars
const char* OffString ="Off";// 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
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
	/* (<= 6) Message Scroll Speed               */{"DESCSP"},
};
const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Soldering\nSettings",
	/* Power Saving Menu*/"Sleep\nModes",
	/* UI Menu*/"User\nInterface",
	/* Advanced Menu*/"Advanced\nOptions",};
const char* SettingsMenuEntriesDescriptions[4] = {
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
	/* Description Scroll Speed           */"Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Lütfen devam etmeden önce ucun oda sıcaklığında olduğunu garantiye alın!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";               // <=8 chars
const char* UndervoltageString = "Undervoltage";// <=16 chars
const char* InputVoltageString = "Input V: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";// <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";// <=8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Uyuyor...";// <=16 chars
const char* WarningSimpleString = "HOT!";// Must be <= 4 chars
const char* WarningAdvancedString = "UYARI! UÇ SICAK!";// <=16 chars
const char* SleepingTipAdvancedString = "Tip:";// <=6 chars
const char* IdleTipString = "Tip:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";// <=12 chars
const char* OffString ="Off";// 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
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
	/* (<= 6) Message Scroll Speed               */{"DESCSP"},
};
const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Soldering\nSettings",
	/* Power Saving Menu*/"Sleep\nModes",
	/* UI Menu*/"User\nInterface",
	/* Advanced Menu*/"Advanced\nOptions",};
const char* SettingsMenuEntriesDescriptions[4] = {
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
	/* Power source (DC or batt)          */"Izvor napajanja. Postavlja napon isključivanja. <DC 10V> <S 3.3V po ćeliji>",
	/* Sleep temperature                  */"Temperatura na koju se spušta lemilica nakon određenog vremena mirovanja. <C/F>",
	/* Sleep timeout                      */"Vrijeme mirovanja nakon kojega lemilica spušta temperaturu. <Minute/Sekunde>",
	/* Shutdown timeout                   */"Vrijeme mirovanja nakon kojega će se lemilica ugasiti. <Minute>",
	/* Motion sensitivity level           */"Osjetljivost prepoznavanja pokreta. <0=Ugašeno, 1=Najmanje osjetljivo, 9=Najosjetljivije>",
	/* Temperature in F and C             */"Jedinica temperature. <C=Celzij, F=Fahrenheit>",
	/* Advanced idle display mode enabled */"Prikazivanje detaljnih informacija manjim fontom tijekom čekanja.",
	/* Display rotation mode              */"Orijentacija ekrana. <A=Automatski, L=Ljevoruki, D=Desnoruki>",
	/* Boost enabled                      */"Držanjem prednjeg gumba prilikom lemljenja aktivira se pojačani (Boost) način.",
	/* Boost temperature                  */"Temperatura u pojačanom (Boost) načinu.",
	/* Automatic start mode               */"Ako je aktivno, lemilica po uključivanju napajanja odmah počinje grijati.",
	/* Cooldown blink                     */"Bljeskanje temperature prilikom hlađenja, ako je lemilica vruća.",
	/* Temperature calibration enter menu */"Kalibriranje temperature mjeri razliku temperatura vrška i drške, dok je lemilica hladna.",
	/* Settings reset command             */"Vraćanje svih postavki na tvorničke vrijednosti.",
	/* Calibrate input voltage            */"Kalibracija ulaznog napona. Podešavanje gumbima, dugački pritisak za kraj.",
	/* Advanced soldering screen enabled  */"Prikazivanje detaljnih informacija tijekom lemljenja.",
	/* Description Scroll Speed           */"Brzina kretanja dugačkih poruka <B=brzo, S=sporo>",
};

const char* SettingsCalibrationWarning = "Provjerite da je vršak ohlađen na sobnu temperaturu prije nego što nastavite!";
const char* SettingsResetWarning = "Jeste li sigurni da želite sve postavke vratiti na tvorničke vrijednosti?";
const char* UVLOWarningString = "BATERIJA";             // <=8 chars
const char* UndervoltageString = "PRENIZAK NAPON";// <=16 chars
const char* InputVoltageString = "Napajanje: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Temp vrha: ";// <=12 chars, preferably end with a space
const char* BadTipString = "NEISPRAVAN VRH";// <=8 chars
const char* SleepingSimpleString = "Zzz ";// Must be <= 4 chars
const char* SleepingAdvancedString = "SPAVANJE...";// <=16 chars
const char* WarningSimpleString = "VRUĆ";// Must be <= 4 chars
const char* WarningAdvancedString = "OPREZ, VRUĆE!";// <=16 chars
const char* SleepingTipAdvancedString = "Vrh: ";// <=6 chars
const char* IdleTipString = "Vrh: ";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " / ";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "VRH NIJE SPOJEN!";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Snaga: ";// <=12 chars
const char* OffString ="Off";// 3 chars max

const char SettingRightChar = 'D';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'B';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 11) Power source (DC or batt)          */{"Izvor", "napajanja"},
	/* (<=  9) Sleep temperature                  */{"Temp", "spavanja"},
	/* (<=  9) Sleep timeout                      */{"Vrijeme", "spavanja"},
	/* (<= 10) Shutdown timeout                   */{"Vrijeme", "gašenja"},
	/* (<= 13) Motion sensitivity level           */{"Osjetljivost", "pokreta"},
	/* (<= 13) Temperature in F and C             */{"Jedinica", "temperature"},
	/* (<= 13) Advanced idle display mode enabled */{"Detalji", "pri čekanju"},
	/* (<= 13) Display rotation mode              */{"Rotacija", "ekrana"},
	/* (<= 13) Boost enabled                      */{"Boost", "način"},
	/* (<=  9) Boost temperature                  */{"Boost", "temp"},
	/* (<= 13) Automatic start mode               */{"Auto", "start"},
	/* (<= 13) Cooldown blink                     */{"Upozorenje", "pri hlađenju"},
	/* (<= 16) Temperature calibration enter menu */{"Kalibracija", "temperature"},
	/* (<= 16) Settings reset command             */{"Tvorničke", "postavke"},
	/* (<= 16) Calibrate input voltage            */{"Kalibracija", "napona napajanja"},
	/* (<= 13) Advanced soldering screen enabled  */{"Detalji", "pri lemljenju"},
	/* (<= 11) Display Help Text Scroll Speed     */{"Brzina","poruka"},
};
const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Postavke\nlemljenja",
	/* Power Saving Menu*/"Ušteda\nenergije",
	/* UI Menu*/"Korisničko\nsučelje",
	/* Advanced Menu*/"Napredne\nopcije",};
const char* SettingsMenuEntriesDescriptions[4] = {
	"Postavke pri lemljenju",
	"Postavke spavanja i štednje energije",
	"Postavke korisničkog sučelja",
	"Upravljanje naprednim opcijama"
};
#endif

#ifdef LANG_CS_CZ
const char* SettingsDescriptions[17] = {
	/* Power source (DC or batt)          */"Při nižším napětí ukončí pájení <DC=10V, ?S=?x3.3V pro LiPo,LiIon...>",
	/* Sleep temperature                  */"Teplota v režimu spánku.",
	/* Sleep timeout                      */"Čas do režimu spánku <Minut/Sekund>",
	/* Shutdown timeout                   */"Čas do automatického vypnutí <Minut>",
	/* Motion sensitivity level           */"Citlivost detekce pohybu <0=Vyp, 1=Min, ... 9=Max>",
	/* Temperature in F and C             */"Jednotky měření teploty <C=Celsius, F=Fahrenheit>",
	/* Advanced idle display mode enabled */"Zobrazit podrobnosti na výchozí obrazovce?",
	/* Display rotation mode              */"Orientace obrazovky <A=Auto, L=Levák, P=Pravák>",
	/* Boost enabled                      */"Povolit boost držením předního tlačítka při pájení?",
	/* Boost temperature                  */"Teplota v režimu boost.",
	/* Automatic start mode               */"Při startu ihned nahřát hrot?",
	/* Cooldown blink                     */"Blikání teploty při chladnutí, dokud je hrot horký?",
	/* Temperature calibration enter menu */"Kalibrace měření teploty.",
	/* Settings reset command             */"Obnovení továrního nastavení.",
	/* Calibrate input voltage            */"Kalibrace vstupního napětí. Tlačítky uprav, podržením potvrď.",
	/* Advanced soldering screen enabled  */"Zobrazit podrobnosti při pájení?",
	/* Description Scroll Speed           */"Rychlost skrolování popisků podobných tomuto <P=Pomalu,R=Rychle>",
};

const char* SettingsCalibrationWarning = "Ujistěte se, že hrot má pokojovou teplotu!";
const char* SettingsResetWarning = "Opravdu chcete resetovat zařízení do továrního nastavení?";
const char* UVLOWarningString = "DC LOW";               // <=8 chars
const char* UndervoltageString = "! Nízké napětí !";// <=16 chars
const char* InputVoltageString = "Napětí: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Teplota: ";// <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";// <=8 chars
const char* SleepingSimpleString = "Zzz ";// Must be <= 4 chars
const char* SleepingAdvancedString = "Režim spánku...";// <=16 chars
const char* WarningSimpleString = "HOT!";// Must be <= 4 chars
const char* WarningAdvancedString = "!! HORKÝ HROT !!";// <= 16 chars
const char* SleepingTipAdvancedString = "Hrot:";// <=6 chars
const char* IdleTipString = "Hrot:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Cíl:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "HROT NEPŘIPOJEN";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Ohřev: ";// <=12 chars
const char* OffString ="Off";// 3 chars max

const char SettingRightChar = 'P';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'R';
const char SettingSlowChar = 'P';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 11) Power source (DC or batt)          */{"Zdroj", "napájení"},
	/* (<=  9) Sleep temperature                  */{"Teplota v", "r. spánku"},
	/* (<=  9) Sleep timeout                      */{"Čas do", "r. spánku"},
	/* (<= 10) Shutdown timeout                   */{"Čas do", "vypnutí"},
	/* (<= 13) Motion sensitivity level           */{"Citlivost", "det. pohybu"},
	/* (<= 13) Temperature in F and C             */{"Jednotky", "teploty"},
	/* (<= 13) Advanced idle display mode enabled */{"Podrobnosti", "na vých. obr."},
	/* (<= 13) Display rotation mode              */{"Orientace", "obrazovky"},
	/* (<= 13) Boost enabled                      */{"Povolit", "boost"},
	/* (<=  9) Boost temperature                  */{"Teplota v", "r. boost"},
	/* (<= 13) Automatic start mode               */{"Auto", "start"},
	/* (<= 13) Cooldown blink                     */{"Blikáni při", "chladnutí"},
	/* (<= 16) Temperature calibration enter menu */{"Kalibrovat", "teplotu?"},
	/* (<= 16) Settings reset command             */{"Tovární", "nastavení?"},
	/* (<= 16) Calibrate input voltage            */{"Kalibrovat", "vstupní napětí?"},
	/* (<= 13) Advanced soldering screen enabled  */{"Podrobnosti", "při pájení"},
	/* (<= 13) Display Help Text Scroll Speed     */{"Rychlost", "popisků"},
};

// SettingsMenuEntries lengths <= 13 per line (\n starts second line)
const char* SettingsMenuEntries[4] = {
	/* Soldering Menu    */"Pájecí\nnastavení",
	/* Power Saving Menu */"Režim\nspánku",
	/* UI Menu           */"Uživatelské\nrozhraní",
	/* Advanced Menu     */"Pokročilé\nvolby",
};

const char* SettingsMenuEntriesDescriptions[4] = {
	"Nastavení pájení (boost, auto start...)",
	"Nastavení režimu spánku, automatického vypnutí...",
	"Nastavení uživatelského rozhraní.",
	"Pokročilé volby (detailní obrazovky, kalibrace, tovární nastavení...)"
};
#endif

#ifdef LANG_HUN
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length
	/* Power source (DC or batt)          */"Áramforrás. Beállítja a lekapcsolási feszültséget. <DC 10V> <S 3.3V cellánként>",
	/* Sleep temperature                  */"Alvási hőmérséklet <C>",
	/* Sleep timeout                      */"Elalvási időzítő <Perc/Másodperc>",
	/* Shutdown timeout                   */"Kikapcsolási időzítő <Minutes>",
	/* Motion sensitivity level           */"Mozgás érzékenység beállítása. <0.Ki 1.kevésbé érzékeny 9.legérzékenyebb>",
	/* Temperature in F and C             */"Hőmérsékleti egység <C=Celsius F=Fahrenheit>",
	/* Advanced idle display mode enabled */"Részletes információ megjelenítése kisebb betűméretben a készenléti képernyőn.",
	/* Display rotation mode              */"Megjelenítési tájolás <A. Automatikus L. Balkezes R. Jobbkezes>",
	/* Boost enabled                      */"Elülső gombbal lépjen boost módba, 450C forrasztás közben",
	/* Boost temperature                  */"Hőmérséklet \"boost\" módban",
	/* Automatic start mode               */"Bekapcsolás után automatikusan lépjen forrasztás módba. T=Forrasztás, S=Alvó mód,F=Ki",
	/* Cooldown blink                     */"Villogjon a hőmérséklet hűlés közben, amíg a hegy forró.",
	/* Temperature calibration enter menu */"Hegy hőmérsékletének kalibrálása",
	/* Settings reset command             */"Beállítások alaphelyzetbe állítása",
	/* Calibrate input voltage            */"A bemeneti feszültség kalibrálása. Röviden megnyomva állítsa be, hosszan nyomja meg a kilépéshez.",
	/* Advanced soldering screen enabled  */"Részletes információk megjelenítése forrasztás közben",
	/* Description Scroll Speed           */"Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Folytatás előtt győződj meg róla, hogy a hegy szobahőmérsékletű!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";               // <=8 chars
const char* UndervoltageString = "Undervoltage";// <=16 chars
const char* InputVoltageString = "Input V: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";// <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";// <=8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Alvás...";// <=16 chars
const char* WarningSimpleString = "HOT!";// Must be <= 4 chars
const char* WarningAdvancedString = "FIGYELEM! FORRÓ HEGY!";// <=16 chars
const char* SleepingTipAdvancedString = "Tip:";// <=6 chars
const char* IdleTipString = "Tip:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";// <=12 chars
const char* OffString ="Off";// 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
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
	/* (<= 6) Message Scroll Speed               */{"DESCSP"},
};
const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Soldering\nSettings",
	/* Power Saving Menu*/"Sleep\nModes",
	/* UI Menu*/"User\nInterface",
	/* Advanced Menu*/"Advanced\nOptions",};
const char* SettingsMenuEntriesDescriptions[4] = {
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
	/* Power source (DC or batt)          */"Strømforsyning. Indstil Cutoff Spændingen. <DC 10V <S 3.3V per cell",
	/* Sleep temperature                  */"Dvale Temperatur <C",
	/* Sleep timeout                      */"Dvale Timeout <Minutter/Sekunder",
	/* Shutdown timeout                   */"sluknings Timeout <Minutter",
	/* Motion sensitivity level           */"Bevægelsesfølsomhed <0.Slukket 1.Mindst følsom 9.Mest følsom",
	/* Temperature in F and C             */"Temperatur Enhed <C=Celsius F=Fahrenheit",
	/* Advanced idle display mode enabled */"Vis detialieret information med en mindre skriftstørrelse på standby skærmen.",
	/* Display rotation mode              */"Skærm Orientering <A. Automatisk V. Venstre Håndet H. Højre Håndet",
	/* Boost enabled                      */"Ved tryk på front knap Aktiveres boost-funktionen, 450C tilstand når der loddes",
	/* Boost temperature                  */"Temperatur i \"boost\" mode",
	/* Automatic start mode               */"Start automatisk med lodning når strøm sættes til. L=Lodning, D= Dvale tilstand,S=Slukket",
	/* Cooldown blink                     */"Blink temperaturen på skærmen, mens spidsen stadig er varm.",
	/* Temperature calibration enter menu */"kalibrere spids temperatur.",
	/* Settings reset command             */"Gendan alle indstillinger",
	/* Calibrate input voltage            */"VIN kalibrering. Knapperne justere, Lang tryk for at gå ud",
	/* Advanced soldering screen enabled  */"Vis detialieret information mens der loddes",
	/* Description Scroll Speed           */"Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Sørg for at loddespidsen er ved stuetemperatur, inden du fortsætter!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "Lav Volt";             // <=8 chars
const char* UndervoltageString = "Undervoltage";// <=16 chars
const char* InputVoltageString = "Input V: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";// <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";// <=8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Dvale...";// <=16 chars
const char* WarningSimpleString = "Varm";// Must be <= 4 chars
const char* WarningAdvancedString = "ADVARSEL! VARM LODDESPIDS!";// <=16 chars
const char* SleepingTipAdvancedString = "Tip:";// <=6 chars
const char* IdleTipString = "Tip:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";// <=12 chars
const char* OffString ="Off";// 3 chars max

const char SettingRightChar = 'H';
const char SettingLeftChar = 'V';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
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
	/* (<= 6) Message Scroll Speed               */{"DESCSP"},
};
const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Soldering\nSettings",
	/* Power Saving Menu*/"Sleep\nModes",
	/* UI Menu*/"User\nInterface",
	/* Advanced Menu*/"Advanced\nOptions",};
const char* SettingsMenuEntriesDescriptions[4] = {
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
	/* Power source (DC or batt)          */"Źródło zasilania. Ustaw napięcie odcięcia. <DC 10V> <S 3.3V dla ogniw Li>",
	/* Sleep temperature                  */"Temperatura uśpienia <°C>",
	/* Sleep timeout                      */"Czas uśpienia <Minuty/Sekundy>",
	/* Shutdown timeout                   */"Czas wyłączenia <Minuty>",
	/* Motion sensitivity level           */"Czułość ruchu <0.Wyłączona 1.minimalna 9.maksymalna>",
	/* Temperature in F and C             */"Jednostka temperatury <C=Celsius F=Fahrenheit>",
	/* Advanced idle display mode enabled */"Wyświetla szczegółowe informacje za pomocą mniejszej czcionki na ekranie bezczynnośći <T = wł., N = wył.>",
	/* Display rotation mode              */"Orientacja wyświetlacza <A. Automatyczna L. Leworęczna P. Praworęczna>",
	/* Boost enabled                      */"Użyj przycisku przedniego w celu zwiększenia temperatury <T = wł., N = wył.>",
	/* Boost temperature                  */"Temperatura w trybie \"boost\" ",
	/* Automatic start mode               */"Automatyczne uruchamianie trybu lutowania po włączeniu zasilania. T=Lutowanie, S= Tryb Uspienia ,N=Wyłącz",
	/* Cooldown blink                     */"Temperatura na ekranie miga, gdy grot jest jeszcze gorący. <T = wł., N = wył.>",
	/* Temperature calibration enter menu */"Kalibracja temperatury grota lutownicy",
	/* Settings reset command             */"Zresetuj wszystkie ustawienia",
	/* Calibrate input voltage            */"Kalibracja napięcia wejściowego. Krótkie naciśnięcie, aby ustawić, długie naciśnięcie, aby wyjść.",
	/* Advanced soldering screen enabled  */"Wyświetl szczegółowe informacje podczas lutowania <T = wł., N = wył.>",
	/* Description Scroll Speed           */"Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Przed kontynuowaniem upewnij się, że końcówka osiągnela temperature pokojowa!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";               // <=8 chars
const char* UndervoltageString = "Undervoltage";// <=16 chars
const char* InputVoltageString = "Input V: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";// <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";// <=8 chars
const char* SleepingSimpleString = "Zzz!";// Must be <= 4 chars
const char* SleepingAdvancedString = "Uspienie...";// <=16 chars
const char* WarningSimpleString = "HOT!";// Must be <= 4 chars
const char* WarningAdvancedString = "UWAGA! GORĄCA KOŃCÓWKA!";// <=16 chars
const char* SleepingTipAdvancedString = "Tip:";// <=6 chars
const char* IdleTipString = "Tip:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP DISCONNECTED";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";// <=12 chars
const char* OffString ="Off";// 3 chars max

const char SettingRightChar = 'P';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
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
	/* (<= 6) Message Scroll Speed               */{"DESCSP"},
};
const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Soldering\nSettings",
	/* Power Saving Menu*/"Sleep\nModes",
	/* UI Menu*/"User\nInterface",
	/* Advanced Menu*/"Advanced\nOptions",};
const char* SettingsMenuEntriesDescriptions[4] = {
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
	/* Power source (DC or batt)          */"Fonte de alimentação. Define a tensão de corte. <DC=10V> <S=3.3V/célula>"
	/* Sleep temperature                  */"Temperatura de repouso <C>",
	/* Sleep timeout                      */"Tempo para repouso <Minutos/Segundos>",
	/* Shutdown timeout                   */"Tempo para desligamento <Minutos>",
	/* Motion sensitivity level           */"Sensibilidade ao movimento <0=Desligado 1=Menor 9=Maior>",
	/* Temperature in F and C             */"Unidade de temperatura <C=Celsius F=Fahrenheit>",
	/* Advanced idle display mode enabled */"Exibe informações avançadas quando ocioso",
	/* Display rotation mode              */"Orientação da tela <A.utomática C.anhoto D.estro>",
	/* Boost enabled                      */"Tecla frontal ativa modo \"turbo\"",
	/* Boost temperature                  */"Ajuste de temperatura do modo \"turbo\"",
	/* Automatic start mode               */"Aquece a ponta automaticamente ao ligar",
	/* Cooldown blink                     */"Faz o valor da temperatura piscar durante o resfriamento",
	/* Temperature calibration enter menu */"Calibra a temperatura",
	/* Settings reset command             */"Reverte todos ajustes",
	/* Calibrate input voltage            */"Calibra a tensão de alimentação. Use os botões para ajustar o valor. Mantenha presionado para sair",
	/* Advanced soldering screen enabled  */"Exibe informações avançadas durante o uso",
	/* Description Scroll Speed           */"Velocidade que esse texto é exibido",

#ifdef PIDSETTINGS

	"PID P term. Inverse values! This acts as a divisor. So Larger numbers == typically smaller in other systems",
	"PID I term. Inverse values! This acts as a divisor. So Larger numbers == typically smaller in other systems",
	"PID D term. Inverse values! This acts as a divisor. So Larger numbers == typically smaller in other systems",

#endif

};

const char* SettingsCalibrationWarning = "A ponta deve estar em temperatura ambiente antes de continuar!";
const char* SettingsResetWarning = "Resetar para ajustes de fábrica?";
const char* UVLOWarningString = "DC BAIXO";           // <=8 chars
const char* UndervoltageString = "Subtensão";// <=16 chars
const char* InputVoltageString = "Tensão ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Temperatura ";// <=12 chars, preferably end with a space
const char* BadTipString = "ER PONTA";// <=8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Repouso...";// <=16 chars
const char* WarningSimpleString = "TEMP";// Must be <= 4 chars
const char* WarningAdvancedString = "TEMP ELEVADA!";// <=16 chars
const char* SleepingTipAdvancedString = "Ponta:";// <=6 chars
const char* IdleTipString = "Ponta:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Aj:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "SEM PONTA";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Power: ";// <=12 chars
const char* OffString ="Off";// 3 chars max

const char SettingRightChar = 'D';
const char SettingLeftChar = 'C';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'R';
const char SettingSlowChar = 'L';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
//                                                    1234567890123456   1234567890123456
	/* (<= 11) Power source (DC or batt)          */{"Fonte",           "alimentação"},
	/* (<=  9) Sleep temperature                  */{"Temperat.",       "repouso"},
	/* (<=  9) Sleep timeout                      */{"Tempo",           "repouso"},
	/* (<= 10) Shutdown timeout                   */{"Tempo",           "desligam."},
	/* (<= 13) Motion sensitivity level           */{"Sensibilidade",   "movimento"},
	/* (<= 13) Temperature in F and C             */{"Unidade",         "temperatura"},
	/* (<= 13) Advanced idle display mode enabled */{"Tela repouso",    "avançada"},
	/* (<= 13) Display rotation mode              */{"Orientação",      "tela"},
	/* (<= 13) Boost enabled                      */{"Modo turbo",      "ativado"},
	/* (<=  9) Boost temperature                  */{"Modo turbo",      "temperat."},
	/* (<= 13) Automatic start mode               */{"Partida",         "automática"},
	/* (<= 13) Cooldown blink                     */{"Piscar ao",       "resfriar"},
	/* (<= 16) Temperature calibration enter menu */{"Calibrar",        "temperatura"},
	/* (<= 16) Settings reset command             */{"Reset de",        "fábrica?"},
	/* (<= 16) Calibrate input voltage            */{"Calibrar",        "tensão"},
	/* (<= 13) Advanced soldering screen enabled  */{"Tela trabalho",   "avançada"},
	/* (<= 11) Display Help Text Scroll Speed     */{"Velocidade",      "texto ajuda"},
#ifdef PIDSETTINGS
	{	"PID","P"},
	{	"PID","I"},
	{	"PID","D"},
#endif

};

// SettingsMenuEntries lengths <= 13 per line (\n starts second line)
const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Configurações\nSolda",
	/* Power Saving Menu*/"Modos\nRepouso",
	/* UI Menu*/"Interface\nUsuário",
	/* Advanced Menu*/"Menu\nAvançado",};
const char* SettingsMenuEntriesDescriptions[4] = {
	"Configurações de soldagem",
	"Configurações de economia de energia",
	"Configurações da interface do usuário",
	"Opções avançadas"
};
#endif

#ifdef LANG_LT
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Išjungimo įtampa. <DC 10V arba celių (S) kiekis (3.3V per celę)>",
	/* Sleep temperature                  */"Miego temperatūra <C>",
	/* Sleep timeout                      */"Miego laikas <minutės/sekundės>",
	/* Shutdown timeout                   */"Išjungimo laikas <minutės>",
	/* Motion sensitivity level           */"Jautrumas judesiui <0 - išjungta, 1 - mažiausias, 9 - didžiausias>",
	/* Temperature in F and C             */"Temperatūros vienetai <C - Celsijus, F - Farenheitas>",
	/* Advanced idle display mode enabled */"Ar rodyti papildomą informaciją mažesniu šriftu laukimo ekrane",
	/* Display rotation mode              */"Ekrano orientacija <A - automatinė, K - kairiarankiams, D - dešiniarankiams>",
	/* Boost enabled                      */"Ar lituojant viršutinis mygtukas įjungia turbo režimą",
	/* Boost temperature                  */"Temperatūra turbo režimu",
	/* Automatic start mode               */"Ar pradėti kaitininti iš karto įjungus lituoklį",
	/* Cooldown blink                     */"Ar mirksėti temperatūrą ekrane kol vėstantis antgalis vis dar karštas",
	/* Temperature calibration enter menu */"Antgalio temperatūros kalibravimas",
	/* Settings reset command             */"Nustatyti nustatymus iš naujo",
	/* Calibrate input voltage            */"Įvesties įtampos kalibravimas. Trumpai paspauskite, norėdami nustatyti, ilgai paspauskite, kad išeitumėte",
	/* Advanced soldering screen enabled  */"Ar rodyti išsamią informaciją lituojant",
	/* Description Scroll Speed           */"Greitis, kuriuo šis tekstas slenka",
};

const char* SettingsCalibrationWarning = "Prieš tęsdami įsitikinkite, kad antgalis yra kambario temperatūros!";
const char* SettingsResetWarning = "Ar norite atstatyti nustatymus į numatytas reikšmes?";
const char* UVLOWarningString = "MAŽ VOLT";                // <=8 chars
const char* UndervoltageString = "Žema įtampa";// <=16 chars
const char* InputVoltageString = "Įvestis V: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Antgl Temp: ";// <=12 chars, preferably end with a space
const char* BadTipString = "BLOG ANT";// <=8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Miegu...";// <=16 chars
const char* WarningSimpleString = "KRŠ!";// Must be <= 4 chars
const char* WarningAdvancedString = "ANTGALIS KARŠTAS";// <=16 chars
const char* SleepingTipAdvancedString = "Antgl:";// <=6 chars
const char* IdleTipString = "Ant:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Nust:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "ANTGAL ATJUNGTAS";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Maitinimas: ";// <=12 chars
const char* OffString ="Išj";// 3 chars max

const char SettingRightChar = 'D';
const char SettingLeftChar = 'K';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'T';
const char SettingSlowChar = 'N';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 11) Power source (DC or batt)          */{"Maitinimo", "šaltinis"},
	/* (<=  9) Sleep temperature                  */{"Miego", "temperat."},
	/* (<=  9) Sleep timeout                      */{"Miego", "laikas"},
	/* (<= 10) Shutdown timeout                   */{"Išjungimo", "laikas"},
	/* (<= 13) Motion sensitivity level           */{"Jautrumas", "judesiui"},
	/* (<= 13) Temperature in F and C             */{"Temperatūros", "vienetai"},
	/* (<= 13) Advanced idle display mode enabled */{"Detalus lauki-", "mo ekranas"},
	/* (<= 13) Display rotation mode              */{"Ekrano", "orientacija"},
	/* (<= 13) Boost enabled                      */{"Turbo režimas", "įjungtas"},
	/* (<=  9) Boost temperature                  */{"Turbo", "temperat."},
	/* (<= 13) Automatic start mode               */{"Auto", "paleidimas"},
	/* (<= 13) Cooldown blink                     */{"Atvėsimo", "mirksėjimas"},
	/* (<= 16) Temperature calibration enter menu */{"Kalibruoti", "temperatūrą?"},
	/* (<= 16) Settings reset command             */{"Atstatyti", "nustatymus?"},
	/* (<= 16) Calibrate input voltage            */{"Kalibruoti", "įvesties įtampą?"},
	/* (<= 13) Advanced soldering screen enabled  */{"Detalus lita-", "vimo ekranas"},
	/* (<= 11) Display Help Text Scroll Speed     */{"Greitas apra-","šymo slinkimas"},
};

const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Litavimo\nnustatymai",
	/* Power Saving Menu*/"Miego\nrežimai",
	/* UI Menu*/"Naudotojo\nsąsaja",
	/* Advanced Menu*/"Išplėstiniai\nnustatymai",};
const char* SettingsMenuEntriesDescriptions[4] = {
	"Litavimo nustatymai",
	"Energijos vartojimo nustatymai",
	"Naudotojo sąsajos nustatymai",
	"Išplėstiniai nustatymai"
};
#endif

#ifdef LANG_UA
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Установка напруги відключення. <DC - 10V, 3S - 9.9V, 4S - 13.2V, 5S - 16.5V, 6S - 19.8V>",
	/* Sleep temperature                  */"Температура режиму очікування <C°/F°>",
	/* Sleep timeout                      */"Час до переходу в режим очікування <Доступно отключение>",
	/* Shutdown timeout                   */"Час до відключення <Доступно відключення>",
	/* Motion sensitivity level           */"Акселерометр <0 - Викл. 1 хв. чутливості 9 - макс. чутливості>",
	/* Temperature in F and C             */"Одиниця виміру температури <C - Цельсій, F - Фаренгейт>",
	/* Advanced idle display mode enabled */"Показувати детальну в режимі налаштувань <Замість картинки>",
	/* Display rotation mode              */"Орієнтація дисплея <A - Автоповорот, L - Лівша, R - Правша>",
	/* Boost enabled                      */"Турбо-режим при утриманні кнопки А при пайці",
	/* Boost temperature                  */"Температура в Турбо-режимі",
	/* Automatic start mode               */"Автоматичний перехід в режим пайки при включенні живлення.",
	/* Cooldown blink                     */"Показувати температуру на екрані охолодження, поки жало залишається гарячим, при цьому екран моргає",
	/* Temperature calibration enter menu */"Калібрування температурного датчика.",
	/* Settings reset command             */"Скидання всіх параметрів до вихідних значень.",
	/* Calibrate input voltage            */"Калібрування напруги входу. Налаштувати кнопками, натиснути і утримати щоб завершити.",
	/* Advanced soldering screen enabled  */"Показувати детальну інформацію при пайці.",
	/* Description Scroll Speed           */"Швидкість прокрутки тексту",
};

const char* SettingsCalibrationWarning = "Переконайтеся, що жало охололо до кімнатної температури, перш ніж продовжувати!";
const char* SettingsResetWarning = "Ви дійсно хочете скинути налаштування до значень за замовчуванням?";
const char* UVLOWarningString = "АККУМ--";              // <=8 chars
const char* UndervoltageString = "Під живленням";// <=16 chars
const char* InputVoltageString = "Жив.(B):";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Жало t°: ";// <=12 chars, preferably end with a space
const char* BadTipString = "Жало--";// <=8 chars
const char* SleepingSimpleString = "Сон ";// Must be <= 4 chars
const char* SleepingAdvancedString = "Очікування...";// <=16 chars
const char* WarningSimpleString = " АЙ!";// Must be <= 4 chars
const char* WarningAdvancedString = "УВАГА ГАРЯЧЕ!";// <=16 chars
const char* SleepingTipAdvancedString = "Жало:";// <=6 chars
const char* IdleTipString = "Жало:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " ->";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "Жало вимкнено!";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Живлення: ";// <=12 chars
const char* OffString ="Вим";// 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = '+';
const char SettingSlowChar = '-';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 11) Power source (DC or batt)          */{"Джерело","живлення"},	//7,8
	/* (<=  9) Sleep temperature                  */{"Темпер.","сну"},	//7,3
	/* (<=  9) Sleep timeout                      */{"Таймаут","сну"},	//7,3
	/* (<= 10) Shutdown timeout                   */{"Часу до","вимкнення"},//7,9
	/* (<= 13) Motion sensitivity level           */{"Чутл. сенсо-","ру руху"},	//12,7
	/* (<= 13) Temperature in F and C             */{"Формат темпе-","ратури(C°/F°)"},//13,13
	/* (<= 13) Advanced idle display mode enabled */{"Детальний ре-","жим очікуван."},//13,13
	/* (<= 13) Display rotation mode              */{"Автоповорот","екрану"},//11,6
	/* (<= 13) Boost enabled                      */{"Режим","Турбо"},	//5,5
	/* (<=  9) Boost temperature                  */{"Темпер.","Турбо"},   //7,5
	/* (<= 13) Automatic start mode               */{"Гарячий","старт"},   //7,5
	/* (<= 13) Cooldown blink                     */{"Показ t° при","охолодж."}, //12,8
	/* (<= 16) Temperature calibration enter menu */{"Калібровка","температури"}, //10,11
	/* (<= 16) Settings reset command             */{"Скинути всі","налаштування?"}, //11,13
	/* (<= 16) Calibrate input voltage            */{"Калібровка","напруга"}, //10,7
	/* (<= 13) Advanced soldering screen enabled  */{"Детальний ре-","жим пайки"}, //13,9
	/* (<= 11) Message Scroll Speed               */{"Швидкість","тексту"},	//9,6
};

const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Пайка",
	/* Power Saving Menu*/"Сон",
	/* UI Menu*/"Інтерфейс",
	/* Advanced Menu*/"Інші",
};

const char* SettingsMenuEntriesDescriptions[4] = {
	"Налаштування для режиму пайки. Діють при включеному жалі.",
	"Налаштування при бездіяльності. Корисно що б не обпектися і з часом не спалити житло.",
	"Користувальницький інтерфейс.",
	"Розширені налаштування. Додаткові зручності."
};
#endif

#ifdef LANG_NL
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length.
	/* Power source (DC or batt)          */"Spanningsbron. Stelt drempelspanning in. <DC 10V> <S 3.3V per cel>",
	/* Sleep temperature                  */"Temperatuur in slaapstand <C>",
	/* Sleep timeout                      */"Slaapstand time-out <Minuten/Seconden>",
	/* Shutdown timeout                   */"Automatisch afsluiten time-out <Minuten>",
	/* Motion sensitivity level           */"Bewegingsgevoeligheid <0.uit 1.minst gevoelig 9.meest gevoelig>",
	/* Temperature in F and C             */"Temperatuureenheid <C=Celsius F=Fahrenheit>",
	/* Advanced idle display mode enabled */"Gedetailleerde informatie weergeven in een kleiner lettertype op het slaapscherm.",
	/* Display rotation mode              */"Schermoriëntatie <A. Automatisch L. Linkshandig R. Rechtshandig>",
	/* Boost enabled                      */"Soldeerbout gaat naar een hogere boost-temperatuur wanneer de voorste knop ingedrukt is.",
	/* Boost temperature                  */"Temperatuur in boostmodes",
	/* Automatic start mode               */"Breng de soldeerbout direct op temperatuur bij het opstarten. T=Soldeertemperatuur, S=Slaapstand-temperatuur, F=Uit",
	/* Cooldown blink                     */"Temperatuur laten flikkeren in het hoofdmenu als de soldeerpunt aan het afkoelen is.",
	/* Temperature calibration enter menu */"Temperatuursafwijking van de soldeerpunt calibreren.",
	/* Settings reset command             */"Alle instellingen terugzetten.",
	/* Calibrate input voltage            */"VIN Calibreren. Knoppen lang ingedrukt houden om te bevestigen.",
	/* Advanced soldering screen enabled  */"Gedetailleerde informatie weergeven in een kleiner lettertype op het soldeerscherm.",
	/* Description Scroll Speed           */"Snelheid waarmee de tekst scrolt.",
};

const char* SettingsCalibrationWarning = "Zorg ervoor dat de soldeerpunt op kamertemperatuur is voor je verdergaat!";
const char* SettingsResetWarning = "Ben je zeker dat je alle instellingen naar de standaardwaardes wilt terugzetten?";
const char* UVLOWarningString = "DC LAAG";               // <=8 chars
const char* UndervoltageString = "Onderspanning";// <=16 chars
const char* InputVoltageString = "Input V: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Tip Temp: ";// <=12 chars, preferably end with a space
const char* BadTipString = "DEFECT";// <=8 chars
const char* SleepingSimpleString = "Zzz ";// Must be <= 4 chars
const char* SleepingAdvancedString = "Slaapstand...";// <=16 chars
const char* WarningSimpleString = "HEET!";// Must be <= 4 chars
const char* WarningAdvancedString = "!!! TIP HEET !!!";// <=16 chars
const char* SleepingTipAdvancedString = "Tip:";// <=6 chars
const char* IdleTipString = "Tip:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Set:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "TIP LOSGEKOPPELD";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "vermogen: ";// <=12 chars
const char* OffString ="Off";// 3 chars max

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 11) Power source (DC or batt)          */{"Spannings-", "bron"},
	/* (<=  9) Sleep temperature                  */{"Slaap", "temp"},
	/* (<=  9) Sleep timeout                      */{"Slaap", "time-out"},
	/* (<= 10) Shutdown timeout                   */{"Uitschakel", "time-out"},
	/* (<= 13) Motion sensitivity level           */{"Bewegings-", "gevoeligheid"},
	/* (<= 13) Temperature in F and C             */{"Temperatuur", "eenheid"},
	/* (<= 13) Advanced idle display mode enabled */{"Gedetailleerd", "slaapscherm"},
	/* (<= 13) Display rotation mode              */{"Scherm-", "oriëntatie"},
	/* (<= 13) Boost enabled                      */{"Boostmodes", "ingeschakeld?"},
	/* (<=  9) Boost temperature                  */{"Boost", "temp"},
	/* (<= 13) Automatic start mode               */{"Auto", "start"},
	/* (<= 13) Cooldown blink                     */{"Afkoel", "flikker"},
	/* (<= 16) Temperature calibration enter menu */{"Calibreer", "temperatuur?"},
	/* (<= 16) Settings reset command             */{"Instellingen", "resetten?"},
	/* (<= 16) Calibrate input voltage            */{"Calibreer", "input-voltage?"},
	/* (<= 13) Advanced soldering screen enabled  */{"Gedetailleerd", "soldeerscherm"},
	/* (<= 11) Display Help Text Scroll Speed     */{"Scroll","snelheid"},
};

// SettingsMenuEntries lengths <= 13 per line (\n starts second line)
const char* SettingsMenuEntries[4] = {
	/* Soldering Menu    */"Soldeer\nInstellingen",
	/* Power Saving Menu */"Slaap\nModes",
	/* UI Menu           */"Gebruikers-\nInterface",
	/* Advanced Menu     */"geavanceerde\nInstellingen",
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
	/* Power source (DC or batt)          */"Strömkälla. Anger lägsta spänning. <DC 10V> <S 3.3V per cell>",
	/* Sleep temperature                  */"Vilotemperatur <C>",
	/* Sleep timeout                      */"Vilo-timeout <Minuter/Seconder>",
	/* Shutdown timeout                   */"Avstängnings-timeout <Minuter>",
	/* Motion sensitivity level           */"Rörelsekänslighet <0.Av 1.minst känslig 9.mest känslig>",
	/* Temperature in F and C             */"Temperaturenhet <C=Celsius F=Fahrenheit>",
	/* Advanced idle display mode enabled */"Visa detaljerad information i mindre typsnitt när inaktiv.",
	/* Display rotation mode              */"Visningsläge <A. Automatisk V. Vänsterhänt H. Högerhänt>",

	/* Boost enabled                      */"Aktivera främre knappen för turboläge (temperaturhöjning) vid lödning",
	/* Boost temperature                  */"Temperatur i \"turbo\"-läge",
	/* Automatic start mode               */"Startar automatiskt lödpennan vid uppstart. T=Lödning, S=Viloläge, F=Av",
	/* Cooldown blink                     */"Blinka temperaturen medan spetsen kyls av och fortfarande är varm.",
	/* Temperature calibration enter menu */"Kalibrera spets-kompensation.",
	/* Settings reset command             */"Återställ alla inställningar",
	/* Calibrate input voltage            */"Inspänningskalibrering. Knapparna justerar, håll inne för avslut",
	/* Advanced soldering screen enabled  */"Visa detaljerad information vid lödning",
	/* Description Scroll Speed           */"Hastighet som den här texten rullar i",
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LÅG";               // <=8 chars
const char* UndervoltageString = "Underspänning";// <=16 chars
const char* InputVoltageString = "Inspän. V: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Spetstemp: ";// <=12 chars, preferably end with a space
const char* BadTipString = "SPETSFEL";// <=8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Viloläge...";// <=16 chars
const char* WarningSimpleString = "VARM";// Must be <= 4 chars
const char* WarningAdvancedString = "!! SPETS VARM !!";// <=16 chars
const char* SleepingTipAdvancedString = "Spets:";// <=6 chars
const char* IdleTipString = "Spets:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = " Ange:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "SPETS URTAGEN";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Ström: ";// <=12 chars
const char* OffString ="Av";// 3 chars max

const char SettingRightChar = 'H';
const char SettingLeftChar = 'V';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'S';
const char SettingSlowChar = 'L';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 11) Power source (DC or batt)          */{"Ström-", "källa"},
	/* (<=  9) Sleep temperature                  */{"Vilo-", "temp"},
	/* (<=  9) Sleep timeout                      */{"Vilo-", "timeout"},
	/* (<= 10) Shutdown timeout                   */{"Avstängn.", "timeout"},
	/* (<= 13) Motion sensitivity level           */{"Rörelse-", "känslighet"},
	/* (<= 13) Temperature in F and C             */{"Temperatur-", "enheter"},
	/* (<= 13) Advanced idle display mode enabled */{"Detaljerad", "vid inaktiv"},
	/* (<= 13) Display rotation mode              */{"Visnings", "läge"},
	/* (<= 13) Boost enabled                      */{"Turboläge", "aktiverat"},
	/* (<=  9) Boost temperature                  */{"Turbo-", "temp"},
	/* (<= 13) Automatic start mode               */{"Auto", "start"},
	/* (<= 13) Cooldown blink                     */{"Nedkylnings-", "blink"},
	/* (<= 16) Temperature calibration enter menu */{"Kalibrera", "temperatur?"},
	/* (<= 16) Settings reset command             */{"Fabriks-", "inställ?"},
	/* (<= 16) Calibrate input voltage            */{"Kalibrera", "inspänning?"},
	/* (<= 13) Advanced soldering screen enabled  */{"Detaljerad", "lödng.skärm"},
	/* (<= 11) Display Help Text Scroll Speed     */{"Beskrivning","rullhast."},
};

// SettingsMenuEntries lengths <= 13 per line (\n starts second line)
const char* SettingsMenuEntries[4] = {
	/* Soldering Menu    */"Lödnings-\ninställningar",
	/* Power Saving Menu */"Vilo-\nlägen",
	/* UI Menu           */"Användar-\ngränssnitt",
	/* Advanced Menu     */"Avancerade\nalternativ",
};

const char* SettingsMenuEntriesDescriptions[4] = {
	"Lödningsinställningar",
	"Viloläges-inställningar",
	"Användargränssnitts-inställningar",
	"Avancerade alternativ"
};
#endif

#ifdef LANG_SR
const char* SettingsDescriptions[17] =
		{
// These are all the help text for all the settings.
// No requirements on spacing or length.
				/* Power source (DC or batt) / "Izvor napajanja. Postavlja napon isključivanja. <DC 10V> <S 3.3V po ćeliji>",
				 / Sleep temperature / "Temperatura na koju se spušta lemilica nakon određenog vremena mirovanja. <C/F>",
				 / Sleep timeout / "Vreme mirovanja nakon koga lemilica spušta temperaturu. <Minute/Sekunde>",
				 / Shutdown timeout / "Vreme mirovanja nakon koga će se lemilica ugasiti. ",
				 / Motion sensitivity level / "Osetljivost prepoznavanja pokreta. <0=Ugašeno, 1=Najmanje osetljivo, 9=Najosjetljivije>",
				 / Temperature in F and C / "Jedinica temperature. <C=Celzijus, F=Fahrenheit>",
				 / Advanced idle display mode enabled / "Prikazivanje detaljnih informacija manjim fontom tokom čekanja.",
				 / Display rotation mode / "Orijentacija ekrana. <A=Automatski, L=Levoruki, D=Desnoruki>",
				 / Boost enabled / "Držanjem prednjeg dugmeta prilikom lemljenja aktivira se pojačani (Boost) način.",
				 / Boost temperature / "Temperatura u pojačanom (Boost) načinu.",
				 / Automatic start mode / "Ako je aktivno, lemilica po uključivanju napajanja odmah počinje grejati.",
				 / Cooldown blink / "Blinkanje temperature prilikom hlađenja, ako je lemilica vruća.",
				 / Temperature calibration enter menu / "Kalibrisanje temperature meri razliku temperatura vrha i drške, dok je lemilica hladna.",
				 / Settings reset command / "Vraćanje svih podešavanja na fabričke vrednosti.",
				 / Calibrate input voltage / "Kalibracija ulaznog napona. Podešavanje tastera, dugački pritisak za kraj.",
				 / Advanced soldering screen enabled / "Prikazivanje detaljnih informacija tokom lemljenja.",
				 / Description Scroll Speed */"Brzina kretanja dugačkih poruka <B=brzo, S=sporo>", };

const char* SettingsCalibrationWarning =
		"Proverite da li je vrh ohlađen na sobnu temperaturu pre nego što nastavite!";
const char* SettingsResetWarning =
		"Jeste li sigurni da želite da vratite sva podešavanja na fabričke vrednosti?";
const char* UVLOWarningString = "BATERIJA"; // <=8 chars
const char* UndervoltageString = "PRENIZAK NAPON"; // <=16 chars
const char* InputVoltageString = "Napajanje: "; // <=11 chars, preferably end with a space
const char* WarningTipTempString = "Temp vrha: "; // <=12 chars, preferably end with a space
const char* BadTipString = "NEISPRAVAN VRH"; // <=8 chars
const char* SleepingSimpleString = "Zzz "; // Must be <= 4 chars
const char* SleepingAdvancedString = "SPAVANJE..."; // <=16 chars
const char* WarningSimpleString = "VRUĆ"; // Must be <= 4 chars
const char* WarningAdvancedString = "OPREZ, VRUĆE!"; // <=16 chars
const char* SleepingTipAdvancedString = "Vrh: "; // <=6 chars
const char* IdleTipString = "Vrh: "; // IdleTipString+IdleSetString <= 10
const char* IdleSetString = " / "; // preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "VRH NIJE SPOJEN!"; // <=16 chars
const char* SolderingAdvancedPowerPrompt = "Snaga: "; // <=12 chars
const char* OffString = "Off"; // 3 chars max

const char SettingRightChar = 'D';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';

const char SettingFastChar = 'B';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] =
		{
		/* (<= 11) Power source (DC or batt) */{ "Izvor", "napajanja" },
		/* (<= 9) Sleep temperature */{ "Temp", "spavanja" },
		/* (<= 9) Sleep timeout */{ "Vreme", "spavanja" },
		/* (<= 10) Shutdown timeout */{ "Vreme", "gašenja" },
		/* (<= 13) Motion sensitivity level */{ "Osetljivost", "pokreta" },
		/* (<= 13) Temperature in F and C */{ "Jedinica", "temperature" },
		/* (<= 13) Advanced idle display mode enabled */{ "Detalji",
				"pri čekanju" },
		/* (<= 13) Display rotation mode */{ "Rotacija", "ekrana" },
		/* (<= 13) Boost enabled */{ "Boost", "način" },
		/* (<= 9) Boost temperature */{ "Boost", "temp" },
		/* (<= 13) Automatic start mode */{ "Auto", "start" },
		/* (<= 13) Cooldown blink */{ "Upozorenje", "pri hlađenju" },
		/* (<= 16) Temperature calibration enter menu */{ "Kalibracija",
				"temperature" },
		/* (<= 16) Settings reset command */{ "Fabričke", "postavke" },
				/* (<= 16) Calibrate input voltage */{ "Kalibracija",
						"napona napajanja" },
				/* (<= 13) Advanced soldering screen enabled */{ "Detalji",
						"pri lemljenju" },
				/* (<= 11) Display Help Text Scroll Speed */{ "Brzina", "poruka" }, };
const char* SettingsMenuEntries[4] = {
/*Soldering Menu*/"Postavke\nlemljenja",
/* Power Saving Menu*/"Ušteda\nenergije",
/* UI Menu*/"Korisničke\nopcije",
/* Advanced Menu*/"Napredne\nopcije", };
const char* SettingsMenuEntriesDescriptions[4] = { "Postavke pri lemljenju",
		"Postavke spavanja i štednje energije", "Postavke korisničkih opcija",
		"Upravljanje naprednim opcijama" };
#endif

#ifdef LANG_NB
const char* SettingsDescriptions[17] = {
	// These are all the help text for all the settings.
	// No requirements on spacing or length
	/* Power source (DC or batt)          */"Strømforsyning. Sett nedre spenning for automatisk nedstenging. <DC 10V <S 3.3V per celle",
	/* Sleep temperature                  */"Dvaletemperatur <C",
	/* Sleep timeout                      */"Tid før dvale <Minutter/Sekunder",
	/* Shutdown timeout                   */"Tid før automatisk nedstenging <Minutter",
	/* Motion sensitivity level           */"Bevegelsesfølsomhet <0.Inaktiv 1.Minst følsom 9.Mest følsom",
	/* Temperature in F and C             */"Temperaturskala <C=Celsius F=Fahrenheit",
	/* Advanced idle display mode enabled */"Vis detaljert informasjon med liten skrift på dvaleskjermen.",
	/* Display rotation mode              */"Skjermretning <A. Automatisk V. Venstrehendt H. Høyrehendt",
	/* Boost enabled                      */"Frontknappen aktiverer kraftfunksjonen, 450C ved lodding",
	/* Boost temperature                  */"Temperatur i \"kraft\"-modus",
	/* Automatic start mode               */"Start automatisk med lodding når strøm kobles til. L=Lodding, D=Dvale, I=Inaktiv",
	/* Cooldown blink                     */"Blink temperaturen på skjermen mens spissen fortsatt er varm.",
	/* Temperature calibration enter menu */"Kalibrer spiss-temperatur.",
	/* Settings reset command             */"Tilbakestill alle innstillinger",
	/* Calibrate input voltage            */"Kalibrer spenning. Knappene justerer. Langt trykk for å gå ut",
	/* Advanced soldering screen enabled  */"Vis detaljert informasjon ved lodding",
	/* Description Scroll Speed           */"Hastigheten på rulletekst",
};
const char* SettingsCalibrationWarning = "Sørg for at loddespissen har romtemperatur før du fortsetter!";
const char* SettingsResetWarning = "Er du sikker på at du vil tilbakestille til standardinnstillinger?";
const char* UVLOWarningString = "Lavspenn";             // <=8 chars
const char* UndervoltageString = "Underspenning";// <=16 chars
const char* InputVoltageString = "Innspenn.: ";// <=11 chars, preferably end with a space
const char* WarningTipTempString = "Spisstemp: ";// <=12 chars, preferably end with a space
const char* BadTipString = "BAD TIP";// <=8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Dvale...";// <=16 chars
const char* WarningSimpleString = "Varm";// Must be <= 4 chars
const char* WarningAdvancedString = "VARM SPISS!";// <=16 chars
const char* SleepingTipAdvancedString = "Spiss:";// <=6 chars
const char* IdleTipString = "Spiss:";// IdleTipString+IdleSetString <= 10
const char* IdleSetString = "Set:";// preferably start with a space; IdleTipString+IdleSetString <= 10
const char* TipDisconnectedString = "SPISS FRAKOBLET";// <=16 chars
const char* SolderingAdvancedPowerPrompt = "Effekt: ";// <=12 chars
const char* OffString ="Av";// 3 chars max
const char SettingRightChar = 'H';
const char SettingLeftChar = 'V';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'H';
const char SettingSlowChar = 'L';
const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
	/* (<= 5) Power source (DC or batt)          */{"Kilde"},
	/* (<= 4) Sleep temperature                  */{"DTmp"},
	/* (<= 4) Sleep timeout                      */{"DTid"},
	/* (<= 5) Shutdown timeout                   */{"AvTid"},
	/* (<= 6) Motion sensitivity level           */{"BSensr"},
	/* (<= 6) Temperature in F and C             */{"TmpEnh"},
	/* (<= 6) Advanced idle display mode enabled */{"AvDvSk"},
	/* (<= 6) Display rotation mode              */{"SkRetn"},
	/* (<= 6) Boost enabled                      */{"Kraft"},
	/* (<= 4) Boost temperature                  */{"KTmp"},
	/* (<= 6) Automatic start mode               */{"AStart"},
	/* (<= 6) Cooldown blink                     */{"KjBlnk"},
	/* (<= 8) Temperature calibration enter menu */{"TempKal?"},
	/* (<= 8) Settings reset command             */{"TilbStl?"},
	/* (<= 8) Calibrate input voltage            */{"KalSpIn?"},
	/* (<= 6) Advanced soldering screen enabled  */{"AvLdSk"},
	/* (<= 6) Message Scroll Speed               */{"RullHa"},
};
const char* SettingsMenuEntries[4] = {
	/*Soldering Menu*/"Lodde-\ninnst.",
	/* Power Saving Menu*/"Dvale-\ninnst.",
	/* UI Menu*/"Bruker-\ngrensesn.",
	/* Advanced Menu*/"Avanserte\nvalg",};
const char* SettingsMenuEntriesDescriptions[4] = {
	"Loddeinnstillinger",
	"Dvaleinnstillinger",
	"Brukergrensesnitt-innstillinger",
	"Avanserte valg"
};
#endif
