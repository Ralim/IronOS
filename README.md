# TS100
This is a complete rewrite of the open source software for the ts100 soldering iron.
This project is feature complete for use as a soldering iron, *so please suggest any feature improvements you would like!*
A short(ish) video that goes through every single menu option in the firmware is available [over here](https://www.youtube.com/watch?v=WlnpboYfxNk)
For the french users, there is a article which explain every thing [here](https://maelremrem.xyz/2017/09/08/electronique/outils/ralim-firmware-alternatif-ts100-test-installation/)

This project was started to remove the need for USB for changing system settings.
In the latest official firmware they have also added a settings menu system, so it is still worth comparing the two firmwares to select your preferred option.

**Please note that when running the iron off a Lithium battery pack, the Iron is only rated to 24V input. So using a fully charged 6S battery exceeds this rating, and is done so at your own risk.
Please calibrate your irons voltage reading when you are using a lithium battery after any firmware upgrades.**

## Features
* Soldering / Temperature control
* Full PID iron temperature control
* Automatic sleep with selectable sensitivity
* Motion wake support
* Settings menu
* Set a voltage lower limit for Lithium batteries so you dont kill your battery pack. Please calibrate your input voltage first!
* All settings saved
* Improved readability Fonts
* Use hardware features to improve reliability
* Can disable movement detection if desired
* Calibration of the temperature offset
* Boost mode lets you temporarily change the temperature when soldering
* Battery charge level indicatior if power source set to a lipo cell count.
* Custom bootup logo support
* Automatic LCD rotation based on orientation

# Upgrading your ts100 iron

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
* Pressing the button near the power input enters the settings menu.
* Pressing both buttons together enters the Extras menu

## Soldering mode

In this mode the iron works as you would expect, pressing either button will take you to a temperature change screen. Use each button to go up and down in temperature. Pressing both buttons will exit you from the temperature menu (or wait 3 seconds and it will time out).
Pressing both buttons will also exit the soldering mode.

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
* TMPUNIT -> Temperature unit, C or F
* TMPRND -> Temperature Rounding, {1,5,10}
* TMPSPD -> How fast the temperature should update in the soldering status screen.
* DSPROT -> Display rotation mode, Automatic, Left handed or Right handed
* BOOST -> Enable boost mode
* BTMP -> Set the temperature for the boost mode

Temperature rounding means that the unit will round off the temperature before displaying. This can helpt to reduce the flickering of the temperature when the unit oscillates between two temperatures.

## Extras Menu

This menu defaults to showing the current temperature on the tip. 
Pressing the button near the iron tip will show the current input voltage. Pressing the other button while this is show will allow you to calibrate the reading if your iron is like mine and is not overly accurate out of the factory. (Press buttons to change measurement up and down, press both to exit and save). 

Pressing the button near the usb enters the temperature offset setting menu, when the iron is cold, pressing the other button will start the unit calibrating for any offset in the tip temperature.

### Calibrating input voltage

Due to the tolerance on the resistors used for the input voltage divider, some irons can be up to 0.6V out on the voltage measurement.
Please, Please, calibrate your iron if you have any issues with the cutoff voltage. This is more critical than before with the new cell count based cutout voltage.

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
3. Press both buttons on the idle screen (showing the logo for the iron. The screen you see after power is applied).
4. The iron will now show the current tip temperature.
5. Press the button near the USB port.
6. The display will change to "CAL TEMP"
7. Press the button near the tip of the iron to confirm.
8. The display should change to "CAL OK". If you recieve "CAL FAIL" let your tip cool down for longer, its too hot.
9. Press the button near the USB port to exit back to live temperature display
10. Press both buttons at the same time to return to the idle screen.
11. You're done. Enjoy your iorn.

### Boost mode

This allows you to change the front key (one near the tip) to become a boost button instead of going to temperature editing when in soldering mode. This allows you to set this button to change the soldering temperature for short periods. For example when soldering a big joint and you want to boost the temperature a bit.

The boost temperature is set in the settings menu.

## Commercial Use

This software is provided as-is, so I cannot provide any commercial support for the firmware. However you are more than welcome to distribute links to the firmware, or provide irons with this software on them. 
Please do not re-host the files, but rather link to this page, so that there are not old versions of the firmware hanging around. If this firmware does make you money, it would be nice to recieve a donation, however I dont enforce this.

## Thanks

If you love this firmware and want to continue my caffine addiction, you can do so here (or email me for other options) : https://paypal.me/RalimTek
