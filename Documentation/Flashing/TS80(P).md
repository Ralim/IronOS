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

# TS80 / TS80P

This is completely safe, but if it goes wrong just put the `.hex` file from the official website ([TS80](https://www.minidso.com/forum.php?mod=viewthread&tid=868&extra=page%3D1)/[TS80P](https://www.minidso.com/forum.php?mod=viewthread&tid=4070&extra=page%3D1) onto the unit and you're back to the old firmware. Downloads for the `.hex` files to flash are available on the [releases page.](https://github.com/Ralim/IronOS/releases) The file you want is called TS80.zip or TS80P.zip. Inside the zip file (make sure to extract the file before flashing with it) will be a file called `TS80_{Language-Code}.hex`/`TS80P_{Language-Code}.hex`.

Officially the bootloader on the devices only works under Windows (use the built-in File Explorer, as alternative file managers or copy handlers like Teracopy will fail). However, users have reported that it does work under Mac, and can be made to work under Linux _sometimes_. Details over on the [wiki page](https://github.com/Ralim/TS80/wiki/Upgrading-Firmware).

1. Hold the button closest to the tip (MHP30 the left button on the back), and plug in the USB to the computer.
2. The unit will appear as a USB drive. (Screen will say `DFU` on it.)
3. Drag the `.hex` file onto the USB drive.
4. The unit will disconnect and reconnect.
5. The filename will have changed to end in _.RDY_ or _.ERR_
6. If it ends with _.RDY_ you're done! Otherwise, something went wrong.
7. If it didn't work the first time, try copying the file again without disconnecting the device, often it will work on the second shot.
8. Disconnect the USB and power up the device. You're good to go.

For the more adventurous out there, you can also load this firmware onto the device using an SWD programmer, for easier installation follow the guide at the end of this document.

On the USB port, `USB_D+` is shorted to `SWDIO` and `USB_D-` is shorted to `SWCLK` so debugging works without disassembly (attach while staying in the bootloader). Installing [IronOS-dfu](https://github.com/Ralim/IronOS-dfu) is recommended as it allows reliable flashing of binary files with [dfu-util](http://dfu-util.sourceforge.net/).

## Mac

sgr1ff1n (Shane) commented in [issue 11](https://github.com/Ralim/IronOS/issues/11) that upgrading worked on their Mac as per normal:

> I just wanted to say that I was able to update the firmware on my TS100 from the stock version to 1.08 found in this repository using my Mac. I simply followed the same steps however through Finder. I have a MacBook Pro (13-inch, Mid 2012) running Sierra 10.12.4 (16E195).

## Linux

While in the past there were reports of unreliable upgrades, the consensus in [issue 11](https://github.com/Ralim/IronOS/issues/11) is that things work mostly as expected in Linux.

@awigen has contributed a script [flash_TS100_linux.sh](https://raw.githubusercontent.com/Ralim/IronOS/master/Flashing/flash_TS100_linux.sh) that works on Ubuntu 16.04 as well as other distros.

If you want to do it manually (or if the script does not work for some reason) the general procedure is the same as for Windows, the differences are in the way to mount the unit and copy the firmware.
Remember that after flashing, the firmware filename will have changed to end in `.RDY` or `.ERR` or `.NOT` and only `.RDY` means the flashing was successful!

- The unit has to be mounted as `msdos` type (thanks @balrog-kun for having spotted it). You may disable automount, but unmounting the automounted drive and remounting as `msdos` works fine. You do not need to turn off automounting, but you do need to unmount the device with `umount`.
- It is recommended to use an all-caps filename for the firmware, even if successful flashing were done with lower case names.
- Avoid USB hubs, plug directly in your computer.
- If it fails, try again several times without unplugging. Just let it remount.

Example, to be run as root, once the unit has been plugged in DFU mode and auto-mounted:

```bash
FW=TS80.hex
unset NAME
eval $(lsblk -P -p -d --output NAME,MODEL|grep "DFU[ _]Disk")
[ -z ${NAME+x} ] && exit 1  # Could not find DFU device
umount "$NAME"
mkdir /tmp/mntdfu
mount -t msdos "$NAME" /tmp/mntdfu
cp "$FW" "/tmp/mntdfu/$(basename $FW|tr a-z A-Z)"
sync
umount /tmp/mntdfu
rmdir /tmp/mntdfu
```

Device will reboot and automount will rerun if not disabled.
Check the extension of your firmware, it should be `.RDY` now.

## FAQ

#### The file is showing up with the extension `.ERR`

This can occur during the programming process if any of the checks in the bootloader fail. This is often triggered by anti-virus software or using a non-Windows host OS.

First, try just copying the file a second time.

1. Attach the iron in DFU mode.
2. Copy the `.hex` file to the device.
3. The device disconnects and connects with the `.ERR` file.
4. Copy the same `.hex` file again **⛔ DO NOT TRY AND DELETE THE OLD ONE ⛔**.
5. The device will disconnect and reconnect again.
6. The device _should_ now have the `.RDY` file.
7. You're done.

If this fails and you are on Mac or Linux reading the wiki page about programming can help. There is also a very long issue thread going through all of the different attempts around this too.

If you are on Windows, it's often best to try another computer (friends, work, partners etc.).

#### Device randomly disconnects or does not show up in DFU mode

1. Check if the USB cable you are using has the data pins; test it on another device. There are a surprisingly large number of micro-USB cables that are power _only_.

2. Try other USB ports. Often different USB controllers will interact with the units differently due to design quirks in the Miniware design.

### Alternative bootloader

If you are an advanced user, and you have used `usb-dfu` tools before, or you would like to learn; there is an alternative bootloader for these irons.
This will **NOT** show up as a USB storage drive, but instead show up using a standard DFU protocol device. You can then use dfu tools or GUIs to upgrade the iron using the `.bin` files that are posted to the releases page.

To install this alternative bootloader, follow the instructions [here](https://github.com/Ralim/IronOS-dfu/blob/mainline/docs/Bootloader.md).

Note that this is only recommended for users who know what they are doing. If you don't understand how this works, please don't flash this.
