# Version 2 development branch

This firmware is not complete, it is missing some features such as :

* Russian font has issues atm
* Soldering detailed view coming
While it most likely will work, It is not meant for production use *just* yet!


# TS100
This is a complete rewrite of the open source software for the ts100 soldering iron.
The version two fork of this code has no shared code with the original firmware from Miniware (E-design) group.
This project is concidered feature complete for use as a soldering iron, *so please suggest any feature improvements you would like!*

A short(ish) video that goes through every single menu option in the firmware is available [over here](https://www.youtube.com/watch?v=WlnpboYfxNk). 
This video was created on an earlier 1.x version of the firmware, so alot has changed and a new video will be coming soon for the 2.x fork.

*This firmware does **NOT** support the usb port while running for changing settings. This is done through the onscreen menu only. Logos are edited using the tool or python script and uploaded in DFU mode.*

*Please note that when running the iron off a Lithium battery pack, the Iron is only rated to 24V input. So using a fully charged 6S battery *slightly* exceeds this rating, and is done so at your own risk.
Please calibrate your irons voltage reading when you are using a lithium battery after any firmware upgrades.*

## Features
* PID iron temperature control
* Automatic sleep with selectable sensitivity
* Motion wake support
* Settings menu on the unit
* Set a voltage lower limit for Lithium batteries so you dont kill your battery pack
* All settings saved to flash when you exit the menu
* Improved readability Fonts
* Use hardware features to improve reliability
* Can disable movement detection if desired
* Calibration of the thermocouple offset
* Boost mode lets you temporarily change the temperature when soldering (ie raise temperature for short periods of time)
* Battery charge level indicatior if power source set to a lipo cell count.
* Custom bootup logo support
* Automatic LCD rotation based on orientation

## Upgrading your ts100 iron

This is completely safe, if it goes wrong just put the .hex file from the official website onto the unit and your back to the old firmware. Downloads for the hex files to flash are available on the [releases page.](https://github.com/Ralim/ts100/releases) The file you want is called *ts100.hex* unless you want the translations, they are ts100_*language short name*.hex.

Officially the bootloader on the iron only works under windows. However, users have reported that it does work under Mac, and can be made to work under Linux *sometimes*. Details over on the [wiki page](https://github.com/Ralim/ts100/wiki/Upgrading-Firmware).

1. Hold the button closest to the tip, and plug in the USB to the computer.
2. The unit will appear as a USB drive.
3. Drag the .hex file onto the USB drive.
4. The unit will disconnect and reconnect.
5. The filename will have changed to end in .RDY or .ERR 
6. If it ends with .RDY you're done! Otherwise something went wrong.
7. Disconnect the USB and power up the iron. You're good to go.

For the more adventurerous out there, you can also load this firmware onto the device using a SWD programmer.
On the bottom of the MCU riser pcb, there are 4 pads for programming.
There is a complete device flash backup included in this repository. (Note this includes the bootloader, so will need a SWD programmer to load onto the unit). Please do not use the backup of the bootloader for anything malicious, its only saved here for those who are tinkering with their iron and decide to replace it.

## Setting a custom bootup image

This firmware uses a different method of updating the bootup image.
This removes the need for emulating a USB drive on the iron just to allow for a bootup image to be setup.
There are further instructions on the [wiki](https://github.com/Ralim/ts100/wiki/Logo-Editor). Instructions are kept on the wiki so that users can update the information if they find extra helpful information.

## New Menu System

This new firmware uses a new menu system to allow access to the settings on the device.
When on the main screen, the unit shows prompts for the two most common operations.
* Pressing the button near the tip enters soldering mode
* Pressing the button near the USB enters the settings menu.
* Holding the button near the tip will enter soldering temperature adjust mode (This is the same as the one in the soldering menu, just to let you edit before heating up).
* Holding the button near the USB end will show the firmware version details.
## Soldering mode

In this mode the iron works as you would expect, pressing either button will take you to a temperature change screen. 
Use each button to go up and down in temperature. Pressing both buttons will exit you from the temperature menu (or wait 3 seconds and it will time out).
Pressing both buttons or holding the button near the USB will exit the soldering mode.
Holding the button at the front of the iron will enter boost mode (if enabled).

## Settings Menu

This menu allows you to cycle through all the options and set their values.
The button near the tip cycles through the options, and the one near the usb changes the selected option.
Note that settings are not saved until you exit the menu, and some settings such as screen flip do not apply until a power cycle is applied.
If you leave the unit alone (ie don't press any buttons) on a setting, after 3 seconds the screen will scroll a longer version of the name

* PWRSC -> Power source, select a cell count if using a LiPo, or DC to disable the shutdown. (Sets it to minimum of 10V).
* STMP -> The temperature the unit drops to in sleep mode
* SLTME -> Sleep time, how long it takes before the unit goes to sleep
* SHTME -> Shutdown Time, how long the unit will wait after movement before shutting down completely
* MSENSE -> Motion Sensitivity,0*9,0 means motion sensing is turned off, 9 is most sensitive, 1 is least sensitive (ie takes more movement to trigger)
* ADVDSP -> Enable the advanced display (shows more details)
* DSPROT -> Display rotation mode, Automatic, Left handed or Right handed
* BOOST -> Enable boost mode
* BTMP -> Set the temperature for the boost mode
* ASART -> Automatically start the unit when power is applied (i.e. the unit will go straight into soldering mode)
* CLBLNK -> Blink the screen as an alert when cooling down
* TMP CAL -> Use to calibrate offset on the tip temperature
* RESET -> Use to force a complete reset on exit of the settings menu
* TMPUNIT -> Temperature unit, C or F

### Calibrating input voltage

Due to the tolerance on the resistors used for the input voltage divider, some irons can be up to 0.6V out on the voltage measurement.
Please calibrate your iron if you have any issues with the cutoff voltage. 
Note that cutoff messages can also be triggered by using a power supply that is too weak and fails under the load of the iron.
This is more critical than before with the new cell count based cutout voltage.

To calibrate your Iron:

1. Measure the input voltage with a multimeter and note it down.
2. Connect the input to your iron.
3. On the home screen (showing iron symbol), press both buttons simultainiously.
4. The iron will now show the tip temperature.
5. Press the button near the soldering iron tip.
6. The screen will display the measured input voltage.
7. If this is the same as what you measured before skip to step 13
8. Otherwise, press the button near the USB end of the iron
9. The voltage will now slowly blink.
10. Use the buttons to adjust the reading up and down until it reads as close as possible to the voltage you measured earlier.
11. When it is reading as close as possible, press both buttons at once.
12. The screen will go back to just showing the input voltage.
13. Press the button near the tip of the iron to exit back to the live temperature display.
14. Press both buttons at once to exit back to the idle screen.
15. You're done. Enjoy your iron.

### Calibrating tip offset

Some tips will have an offset on their readings, to calibrate this out perform the following steps:

1. Connect power to your iron
2. Make sure the tip is at room temperature (ie. wait for a fair while after using the iron before calibration)
3. Enter the settings menu
4. Scroll down to *TMP CAL*
5. Press the button to change the option (tip button)
6. The display will start to scroll a warning message to check that the tip is at ambient temperature!
7. Press the button near the tip of the iron to confirm.
8. The display will go to "...." for a short period of time as the unit measures the tip temperature and the handle temperature and compares them
9. The display will then go back to *TMP CAL*
10. Calibration is done, just exit the settings menu as normal 
11. You're done. Enjoy your iron.

### Boost mode

This allows you to change the front key (one near the tip) to become a boost button when you hold it for > 2 seconds. This allows you to set this button to change the soldering temperature for short periods. For example when soldering a big joint and you want to boost the temperature a bit.

The boost temperature is set in the settings menu.


## Thanks

If you love this firmware and want to continue my caffine addiction, you can do so here (or email me for other options) : https://paypal.me/RalimTek

## Licence

The code in this repository that is based on the STM tools is under a BSD like licence.
The code created by the communitiy is GNU GPLv3.
The FreeRToS is under its own licence.

## Commercial Use

This software is provided as-is, so I cannot provide any commercial support for the firmware. However you are more than welcome to distribute links to the firmware, or provide irons with this software on them. 
Please do not re-host the files, but rather link to this page, so that there are not old versions of the firmware scattered around. If this firmware does make you money, it would be nice to recieve a donation, however there is no enforcement.
