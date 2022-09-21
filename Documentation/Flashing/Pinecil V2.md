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
Select the appropriate file type for your unit, in general Miniware devices need `.hex` and Pinecil needs `.dfu`.
Flash according to details below

### Bleeding edge / latest

For the _latest_ code, you will need to download the zip file from the artifacts page on the build for what you want.
Head to the [Actions](https://github.com/Ralim/IronOS/actions) page and then select the run for the appropriate branch you would like.
In general you probably want `master`.

Once you click on a run, scroll down to the "Artifacts" section and then click on your model to download a zip file.
Then this works the same as a production release (use the correct file).

# Pinecil V2

- The MCU in V2 is Bouffalo BL706 and does _not_ use usb-dfu for flashing as the previous MCU did.
- The current firmware (2.18) is very fresh and no upgrade is available/needed.
- When an update is released for V2, then IronOS will also include an update method to follow.
- Background on the [BL706 chipset](https://lupyuen.github.io/articles/bl706)
