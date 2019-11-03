# Version Changes
V2.00

* Complete re-write of the low layer system to use the STM32 HAL for easier development
* This allowed easier setup for the new ADC auto measuring system
* Better tip PWM control
* Moved to FreeRTOS for scheduling
* Complete re-write from blank
* Added detailed screen views
* Added smaller font for said screen views


V1.17

* Added blinking cooldown display
* Allowed smaller sleep timeout values
* New font!
* Automatic startup option

V1.16

* Added automatic rotation support
* Added power display graph

V1.15

* Added support for a custom bootup logo to be programmed via the DFU bootloader

V1.14

* Changed input voltage cutoff to be based on cell count rather than voltage

V1.13

* Swapped buttons for menu to prevent accidentally changing first menu item
* Added auto key repeat

V1.12

* Increases sensitivity options to be 1*9 with 0 off state
* Fixes issue where going from COOL *> soldering can leave screen off

V1.11

* Boost mode
* Change sensitivity options to be 1*8

V1.10

* Adds help text to settings
* Improves settings for the display update rate

V1.09

* Adds display modes, for slowing down or simplifying the display

V1.08

* Fix settings menu not showing flip display

V1.07

* Adds shutdown time to automatically shutdown the iron after inactivity

V1.06

* Changes H and C when the iron is heating to the minidso chevron like images

V1.05

* Adds ability to calibrate the input voltage measurement

V1.04

* Increased accuracy of the temperature control
* Improved PID response slightly
* Allows temperature offset calibration
* Nicer idle screen

V1.03

* Improved Button handling
* Ability to set motion sensitivity
* DC voltmeter page shows input voltage

V1.02

* Adds hold both buttons on IDLE to access the therometer mode
* Changes the exit soldering mode to be holding both buttons (Like original firmware)
