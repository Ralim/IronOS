# TS100
This is a complete open source re-write of the software for the ts100 soldering iron.
This project is feature complete for use as a soldering iron, but is still open to ideas and suggestions.


This project was started to remove the need for USB for changing system settings.
In the latest official firmware they have also added a settings menu system, so it is still worth comparing the two firmwares to select your preferred option.

## Features Working
* Soldering / Temperature control
* Full PID iron temperature control
* Adjusting temperature
* Automatic sleep
* Motion wake support
* Settings menu
* Input voltage UVLO measurement
* Saving settings to flash for persistence
* Improved GUI Fonts
* Use hardware I2C for communications
* Can disable movement detection if desired

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
This menu can be accessed as shown in following flow chart, in the settings numbers roll over from top to bottom.
![TS100: Software Menu](TS100.png "Fairly easy to learn")


# Version Changes:
V1.02
	- Adds hold both buttons on IDLE to access the therometer mode.
	- Changes the exit soldering mode to be holding both buttons (Like original firmware).
