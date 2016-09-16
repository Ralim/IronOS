# System Design

# Movement Detection
The unit has a MMA8652FC Accelerometer on the small sub board along with the STM32.
This is used for motion detection of the soldering iron.

# USB Disk
When the unit is plugged into the computer, it presents a FAT device to the operating system over the usb connection.
This is implemented using a ram buffer for the disk.




# Future improvements to be made
These features are things I would like to add to the firmware

* Use the sysTick hardware for timing of events
* Move all the settings to an on device menu system instead of usb link
* Move error codes into a unified struct
* Rewrite most of the OLED stack to not require hardcoded values
* Rework the modes of the unit to be neater
