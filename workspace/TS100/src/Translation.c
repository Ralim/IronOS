/*
 * Translation.c
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

const char* SettingsLongNames[12] =
		{
		/*These are all the help text for all the settings.*/
		/*All must start with 6 spaces so they come on screen nicely.*/
		"Power source. Sets cutoff voltage. <DC 10V> <S 3.3V per cell>",
		"Sleep Temperature <C>",
		"Sleep Timeout <Minutes>",
		"Shutdown Timeout <Minutes>",
		"Motion Sensitivity <0.Off 1.least sensitive 9.most sensitive>",
		"Display detailed information in a smaller font.",
		"Display Orientation <A. Automatic L. Left Handed R. Right Handed>",
		"Enable front key enters boost mode 450C mode when soldering",
		"Temperature when in \"boost\" mode",
		"Automatically starts the iron into soldering on power up. T=Soldering, S= Sleep mode,F=Off",
		"Blink the temperature on the cooling screen while the tip is still hot.",
		"Reset all settings",
		};

const char* SettingsShortNames[12] = {
		"PWRSC ",
		"STMP ",
		"STME ",
		"SHTME ",
		"MSENSE ",
		"ADVDSP ",
		"DSPROT ",
		"BOOST  ",
		"BTMP ",
		"ASTART ",
		"CLBLNK ",
		"RESET? "
		};
