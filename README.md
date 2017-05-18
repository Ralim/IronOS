# TS100
This is a complete re-write of the open source software for the ts100 soldering iron.
This project is feature complete for use as a soldering iron, but is still open to ideas and suggestions.

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

# Upgrading your ts100 iron
This is completely safe, if it goes wrong just put the .hex file from the official website onto the unit and your back to the old firmware. Downloads for the hex files to flash are available on the [releases page.](https://github.com/Ralim/ts100/releases)
**You will need a windows computer (7,8,10 tested), using the normal windows explorer to load the firmware.
The bootloader does not appear to work under mac or linux at the moment.**

1. Hold the button closest to the tip, and plug in the USB to the computer.
2. The unit will appear as a USB drive.
3. Drag the .hex file onto the USB drive.
4. The unit will disconnect and reconnect.
5. The filename will have changed to end in .RDY or .ERR .
6. If it ends with .RDY your done! Otherwise something went wrong.
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

* UVCO -> Undervoltage cut out level, settable in 1V increments from 10-24V
* STIME -> Sleep time, how long it takes before the unit goes to sleep
* STMP -> The temperature the unit drops to in sleep mode
* MOTION -> Wether motion detection is enabled or not
* TMPUNIT -> Temperature unit, C or F
* FLPDSP -> Flip display for left handed users
* SENSE -> Motion Sensitivity, H is more sensitive. L is lowest sensitivity (ie takes more movement to trigger)

## Extras Menu
This menu defaults to showing the current temperature on the tip. Pressing the button near the iron tip will show the current input voltage. Pressing the button near the usb enters the temperature offset setting menu, when the iron is cold, pressing the other button will start the unit calibrating for any offset in the tip temperature.

# Version Changes:
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
