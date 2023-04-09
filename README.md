[![CI Build](https://github.com/Ralim/IronOS/actions/workflows/push.yml/badge.svg)](https://github.com/Ralim/IronOS/actions/workflows/push.yml)
[![Total Downloads](https://img.shields.io/github/downloads/ralim/IronOS/total)](https://github.com/Ralim/IronOS)
[![Contributors](https://img.shields.io/github/contributors-anon/ralim/ironos?color=blue&style=flat)](https://github.com/Ralim/IronOS/graphs/contributors)
[![Latest Release](https://img.shields.io/github/v/release/ralim/IronOS)](https://github.com/Ralim/IronOS/releases/latest)

# IronOS - Flexible Soldering iron control Firmware

_This repository was formerly known as TS100, it's the same great code. Just with more supported devices._

Originally conceived as an alternative firmware for the TS100, this firmware has evolved into a complex soldering iron control firmware.

The firmware implements all of the standard features of a 'smart' soldering iron, with lots of little extras and tweaks.
I highly recommend reading the installation guide fully when installing on your iron. And after install just explore the settings menu.

For soldering irons that are designed to be powered by 'smart' power sources (PD and QC), the firmware supports settings around the negotiated power and voltage.
For soldering irons that are designed to be powered by batteries (TS100 & Pinecil), settings for a cutoff voltage for battery protection are supported.

Currently **31** languages are supported. When downloading the firmware for your soldering iron, take note of the language code in the file name.

This project is considered feature complete for use as a soldering iron, _so please suggest any feature improvements you would like!_

_This firmware does **NOT** support the USB port while running for changing settings. This is done through the onscreen menu only. Logos are edited on a computer and flashed like firmware._

|   Device   | DC  | QC  | PD  | EPR | BLE | Battery | Recommended |
| :--------: | :-: | :-: | :-: | :-: | :-: | :-----: | :---------: |
|   MHP30    | ❌  | ❌  | ✔️  | ❌  | ❌  |   ❌    |      ✔️       |
| Pinecil V1 | ✔️  | ✔️  | ✔️  | ❌  | ❌  |   ✔️    |      ✔️       |
| Pinecil V2 | ✔️  | ✔️  | ✔️  | ✔️  | ✔️  |   ✔️    |      ✔️       |
|   TS80P    | ❌  | ✔️  | ✔️  | ❌  | ❌  |   ✔️    |      ✔️       |
|   TS100    | ✔️  | ❌  | ❌  | ❌  | ❌  |   ✔️    |      ❌*      |
|    TS80    | ❌  | ✔️  | ❌  | ❌  | ❌  |   ✔️    |     ❌**      |

\*Please note that Miniware started shipping TS100's using cloned STM32 Chips. While these do work with IronOS, their DFU bootloader works terribly, and it is hard to get it to successfully flash larger firmware images like IronOS without timing out. This is the main reason why the TS100 is **_no longer recommended_**.

\**TS80 is replaced by TS80P. Production ramped down a long time ago and it's just existing stock clearing the system. It's marked not recommended being optimistic that people might pause and buy the far superior TS80P instead. This is the main reason why the TS80 is **_no longer recommended_**.

## Getting Started

To get started with IronOS firmware, please jump to [Getting Started Guide](https://ralim.github.io/IronOS/GettingStarted/).
But the [TL;DR](https://www.merriam-webster.com/dictionary/TL%3BDR) is to press the button near the front of the iron to heat up. Use the button near the back of the iron to enter the settings menu.
Long hold the rear button in soldering mode to exit back to the start screen.

## Installation

For notes on installation for your device, please refer to the flashing guide for your device:

- [MHP30](https://ralim.github.io/IronOS/Flashing/MHP30)
- [Pinecil V1](https://ralim.github.io/IronOS/Flashing/Pinecil%20V1/)
- [Pinecil V2](https://ralim.github.io/IronOS/Flashing/Pinecil%20V2/)
- [TS80 / TS80P](https://ralim.github.io/IronOS/Flashing/TS80%28P%29/)
- [TS100](https://ralim.github.io/IronOS/Flashing/TS100)

## Key Features

- PID style iron temperature control
- Automatic sleep with selectable sensitivity
- Motion wake support
- All settings exposed in the intuitive menu
- (TS100) Set a voltage lower limit for Lithium batteries so you don't kill your battery pack
- (TS80) Set 18 W or 24 W settings for your power bank
- (TS80P) Automatically negotiates appropriate PD and falls back to QC mode like TS80
- (Pinecil) Supports all 3 power modes (PD, QC, DC In).
- (Pinecilv2) Supports USB-PD EPR for 28V operation.
- Improved readability Fonts, supporting multiple languages
- Use hardware features to improve reliability
- Can disable movement detection if desired
- Boost mode lets you temporarily change the temperature when soldering (i.e. raise the temperature for short periods)
- (TS100/Pinecil) Battery charge level indicator if power source set to a lipo cell count
- (TS80/TS80P/Pinecil) Power bank operating voltage is displayed
- [Custom boot up logo support](https://ralim.github.io/IronOS/Logo/)
- Automatic LCD rotation based on the orientation

## Menu System

This new firmware uses a new menu system to allow access to the settings on the device.
When on the main screen and having the tip plugged in, the unit shows a pair of prompts for the two most common operations.

- Pressing the button near the tip enters the _soldering mode_
- Pressing the button near the USB end enters the _settings menu_
- When not in _soldering mode_, holding down the button near the tip will enter _soldering temperature adjust mode_ (This is the same as the one in the _soldering mode_, but allows to adjust the temperature before heating up), in _soldering mode_ however this will activate _boost mode_ as long as you hold down the button.
- Holding down the button near the USB end will show the _[debug menu](https://ralim.github.io/IronOS/DebugMenu/)._ In _soldering mode_ this ends the heating.

Operation details are over in the [Menu information.](https://ralim.github.io/IronOS/Menu/)

## Translations

Is your preferred language missing localisation of some of the text?
Translations are stored as `json` files in the Translations folder.
PR's are loved and accepted to enhance the firmware.

## Thanks

If you love this firmware and want to continue my caffeine addiction, you can do so [here](https://paypal.me/RalimTek) (or email me for other options).
I also want to give a shout out to all of the [Fantastic Contributors](https://github.com/Ralim/IronOS/graphs/contributors).

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
