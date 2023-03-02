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
Flash according to details below

### Bleeding edge / latest

For the _latest_ code, you need to download the zip file from the artifacts page for the build that you want.
Head to the [Actions](https://github.com/Ralim/IronOS/actions) page and then select the run for the appropriate branch and beta you would like.
In general you probably want `master`.

Once you click on a run, scroll down to the "Artifacts" section and then click on your device model name to download a zip file.
Then this works the same as a production release (use the correct file).

# Pinecil V2

- The MCU in Pinecil V2 is Bouffalo BL706 and does _not_ use usb-dfu for flashing as the previous Pinecil V1 MCU did.
- See the Pinecil Wiki page [here](https://wiki.pine64.org/wiki/Pinecil#Firmware_&_Updates) for instructions.
- The V2 uses the [BLISP flasher](https://github.com/pine64/blisp) to upload the firmware to the MCU.
- Background on the [BL706 chipset](https://lupyuen.github.io/articles/bl706)
