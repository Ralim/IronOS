# System Design

# Movement Detection
The unit has a MMA8652FC Accelerometer on the small sub board along with the STM32.
This is used for motion detection of the soldering iron.

# USB Disk
When the unit is plugged into the computer, it presents a FAT device to the operating system over the usb connection.
This is implimented using a ram buffer for the disk.



