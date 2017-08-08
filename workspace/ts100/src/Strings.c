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
				"      Enable front key boost 450C mode when soldering",
				"      Temperature when in boost mode",
				"      Changes the arrows to a power display when soldering",
				"      Automatically starts the iron into soldering on power up.",
				"      Blink the temperature on the cooling screen while the tip is still hot." };

const char* TempCalStatus[3] = { "Cal Temp", "Cal OK  ", "Cal Fail" }; //All fixed 8 chars
const char* UVLOWarningString = "Low Volt"; //Fixed width 8 chars
const char* CoolingPromptString = "COOL "; //Fixed width 5 chars
const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingFastChar = 'F';
const char SettingMediumChar = 'F';
const char SettingSlowChar = 'F';
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
	"      Fuente de energia. Ajusta el limite inferior de voltaje. <DC 10V> <S 3.3V por celda>",
	"      Temperatura en reposo. <C>", "      Tiempo hasta activar reposo. <Minutos>",
	"      Tiempo hasta apagado. <Minutos>",
	"      Sensibilidad del movimiento. <0.Apagado 1.El menos sensible 9.El mas sensible>",
	"      Unidad de temperatura.", "      Cantidad de redondeo de la temperatura.",
	"      Tasa de actualización de la temperatura.",
	"      Orientacion de la pantalla <A. Automatico L. Mano izquierda R. Mano derecha>",
	"      Activar el boton <Boost> en modo soldadura.",
	"      Temperatura en modo <Boost>.",
	"      Cambiar las flechas en pantalla por indicador de potencia en modo soldadura.",
	"      Iniciar directamente modo soldadura en el encendido.",
	"      Blink the temperature on the cooling screen while the tip is still hot."};/*Needs translation*/

const char* TempCalStatus[3] = {"CAL TEMP", "CAL OK  ", "CAL FAIL"}; //All fixed 8 chars
const char* UVLOWarningString = "LOW VOLT";//Fixed width 8 chars
const char* CoolingPromptString = "COOL ";//Fixed width 5 chars
const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingFastChar = 'F';
const char SettingMediumChar = 'F';
const char SettingSlowChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
const char SettingTempCChar = 'C';
const char SettingTempFChar = 'F';
#endif

const char* SettingsShortNames[14] = { "PWRSC ", "STMP ", "SLTME ", "SHTME ",
		"MSENSE ", "TMPUNT ", "TMPRND ", "TMPSPD ", "DSPROT ", "BOOST  ",
		"BTMP ", "PWRDSP ", "ASTART ", "CLBLNK " };
