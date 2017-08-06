/*
 * Strings.c
 *
 *  Created on: 5Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Strings.h"
#define LANG_EN
#ifdef LANG_EN
const char* SettingsLongNames[13] =
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
				"      Automatically starts the iron into soldering on power up." };

#endif
#ifdef LANG_ES
const char* SettingsLongNames[13] =
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
	"      Automatically starts the iron into soldering on power up."};

#endif

const char* SettingsShortNames[13] = { "PWRSC ", "STMP ", "SLTME ", "SHTME ",
		"MSENSE ", "TMPUNT ", "TMPRND ", "TMPSPD ", "DSPROT ", "BOOST  ",
		"BTMP ", "PWRDSP ", "ASTART " };
