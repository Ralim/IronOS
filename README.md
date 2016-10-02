# TS100
This is a complete open source re-write of the software for the ts100 soldering iron.

This was started to remove the need for USB for changing system settings.

The software has similar functionality to the original firmware.

## Features Working
* Soldering / Temperature control
* Full PID Iron Temp
* Adjusting temperature
* Automatic sleep
* Motion wake support
* Basic settings menu
* Input voltage UVLO measurement
* Saving settings to flash for persistence
* Improved GUI
* Use hardware I2C for communications
* Can disable movement detection if desired
## Features still to be implemented
* Manual Temp Calibration

# Upgrading your ts100 iron
This is completely safe, if it goes wrong just put the .hex file from the official website onto the unit and your back to the old firmware :)

1. Hold the button closest to the tip, and plug in the USB to the computer.
2. The unit will appear as a USB drive.
3. Drag the .hex file onto the USB drive.
4. The unit will disconnect and reconnect.
5. The filename will have changed to end in .RDY or .ERR .
6. If it ends with .RDY your done! Otherwise something went wrong.
7. If it went wrong try on a windows computer, some Mac / Linux machines do not play well with their boot loader.


# New Menu System
This new firmware uses a new menu system to allow access to the settings on the device.
This menu can be accessed as shown in following flow chart, in the settings numbers roll over from top to bottom.
![TS100: Software Menu](http://ralimtek.com/images/TS100.png "Fairly easy to learn")
