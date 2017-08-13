/*
 * Strings.c
 *
 *  Created on: 5Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Strings.h"
#define LANG_EN
#ifdef LANG_EN
const char* SettingsLongNames[14] =
		{
		/*These are all the help text for all the settings.*/
		/*All must start with 6 spaces so they come on screen nicely.*/
		"      Power source. Sets cutoff voltage. <DC 10V> <S 3.3V per cell>",
				"      Sleep Temperature <C>", "      Sleep Timeout <Minutes>",
				"      Shutdown Timeout <Minutes>",
				"      Motion Sensitivity <0.Off 1.least sensitive 9.most sensitive>",
				"      Temperature Unit", "      Temperature Rounding Amount",
				"      Temperature Display Update Rate",
				"      Display Orientation <A. Automatic L. Left Handed R. Right Handed>",
				"      Enable front key enters boost mode 450C mode when soldering",
				"      Temperature when in \"boost\" mode",
				"      Changes the arrows to a power display when soldering",
				"      Automatically starts the iron into soldering on power up. T=Soldering, S= Sleep mode,F=Off",
				"      Blink the temperature on the cooling screen while the tip is still hot." };

const char* TempCalStatus[3] = { "Cal Temp", "Cal OK  ", "Cal Fail" }; //All fixed 8 chars
const char* UVLOWarningString = "Low Volt"; //Fixed width 8 chars
const char* CoolingPromptString = "Off  "; //Fixed width 5 chars
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
#ifdef LANG_ES
const char* SettingsLongNames[14] =
{
	/*These are all the help text for all the settings.*/
	/*All must start with 6 spaces so they come on screen nicely.*/
	"      Fuente de energía. Ajusta el límite inferior de voltaje. <DC 10V> <S 3.3V por celda>",
	"      Temperatura en reposo. <C>",
	"      Tiempo hasta activar reposo. <Minutos>",
	"      Tiempo hasta apagado. <Minutos>",
	"      Sensibilidad del movimiento. <0. Apagado, 1. El menos sensible, 9. El más sensible>",
	"      Unidad de temperatura.",
	"      Redondeo de la temperatura.",
	"      Tasa de actualización de la temperatura.",
	"      Orientación de la pantalla <A. Automático, I. Mano izquierda, D. Mano derecha>",
	"      Activar el botón \"Boost\" en modo soldadura.",
	"      Temperatura en modo \"Boost\".",
	"      Cambiar las flechas en pantalla por indicador de potencia en modo soldadura.",
	"      Iniciar modo soldadura en el encendido. <V. Sí, S. Modo reposo, F. No ",
	"      Parpadea la temperatura en el enfriamiento si la punta sigue caliente."};

const char* TempCalStatus[3] = {"CAL TEMP", "CAL OK  ", "CAL FAIL"}; //All fixed 8 chars
const char* UVLOWarningString = "LOW VOLT";//Fixed width 8 chars
const char* CoolingPromptString = "Cool ";//Fixed width 5 chars
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
const char* SettingsLongNames[14] =
{
	/*These are all the help text for all the settings.*/
	/*All must start with 6 spaces so they come on screen nicely.*/
	"      Stromversorgung. Setzt Abschaltspannung <DC 10V> <S 3.3V per cell>",
	"      Ruhetemperatur <C>", "      Ruhemodus nach <Minuten>",
	"      Abschaltzeit <Minuten>",
	"      Bewegungsempfindlichkeit <0.Aus 1.Minimal 9.Maximal>",
	"      Temperatur Einheit", "      Temperatur Runden",
	"      Temperaturanzeige Updaterate",
	"      Anzeigerichtung <A. Automatisch L. Linkshändig R. Rechtshändig>",
	"      Fronttaste für Temperaturboost einschalten",
	"      Temperatur im \"boost\"-Modus",
	"      Ändert \"Temperaturpfeile\" in Leistungsbalken",
	"      Automatischer Start beim Einschalten. J=Löttemp., R=Ruhemodus,N=Aus",
	"      Temperatur blinkt beim Abkühlen, solange noch heiß."};

const char* TempCalStatus[3] = {"Cal Temp", "Cal OK  ", "Cal Fehl"}; //All fixed 8 chars
const char* UVLOWarningString = "V gering";//Fixed width 8 chars
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
const char* SettingsShortNames[14] = { "PWRSC ", "STMP ", "STME ", "SHTME ",
		"MSENSE ", "TMPUNT ", "TMPRND ", "TMPSPD ", "DSPROT ", "BOOST  ",
		"BTMP ", "PWRDSP ", "ASTART ", "CLBLNK " };
