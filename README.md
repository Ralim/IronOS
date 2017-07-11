# TS100
This is a complete re-write of the open source software for the ts100 soldering iron.
This project is feature complete for use as a soldering iron, *but is still open to ideas and suggestions.*

This project was started to remove the need for USB for changing system settings.
In the latest official firmware they have also added a settings menu system, so it is still worth comparing the two firmwares to select your preferred option.

## Features
* Soldering / Temperature control
* Full PID iron temperature control
* Automatic sleep with selectable sensitivity
* Motion wake support
* Settings menu
* Input voltage UVLO measurement for battery powered use
* All settings saved
* Improved readability Fonts
* Use hardware features to improve reliability
* Can disable movement detection if desired
* Calibration of the temperature offset
* Boost mode lets you temporarily change the temperature when soldering

# Upgrading your ts100 iron
This is completely safe, if it goes wrong just put the .hex file from the official website onto the unit and your back to the old firmware. Downloads for the hex files to flash are available on the [releases page.](https://github.com/Ralim/ts100/releases)
Officially the bootloader on the iron only works under windows. However, users have reported that it does work under Mac, and can be made to work under Linux. Details over on the [wiki page](https://github.com/Ralim/ts100/wiki/Upgrading-Firmware).

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

# New Menu System
This new firmware uses a new menu system to allow access to the settings on the device.
When on the main screen, the unit shows prompts for the two most common operations.
-> Pressing the button near the tip enters soldering mode
-> Pressing the button near the power input enters the settings menu.
-> Pressing both buttons together enters the Extras menu
## Soldering mode
In this mode the iron works as you would expect, pressing either button will take you to a temperature change screen. Use each button to go up and down in temperature. Pressing both buttons will exit you from the temperature menu (or wait 3 seconds and it will time out).
Pressing both buttons will also exit the soldering mode.

## Settings Menu
This menu allows you to cycle through all the options and set their values.
The button near the tip cycles through the options, and the one near the usb changes the selected option.
Note that settings are not saved until you exit the menu, and some settings such as screen flip do not apply until a power cycle is applied.
If you leave the unit alone (ie don't press any buttons) on a setting, after 3 seconds the screen will scroll a longer version of the name

* UVCO -> Undervoltage cut out level, settable in 1V increments from 10-24V
* STMP -> The temperature the unit drops to in sleep mode
* SLTME -> Sleep time, how long it takes before the unit goes to sleep
* SHTME -> Shutdown Time, how long the unit will wait after movement before shutting down completely
* MSENSE -> Motion Sensitivity,0-9,0 means motion sensing is turned off, 9 is most sensitive, 1 is least sensitive (ie takes more movement to trigger)
* TMPUNIT -> Temperature unit, C or F
* TMPRND -> Temperature Rounding, {1,5,10}
* TMPSPD -> How fast the temperature should update in the soldering status screen.
* FLPDSP -> Flip display for left handed users
* BOOST -> Enable boost mode
* BTMP -> Set the temperature for the boost mode

Temperature rounding means that the unit will round off the temperature before displaying. This can helpt to reduce the flickering of the temperature when the unit oscillates between two temperatures.
## Extras Menu
This menu defaults to showing the current temperature on the tip. 
Pressing the button near the iron tip will show the current input voltage. Pressing the other button while this is show will allow you to calibrate the reading if your iron is like mine and is not overly accurate out of the factory. (Press buttons to change measurement up and down, press both to exit and save). 

Pressing the button near the usb enters the temperature offset setting menu, when the iron is cold, pressing the other button will start the unit calibrating for any offset in the tip temperature.
## Boost mode
This allows you to change the front key (one near the tip) to become a boost button instead of going to temperature editing when in soldering mode. This allows you to set this button to change the soldering temperature for short periods. For example when soldering a big joint and you want to boost the temperature a bit.

The boost temperature is set in the settings menu.
# Version Changes:
V1.12
- Increases sensitivity options to be 1-9 with 0 off state
- Fixes issue where going from COOL -> soldering can leave screen off.

V1.11
- Boost mode
- Change sensitivity options to be 1-8

V1.10
- Adds help text to settings
- Improves settings for the display update rate

V1.09
- Adds display modes, for slowing down or simplifying the display

V1.08
- Fix settings menu not showing flip display

V1.07
- Adds shutdown time to automatically shutdown the iron after inactivity

V1.06
- Changes H and C when the iron is heating to the minidso chevron like images

V1.05
- Adds ability to calibrate the input voltage measurement

V1.04
- Increased accuracy of the temperature control
- Improved PID response slightly
- Allows temperature offset calibration
- Nicer idle screen

V1.03 
- Improved Button handling
- Ability to set motion sensitivity
- DC voltmeter page shows input voltage

V1.02
- Adds hold both buttons on IDLE to access the therometer mode.
- Changes the exit soldering mode to be holding both buttons (Like original firmware).

If you _really_ loved this firmware and want to continue my caffine addiction, you can do so here (or email me) : https://paypal.me/RalimTek
