/*
 * Strings.c
 *
 *  Created on: 5Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Strings.h"
const char* SettingsLongNames[12] =
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
				"      Changes the arrows to a power display when soldering" };

const char* SettingsShortNames[12]=
{
		"PWRSC",
		"STMP",
		"SLTME",
		"SHTME",
		"MSENSE",
		"TMPUNT",
		"TMPRND",
		"TMPSPD",
		"DSPROT",
		"BOOST ",
		"BTMP",
		"PWRDSP",
};
