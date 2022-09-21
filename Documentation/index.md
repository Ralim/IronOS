# IronOS - Flexible Soldering iron control Firmware

The firmware implements all of the standard features of a 'smart' soldering iron, with lots of little extras and tweaks.
I highly recommend reading the installation guide fully when installing on your iron. And after install just explore the settings menu.

For soldering irons that are designed to be powered by 'smart' power sources (PD and QC), the firmware supports settings around the negotiated power and voltage.
For soldering irons that are designed to be powered by batteries (TS100 & Pinecil), settings for a cutoff voltage for battery protection are supported.

Currently **31** languages are supported. When downloading the firmware for your soldering iron, take note of the language code in the file name.

This project is considered feature complete for use as a soldering iron, _so please suggest any feature improvements you would like!_

_This firmware does **NOT** support the USB port while running for changing settings. This is done through the onscreen menu only. Logos are edited on a computer and flashed like firmware._

|   Device   | DC  | QC  | PD  | EPR | BLE | Battery | Recommended |
| :--------: | :-: | :-: | :-: | :-: | :-: | :-----: | :---------: |
|   MHP30    | ❌  | ❌  | ✔️  | ❌  | ❌  |   ❌    |     ✔️      |
| Pinecil V1 | ✔️  | ✔️  | ✔️  | ❌  | ❌  |   ✔️    |     ✔️      |
| Pinecil V2 | ✔️  | ✔️  | ✔️  | ✔️  | ✔️  |   ✔️    |     ✔️      |
|   TS80P    | ❌  | ✔️  | ✔️  | ❌  | ❌  |   ✔️    |     ✔️      |
|   TS100    | ✔️  | ❌  | ❌  | ❌  | ❌  |   ✔️    |     ❌      |
|    TS80    | ❌  | ✔️  | ❌  | ❌  | ❌  |   ✔️    |     ❌      |

\*Please note that Miniware started shipping TS100's using cloned STM32 Chips. While these do work with IronOS, their DFU bootloader works terribly, and it is hard to get it to successfully flash larger firmware images like IronOS without timing out. This is the main reason why the TS100 is **_no longer recommended_**.

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
