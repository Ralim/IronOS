# Version Changes

## V2.14

- Fixing auto rotation bug in the LIS accelerometer in the TS80/TS80P
- Adds support for two new accelerometers
  -- SC7A20 (Future Pinecil batch) #786
  -- MSA301 (Newer TS80P) #761
- Add warnings if accelerometer or USB-PD IC's are not detected #752
  -- Only shows for first few boots, to help catch unsupported models
- Fixed cooling down blink to be sane speed #769
- Cleanup of threads and slightly faster power negotiation #790

- Updates to flashing scripts #775
- Documentation updates all over the place (and the wiki was given a cleanup)|
- Updates to makefile #792 #787
- Cleanup the folder name of the source code #800
- clang-format spec setup #801

## V2.13

- First _official_ Pinecil release
- All of the wire for Pinecil releases added
- Updated Translations
- Delay accelerometer to help with entering sleep on startup
- Dual speed PWM to help with power limit control
- Improve heat up time
- Adds locking mode
- Improved docs all over the place
- Repo rename occured TS100 -> IronOS
- Hall effect sensor support added (not fitted in Pinecil but optional)
- QC 20V support for Pinecil
- CI upgrades for faster builds
- Fixed bug with accelerometer model on Pinecil
- Rework of all of the temperature curves for better accuracy

## V2.12

- Only released as pre-release
- [TS80P] Improvements to the PD negotiation to handle a few more adapters cleanly
- Pause on the last item in a list
- Clean up the menu (removed both enables and settings, so that you can turn things off easier)
- Removing the very old single line menu style.

## V2.11

- First TS80P support
- Added in a USB-PD driver stack for the FUSB302
- Fixed some graphical glitches

## V2.10

- GUI polish (animations and scroll bars)
- Power pulse to keep power supplies alive
- Adjustable tip response gain

## V2.09

- Adjustable steps in temperature adjustment
- Git hash now in build string
- Adjustable language to set if US units are available or not
- Some minor QC3 improvements

## V2.08

- Fixes auto start in sleep mode
- Power limiters

## V2.07

- QC fixes
- Cosmetic fixes for leading 0's

## V2.06

- Warning on settings reset
- Temp temp re-write
- Display calibration offset
- Hide some leading 0's
- Menu timeouts

## V2.05

- Language updates

## V2.04

- GUI updates

## V2.03

- Support for new accelerometers

## V2.02

- Adds small font

## V2.01

- Newer settings menu

## V2.00

- Complete re-write of the low layer system to use the STM32 HAL for easier development
- This allowed easier setup for the new ADC auto measuring system
- Better tip PWM control
- Moved to FreeRTOS for scheduling
- Complete re-write from blank
- Added detailed screen views
- Added smaller font for said screen views

## V1.17

- Added blinking cooldown display
- Allowed smaller sleep timeout values
- New font!
- Automatic startup option

## V1.16

- Added automatic rotation support
- Added power display graph

## V1.15

- Added support for a custom bootup logo to be programmed via the DFU bootloader

## V1.14

- Changed input voltage cutoff to be based on cell count rather than voltage

## V1.13

- Swapped buttons for menu to prevent accidentally changing first menu item
- Added auto key repeat

## V1.12

- Increases sensitivity options to be 1\*9 with 0 off state
- Fixes issue where going from COOL \*> soldering can leave screen off

## V1.11

- Boost mode
- Change sensitivity options to be 1\*8

## V1.10

- Adds help text to settings
- Improves settings for the display update rate

## V1.09

- Adds display modes, for slowing down or simplifying the display

## V1.08

- Fix settings menu not showing flip display

## V1.07

- Adds shutdown time to automatically shutdown the iron after inactivity

## V1.06

- Changes H and C when the iron is heating to the minidso chevron like images

## V1.05

- Adds ability to calibrate the input voltage measurement

## V1.04

- Increased accuracy of the temperature control
- Improved PID response slightly
- Allows temperature offset calibration
- Nicer idle screen

## V1.03

- Improved Button handling
- Ability to set motion sensitivity
- DC voltmeter page shows input voltage

## V1.02

- Adds hold both buttons on IDLE to access the therometer mode
- Changes the exit soldering mode to be holding both buttons (Like original firmware)
