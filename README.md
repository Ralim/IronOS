[![Codeship Status for ts100](https://app.codeship.com/projects/c77dbfe0-0764-0138-9520-3eb35bdb2b2c/status?branch=master)](https://app.codeship.com/projects/379118)

# TS100 & TS80 Firmware


This firmware is a fresh start for these soldering irons. Designed from the ground up as fully featured control software that leaves nothing to be desired.
It originally started back at the end of 2016 and has since seen vast improvements, especially some very smart work by community members.

The firmware supports everything you would expect in the soldering iron and has the same features as the stock firmware with some nice extras packed in.
There is a comprehensive menu system in the iron that allows for adjustment of all the settings on the unit, and support for various languages is added by the community.

On the TS100 there are settings to allow you to specify a cutoff voltage for different lithium-ion battery packs to protect your power source.
For the TS80 these are removed and replaced with the selection of 18W or 24W depending on what your QC power bank can support.
Otherwise, both systems work very similar and should support all the features.

This project is considered feature complete for use as a soldering iron, *so please suggest any feature improvements you would like!*

*This firmware does **NOT** support the USB port while running for changing settings. This is done through the onscreen menu only. Logos are edited using the tool or python script and uploaded in DFU mode.*

## Core Features

* PID iron temperature control
* Automatic sleep with selectable sensitivity
* Motion wake support
* Settings menu on the unit
* (TS100) Set a voltage lower limit for Lithium batteries so you don't kill your battery pack
* (TS80 ) Set 18W or 24W settings for your power bank
* All settings saved to flash when you exit the menu
* Improved readability Fonts
* Use hardware features to improve reliability
* Can disable movement detection if desired
* Full tip profile calibration
* Boost mode lets you temporarily change the temperature when soldering (ie raise the temperature for short periods of time)
* (TS100) Battery charge level indicator if power source set to a lipo cell count
* (TS80) Power bank operating voltage is displayed
* Custom boot up logo support
* Automatic LCD rotation based on the orientation
* Supports both the version 1 and version 2 hardware (different accelerometers)

# Upgrading your iron

This is completely safe, but if it goes wrong just put the .hex file from the official website onto the unit and you're back to the old firmware. Downloads for the hex files to flash are available on the [releases page.](https://github.com/Ralim/ts100/releases) The file you want is called *(MODEL)_EN.hex* unless you want the translations, they are (MODEL)_*language short name*.hex. Where (MODEL) is either TS100 or TS80.

Officially the bootloader on the iron only works under Windows. However, users have reported that it does work under Mac, and can be made to work under Linux *sometimes*. Details over on the [wiki page](https://github.com/Ralim/ts100/wiki/Upgrading-Firmware).

```
1. Hold the button closest to the tip, and plug in the USB to the computer.
2. The unit will appear as a USB drive.
3. Drag the .hex file onto the USB drive.
4. The unit will disconnect and reconnect.
5. The filename will have changed to end in .RDY or .ERR
6. If it ends with .RDY you're done! Otherwise, something went wrong.
7. If it didn't work the first time, try copying the file again without disconnecting the iron, often it will work on the second shot.
8. Disconnect the USB and power up the iron. You're good to go.
```


For the more adventurous out there, you can also load this firmware onto the device using an SWD programmer.
On the bottom of the MCU riser PCB, there are 4 pads for programming.
There is a complete device flash backup included in this repository. (Note this includes the bootloader, so will need an SWD programmer to load onto the unit).
For the TS80 the SWD pins are used for the QC negotiation, so you can actually connect to the SWD power via the USB connector.

## Setting a custom bootup image

This firmware uses a different method of updating the bootup image.
This removes the need for emulating a USB drive on the iron just to allow for a bootup image to be setup.
There are further instructions on the [wiki](https://github.com/Ralim/ts100/wiki/Logo-Editor).
Instructions are kept on the wiki so that users can update the information if they find extra helpful information.

# Menu System

This new firmware uses a new menu system to allow access to the settings on the device.
When on the main screen, the unit shows prompts for the two most common operations.

* Pressing the button near the tip enters soldering mode
* Pressing the button near the USB enters the settings menu
* Holding the button near the tip will enter soldering temperature adjust mode (This is the same as the one in the soldering menu, just to let you edit before heating up)
* Holding the button near the USB end will show the firmware version details

More details are over in the [Menu information.](menu.md)

## Thanks


If you love this firmware and want to continue my caffeine addiction, you can do so here (or email me for other options) : https://paypal.me/RalimTek
I also want to give a shout out to all of the [Fantastic Contributors](https://github.com/Ralim/ts100/graphs/contributors).

Especially to the following users, who have helped in various ways that are massively appreciated:

* [dhiltonp](https://github.com/dhiltonp)
* [Mrkvozrout](https://github.com/Mrkvozrout)
* [jonnieZG](https://github.com/jonnieZG)
* [federck](https://github.com/federck)
* [jvitkauskas](https://github.com/jvitkauskas)
* [doegox](https://github.com/doegox)
* [perillamint](https://github.com/perillamint)


## Licence

The code in this repository that is based on the STM tools is under a BSD-like licence.
The code created by the community is GNU GPLv3. Unless noted elsewhere.
The FreeRToS is under its own licence.

## Commercial Use

This software is provided as-is, so I cannot provide any commercial support for the firmware. However, you are more than welcome to distribute links to the firmware, or provide irons with this software on them.
Please do not re-host the files, but rather link to this page, so that there are no old versions of the firmware scattered around. If this firmware does make you money, it would be nice to receive a donation, however, there is no enforcement.
