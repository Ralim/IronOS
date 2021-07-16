# Flashing / Upgrading your iron

## Downloading source file

### Main release

Main releases are made to the [releases page](https://github.com/Ralim/IronOS/releases).
Download the zip file that matches your model of soldering iron, and extract it.
You then need to use the appropriate file type for your unit, in general Miniware irons are `.hex` and Pinecil is `.bin`.
Flash according to details below

### Bleeding edge / latest

For the _latest_ code, you will need to download the zip file from the artifacts page on the build for what you want.
Head to the [Actions](https://github.com/Ralim/IronOS/actions) page and then select the run for the appropriate branch you would like.
In general you probably want `master`.

Once you click on a run, scroll down to the "Artifacts" section and then click on your model to download a zip file.
Then this works the same as a production release (use the correct file).

## Miniware irons (TS100, TS80, TS80P)

This is completely safe, but if it goes wrong just put the .hex file from the [official website](http://www.miniware.com.cn/product-category/soldering-pen/electric-soldering-kit/) onto the unit and you're back to the old firmware. Downloads for the hex files to flash are available on the [releases page.](https://github.com/Ralim/IronOS/releases) The file you want is called _(MODEL)\_EN.hex_ unless you want the translations, they are (MODEL)\__language short name_.hex. Where (MODEL) is either TS100 or TS80.

Officially the bootloader on the iron only works under Windows (use the built-in File Explorer, as alternative file managers or copy handlers like Teracopy will fail). However, users have reported that it does work under Mac, and can be made to work under Linux _sometimes_. Details over on the [wiki page](https://github.com/Ralim/ts100/wiki/Upgrading-Firmware).

1. Hold the button closest to the tip, and plug in the USB to the computer.
2. The unit will appear as a USB drive. ( Screen will say `DFU` on it.)
3. Drag the .hex file onto the USB drive.
4. The unit will disconnect and reconnect.
5. The filename will have changed to end in .RDY or .ERR
6. If it ends with .RDY you're done! Otherwise, something went wrong.
7. If it didn't work the first time, try copying the file again without disconnecting the iron, often it will work on the second shot.
8. Disconnect the USB and power up the iron. You're good to go.

For the more adventurous out there, you can also load this firmware onto the device using an SWD programmer.

On the bottom of the MCU riser PCB, there are 4 pads for programming. On v2.51A PCB revision `USB_D+` is shorted to `SWDIO` and `USB_D-` is shorted to `SWCLK` so debugging works without disassembly (attach while staying in the bootloader). Installing [dapboot from eDesignOSS](https://github.com/eDesignOSS/dapboot) (`make TARGET=TS100 -C src`) is recommended as it allows reliable flashing of binary files with [dfu-util](http://dfu-util.sourceforge.net/).

There is a complete device flash backup included in this repository. (Note this includes the bootloader, so will need an SWD programmer to load onto the unit).

For the TS80 the SWD pins are used for the QC negotiation, so you can actually connect to the SWD power via the USB connector.

# Mac

sgr1ff1n (Shane) commented in [issue 11](https://github.com/Ralim/ts100/issues/11) that upgrading worked on their Mac as per normal:

> I just wanted to say that I was able to update the firmware on my ts100 from the stock version to 1.08 found in this repository using my Mac. I simply followed the same steps however through Finder. I have a MacBook Pro (13-inch, Mid 2012) running Sierra 10.12.4 (16E195).

# Linux

While in past there were reports of unreliable upgrades, the consensus in [issue 11](https://github.com/Ralim/ts100/issues/11) is that things work mostly as expected in Linux.

@awigen has contributed a script [flash_ts100_linux.sh](https://raw.githubusercontent.com/Ralim/ts100/master/Flashing/flash_ts100_linux.sh) that works on Ubuntu 16.04 as well as other distros.

If you want to do it manually (or if the script does not work for some reason) the general procedure is the same as for Windows, the differences are in the way to mount the unit and copy the firmware.
Remember that after flashing, the firmware filename will have changed to end in `.RDY` or `.ERR` or `.NOT` and only `.RDY` means the flashing was successful!

- The unit has to be mounted as `msdos` type (thanks @balrog-kun for having spotted it). You may disable automount, but unmounting the automounted drive and remounting as `msdos` works fine. You do not need to turn off automounting, but you do need to unmount the device with `umount`.
- It is recommended to use an all-caps filename for the firmware, even if successful flashing were done with lower case names.
- Avoid USB hubs, plug directly in your computer.
- If it fails, try again several times without unplugging. Just let it remount.

Example, to be run as root, once the unit has been plugged in DFU mode and auto-mounted:

```bash
FW=ts100.hex
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

## Pinecil (Pine64)

The MCU used in the Pinecil supports usb-dfu.
To enter DFU mode, hold the button at the back of the iron while connecting to usb over usb-c.
Generally all usb controllers work, but some hubs have issues so avoiding hubs may be best.
Once the device is connected while holding the button, the screen will stay **off**.
This is normal, and then you can use the appropriate tool to load the .hex or .bin file as per your OS.

You can also refer to the Pine64 [Wiki](https://wiki.pine64.org/wiki/Pinecil).

### Linux and Mac

Using `dfu-util` you can flash the firmware using a command line as such:

```
dfu-util -d 28e9:0189 -a 0 -D Pinecil_EN.bin -s 0x08000000:mass-erase:force
```

### Windows

For windows the easiest tool to use is the vendors gui tool from [here](http://www.gd32mcu.com/download/down/document_id/176/path_type/1).
Should the link break, its called "GD32 MCU Dfu Tool" and can be grabbed from http://www.gd32mcu.com/en/download/.
You will also need the matching drivers from [here](http://www.gd32mcu.com/download/down/document_id/244/path_type/1) installed if you have not used gigadevice parts before.
These are called "GD32 Dfu Drivers" if the link breaks.

### FAQ

#### [Miniware] The file is showing up with the extension `.ERR`

This can occur during the programming process if any of the checks in the bootloader fail. This is often triggered by anti-virus software or using a non-windows host OS.

First, try just copying the file a second time.

1. Attach the iron in DFU mode
2. Copy the hex file to the device
3. The device disconnects and connects with the `.ERR` file
4. Copy the same hex file again **DO NOT TRY AND DELETE THE OLD ONE**
5. The device will disconnect and reconnect again
6. The device _should_ now have the `.RDY` file
7. You're done.

If this fails and you are on Mac or Linux reading the wiki page about programming can help. There is also a very long issue thread going through all of the different attempts around this too.

If you are on windows, it's often best to try another computer (friends, work, partners etc).

#### [Miniware] Device randomly disconnects or does not show up in DFU mode

First, check the USB cable you are using has the data pins; test it on another device. There are a surprisingly large number of usb-micro cables that are power _only_.

Secondly, try other USB ports. Often different USB controllers will interact with the units differently due to design quirks in the miniware design.

## [Miniware] Alternative bootloader

If you are an advanced user, and you have used `usb-dfu` tools before, or you would like to learn; there is an alternative bootloader for these irons.
This will **NOT** show up as a USB storage drive, but instead show up using a standard DFU protocol device. You can then use dfu tools or GUI's to upgrade the iron using the `.bin` files that are posted to the releases page.

To change to dapboot based alternative bootloader, you need to flash the hex file from [here](https://github.com/eDesignOSS/ts100-bl-flasher/releases).
`ts100-stockbl.hex` will reflash the stock bootloader, `ts100-dapboot.hex` will flash the new dapboot based usb bootloader.

Note that this is only recommended for users who know what they are doing. If you don't understand how this works, please don't flash this.
