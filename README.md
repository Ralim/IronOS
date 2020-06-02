[![Codeship Status for ts100](https://app.codeship.com/projects/c77dbfe0-0764-0138-9520-3eb35bdb2b2c/status?branch=master)](https://app.codeship.com/projects/379118)
![C/C++ CI](https://github.com/Ralim/ts100/workflows/C/C++%20CI/badge.svg)

# Flexible Soldering iron control Firmware

Originally concieved as an alternative firmware for the TS100, this firmware has evolved into a complex soldering iron control firmware.

The firmware implements all of the standard features of a "smart" soldering iron, with lots of little extras and tweaks.
I highly reccomend reading the installation guide fully when installing on your iron.

For soldering irons that are designed to be powered by "smart" power sources (QC and PD), the firmware supports settings around the negotiated power and voltage.
For soldering irons that are designed to be powered by batteries (TS100), settings for a cutoff voltage for battery protection are supported.

There are 25 languages currently supported as of present. When downloading the firmware for your soldering iron take node of the language code in the file name.

This project is considered feature complete for use as a soldering iron, *so please suggest any feature improvements you would like!*

*This firmware does **NOT** support the USB port while running for changing settings. This is done through the onscreen menu only. Logos are edited using the tool or python script and uploaded in DFU mode.*

## Key Features

* PID style iron temperature control
* Automatic sleep with selectable sensitivity
* Motion wake support
* All settings exposed in intuitive menu
* (TS100) Set a voltage lower limit for Lithium batteries so you don't kill your battery pack
* (TS80) Set 18W or 24W settings for your power bank
* Improved readability Fonts
* Use hardware features to improve reliability
* Can disable movement detection if desired
* Boost mode lets you temporarily change the temperature when soldering (i.e. raise the temperature for short periods of time)
* (TS100) Battery charge level indicator if power source set to a lipo cell count
* (TS80) Power bank operating voltage is displayed
* [Custom boot up logo support](upgrading.md)
* Automatic LCD rotation based on the orientation
* Supports both the version 1 and version 2 hardware (different accelerometers)

# Menu System

This new firmware uses a new menu system to allow access to the settings on the device.
When on the main screen, the unit shows prompts for the two most common operations.

* Pressing the button near the tip enters soldering mode
* Pressing the button near the USB enters the settings menu
* Holding the button near the tip will enter soldering temperature adjust mode (This is the same as the one in the soldering menu, just to let you edit before heating up)
* Holding the button near the USB end will show the firmware version details

Detailed operation details are over in the [Menu information.](menu.md)

## Thanks

If you love this firmware and want to continue my caffeine addiction, you can do so [here](https://paypal.me/RalimTek) (or email me for other options).
I also want to give a shout out to all of the [Fantastic Contributors](https://github.com/Ralim/ts100/graphs/contributors).

Especially to the following users, who have helped in various ways that are massively appreciated:

* [Dhiltonp](https://github.com/dhiltonp)
* [Mrkvozrout](https://github.com/Mrkvozrout)
* [JonnieZG](https://github.com/jonnieZG)
* [Federck](https://github.com/federck)
* [Jvitkauskas](https://github.com/jvitkauskas)
* [Doegox](https://github.com/doegox)
* [Perillamint](https://github.com/perillamint)
* [GeminiServer](https://github.com/GeminiServer)
* [PixelPirate](https://github.com/PixelPirate)
* [Firebie](https://github.com/Firebie)
* [Agatti](https://github.com/agatti)
* [Discip](https://github.com/discip)

## Licence

The code created by the community is GNU GPLv3. Unless noted elsewhere.
Other components such as FreeRTOS have their own licence.

## Commercial Use

This software is provided as-is, so I cannot provide any commercial support for the firmware. However, you are more than welcome to distribute links to the firmware, or provide irons with this software on them.
Please do not re-host the files, but rather link to this page, so that there are no old versions of the firmware scattered around.
