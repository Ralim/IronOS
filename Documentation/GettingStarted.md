# Getting Started

Getting started with IronOS on your Pinecil/TS80/TS80P/TS100.
If your device did not come with IronOS already installed, or if you need to update to the latest version; please see the flashing guide for your device:

- [MHP30](https://ralim.github.io/IronOS/Flashing/MHP30)
- [Pinecil V1](https://ralim.github.io/IronOS/Flashing/Pinecil%20V1/)
- [Pinecil V2](https://ralim.github.io/IronOS/Flashing/Pinecil%20V2/)
- [TS80 / TS80P](https://ralim.github.io/IronOS/Flashing/TS80%28P%29/)
- [TS100](https://ralim.github.io/IronOS/Flashing/TS100)

It is recommended to update to the newest stable release.

Once your Iron has been flashed, on first power on it _may_ warn you about the system settings being reset.
_Do not panic_; this is 100% completely normal. This is here to note to you that they have been reset to handle the internal structure changing.

If you receive a warning about the accelerometer or USB-PD not being detected, please see [here](https://ralim.github.io/IronOS/HardwareIssues/).

## The Home screen (or idle screen)

This is the landing page of the firmware, from here you can choose to either go into the [settings menu](#Settings-Menu) or go into [soldering mode](#Soldering-Mode).

By default this will show a screen similar to the one below:

![Home Screen](https://raw.githubusercontent.com/Ralim/IronOS/dev/Documentation/images/HomeScreen.png)

Note that this may be drawn mirrored depending on the orientation of your screen (detailed mode shows a different home screen).

The soldering iron symbol on the screen will appear near the tip. This is here to indicate that pressing the button closest to the front of the iron will enter soldering mode.

And naturally, the slider controls icon (or spanner icon in older versions) represents that pressing the button near the rear of the soldering iron will enter the settings menu.

In the settings, you can turn on a detailed idle screen instead. The buttons still function the same, however, the image will be swapped for a text telling you the current status of the iron with extra details.

Depending on how your device is being powered, at right side of the screen, the firmware will either show the voltage your unit is being provided with, a battery icon (if battery mode is enabled) or a power plug icon.

If you see an (**X**) where the soldering iron should be, this indicates that the firmware can't see the tip connected. This could indicate a problem with the iron or tip. First, try removing the tip screw and tip and gently reinstalling both; ensure that the tip is seated all the way back. If the issue persists please see the [hardware issues section](https://ralim.github.io/IronOS/HardwareIssues/).

This OLED screen features burn-in protection; if no buttons or movement have been detected for a while it will automatically blank the screen to reduce burn-in when the iron is left unattended. Any movement or button press will wake the screen.

### Hidden Extras

Additionally to the two icons shown, there are two "hidden" actions that can be performed on this menu.

If you press and hold the button near the tip (`+/A`), this enters the temperature adjustment screen. Normally this is not required; but if you would like to adjust the set temperature _before_ the tip starts to heat, this can be useful.

If you press and hold the button near the rear of the iron (`-/B`), it will take you into the [debug menu](https://ralim.github.io/IronOS/DebugMenu/).

## Soldering Mode

When you press the button to enter the soldering mode, the iron will instantly start to heat up the tip.

The firmware defaults to 320 °C as the set point for the soldering mode, however on this screen you can enter into the adjustment screen by pressing either button.

Pressing and holding the button near the tip will enter **Boost** mode. This allows a temporary override of the set temperature to a higher (or lower) value. This can be useful as a way to force the tip to a higher temperature to drive more wattage into a large joint when the thermal connection is not ideal.

Pressing and holding the rear button will exit soldering mode and land you back at the home screen. You can also do this by pressing both buttons at once and this will also work, this is a bit harder to do but is kept for compatibility with the Miniware firmware.

Pressing and holding **both** buttons at once will enter locked mode, which will prevent the buttons from doing anything. You can in the settings allow boost mode in locked mode optionally. This can be useful if you find yourself hitting the buttons and entering into the temperature adjustment screen by accident.

### Idle Sleep

If the iron detects a period of time without any significant movement, it will enter sleep mode. This is indicated with a screen graphic similar to Zzzz (or text in detailed mode).

In Sleep mode, the temperature of the iron automatically lowers to 150 °C (default), which is just below the melting point of the solder. This helps reduce rate of oxidation and damage to the iron tip. In general, when not using the iron, unplug it or let it sleep to increase the longevity of replaceable tips. The default sleep temperature can be customized.

Simply picking up or moving the iron will wake it back up into soldering mode. You can also press any button and this will also wake the iron up.

#### Optional Hall Effect Feature (Pinecil only):

Pinecil has an unpopulated footprint (U14) for a hall effect sensor (Si7210-B-00-IV). Adding the sensor and placing a neodymium magnet on the holder stand will trigger Pinecil to sleep after it enters the stand, and Zzzz will appear on-screen. The magnet is positioned on the stand in proximity to the sensor/handle which then activates one of 10 user defined settings (0=off, 1=lowest sensitivity, 9=highest sensitivity). Read the Hall Sensor document for [details on installation](https://ralim.github.io/IronOS/HallSensor/).

### Idle Shutdown

If, after entering sleep mode, the iron still does not see movement for a much longer time (default=10 minutes); it will shut down and return to the home screen.

## Settings Menu

The settings menu is the most evolving aspect of the firmware, so each option is not documented here. However, do not panic, as every menu option has an on-screen description so you don't _need_ to come back here to figure them all out.

To navigate the menu, the two buttons act separately.
The rear button (`-/B`) is pressed to enter the menu and scrolls down the main options, and the other front button (`+/A`) will enter and change the current option.

To see a description of an option, just wait, and after a few seconds, it will scroll across the screen.

The menu is comprised of a 'main menu' of categories and then sub-items that allow you to adjust parameters.

You can long hold buttons to change through options faster, and there is some acceleration when holding the buttons.

There is a small scrollbar that appears along the right edge of the screen to indicate how far through the current list you are (looks like a dot).

Additionally, this scrollbar will blink rapidly when you are on the last value in a range of a sub-menu. For example, if you are in Motion Sensitivity, which has a range of 0 - 9, it will blink when you are at 9.

I highly recommend taking a few minutes to go through all of the options in the menu to get a feel for what you can change, almost every aspect of the internal system is adjustable to suit your needs.

If you want to start over, simply go to Advanced settings > Restore default settings, confirm using the front (`+/A`) button. This sets all menu items to defaults, and keeps the same version firmware.
