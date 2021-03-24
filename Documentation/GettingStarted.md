# Getting Started

Getting started with Iron OS on your Pinecil/TS80/TS80P/TS100.
If your device did not come with Iron OS already installed, or if you need to update to the latest version; please see the [Flashing Guide](Documentation/Flashing.md).

Once your Iron has been flashed, one first power on it _may_ warn you about the system settings being reset.
_Do not panic_; this is 100% completely normal. This is here to note to you that they have been reset to handle the internal structure changing.

If you receive a warning about the accelerometer or USB-PD not being detected, please see [here](Documentation/HardwareIssues.md).

## The Home screen (or idle screen)

This is the landing page of the firmware, from here you can choose to either go into the [settings menu](##-Settings-Menu) or go into [soldering mode](##-Soldering-Mode).

By default this will show a screen similar to the one below:

![Home Screen](Documentation/images/HomeScreen.png)

Note that this may be drawn mirrored depending on the orientation of your screen.

The soldering iron symbol on the screen will appear near the tip. This is here to indicate that pressing the button closest to the front of the soldering iron will enter soldering mode.

And naturally, the spanner like icon represents that pressing the button near the rear of the soldering iron will enter the settings menu.

In the settings you can turn on an advanted idle screen instead. The buttons still function the same, however, the image will be swapped for text telling you the current status of the iron with extra details.

Depending on how your device is being powered, at one end of the screen, the firmware will either draw text to show you the current voltage your unit is being provided with, a battery icon (if battery mode is enabled) or a power plug icon.

If you see exclamation marks (!!!) where the solderion icon should be, this indicates that the firmware can't see the tip connected. This could indicate a problem with the iron or tip if there is one inserted. First try removing and reinstalling the tip. If the issue persists please see the [hardware issues section](Documentation/HardwareIssues.md).

### Hidden Extras

Additionally to the two icons shown, there are two "hidden" actions that can be performed on this menu.

If you press and hold on the button near the tip of the soldering iron, this will bring up the temperature adjustment screen. Normally this is not required; but if you would like to adjust the set temperature _before_ the tip starts to heat, this can be useful.

If you press and hold the button near the rear of the iron it will take you into the [debug menu](Documentation/DebugMenu.md).

## Soldering Mode

When you press the button to enter into the soldering mode, the iron will instantly start to heat up the tip.

## Settings Menu
