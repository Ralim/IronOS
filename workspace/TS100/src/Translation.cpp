#include "Translation.h"
#ifndef LANG
#define LANG_EN
#endif

#ifdef LANG_EN
// ---- English ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Power source. Sets cutoff voltage. <DC 10V> <S 3.3V per cell>",
  /* SleepTemperature          */ "Sleep Temperature <C>",
  /* SleepTimeout              */ "Sleep Timeout <Minutes/Seconds>",
  /* ShutdownTimeout           */ "Shutdown Timeout <Minutes>",
  /* MotionSensitivity         */ "Motion Sensitivity <0.Off 1.least sensitive 9.most sensitive>",
  /* TemperatureUnit           */ "Temperature Unit <C=Celsius F=Fahrenheit>",
  /* AdvancedIdle              */ "Display detailed information in a smaller font on the idle screen.",
  /* DisplayRotation           */ "Display Orientation <A. Automatic L. Left Handed R. Right Handed>",
  /* BoostEnabled              */ "Enable front key enters boost mode 450C mode when soldering",
  /* BoostTemperature          */ "Temperature when in \"boost\" mode",
  /* AutoStart                 */ "Automatically starts the iron into soldering on power up. T=Soldering, S= Sleep mode,F=Off",
  /* CooldownBlink             */ "Blink the temperature on the cooling screen while the tip is still hot.",
  /* TemperatureCalibration    */ "Calibrate tip offset.",
  /* SettingsReset             */ "Reset all settings",
  /* VoltageCalibration        */ "VIN Calibration. Buttons adjust, long press to exit",
  /* AdvancedSoldering         */ "Display detailed information while soldering",
  /* ScrollingSpeed            */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* SettingsResetWarning = "Are you sure you want to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";
const char* UndervoltageString = "Undervoltage";
const char* InputVoltageString = "Input V: ";
const char* WarningTipTempString = "Tip Temp: ";
const char* BadTipString = "BAD TIP";
const char* SleepingSimpleString = "Zzzz";
const char* SleepingAdvancedString = "Sleeping...";
const char* WarningSimpleString = "HOT!";
const char* WarningAdvancedString = "!!! TIP HOT !!!";
const char* SleepingTipAdvancedString = "Tip:";
const char* IdleTipString = "Tip:";
const char* IdleSetString = " Set:";
const char* TipDisconnectedString = "TIP DISCONNECTED";
const char* SolderingAdvancedPowerPrompt = "Power: ";
const char* OffString = "Off";

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ { "Power", "source" },
  /* SleepTemperature          */ { "Sleep", "temp" },
  /* SleepTimeout              */ { "Sleep", "timeout" },
  /* ShutdownTimeout           */ { "Shutdown", "timeout" },
  /* MotionSensitivity         */ { "Motion", "sensitivity" },
  /* TemperatureUnit           */ { "Temperature", "units" },
  /* AdvancedIdle              */ { "Detailed", "idle screen" },
  /* DisplayRotation           */ { "Display", "orientation" },
  /* BoostEnabled              */ { "Boost mode", "enabled" },
  /* BoostTemperature          */ { "Boost", "temp" },
  /* AutoStart                 */ { "Auto", "start" },
  /* CooldownBlink             */ { "Cooldown", "blink" },
  /* TemperatureCalibration    */ { "Calibrate", "temperature?" },
  /* SettingsReset             */ { "Factory", "Reset?" },
  /* VoltageCalibration        */ { "Calibrate", "input voltage?" },
  /* AdvancedSoldering         */ { "Detailed", "solder screen" },
  /* ScrollingSpeed            */ { "Description", "Scroll Speed" },
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Soldering\nSettings",
  /* PowerSavingMenu           */ "Sleep\nModes",
  /* UIMenu                    */ "User\nInterface",
  /* AdvancedMenu              */ "Advanced\nOptions",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Soldering settings",
  /* PowerSavingMenu           */ "Power saving settings",
  /* UIMenu                    */ "User interface settings",
  /* AdvancedMenu              */ "Advanced options",
};

#endif

#ifdef LANG_BG
// ---- български ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Източник на захранване. Минимално напрежение. <DC 10V> <S 3.3V за клетка>",
  /* SleepTemperature          */ "Температура при режим \"сън\" <C>",
  /* SleepTimeout              */ "Включване в режим \"сън\" след: <Минути/Секунди>",
  /* ShutdownTimeout           */ "Изключване след <Минути>",
  /* MotionSensitivity         */ "Усещане за движение <0.Изключено 1.Слабо 9.Силно>",
  /* TemperatureUnit           */ "Единици за температура <C=Целзии F=Фаренхайт>",
  /* AdvancedIdle              */ "Покажи детайлна информация със ситен шрифт на екрана в режим на покой.",
  /* DisplayRotation           */ "Ориентация на дисплея <A. Автоматично L. Лява Ръка R. Дясна Ръка>",
  /* BoostEnabled              */ "Ползвай предния бутон за \"турбо\" режим с температура до 450C при запояване",
  /* BoostTemperature          */ "Температура за \"турбо\" режим",
  /* AutoStart                 */ "Режим на поялника при включване на захранването. T=Работен, S=Сън, F=Изключен",
  /* CooldownBlink             */ "След изключване от работен режим, индикатора за температура да мига докато човката на поялника все още е топла",
  /* TemperatureCalibration    */ "Калибриране на температурата",
  /* SettingsReset             */ "Връщане на фабрични настройки",
  /* VoltageCalibration        */ "Калибриране на входното напрежение (VIN). Задръжте бутонa за изход",
  /* AdvancedSoldering         */ "Детайлна информация в работен режим при запояване",
  /* ScrollingSpeed            */ "Скорост на движение на този текст",
};

const char* SettingsCalibrationWarning = "Уверете се, че човката на поялника е със стайна температура преди да продължите!";
const char* SettingsResetWarning = "Сигурни ли сте, че искате да върнете фабричните настройки?";
const char* UVLOWarningString = "Ниско V!";
const char* UndervoltageString = "Ниско Напрежение";
const char* InputVoltageString = "Входно V: ";
const char* WarningTipTempString = "Темп.: ";
const char* BadTipString = "ЛОШ ВРЪХ";
const char* SleepingSimpleString = "Сън";
const char* SleepingAdvancedString = "Хър Хър Хър...";
const char* WarningSimpleString = "ОХ!";
const char* WarningAdvancedString = "ВНИМАНИЕ! ТОПЛО!";
const char* SleepingTipAdvancedString = "Връх:";
const char* IdleTipString = "Връх:";
const char* IdleSetString = " Set:";
const char* TipDisconnectedString = "ВРЪХ ЛОША ВРЪЗКА";
const char* SolderingAdvancedPowerPrompt = "Захранване: ";
const char* OffString = "Off";

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ { "Източник", "захранване" },
  /* SleepTemperature          */ { "Темп.", "сън" },
  /* SleepTimeout              */ { "Време", "сън" },
  /* ShutdownTimeout           */ { "Време", "изкл." },
  /* MotionSensitivity         */ { "Усещане", "за движение" },
  /* TemperatureUnit           */ { "Единици за", "температура" },
  /* AdvancedIdle              */ { "Детайлен", "екран в покой" },
  /* DisplayRotation           */ { "Ориентация", "на дисплея" },
  /* BoostEnabled              */ { "Турбо режим", "пуснат" },
  /* BoostTemperature          */ { "Турбо", "темп." },
  /* AutoStart                 */ { "Автоматичен", "работен режим" },
  /* CooldownBlink             */ { "Мигай при", "топъл поялник" },
  /* TemperatureCalibration    */ { "Калибриране", "температура?" },
  /* SettingsReset             */ { "Фабрични", "настройки?" },
  /* VoltageCalibration        */ { "Калибриране", "напрежение?" },
  /* AdvancedSoldering         */ { "Детайлен", "работен екран" },
  /* ScrollingSpeed            */ { "Скорост", "на текста" },
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Поялник\nНастройки",
  /* PowerSavingMenu           */ "Режими\nНастройки",
  /* UIMenu                    */ "Интерфейс\nНастройки",
  /* AdvancedMenu              */ "Разширени\nНастройки",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Настройки на поялника",
  /* PowerSavingMenu           */ "Настройки енергоспестяване",
  /* UIMenu                    */ "Настройки на интерфейса",
  /* AdvancedMenu              */ "Допълнителни настройки",
};

#endif

#ifdef LANG_CS_CZ
// ---- Český ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Při nižším napětí ukončí pájení <DC=10V, ?S=?x3.3V pro LiPo,LiIon...>",
  /* SleepTemperature          */ "Teplota v režimu spánku.",
  /* SleepTimeout              */ "Čas do režimu spánku <Minut/Sekund>",
  /* ShutdownTimeout           */ "Čas do automatického vypnutí <Minut>",
  /* MotionSensitivity         */ "Citlivost detekce pohybu <0=Vyp, 1=Min, ... 9=Max>",
  /* TemperatureUnit           */ "Jednotky měření teploty <C=Celsius, F=Fahrenheit>",
  /* AdvancedIdle              */ "Zobrazit podrobnosti na výchozí obrazovce?",
  /* DisplayRotation           */ "Orientace obrazovky <A=Auto, L=Levák, P=Pravák>",
  /* BoostEnabled              */ "Povolit boost držením předního tlačítka při pájení?",
  /* BoostTemperature          */ "Teplota v režimu boost.",
  /* AutoStart                 */ "Při startu ihned nahřát hrot?",
  /* CooldownBlink             */ "Blikání teploty při chladnutí, dokud je hrot horký?",
  /* TemperatureCalibration    */ "Kalibrace měření teploty.",
  /* SettingsReset             */ "Obnovení továrního nastavení.",
  /* VoltageCalibration        */ "Kalibrace vstupního napětí. Tlačítky uprav, podržením potvrď.",
  /* AdvancedSoldering         */ "Zobrazit podrobnosti při pájení?",
  /* ScrollingSpeed            */ "Rychlost skrolování popisků podobných tomuto <P=Pomalu,R=Rychle>",
};

const char* SettingsCalibrationWarning = "Ujistěte se, že hrot má pokojovou teplotu!";
const char* SettingsResetWarning = "Opravdu chcete resetovat zařízení do továrního nastavení?";
const char* UVLOWarningString = "DC LOW";
const char* UndervoltageString = "! Nízké napětí !";
const char* InputVoltageString = "Napětí: ";
const char* WarningTipTempString = "Teplota: ";
const char* BadTipString = "BAD TIP";
const char* SleepingSimpleString = "Zzz ";
const char* SleepingAdvancedString = "Režim spánku...";
const char* WarningSimpleString = "HOT!";
const char* WarningAdvancedString = "!! HORKÝ HROT !!";
const char* SleepingTipAdvancedString = "Hrot:";
const char* IdleTipString = "Hrot:";
const char* IdleSetString = " Cíl:";
const char* TipDisconnectedString = "HROT NEPŘIPOJEN";
const char* SolderingAdvancedPowerPrompt = "Ohřev: ";
const char* OffString = "Off";

const char SettingRightChar = 'P';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'R';
const char SettingSlowChar = 'P';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ { "Zdroj", "napájení" },
  /* SleepTemperature          */ { "Teplota v", "r. spánku" },
  /* SleepTimeout              */ { "Čas do", "r. spánku" },
  /* ShutdownTimeout           */ { "Čas do", "vypnutí" },
  /* MotionSensitivity         */ { "Citlivost", "det. pohybu" },
  /* TemperatureUnit           */ { "Jednotky", "teploty" },
  /* AdvancedIdle              */ { "Podrobnosti", "na vých. obr." },
  /* DisplayRotation           */ { "Orientace", "obrazovky" },
  /* BoostEnabled              */ { "Povolit", "boost" },
  /* BoostTemperature          */ { "Teplota v", "r. boost" },
  /* AutoStart                 */ { "Auto", "start" },
  /* CooldownBlink             */ { "Blikáni při", "chladnutí" },
  /* TemperatureCalibration    */ { "Kalibrovat", "teplotu?" },
  /* SettingsReset             */ { "Tovární", "nastavení?" },
  /* VoltageCalibration        */ { "Kalibrovat", "vstupní napětí?" },
  /* AdvancedSoldering         */ { "Podrobnosti", "při pájení" },
  /* ScrollingSpeed            */ { "Rychlost", "popisků" },
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Pájecí\nnastavení",
  /* PowerSavingMenu           */ "Režim\nspánku",
  /* UIMenu                    */ "Uživatelské\nrozhraní",
  /* AdvancedMenu              */ "Pokročilé\nvolby",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Nastavení pájení (boost, auto start...)",
  /* PowerSavingMenu           */ "Nastavení režimu spánku, automatického vypnutí...",
  /* UIMenu                    */ "Nastavení uživatelského rozhraní.",
  /* AdvancedMenu              */ "Pokročilé volby (detailní obrazovky, kalibrace, tovární nastavení...)",
};

#endif

#ifdef LANG_DE
// ---- Deutsch ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Spannungsquelle (Abschaltspannung) <DC=10V, nS=n*3.3V für n LiIon-Zellen>",
  /* SleepTemperature          */ "Ruhetemperatur (In der eingestellten Einheit)",
  /* SleepTimeout              */ "Ruhemodus nach <Sekunden/Minuten>",
  /* ShutdownTimeout           */ "Abschalten nach <Minuten>",
  /* MotionSensitivity         */ "Bewegungsempfindlichkeit <0=Aus, 1=Minimal ... 9=Maximal>",
  /* TemperatureUnit           */ "Temperatureinheit <C=Celsius, F=Fahrenheit>",
  /* AdvancedIdle              */ "Detaillierte Anzeige im Ruhemodus <J=An, N=Aus>",
  /* DisplayRotation           */ "Ausrichtung der Anzeige <A=Automatisch, L=Linkshändig, R=Rechtshändig>",
  /* BoostEnabled              */ "Vordere Taste für Temperaturboost verwenden <J=An, N=Aus>",
  /* BoostTemperature          */ "Temperatur im Boostmodus  (In der eingestellten Einheit)",
  /* AutoStart                 */ "Automatischer Start des Lötmodus beim Einschalten der Spannungsversorgung. <J=An, N=Aus>",
  /* CooldownBlink             */ "Blinkende Temperaturanzeige beim Abkühlen, solange heiß. <J=An, N=Aus>",
  /* TemperatureCalibration    */ "Kalibrierung der Lötspitzentemperatur",
  /* SettingsReset             */ "Alle Einstellungen zurücksetzen",
  /* VoltageCalibration        */ "Kalibrierung der Eingangsspannung. Kurzer Tastendruck zum Einstellen, langer Tastendruck zum Verlassen.",
  /* AdvancedSoldering         */ "Detaillierte Anzeige im Lötmodus <J=An, N=Aus>",
  /* ScrollingSpeed            */ "Scrollgeschwindigkeit der Texte",
};

const char* SettingsCalibrationWarning = "Vor dem Fortfahren muss die Lötspitze vollständig abgekühlt sein!";
const char* SettingsResetWarning = "Sind Sie sicher, dass Sie alle Werte Zurücksetzen wollen?";
const char* UVLOWarningString = "V niedr.";
const char* UndervoltageString = "Unterspannung";
const char* InputVoltageString = "V Eingang: ";
const char* WarningTipTempString = "Temperatur: ";
const char* BadTipString = "DEFEKT";
const char* SleepingSimpleString = "Zzz ";
const char* SleepingAdvancedString = "Ruhemodus...";
const char* WarningSimpleString = "HEIß";
const char* WarningAdvancedString = "! Achtung Heiß !";
const char* SleepingTipAdvancedString = "Temp:";
const char* IdleTipString = "Ist:";
const char* IdleSetString = " Soll:";
const char* TipDisconnectedString = "Spitze fehlt";
const char* SolderingAdvancedPowerPrompt = "Leistung: ";
const char* OffString = "Off";

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ { "Spannungs-", "quelle" },
  /* SleepTemperature          */ { "Ruhetemp-", "eratur" },
  /* SleepTimeout              */ { "Ruhever-", "zögerung" },
  /* ShutdownTimeout           */ { "Abschalt-", "zeit" },
  /* MotionSensitivity         */ { "Bewegungs-", "empfindlichk." },
  /* TemperatureUnit           */ { "Temperatur-", "einheit" },
  /* AdvancedIdle              */ { "Detaillierte", "Ruheansicht" },
  /* DisplayRotation           */ { "Anzeige-", "ausrichtung" },
  /* BoostEnabled              */ { "Boosttaste", "aktiv?" },
  /* BoostTemperature          */ { "Boosttemp-", "eratur" },
  /* AutoStart                 */ { "Start im", "Lötmodus?" },
  /* CooldownBlink             */ { "Abkühl-", "blinken?" },
  /* TemperatureCalibration    */ { "Temperatur", "kalibrieren?" },
  /* SettingsReset             */ { "Einstellungen", "zurücksetzen?" },
  /* VoltageCalibration        */ { "Eingangsspannung", "kalibrieren?" },
  /* AdvancedSoldering         */ { "Detaillierte", "Lötansicht" },
  /* ScrollingSpeed            */ { "Scroll-", "geschw." },
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Löt-\neinstellungen",
  /* PowerSavingMenu           */ "Schlaf-\nmodus",
  /* UIMenu                    */ "Menü-\neinstellungen",
  /* AdvancedMenu              */ "Erweiterte\nEinstellungen",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Löteinstellungen",
  /* PowerSavingMenu           */ "Energiespareinstellungen",
  /* UIMenu                    */ "Menüeinstellungen",
  /* AdvancedMenu              */ "Erweiterte Einstellungen",
};

#endif

#ifdef LANG_DK
// ---- Dansk ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Strømforsyning. Indstil Cutoff Spændingen. <DC 10V <S 3.3V per cell",
  /* SleepTemperature          */ "Dvale Temperatur <C",
  /* SleepTimeout              */ "Dvale Timeout <Minutter/Sekunder",
  /* ShutdownTimeout           */ "sluknings Timeout <Minutter",
  /* MotionSensitivity         */ "Bevægelsesfølsomhed <0.Slukket 1.Mindst følsom 9.Mest følsom",
  /* TemperatureUnit           */ "Temperatur Enhed <C=Celsius F=Fahrenheit",
  /* AdvancedIdle              */ "Vis detialieret information med en mindre skriftstørrelse på standby skærmen.",
  /* DisplayRotation           */ "Skærm Orientering <A. Automatisk V. Venstre Håndet H. Højre Håndet",
  /* BoostEnabled              */ "Ved tryk på front knap Aktiveres boost-funktionen, 450C tilstand når der loddes",
  /* BoostTemperature          */ "Temperatur i \"boost\" mode",
  /* AutoStart                 */ "Start automatisk med lodning når strøm sættes til. L=Lodning, D= Dvale tilstand,S=Slukket",
  /* CooldownBlink             */ "Blink temperaturen på skærmen, mens spidsen stadig er varm.",
  /* TemperatureCalibration    */ "kalibrere spids temperatur.",
  /* SettingsReset             */ "Gendan alle indstillinger",
  /* VoltageCalibration        */ "VIN kalibrering. Knapperne justere, Lang tryk for at gå ud",
  /* AdvancedSoldering         */ "Vis detialieret information mens der loddes",
  /* ScrollingSpeed            */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Sørg for at loddespidsen er ved stuetemperatur, inden du fortsætter!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "Lav Volt";
const char* UndervoltageString = "Undervoltage";
const char* InputVoltageString = "Input V: ";
const char* WarningTipTempString = "Tip Temp: ";
const char* BadTipString = "BAD TIP";
const char* SleepingSimpleString = "Zzzz";
const char* SleepingAdvancedString = "Dvale...";
const char* WarningSimpleString = "Varm";
const char* WarningAdvancedString = "ADVARSEL! VARM LODDESPIDS!";
const char* SleepingTipAdvancedString = "Tip:";
const char* IdleTipString = "Tip:";
const char* IdleSetString = " Set:";
const char* TipDisconnectedString = "TIP DISCONNECTED";
const char* SolderingAdvancedPowerPrompt = "Power: ";
const char* OffString = "Off";

const char SettingRightChar = 'H';
const char SettingLeftChar = 'V';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ "PWRSC",
  /* SleepTemperature          */ "STMP",
  /* SleepTimeout              */ "STME",
  /* ShutdownTimeout           */ "SHTME",
  /* MotionSensitivity         */ "MSENSE",
  /* TemperatureUnit           */ "TMPUNT",
  /* AdvancedIdle              */ "ADVIDL",
  /* DisplayRotation           */ "DSPROT",
  /* BoostEnabled              */ "BOOST",
  /* BoostTemperature          */ "BTMP",
  /* AutoStart                 */ "ASTART",
  /* CooldownBlink             */ "CLBLNK",
  /* TemperatureCalibration    */ "TMP CAL?",
  /* SettingsReset             */ "RESET?",
  /* VoltageCalibration        */ "CAL VIN?",
  /* AdvancedSoldering         */ "ADVSLD",
  /* ScrollingSpeed            */ "DESCSP",
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Soldering\nSettings",
  /* PowerSavingMenu           */ "Sleep\nModes",
  /* UIMenu                    */ "User\nInterface",
  /* AdvancedMenu              */ "Advanced\nOptions",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Soldering settings",
  /* PowerSavingMenu           */ "Power Saving Settings",
  /* UIMenu                    */ "User Interface settings",
  /* AdvancedMenu              */ "Advanced options",
};

#endif

#ifdef LANG_ES
// ---- Español ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Fuente de energía. Ajusta el límite inferior de voltaje. <DC=10V S=3.3V por celda>",
  /* SleepTemperature          */ "Temperatura en reposo. <C>",
  /* SleepTimeout              */ "Tiempo hasta activar reposo. <Minutos>",
  /* ShutdownTimeout           */ "Tiempo hasta apagado. <Minutos>",
  /* MotionSensitivity         */ "Sensibilidad del movimiento. <0=Apagado 1=El menos sensible 9=El más sensible>",
  /* TemperatureUnit           */ "Unidad de temperatura.",
  /* AdvancedIdle              */ "Muestra información detallada pequena en la pantalla inactiva.",
  /* DisplayRotation           */ "Orientación de la pantalla <A=Automático I=Mano izquierda D=Mano derecha>",
  /* BoostEnabled              */ "Activar el botón \"Boost\" en modo soldadura.",
  /* BoostTemperature          */ "Temperatura en modo \"Boost\". <C>",
  /* AutoStart                 */ "Iniciar modo soldadura en el encendido. <V=Sí S=Modo reposo F=No>",
  /* CooldownBlink             */ "Parpadea la temperatura en el enfriamiento si la punta sigue caliente.",
  /* TemperatureCalibration    */ "Calibrar desviación térmica de la punta.",
  /* SettingsReset             */ "Volver a estado de fábrica.",
  /* VoltageCalibration        */ "Calibración VIN (Voltaje de entrada). Ajuste con ambos botones, pulsación larga para salir.",
  /* AdvancedSoldering         */ "Mostrar información detallada mientras suelda.",
  /* ScrollingSpeed            */ "Velocidad de desplazamiento del texto.",
};

const char* SettingsCalibrationWarning = "¡Asegúrese que la punta esté a temperatura ambiente antes de proceder!";
const char* SettingsResetWarning = "¿Está seguro de volver a estado de fábrica?";
const char* UVLOWarningString = "DC BAJO";
const char* UndervoltageString = "Subtensión";
const char* InputVoltageString = "V entrada: ";
const char* WarningTipTempString = "Temp Punta: ";
const char* BadTipString = "ER PUNTA";
const char* SleepingSimpleString = "Zzzz";
const char* SleepingAdvancedString = "En reposo...";
const char* WarningSimpleString = "TEMP";
const char* WarningAdvancedString = "¡PUNTA CALIENTE!";
const char* SleepingTipAdvancedString = "Punta:";
const char* IdleTipString = "Punta:";
const char* IdleSetString = " Aj:";
const char* TipDisconnectedString = "SIN PUNTA";
const char* SolderingAdvancedPowerPrompt = "POTENCIA: ";
const char* OffString = "Off";

const char SettingRightChar = 'D';
const char SettingLeftChar = 'I';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'R';
const char SettingSlowChar = 'L';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ "PWRSC",
  /* SleepTemperature          */ "STMP",
  /* SleepTimeout              */ "STME",
  /* ShutdownTimeout           */ "SHTME",
  /* MotionSensitivity         */ "MSENSE",
  /* TemperatureUnit           */ "TMPUNT",
  /* AdvancedIdle              */ "ADVIDL",
  /* DisplayRotation           */ "DSPROT",
  /* BoostEnabled              */ "BOOST",
  /* BoostTemperature          */ "BTMP",
  /* AutoStart                 */ "ASTART",
  /* CooldownBlink             */ "CLBLNK",
  /* TemperatureCalibration    */ "TMP CAL?",
  /* SettingsReset             */ "RESET?",
  /* VoltageCalibration        */ "CAL VIN?",
  /* AdvancedSoldering         */ "ADVSLD",
  /* ScrollingSpeed            */ "DESCSP",
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Opciones de\nSoldadura",
  /* PowerSavingMenu           */ "Modos de\nReposo",
  /* UIMenu                    */ "Interfaz\nde Usuario",
  /* AdvancedMenu              */ "Opciones\nAvanzadas",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Opciones de Soldadura",
  /* PowerSavingMenu           */ "Opciones de ahorro energético",
  /* UIMenu                    */ "Opciones de interfaz de usuario",
  /* AdvancedMenu              */ "Opciones avanzadas",
};

#endif

#ifdef LANG_FR
// ---- Français ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Source d'alimentation. Règle la tension de coupure <DC=10V S=3.3V par cellules>",
  /* SleepTemperature          */ "Température en veille <C>",
  /* SleepTimeout              */ "Délai avant mise en veille <Minutes>",
  /* ShutdownTimeout           */ "Délai avant extinction <Minutes>",
  /* MotionSensitivity         */ "Sensibilité du capteur de mouvement <0=Inactif 1=Peu sensible 9=Tres sensible>",
  /* TemperatureUnit           */ "Unité de température <C=Celsius F=Fahrenheit>",
  /* AdvancedIdle              */ "Afficher des informations détaillées lors de la veille.",
  /* DisplayRotation           */ "Orientation de l'affichage <A=Automatique G=Gaucher D=Droitier>",
  /* BoostEnabled              */ "Activer le mode \"Boost\" en maintenant le bouton de devant pendant la soudure",
  /* BoostTemperature          */ "Température du mode \"Boost\"",
  /* AutoStart                 */ "Démarrer automatiquement la soudure a l'allumage <A=Activé, V=Mode Veille, D=Désactivé>",
  /* CooldownBlink             */ "Faire clignoter la température lors du refroidissement tant que la panne est chaude.",
  /* TemperatureCalibration    */ "Etalonner température de la panne.",
  /* SettingsReset             */ "Réinitialiser tous les réglages",
  /* VoltageCalibration        */ "Etalonner tension d'entrée. Boutons pour ajuster, appui long pour quitter",
  /* AdvancedSoldering         */ "Afficher des informations détaillées pendant la soudure",
  /* ScrollingSpeed            */ "Vitesse de défilement de ce texte à",
};

const char* SettingsCalibrationWarning = "Assurez-vous que la panne soit à température ambiante avant de continuer!";
const char* SettingsResetWarning = "Voulez-vous vraiment réinitialiser les paramètres aux valeurs d'usine?";
const char* UVLOWarningString = "DC FAIBLE";
const char* UndervoltageString = "Sous-tension";
const char* InputVoltageString = "V d'entrée: ";
const char* WarningTipTempString = "Temp. Panne: ";
const char* BadTipString = "PANNE HS";
const char* SleepingSimpleString = "Zzzz";
const char* SleepingAdvancedString = "En veille...";
const char* WarningSimpleString = "HOT!";
const char* WarningAdvancedString = "ATTENTION! CHAUD";
const char* SleepingTipAdvancedString = "PANNE:";
const char* IdleTipString = "PANNE:";
const char* IdleSetString = " Set:";
const char* TipDisconnectedString = "PANNE DEBRANCH";
const char* SolderingAdvancedPowerPrompt = "Puissance: ";
const char* OffString = "Off";

const char SettingRightChar = 'D';
const char SettingLeftChar = 'G';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ { "Source", "d'alim" },
  /* SleepTemperature          */ { "Temp.", "veille" },
  /* SleepTimeout              */ { "Délai", "veille" },
  /* ShutdownTimeout           */ { "Délai", "extinction" },
  /* MotionSensitivity         */ { "Sensibilité", "au mouvement" },
  /* TemperatureUnit           */ { "Unité de", "température" },
  /* AdvancedIdle              */ { "Ecran veille", "détaillé" },
  /* DisplayRotation           */ { "Orientation", "de l'écran" },
  /* BoostEnabled              */ { "Activation du", "mode Boost" },
  /* BoostTemperature          */ { "Temp.", "Boost" },
  /* AutoStart                 */ { "Démarrage", "automatique" },
  /* CooldownBlink             */ { "Refroidir en", "clignottant" },
  /* TemperatureCalibration    */ { "Etalonner", "température" },
  /* SettingsReset             */ { "Réinitialisation", "d'usine" },
  /* VoltageCalibration        */ { "Etalonner", "tension d'entrée" },
  /* AdvancedSoldering         */ { "Ecran soudure", "détaillé" },
  /* ScrollingSpeed            */ { "Description ", "Vitesse de défilement" },
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Soudure\nParamètres",
  /* PowerSavingMenu           */ "Mode\nVeille",
  /* UIMenu                    */ "Interface\nUtilisateur",
  /* AdvancedMenu              */ "Options\nAdvanced",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Paramètres de soudage",
  /* PowerSavingMenu           */ "Paramètres d'économie d'énergie",
  /* UIMenu                    */ "Paramètres de l'interface utilisateur",
  /* AdvancedMenu              */ "Options avancées",
};

#endif

#ifdef LANG_HR
// ---- Hrvatski ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Izvor napajanja. Postavlja napon isključivanja. <DC 10V> <S 3.3V po ćeliji>",
  /* SleepTemperature          */ "Temperatura na koju se spušta lemilica nakon određenog vremena mirovanja. <C/F>",
  /* SleepTimeout              */ "Vrijeme mirovanja nakon kojega lemilica spušta temperaturu. <Minute/Sekunde>",
  /* ShutdownTimeout           */ "Vrijeme mirovanja nakon kojega će se lemilica ugasiti. <Minute>",
  /* MotionSensitivity         */ "Osjetljivost prepoznavanja pokreta. <0=Ugašeno, 1=Najmanje osjetljivo, 9=Najosjetljivije>",
  /* TemperatureUnit           */ "Jedinica temperature. <C=Celzij, F=Fahrenheit>",
  /* AdvancedIdle              */ "Prikazivanje detaljnih informacija tijekom čekanja.",
  /* DisplayRotation           */ "Orijentacija ekrana. <A=Automatski, L=Ljevoruki, D=Desnoruki>",
  /* BoostEnabled              */ "Držanjem prednjeg gumba prilikom lemljenja aktivira se pojačani (Boost) način.",
  /* BoostTemperature          */ "Temperatura u pojačanom (Boost) načinu.",
  /* AutoStart                 */ "Ako je aktivno, lemilica po uključivanju napajanja odmah počinje grijati.",
  /* CooldownBlink             */ "Bljeskanje temperature prilikom hlađenja, ako je lemilica vruća.",
  /* TemperatureCalibration    */ "Kalibriranje temperature mjeri razliku temperatura vrška i drške, dok je lemilica hladna.",
  /* SettingsReset             */ "Vraćanje svih postavki na tvorničke vrijednosti.",
  /* VoltageCalibration        */ "Kalibracija ulaznog napona. Podešavanje gumbima, dugački pritisak za kraj.",
  /* AdvancedSoldering         */ "Prikazivanje detaljnih informacija tijekom lemljenja.",
  /* ScrollingSpeed            */ "Brzina kretanja dugačkih poruka. <B=brzo, S=sporo>",
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* SettingsResetWarning = "Jeste li sigurni da želite sve postavke vratiti na tvorničke vrijednosti?";
const char* UVLOWarningString = "BATERIJA";
const char* UndervoltageString = "PRENIZAK NAPON";
const char* InputVoltageString = "Napajanje: ";
const char* WarningTipTempString = "Temp vrha: ";
const char* BadTipString = "LOŠ VRH";
const char* SleepingSimpleString = "Zzz ";
const char* SleepingAdvancedString = "SPAVANJE...";
const char* WarningSimpleString = "VRUĆ";
const char* WarningAdvancedString = "OPREZ, VRUĆE!";
const char* SleepingTipAdvancedString = "Vrh: ";
const char* IdleTipString = "Vrh: ";
const char* IdleSetString = " / ";
const char* TipDisconnectedString = "VRH NIJE SPOJEN!";
const char* SolderingAdvancedPowerPrompt = "Snaga: ";
const char* OffString = "Off";

const char SettingRightChar = 'D';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'B';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ { "Izvor", "napajanja" },
  /* SleepTemperature          */ { "Temp", "spavanja" },
  /* SleepTimeout              */ { "Vrijeme", "spavanja" },
  /* ShutdownTimeout           */ { "Vrijeme", "gašenja" },
  /* MotionSensitivity         */ { "Osjetljivost", "pokreta" },
  /* TemperatureUnit           */ { "Jedinica", "temperature" },
  /* AdvancedIdle              */ { "Detalji", "pri čekanju" },
  /* DisplayRotation           */ { "Rotacija", "ekrana" },
  /* BoostEnabled              */ { "Boost", "način" },
  /* BoostTemperature          */ { "Boost", "temp" },
  /* AutoStart                 */ { "Auto", "start" },
  /* CooldownBlink             */ { "Upozorenje", "pri hlađenju" },
  /* TemperatureCalibration    */ { "Kalibracija", "temperature" },
  /* SettingsReset             */ { "Tvorničke", "postavke" },
  /* VoltageCalibration        */ { "Kalibracija", "napona napajanja" },
  /* AdvancedSoldering         */ { "Detalji", "pri lemljenju" },
  /* ScrollingSpeed            */ { "Brzina", "poruka" },
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Postavke\nlemljenja",
  /* PowerSavingMenu           */ "Ušteda\nenergije",
  /* UIMenu                    */ "Korisničko\nsučelje",
  /* AdvancedMenu              */ "Napredne\nopcije",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Postavke pri lemljenju",
  /* PowerSavingMenu           */ "Postavke spavanja i štednje energije",
  /* UIMenu                    */ "Postavke korisničkog sučelja",
  /* AdvancedMenu              */ "Upravljanje naprednim opcijama",
};

#endif

#ifdef LANG_HU
// ---- Magyar ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Áramforrás. Beállítja a lekapcsolási feszültséget. <DC 10V> <S 3.3V cellánként>",
  /* SleepTemperature          */ "Alvási hőmérséklet <C>",
  /* SleepTimeout              */ "Elalvási időzítő <Perc/Másodperc>",
  /* ShutdownTimeout           */ "Kikapcsolási időzítő <Minutes>",
  /* MotionSensitivity         */ "Mozgás érzékenység beállítása. <0.Ki 1.kevésbé érzékeny 9.legérzékenyebb>",
  /* TemperatureUnit           */ "Hőmérsékleti egység <C=Celsius F=Fahrenheit>",
  /* AdvancedIdle              */ "Részletes információ megjelenítése kisebb betűméretben a készenléti képernyőn.",
  /* DisplayRotation           */ "Megjelenítési tájolás <A. Automatikus L. Balkezes R. Jobbkezes>",
  /* BoostEnabled              */ "Elülső gombbal lépjen boost módba, 450C forrasztás közben",
  /* BoostTemperature          */ "Hőmérséklet \"boost\" módban",
  /* AutoStart                 */ "Bekapcsolás után automatikusan lépjen forrasztás módba. T=Forrasztás, S=Alvó mód,F=Ki",
  /* CooldownBlink             */ "Villogjon a hőmérséklet hűlés közben, amíg a hegy forró.",
  /* TemperatureCalibration    */ "Hegy hőmérsékletének kalibrálása",
  /* SettingsReset             */ "Beállítások alaphelyzetbe állítása",
  /* VoltageCalibration        */ "A bemeneti feszültség kalibrálása. Röviden megnyomva állítsa be, hosszan nyomja meg a kilépéshez.",
  /* AdvancedSoldering         */ "Részletes információk megjelenítése forrasztás közben",
  /* ScrollingSpeed            */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Folytatás előtt győződj meg róla, hogy a hegy szobahőmérsékletű!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";
const char* UndervoltageString = "Undervoltage";
const char* InputVoltageString = "Input V: ";
const char* WarningTipTempString = "Tip Temp: ";
const char* BadTipString = "BAD TIP";
const char* SleepingSimpleString = "Zzzz";
const char* SleepingAdvancedString = "Alvás...";
const char* WarningSimpleString = "HOT!";
const char* WarningAdvancedString = "FIGYELEM! FORRÓ HEGY!";
const char* SleepingTipAdvancedString = "Tip:";
const char* IdleTipString = "Tip:";
const char* IdleSetString = " Set:";
const char* TipDisconnectedString = "TIP DISCONNECTED";
const char* SolderingAdvancedPowerPrompt = "Power: ";
const char* OffString = "Off";

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ "PWRSC",
  /* SleepTemperature          */ "STMP",
  /* SleepTimeout              */ "STME",
  /* ShutdownTimeout           */ "SHTME",
  /* MotionSensitivity         */ "MSENSE",
  /* TemperatureUnit           */ "TMPUNT",
  /* AdvancedIdle              */ "ADVIDL",
  /* DisplayRotation           */ "DSPROT",
  /* BoostEnabled              */ "BOOST",
  /* BoostTemperature          */ "BTMP",
  /* AutoStart                 */ "ASTART",
  /* CooldownBlink             */ "CLBLNK",
  /* TemperatureCalibration    */ "TMP CAL?",
  /* SettingsReset             */ "RESET?",
  /* VoltageCalibration        */ "CAL VIN?",
  /* AdvancedSoldering         */ "ADVSLD",
  /* ScrollingSpeed            */ "DESCSP",
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Soldering\nSettings",
  /* PowerSavingMenu           */ "Sleep\nModes",
  /* UIMenu                    */ "User\nInterface",
  /* AdvancedMenu              */ "Advanced\nOptions",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Soldering settings",
  /* PowerSavingMenu           */ "Power Saving Settings",
  /* UIMenu                    */ "User Interface settings",
  /* AdvancedMenu              */ "Advanced options",
};

#endif

#ifdef LANG_IT
// ---- Italiano ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Scegli la sorgente di alimentazione; imposta la soglia di scaricamento per alimentazione Li-Po <DC: 10V; S: 3.3V per cella>",
  /* SleepTemperature          */ "Imposta temperatura in modalità standby <°C>",
  /* SleepTimeout              */ "Imposta timer per entrare in modalità standby <minuti/secondi>",
  /* ShutdownTimeout           */ "Imposta timer per lo spegnimento <minuti>",
  /* MotionSensitivity         */ "Imposta sensibilità al movimento per uscire dalla modalità standby <0: nessuna; 1: minima; 9: massima>",
  /* TemperatureUnit           */ "Scegli l'unità di misura per la temperatura <C: grado Celsius; F: grado Farenheit>",
  /* AdvancedIdle              */ "Mostra informazioni dettagliate con un carattere più piccolo nella schermata principale",
  /* DisplayRotation           */ "Imposta orientamento del display <A: automatico; S: mano sinistra; D: mano destra>",
  /* BoostEnabled              */ "Il tasto anteriore attiva la funzione \"boost\" durante la saldatura",
  /* BoostTemperature          */ "Imposta la temperatura in funzione \"boost\"",
  /* AutoStart                 */ "Attiva automaticamente il saldatore quando viene alimentato <A: saldatura; S: standby; D: disattiva>",
  /* CooldownBlink             */ "Mostra la temperatura durante il raffreddamento se la punta è ancora calda",
  /* TemperatureCalibration    */ "Calibra la differenza di temperatura rilevata da quella presente sulla punta",
  /* SettingsReset             */ "Ripristina tutte le impostazioni",
  /* VoltageCalibration        */ "Calibra la tensione in ingresso; regola con i bottoni, tieni premuto per uscire",
  /* AdvancedSoldering         */ "Mostra informazioni dettagliate in modalità saldatura",
  /* ScrollingSpeed            */ "Imposta la velocità di scorrimento del testo <L: lento; V: veloce>",
};

const char* SettingsCalibrationWarning = "Assicurati che la punta si trovi a temperatura ambiente prima di continuare!";
const char* SettingsResetWarning = "Ripristinare le impostazioni iniziali?";
const char* UVLOWarningString = "DC BASSA";
const char* UndervoltageString = "DC INSUFFICIENTE";
const char* InputVoltageString = "V ingresso:";
const char* WarningTipTempString = "Temp punta:";
const char* BadTipString = "PUNTA NO";
const char* SleepingSimpleString = "Zzz ";
const char* SleepingAdvancedString = "Standby";
const char* WarningSimpleString = "HOT!";
const char* WarningAdvancedString = "PUNTA CALDA!";
const char* SleepingTipAdvancedString = "Punta:";
const char* IdleTipString = "Punta:";
const char* IdleSetString = " Im:";
const char* TipDisconnectedString = "PUNTA ASSENTE";
const char* SolderingAdvancedPowerPrompt = "Potenza:";
const char* OffString = "OFF";

const char SettingRightChar = 'D';
const char SettingLeftChar = 'S';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'V';
const char SettingSlowChar = 'L';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ { "Sorgente", "alimentaz" },
  /* SleepTemperature          */ { "Temp", "standby" },
  /* SleepTimeout              */ { "Timer", "standby" },
  /* ShutdownTimeout           */ { "Timer", "spegnimento" },
  /* MotionSensitivity         */ { "Sensibilità", "al movimento" },
  /* TemperatureUnit           */ { "Unità di", "temperatura" },
  /* AdvancedIdle              */ { "Mostra", "dettagli" },
  /* DisplayRotation           */ { "Orientamento", "display" },
  /* BoostEnabled              */ { "Funzione", "\"boost\"" },
  /* BoostTemperature          */ { "Temp", "\"boost\"" },
  /* AutoStart                 */ { "Avvio", "automatico" },
  /* CooldownBlink             */ { "Avviso", "punta calda" },
  /* TemperatureCalibration    */ { "Calibrazione", "temperatura" },
  /* SettingsReset             */ { "Ripristino", "impostazioni" },
  /* VoltageCalibration        */ { "Calibrazione", "tensione" },
  /* AdvancedSoldering         */ { "Dettagli", "saldatura" },
  /* ScrollingSpeed            */ { "Velocità", "testo" },
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Opzioni\nsaldatura",
  /* PowerSavingMenu           */ "Risparmio\nenergetico",
  /* UIMenu                    */ "Interfaccia\nutente",
  /* AdvancedMenu              */ "Opzioni\navanzate",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Menù impostazioni saldatura",
  /* PowerSavingMenu           */ "Menù risparmio energetico",
  /* UIMenu                    */ "Menù interfaccia utente",
  /* AdvancedMenu              */ "Menù impostazioni avanzate",
};

#endif

#ifdef LANG_LT
// ---- Lietuvių ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Išjungimo įtampa. <DC 10V arba celių (S) kiekis (3.3V per celę)>",
  /* SleepTemperature          */ "Miego temperatūra <C>",
  /* SleepTimeout              */ "Miego laikas <minutės/sekundės>",
  /* ShutdownTimeout           */ "Išjungimo laikas <minutės>",
  /* MotionSensitivity         */ "Jautrumas judesiui <0 - išjungta, 1 - mažiausias, 9 - didžiausias>",
  /* TemperatureUnit           */ "Temperatūros vienetai <C - Celsijus, F - Farenheitas>",
  /* AdvancedIdle              */ "Ar rodyti papildomą informaciją mažesniu šriftu laukimo ekrane",
  /* DisplayRotation           */ "Ekrano orientacija <A - automatinė, K - kairiarankiams, D - dešiniarankiams>",
  /* BoostEnabled              */ "Ar lituojant viršutinis mygtukas įjungia turbo režimą",
  /* BoostTemperature          */ "Temperatūra turbo režimu",
  /* AutoStart                 */ "Ar pradėti kaitininti iš karto įjungus lituoklį",
  /* CooldownBlink             */ "Ar mirksėti temperatūrą ekrane kol vėstantis antgalis vis dar karštas",
  /* TemperatureCalibration    */ "Antgalio temperatūros kalibravimas",
  /* SettingsReset             */ "Nustatyti nustatymus iš naujo",
  /* VoltageCalibration        */ "Įvesties įtampos kalibravimas. Trumpai paspauskite, norėdami nustatyti, ilgai paspauskite, kad išeitumėte",
  /* AdvancedSoldering         */ "Ar rodyti išsamią informaciją lituojant",
  /* ScrollingSpeed            */ "Greitis, kuriuo šis tekstas slenka",
};

const char* SettingsCalibrationWarning = "Prieš tęsdami įsitikinkite, kad antgalis yra kambario temperatūros!";
const char* SettingsResetWarning = "Ar norite atstatyti nustatymus į numatytas reikšmes?";
const char* UVLOWarningString = "MAŽ VOLT";
const char* UndervoltageString = "Žema įtampa";
const char* InputVoltageString = "Įvestis V: ";
const char* WarningTipTempString = "Antgl Temp: ";
const char* BadTipString = "BLOG ANT";
const char* SleepingSimpleString = "Zzzz";
const char* SleepingAdvancedString = "Miegu...";
const char* WarningSimpleString = "HOT!";
const char* WarningAdvancedString = "ANTGALIS KARŠTAS";
const char* SleepingTipAdvancedString = "Antgl:";
const char* IdleTipString = "Ant:";
const char* IdleSetString = " Nust:";
const char* TipDisconnectedString = "ANTGAL ATJUNGTAS";
const char* SolderingAdvancedPowerPrompt = "Maitinimas: ";
const char* OffString = "Išj";

const char SettingRightChar = 'D';
const char SettingLeftChar = 'K';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'T';
const char SettingSlowChar = 'N';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_DOUBLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ { "Maitinimo", "šaltinis" },
  /* SleepTemperature          */ { "Miego", "temperat." },
  /* SleepTimeout              */ { "Miego", "laikas" },
  /* ShutdownTimeout           */ { "Išjungimo", "laikas" },
  /* MotionSensitivity         */ { "Jautrumas", "judesiui" },
  /* TemperatureUnit           */ { "Temperatūros", "vienetai" },
  /* AdvancedIdle              */ { "Detalus lauki", "mo ekranas" },
  /* DisplayRotation           */ { "Ekrano", "orientacija" },
  /* BoostEnabled              */ { "Turbo režimas", "įjungtas" },
  /* BoostTemperature          */ { "Turbo", "temperat." },
  /* AutoStart                 */ { "Auto", "paleidimas" },
  /* CooldownBlink             */ { "Atvėsimo", "mirksėjimas" },
  /* TemperatureCalibration    */ { "Kalibruoti", "temperatūrą?" },
  /* SettingsReset             */ { "Atstatyti", "nustatymus?" },
  /* VoltageCalibration        */ { "Kalibruoti", "įvesties įtampą?" },
  /* AdvancedSoldering         */ { "Detalus lita-", "vimo ekranas" },
  /* ScrollingSpeed            */ { "Greitas apr", "ašym. slink" },
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Litavimo\nnustatymai",
  /* PowerSavingMenu           */ "Miego\nrežimai",
  /* UIMenu                    */ "Naudotojo\nsąsaja",
  /* AdvancedMenu              */ "Išplėstin.\nnustatymai",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Litavimo nustatymai",
  /* PowerSavingMenu           */ "Energijos vartojimo nustatymai",
  /* UIMenu                    */ "Naudotojo sąsajos nustatymai",
  /* AdvancedMenu              */ "Išplėstiniai nustatymai",
};

#endif

#ifdef LANG_PL
// ---- Polski ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Źródło zasilania. Ustaw napięcie odcięcia. <DC 10V> <S 3.3V dla ogniw Li>",
  /* SleepTemperature          */ "Temperatura uśpienia <°C>",
  /* SleepTimeout              */ "Czas uśpienia <Minuty/Sekundy>",
  /* ShutdownTimeout           */ "Czas wyłączenia <Minuty>",
  /* MotionSensitivity         */ "Czułość ruchu <0.Wyłączona 1.minimalna 9.maksymalna>",
  /* TemperatureUnit           */ "Jednostka temperatury <C=Celsius F=Fahrenheit>",
  /* AdvancedIdle              */ "Wyświetla szczegółowe informacje za pomocą mniejszej czcionki na ekranie bezczynnośći <T = wł., N = wył.>",
  /* DisplayRotation           */ "Orientacja wyświetlacza <A. Automatyczna L. Leworęczna P. Praworęczna>",
  /* BoostEnabled              */ "Użyj przycisku przedniego w celu zwiększenia temperatury <T = wł., N = wył.>",
  /* BoostTemperature          */ "Temperatura w trybie \"boost\" ",
  /* AutoStart                 */ "Automatyczne uruchamianie trybu lutowania po włączeniu zasilania. T=Lutowanie, S= Tryb Uspienia ,N=Wyłącz",
  /* CooldownBlink             */ "Temperatura na ekranie miga, gdy grot jest jeszcze gorący. <T = wł., N = wył.>",
  /* TemperatureCalibration    */ "Kalibracja temperatury grota lutownicy",
  /* SettingsReset             */ "Zresetuj wszystkie ustawienia",
  /* VoltageCalibration        */ "Kalibracja napięcia wejściowego. Krótkie naciśnięcie, aby ustawić, długie naciśnięcie, aby wyjść.",
  /* AdvancedSoldering         */ "Wyświetl szczegółowe informacje podczas lutowania <T = wł., N = wył.>",
  /* ScrollingSpeed            */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Przed kontynuowaniem upewnij się, że końcówka osiągnela temperature pokojowa!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";
const char* UndervoltageString = "Undervoltage";
const char* InputVoltageString = "Input V: ";
const char* WarningTipTempString = "Tip Temp: ";
const char* BadTipString = "BAD TIP";
const char* SleepingSimpleString = "Zzz!";
const char* SleepingAdvancedString = "Uspienie...";
const char* WarningSimpleString = "HOT!";
const char* WarningAdvancedString = "UWAGA! GORĄCA KOŃCÓWKA!";
const char* SleepingTipAdvancedString = "Tip:";
const char* IdleTipString = "Tip:";
const char* IdleSetString = " Set:";
const char* TipDisconnectedString = "TIP DISCONNECTED";
const char* SolderingAdvancedPowerPrompt = "Power: ";
const char* OffString = "Off";

const char SettingRightChar = 'P';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ "PWRSC",
  /* SleepTemperature          */ "STMP",
  /* SleepTimeout              */ "STME",
  /* ShutdownTimeout           */ "SHTME",
  /* MotionSensitivity         */ "MSENSE",
  /* TemperatureUnit           */ "TMPUNT",
  /* AdvancedIdle              */ "ADVIDL",
  /* DisplayRotation           */ "DSPROT",
  /* BoostEnabled              */ "BOOST",
  /* BoostTemperature          */ "BTMP",
  /* AutoStart                 */ "ASTART",
  /* CooldownBlink             */ "CLBLNK",
  /* TemperatureCalibration    */ "TMP CAL?",
  /* SettingsReset             */ "RESET?",
  /* VoltageCalibration        */ "CAL VIN?",
  /* AdvancedSoldering         */ "ADVSLD",
  /* ScrollingSpeed            */ "DESCSP",
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Soldering\nSettings",
  /* PowerSavingMenu           */ "Sleep\nModes",
  /* UIMenu                    */ "User\nInterface",
  /* AdvancedMenu              */ "Advanced\nOptions",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Soldering settings",
  /* PowerSavingMenu           */ "Power Saving Settings",
  /* UIMenu                    */ "User Interface settings",
  /* AdvancedMenu              */ "Advanced options",
};

#endif

#ifdef LANG_PT
// ---- Portugues ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Fonte de alimentação. Define a tensão de corte. <DC 10V> <S 3.3V por célula>",
  /* SleepTemperature          */ "Temperatura de repouso <C>",
  /* SleepTimeout              */ "Tempo para repouso <Minutos/Segundos>",
  /* ShutdownTimeout           */ "Tempo para desligamento <Minutos>",
  /* MotionSensitivity         */ "Sensibilidade ao movimento <0=Desligado 1=Menor 9=Maior>",
  /* TemperatureUnit           */ "Unidade de temperatura <C=Celsius F=Fahrenheit>",
  /* AdvancedIdle              */ "Exibe informações avançadas quando ocioso",
  /* DisplayRotation           */ "Orientação da tela <A.utomática C.anhoto D.estro>",
  /* BoostEnabled              */ "Tecla frontal ativa modo \"turbo\"",
  /* BoostTemperature          */ "Ajuste de temperatura do modo \"turbo\"",
  /* AutoStart                 */ "Temperatura de aquecimento ao ligar <T=Trabalho S=Repouso F=Desligado>",
  /* CooldownBlink             */ "Exibe a temperatura durante o resfriamento",
  /* TemperatureCalibration    */ "Calibra a temperatura",
  /* SettingsReset             */ "Reverte todos ajustes",
  /* VoltageCalibration        */ "Calibra a tensão e configura os botões. Mantenha presionado para sair",
  /* AdvancedSoldering         */ "Exibe informações avançadas durante o uso",
  /* ScrollingSpeed            */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "A ponta deve estar em temperatura ambiente antes de continuar!";
const char* SettingsResetWarning = "Reverter para os ajustes de fábrica?";
const char* UVLOWarningString = "DC BAIXO";
const char* UndervoltageString = "Subtensão";
const char* InputVoltageString = "Tensão ";
const char* WarningTipTempString = "Temperatura ";
const char* BadTipString = "ER PONTA";
const char* SleepingSimpleString = "Zzzz";
const char* SleepingAdvancedString = "Repouso...";
const char* WarningSimpleString = "TEMP";
const char* WarningAdvancedString = "TEMP ELEVADA!";
const char* SleepingTipAdvancedString = "Ponta:";
const char* IdleTipString = "Ponta:";
const char* IdleSetString = " Aj:";
const char* TipDisconnectedString = "SEM PONTA";
const char* SolderingAdvancedPowerPrompt = "Power: ";
const char* OffString = "Off";

const char SettingRightChar = 'D';
const char SettingLeftChar = 'C';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ "FONTE",
  /* SleepTemperature          */ "TMPE",
  /* SleepTimeout              */ "TMPO",
  /* ShutdownTimeout           */ "DESLI",
  /* MotionSensitivity         */ "MOVIME",
  /* TemperatureUnit           */ "UNIDAD",
  /* AdvancedIdle              */ "OCIOSO",
  /* DisplayRotation           */ "ORIENT",
  /* BoostEnabled              */ "TURBO",
  /* BoostTemperature          */ "TTMP",
  /* AutoStart                 */ "MODOAT",
  /* CooldownBlink             */ "RESFRI",
  /* TemperatureCalibration    */ "CAL.TEMP",
  /* SettingsReset             */ "RESETAR",
  /* VoltageCalibration        */ "CAL.VOLT",
  /* AdvancedSoldering         */ "AVNCAD",
  /* ScrollingSpeed            */ "DESCSP",
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Soldering\nSettings",
  /* PowerSavingMenu           */ "Sleep\nModes",
  /* UIMenu                    */ "User\nInterface",
  /* AdvancedMenu              */ "Advanced\nOptions",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Soldering settings",
  /* PowerSavingMenu           */ "Power Saving Settings",
  /* UIMenu                    */ "User Interface settings",
  /* AdvancedMenu              */ "Advanced options",
};

#endif

#ifdef LANG_RU
// ---- Русский ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Источник питания. Установка напряжения отключения. <DC 10V> <S 3.3 V на батарею>",
  /* SleepTemperature          */ "Температура режима ожидания <С>",
  /* SleepTimeout              */ "Время до перехода в режим ожидания <Минуты>",
  /* ShutdownTimeout           */ "Время до отключения <Минуты>",
  /* MotionSensitivity         */ "Акселерометр <0. Выкл. 1. мин. чувствительный 9. макс. чувствительный>",
  /* TemperatureUnit           */ "В чем измерять температуру",
  /* AdvancedIdle              */ "Показывать детальную информацию маленьким шрифтом на домашнем экране",
  /* DisplayRotation           */ "Ориентация дисплея <A. Автоматический, Л. Левая рука, П. Правая рука>",
  /* BoostEnabled              */ "Турбо-режим при удержании кнопки А при пайке ",
  /* BoostTemperature          */ "Температура в турбо-режиме",
  /* AutoStart                 */ "Автоматический запуск паяльника при включении питания. T=Нагрев, S=Режим ожидания,F=Выкл.",
  /* CooldownBlink             */ "Показывать температуру на экране охлаждения, пока жало остается горячим.",
  /* TemperatureCalibration    */ "Калибровка термодатчика.",
  /* SettingsReset             */ "Сброс всех настроек.",
  /* VoltageCalibration        */ "Калибровка напряжения входа. Настройка кнопками, нажать и удержать чтобы завершить.",
  /* AdvancedSoldering         */ "Показывать детальную информацию при пайке.",
  /* ScrollingSpeed            */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Убедитесь, что жало остыло до комнатной температуры, прежде чем продолжать!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "БАТ РАЗР";
const char* UndervoltageString = "Undervoltage";
const char* InputVoltageString = "Input V: ";
const char* WarningTipTempString = "Tip Temp: ";
const char* BadTipString = "BAD TIP";
const char* SleepingSimpleString = "Хррр";
const char* SleepingAdvancedString = "Ожидание...";
const char* WarningSimpleString = " АЙ!";
const char* WarningAdvancedString = "!!! ГОРЯЧО !!!";
const char* SleepingTipAdvancedString = "Tip:";
const char* IdleTipString = "Tip:";
const char* IdleSetString = " Set:";
const char* TipDisconnectedString = "TIP DISCONNECTED";
const char* SolderingAdvancedPowerPrompt = "Power: ";
const char* OffString = "Off";

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ "ИстП",
  /* SleepTemperature          */ "Тожд",
  /* SleepTimeout              */ "Вожд",
  /* ShutdownTimeout           */ "Тоткл",
  /* MotionSensitivity         */ "ЧувсДв",
  /* TemperatureUnit           */ "ЕдТемп",
  /* AdvancedIdle              */ "ИнфОжд",
  /* DisplayRotation           */ "ПовЭкр",
  /* BoostEnabled              */ "Турбо",
  /* BoostTemperature          */ "Ттур",
  /* AutoStart                 */ "Астарт",
  /* CooldownBlink             */ "Охлажд",
  /* TemperatureCalibration    */ "КалибрТ",
  /* SettingsReset             */ "СБРОС?",
  /* VoltageCalibration        */ "КалибрU?",
  /* AdvancedSoldering         */ "ИнфПай",
  /* ScrollingSpeed            */ "DESCSP",
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Soldering\nSettings",
  /* PowerSavingMenu           */ "Sleep\nModes",
  /* UIMenu                    */ "User\nInterface",
  /* AdvancedMenu              */ "Advanced\nOptions",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Soldering settings",
  /* PowerSavingMenu           */ "Power Saving Settings",
  /* UIMenu                    */ "User Interface settings",
  /* AdvancedMenu              */ "Advanced options",
};

#endif

#ifdef LANG_SE
// ---- SE ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Источник питания. Установка напряжения отключения. <DC 10V> <S 3.3 V на батарею>",
  /* SleepTemperature          */ "Температура Сна  <С>",
  /* SleepTimeout              */ "Переход в режим Сна <Минуты>",
  /* ShutdownTimeout           */ "Переходит в режим ожидания <Минуты>",
  /* MotionSensitivity         */ "Акселерометр <0. Выкл. 1. мин. чувствительный 9. макс. чувствительный>",
  /* TemperatureUnit           */ "В чем измерять температуру",
  /* AdvancedIdle              */ "Display detailed information in a smaller font on the idle screen.",
  /* DisplayRotation           */ "Ориентация Дисплея <A. Автоматический L. Левая Рука R. Правая Рука>",
  /* BoostEnabled              */ "Активация  кнопки A для Турбо режима до  450С при пайке ",
  /* BoostTemperature          */ "Установка температуры для Турбо режима",
  /* AutoStart                 */ "Автоматический запуск паяльника при включении питания. T=Нагрев, S= Режим Сна,F=Выкл.",
  /* CooldownBlink             */ "Мигает температура на экране охлаждения, пока жало остается горячим.",
  /* TemperatureCalibration    */ "Calibrate tip offset.",
  /* SettingsReset             */ "Reset all settings",
  /* VoltageCalibration        */ "VIN Calibration. Buttons adjust, long press to exit",
  /* AdvancedSoldering         */ "Display detailed information while soldering",
  /* ScrollingSpeed            */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";
const char* UndervoltageString = "Undervoltage";
const char* InputVoltageString = "Input V: ";
const char* WarningTipTempString = "Tip Temp: ";
const char* BadTipString = "BAD TIP";
const char* SleepingSimpleString = "Zzzz";
const char* SleepingAdvancedString = "Sleeping...";
const char* WarningSimpleString = "HOT!";
const char* WarningAdvancedString = "WARNING! TIP HOT!";
const char* SleepingTipAdvancedString = "Tip:";
const char* IdleTipString = "Tip:";
const char* IdleSetString = " Set:";
const char* TipDisconnectedString = "TIP DISCONNECTED";
const char* SolderingAdvancedPowerPrompt = "Power: ";
const char* OffString = "Off";

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ "PWRSC",
  /* SleepTemperature          */ "STMP",
  /* SleepTimeout              */ "STME",
  /* ShutdownTimeout           */ "SHTME",
  /* MotionSensitivity         */ "MSENSE",
  /* TemperatureUnit           */ "TMPUNT",
  /* AdvancedIdle              */ "ADVIDL",
  /* DisplayRotation           */ "DSPROT",
  /* BoostEnabled              */ "BOOST",
  /* BoostTemperature          */ "BTMP",
  /* AutoStart                 */ "ASTART",
  /* CooldownBlink             */ "CLBLNK",
  /* TemperatureCalibration    */ "TMP CAL?",
  /* SettingsReset             */ "RESET?",
  /* VoltageCalibration        */ "CAL VIN?",
  /* AdvancedSoldering         */ "ADVSLD",
  /* ScrollingSpeed            */ "DESCSP",
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Soldering\nSettings",
  /* PowerSavingMenu           */ "Sleep\nModes",
  /* UIMenu                    */ "User\nInterface",
  /* AdvancedMenu              */ "Advanced\nOptions",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Soldering settings",
  /* PowerSavingMenu           */ "Power Saving Settings",
  /* UIMenu                    */ "User Interface settings",
  /* AdvancedMenu              */ "Advanced options",
};

#endif

#ifdef LANG_SK
// ---- SK ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Zdroj napatia. Nastavit napatie pre vypnutie (cutoff)  <DC=10V, nS=n*3.3V pre LiIon clanky>",
  /* SleepTemperature          */ "Kludova teplota (v nastavenych jednotkach)",
  /* SleepTimeout              */ "Kludovy rezim po <sekundach/minutach>",
  /* ShutdownTimeout           */ "Cas na vypnutie <minuty>",
  /* MotionSensitivity         */ "Citlivost detekcie pohybu <0=Vyp, 1=Min ... 9=Max>",
  /* TemperatureUnit           */ "Jednotky merania teploty <C=stupne Celzia, F=stupne Fahrenheita>",
  /* AdvancedIdle              */ "Zobrazit detailne informacie v kludovom rezime <T=Zap, F=Vyp>",
  /* DisplayRotation           */ "Orientacia displeja <A=Auto, L=Lavak, R=Pravak>",
  /* BoostEnabled              */ "Povolit tlacidlo pre prudky nahrev <T=Zap, F=Vyp>",
  /* BoostTemperature          */ "Cielova teplota pre prudky nahrev (v nastavenych jednotkach)",
  /* AutoStart                 */ "Pri starte spustit rezim spajkovania <T=Zap, F=Vyp, S=Spanok>",
  /* CooldownBlink             */ "Blikanie ukazovatela teploty pocas chladnutia hrotu <T=Zap, F=Vyp>",
  /* TemperatureCalibration    */ "Kalibracia posunu hrotu",
  /* SettingsReset             */ "Tovarenske nastavenia",
  /* VoltageCalibration        */ "Kalibracia VIN. Kratke stlacenie meni nastavenie, dlhe stlacenie pre navrat",
  /* AdvancedSoldering         */ "Zobrazenie detailov pocas spajkovania <T=Zap, F=Vyp>",
  /* ScrollingSpeed            */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Najprv sa prosim uistite, ze hrot ma izbovu teplotu!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";
const char* UndervoltageString = "Undervoltage";
const char* InputVoltageString = "Input V: ";
const char* WarningTipTempString = "Tip Temp: ";
const char* BadTipString = "BAD TIP";
const char* SleepingSimpleString = "Chrr";
const char* SleepingAdvancedString = "Kludovy rezim...";
const char* WarningSimpleString = "HOT!";
const char* WarningAdvancedString = "HROT JE HORUCI !";
const char* SleepingTipAdvancedString = "Tip:";
const char* IdleTipString = "Tip:";
const char* IdleSetString = " Set:";
const char* TipDisconnectedString = "TIP DISCONNECTED";
const char* SolderingAdvancedPowerPrompt = "Power: ";
const char* OffString = "Off";

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ "PWRSC",
  /* SleepTemperature          */ "STMP",
  /* SleepTimeout              */ "STME",
  /* ShutdownTimeout           */ "SHTME",
  /* MotionSensitivity         */ "MSENSE",
  /* TemperatureUnit           */ "TMPUNT",
  /* AdvancedIdle              */ "ADVIDL",
  /* DisplayRotation           */ "DSPROT",
  /* BoostEnabled              */ "BOOST",
  /* BoostTemperature          */ "BTMP",
  /* AutoStart                 */ "ASTART",
  /* CooldownBlink             */ "CLBLNK",
  /* TemperatureCalibration    */ "TMP CAL?",
  /* SettingsReset             */ "RESET?",
  /* VoltageCalibration        */ "CAL VIN?",
  /* AdvancedSoldering         */ "ADVSLD",
  /* ScrollingSpeed            */ "DESCSP",
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Soldering\nSettings",
  /* PowerSavingMenu           */ "Sleep\nModes",
  /* UIMenu                    */ "User\nInterface",
  /* AdvancedMenu              */ "Advanced\nOptions",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Soldering settings",
  /* PowerSavingMenu           */ "Power Saving Settings",
  /* UIMenu                    */ "User Interface settings",
  /* AdvancedMenu              */ "Advanced options",
};

#endif

#ifdef LANG_TR
// ---- Türk ----

const char* SettingsDescriptions[17] = {
  /* PowerSource               */ "Güç Kaynağı. kesim geriliminı ayarlar. <DC 10V> <S 3.3V hücre başına>",
  /* SleepTemperature          */ "Uyku Sıcaklığı <C>",
  /* SleepTimeout              */ "Uyku Zaman Aşımı <Dakika/Saniye>",
  /* ShutdownTimeout           */ "Kapatma Zaman Aşımı <Dakika>",
  /* MotionSensitivity         */ "Hareket Hassasiyeti <0.Kapalı 1.En az duyarlı 9.En duyarlı>",
  /* TemperatureUnit           */ "Sıcaklık Ünitesi <C=Celsius F=Fahrenheit>",
  /* AdvancedIdle              */ "Boş ekranda ayrıntılı bilgileri daha küçük bir yazı tipi ile göster.",
  /* DisplayRotation           */ "Görüntü Yönlendirme <A. Otomatik L. Solak R. Sağlak>",
  /* BoostEnabled              */ "Lehimleme yaparken ön tuşa basmak Boost moduna sokar(450C)",
  /* BoostTemperature          */ "\"boost\" Modu Derecesi",
  /* AutoStart                 */ "Güç verildiğinde otomatik olarak lehimleme modunda başlat. T=Lehimleme Modu, S= Uyku Modu,F=Kapalı",
  /* CooldownBlink             */ "Soğutma ekranında uç hala sıcakken derece yanıp sönsün.",
  /* TemperatureCalibration    */ "Ucu kalibre et.",
  /* SettingsReset             */ "Bütün ayarları sıfırla",
  /* VoltageCalibration        */ "VIN Kalibrasyonu. Düğmeler ayarlar, çıkmak için uzun bas.",
  /* AdvancedSoldering         */ "Lehimleme yaparken detaylı bilgi göster",
  /* ScrollingSpeed            */ "Speed this text scrolls past at",
};

const char* SettingsCalibrationWarning = "Lütfen devam etmeden önce ucun oda sıcaklığında olduğunu garantiye alın!";
const char* SettingsResetWarning = "Are you sure to reset settings to default values?";
const char* UVLOWarningString = "DC LOW";
const char* UndervoltageString = "Undervoltage";
const char* InputVoltageString = "Input V: ";
const char* WarningTipTempString = "Tip Temp: ";
const char* BadTipString = "BAD TIP";
const char* SleepingSimpleString = "Zzzz";
const char* SleepingAdvancedString = "Uyuyor...";
const char* WarningSimpleString = "HOT!";
const char* WarningAdvancedString = "UYARI! UÇ SICAK!";
const char* SleepingTipAdvancedString = "Tip:";
const char* IdleTipString = "Tip:";
const char* IdleSetString = " Set:";
const char* TipDisconnectedString = "TIP DISCONNECTED";
const char* SolderingAdvancedPowerPrompt = "Power: ";
const char* OffString = "Off";

const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingFastChar = 'F';
const char SettingSlowChar = 'S';

const enum ShortNameType SettingsShortNameType = SHORT_NAME_SINGLE_LINE;
const char* SettingsShortNames[17][2] = {
  /* PowerSource               */ "PWRSC",
  /* SleepTemperature          */ "STMP",
  /* SleepTimeout              */ "STME",
  /* ShutdownTimeout           */ "SHTME",
  /* MotionSensitivity         */ "MSENSE",
  /* TemperatureUnit           */ "TMPUNT",
  /* AdvancedIdle              */ "ADVIDL",
  /* DisplayRotation           */ "DSPROT",
  /* BoostEnabled              */ "BOOST",
  /* BoostTemperature          */ "BTMP",
  /* AutoStart                 */ "ASTART",
  /* CooldownBlink             */ "CLBLNK",
  /* TemperatureCalibration    */ "TMP CAL?",
  /* SettingsReset             */ "RESET?",
  /* VoltageCalibration        */ "CAL VIN?",
  /* AdvancedSoldering         */ "ADVSLD",
  /* ScrollingSpeed            */ "DESCSP",
};

const char* SettingsMenuEntries[4] = {
  /* SolderingMenu             */ "Soldering\nSettings",
  /* PowerSavingMenu           */ "Sleep\nModes",
  /* UIMenu                    */ "User\nInterface",
  /* AdvancedMenu              */ "Advanced\nOptions",
};

const char* SettingsMenuEntriesDescriptions[4] = {
  /* SolderingMenu             */ "Soldering settings",
  /* PowerSavingMenu           */ "Power Saving Settings",
  /* UIMenu                    */ "User Interface settings",
  /* AdvancedMenu              */ "Advanced options",
};

#endif
