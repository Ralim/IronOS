# Flashing / Upgrading your iron

## Downloading source file

In the development of this firmware, there are three _types_ of firmware released.
These are the "Main" stable releases, which generally have high confidence in being bug free.
Release candidates are released slightly more often, and these are generally perfectly fine for everyday use. These are released early to allow for translation checking and for wonderful people to help spot bugs and regressions.
Finally, there are the "mainline" builds, which are built from the main git branch.
These are built on every change and can be found on the Actions tab (see below).

### Main release

Main releases are made to the [releases page](https://github.com/Ralim/IronOS/releases).
Download the zip file that matches your model of soldering iron and extract it.
Select the appropriate file type for your unit, in general Miniware devices need `.hex`, Pinecil V1 needs `.dfu`, and Pinecil V2 needs `.bin`.
Flash according to details below.

### Bleeding edge / latest

For the _latest_ code, you need to download the zip file from the artifacts page for the build that you want.
Head to the [Actions](https://github.com/Ralim/IronOS/actions) page and then select the run for the appropriate branch and beta you would like.
In general you probably want `master`.

Once you click on a run, scroll down to the "Artifacts" section and then click on your device model name to download a zip file.
Then this works the same as a production release (use the correct file).

## Pinecil V2

- The MCU in Pinecil V2 is Bouffalo BL706 and does _not_ use usb-dfu for flashing as the previous Pinecil V1 MCU did.
- See the Pinecil Wiki page [here](https://wiki.pine64.org/wiki/Pinecil#Firmware_&_Updates) for instructions.
- The V2 uses the [BLISP flasher](https://github.com/pine64/blisp) to upload the firmware to the MCU.
- The [Pinecil Wiki](https://wiki.pine64.org/wiki/Pinecil) is a great resource for all things Pinecil.
- Community chat: if there are issues updating, then join the Pine64 > Pinecil channel [here](https://wiki.pine64.org/wiki/Pinecil#Live_Community_Chat). There are knowledgeable members in Discord/Telegram/Matrix. Discord has a bridge bot connection to Telegram and Matrix so that all pine64 volunteers/members can see advice for Pinecil and related items or just get tips on which power supply to purchase.
- One advantage of Pinecil is that you cannot permanently damage it doing a firmware update (because BIN is in ROM); an update could render Pinecil temporarily inoperable if you flash an invalid firmware. But no worries, simply re-flashing with a working firmware copy will fix everything.
- USB-C cable is required to do an update. Generally, all USB controllers work, but some hubs have issues, so it is preferred to avoid USB hubs for updates.
- Background on the [BL706 chipset](https://lupyuen.github.io/articles/bl706)

### Troubleshooting

If you are running into issues such as timeouts during the programming or bootloader errors, the BL702 has a not-amazing USB PHY built in. This can cause problems on cheap cables (especially "thin" ones that tend not to have shielding). One of the authors (Ralim) has found this especially common on the cables supplied with Apple chargers when used with newer Ryzen processor ports.

It is _strongly_ reccomended to use a good quality cable, ideally _short_.
Also try other USB ports, as on some devices they can use different hub's or lengths of signalling, and this can fix the issue.

By the PinecilV2's design, by default some of the internal buses are exposed on the USB3 pins, to enable hacking/debugging/mods. This is suspected it _may_ play poorly on some chipsets. Try using a USB2.0 cable. Others have had luck with chaining USB-C->USB-A->USB-C. This may be due to this, as a lot of these adaptors are USB2 or only USB3 5gbps (half USB3 pins).

Another workaround is to put a USB hub somewhere in the chain, as these will re-form the signal and can work around the issue.

_Finally_, some users have reported issues under Windows that were fixed by changing OS (Typically to a Linux live cd).
