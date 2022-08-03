![CI](https://github.com/Ralim/ts100/workflows/CI%20Build%20all/badge.svg)
![GitHub all](https://img.shields.io/github/downloads/ralim/IronOS/total)
![Latest Release](https://img.shields.io/github/v/release/ralim/IronOS)

# IronOS - Flexible Soldering iron control Firmware

_This repository was formerly known as TS100, it's the same great code. Just with more supported devices._

Originally conceived as an alternative firmware for the TS100, this firmware has evolved into a complex soldering iron control firmware.

The firmware implements all of the standard features of a "smart" soldering iron, with lots of little extras and tweaks.
I highly recommend reading the installation guide fully when installing on your iron. And after install just explore the settings menu.

For soldering irons that are designed to be powered by "smart" power sources (QC and PD), the firmware supports settings around the negotiated power and voltage.
For soldering irons that are designed to be powered by batteries (TS100 & Pinecil), settings for a cutoff voltage for battery protection are supported.

There are 30 languages currently supported as of present. When downloading the firmware for your soldering iron take note of the language code in the file name.

This project is considered feature complete for use as a soldering iron, _so please suggest any feature improvements you would like!_

_This firmware does **NOT** support the USB port while running for changing settings. This is done through the onscreen menu only. Logos are edited using the tool or python script and uploaded in DFU mode._


|  Device 	| DC Supported 	| QC Supported 	| PD Supported 	| EPR Supported | Reccomended 	|
|--------:	|:------------:	|:------------:	|:------------:	|:-----------:	|:-----------:	|
|   MHP30 	|      ❌       |      ❌      |      ✔️      |      ❌       |      ✔️       |
| Pinecil 	|      ✔️       |      ✔️      |      ✔️      |      ✔️*      |      ✔️       |
|   TS100 	|      ✔️       |      ❌      |      ❌      |      ❌       |      ❌       |
|    TS80 	|      ❌       |      ✔️      |      ❌      |      ❌       |      ❌       |
|   TS80P 	|      ❌       |      ✔️      |      ✔️      |      ❌       |      ✔️       |

*Only Pinecil version 2 sold after Aug 2, 2022 has support for EPR PD3.1 28V/140W. The new model also includes BLE bluetooth and has a teal color silicone thumb grip. Original Pinecil was black with a sky blue colored grip.

Please note that Miniware have started to ship TS100's using cloned STM32 Chips. While these do work with IronOS, their DFU bootloader works terribly and it is hard to get it to successfully flash larger firmware images like IronOS without timing out. THis is the main reason why the TS100 is _no longer reccomended_.


## Getting Started

To get started with IronOS firmware, please jump to [Getting Started Guide](Documentation/GettingStarted.md).
But the [TL;DR](https://www.merriam-webster.com/dictionary/TL%3BDR) is to press the button near the front of the iron to heat up. Use the button near the back of the iron to enter the settings menu.
Long hold the rear button in soldering mode to exit back to the start screen.

## Installation

For notes on installation for your device, please refer to the [Flashing Guide](Documentation/Flashing.md).

## Key Features

- PID style iron temperature control
- Automatic sleep with selectable sensitivity
- Motion wake support
- All settings exposed in the intuitive menu
- (TS100) Set a voltage lower limit for Lithium batteries so you don't kill your battery pack
- (TS80) Set 18 W or 24 W settings for your power bank
- (TS80P) Automatically negotiates appropriate PD and falls back to QC mode like TS80
- (Pinecil) Supports all 3 power modes (PD, QC, DC In).
- Improved readability Fonts, supporting multiple languages
- Use hardware features to improve reliability
- Can disable movement detection if desired
- Boost mode lets you temporarily change the temperature when soldering (i.e. raise the temperature for short periods)
- (TS100/Pinecil) Battery charge level indicator if power source set to a lipo cell count
- (TS80/TS80P/Pinecil) Power bank operating voltage is displayed
- [Custom boot up logo support](Documentation/Logo.md)
- Automatic LCD rotation based on the orientation

## Menu System

This new firmware uses a new menu system to allow access to the settings on the device.
When on the main screen and having the tip plugged in, the unit shows a pair of prompts for the two most common operations.

- Pressing the button near the tip enters the *soldering mode*
- Pressing the button near the USB end enters the *settings menu*
- When not in *soldering mode*, holding down the button near the tip will enter *soldering temperature adjust mode* (This is the same as the one in the *soldering mode*, but allows to adjust the temperature before heating up), in *soldering mode* however this will activate *boost mode* as long as you hold down the button.
- Holding down the button near the USB end will show the *[debug menu](Documentation/DebugMenu.md).* In *soldering mode* this ends the heating.

Operation details are over in the [Menu information.](Documentation/Menu.md)

## Thanks

If you love this firmware and want to continue my caffeine addiction, you can do so [here](https://paypal.me/RalimTek) (or email me for other options).
I also want to give a shout out to all of the [Fantastic Contributors](https://github.com/Ralim/ts100/graphs/contributors).

Especially to the following users, who have helped in various ways that are massively appreciated:

- [Dhiltonp](https://github.com/dhiltonp)
- [Mrkvozrout](https://github.com/Mrkvozrout)
- [JonnieZG](https://github.com/jonnieZG)
- [Federck](https://github.com/federck)
- [Jvitkauskas](https://github.com/jvitkauskas)
- [Doegox](https://github.com/doegox)
- [Perillamint](https://github.com/perillamint)
- [GeminiServer](https://github.com/GeminiServer)
- [Patrick Horlebein](https://github.com/PixelPirate)
- [Firebie](https://github.com/Firebie)
- [Agatti](https://github.com/agatti)
- [Discip](https://github.com/discip)
- [Paul Fertser](https://github.com/paulfertser)

Plus the huge number of people who have contributed translations, your effort is massively appreciated.

## Licence

The code created by the community is GNU GPLv3. Unless noted elsewhere.
Other components such as FreeRTOS/USB-PD have their own licence.

## Commercial Use

This software is provided as-is, so I cannot provide any commercial support for the firmware.
However, you are more than welcome to distribute links to the firmware or provide irons with this software on them.
Please do not re-host the files, but rather link to this page, so that there are no old versions of the firmware scattered around.
