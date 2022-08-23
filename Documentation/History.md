# Version Changes

## V2.19
- Bug-fix Infinite Boot Logo
- Shutdown settings for MHP30
- Accelerometer sensitivity for MHP30
- Allow showing unique device ID
- Bug-fix chance of a power pulse at device boot
- Updated translations
- Improved documents, added features table

## V2.18
- Support for animated bootup logo's
- Bootup logo's moved to their own IronOS-Meta repo
- New Vietnamese translation (limited due to screen size)
- Fixes for SC7A20 in TS80(P)
- Updated translations
- Better Instructions/documents

## V2.17
### Big changes
- Indicate status of VBus for modding Pinecil (debug menu)
- Better hall effect sensor sensitivity adjustment (larger range with more steps)
- Temperature increment will "round" to nearest multiple of increase amount
- Build setup migrated to Alpine (You can now build in docker easily, and on PinePhone/PinePhonePro)
- -> Removed proprietary compiler for Pinecil RISCV now all uses normal gcc
- -> Removed using the arm specific build of gcc for the one that alpine ships (Miniware devices)
- Logo generator python script creates `.dfu` files for ease of use with Pinecil
- Upgrades to translations
- Support for new GD32103 based TS100 units turning up on the market
- Raw hall effect reading now shows in the Pinecil debug menu
- Fixed automatic orientation for newer TS80P's with the SC7 accelerometer
- User interface slight changes
- New `metadata.zip` file to allow the Pine Updater to automatically fetch information on releases
### Notes
- VBus mod detection may not play well with all PPS chargers. If your iron reboots when you view this in the debug menu its not a fault. ([#1226](https://github.com/Ralim/IronOS/issues/1226))
- `metadata.zip` is only designed for use by automatic software, ignore it for normal use
- More details on Pinecil VBus mod coming via other channels.
- Hall effect sensor is not fitted to Pinecil's by default, you have to fit this yourself if you want the feature
- Tweaks to the Accelerometer code means the drivers are slightly more fussy. If you run into any issues let us know in the discussion or issues.
- -> Release has been updated to build `e065be3` after one bug with the BMA223 was found.


## V2.16

* Overhaul of the Timer+ADC setup with help from @sandmanRO
* Overhaul of the PID with help from @sandmanRO
* Settings _should_ now upgrade in place to future versions, with resets only happening to new/changed settings
* Shows error if tip runaway (failed temperature sensor) is detected
* USB-PD now has a timeout, to allow forcing QC3 negotiation to start faster
* QC3 Voltages are now adjustable to user desired setpoint
* Added a small tolerance to allow "overvoltage" on QC3 above unit specifications.
* * Please note: Doing this is entirely at your own risk!
* New Advanced view that is much nicer to use and a very good daily driver option from @Mel-kior
* OLED brightness and contrast thanks to @alvinhochun
* Scrollbar is fixed so it doesnt jump around when menus are shown/hidden
* Moved to `.dfu` files from `.bin` to make flashing commands easier
* Every language had translation updates I believe
* Romanian language added


## V2.15


## Feature upgrades:

* MHP30 support
* Multi-lingual firmware combinations now exist for Pinecil
* More fine grained voltage controlled options
* USB-PD improvements (version one and two)
* More configuration options for power pulse
* All font / character encoding has been very reworked
* More translation updates than one can count 
* More languages 😱 

### MHP30

The MHP30 is a small reflow station from Miniware.
Thanks to a massive amount of help from @g3gg0 this firmware brings the beginnings of support for this unit.
Also kudo's to @Vinigas  and @GoJian for helping with testing.
This is not a _final_ version I'm sure, but this is a working, usable version of firmware support.
Programs the same as any one Miniware unit using drag and drop.
**Note: The boot logo scripts will need updates for this unit, so not supported yet.**

The flood doors are now open for feature requests for this unit :)

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
