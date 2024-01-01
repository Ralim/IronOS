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

For the _latest_ code, you will need to download the zip file from the artifacts page on the build for what you want.
Head to the [Actions](https://github.com/Ralim/IronOS/actions) page and then select the run for the appropriate branch you would like.
In general you probably want `master`.

Once you click on a run, scroll down to the "Artifacts" section and then click on your model to download a zip file.
Then this works the same as a production release (use the correct file).

# Pinecil V1

- The MCU used in Pinecil supports usb-dfu. Reference [Pinecil Wiki](https://wiki.pine64.org/wiki/Pinecil) for hardware and firmware instructions.
- Recommended Updater for Windows/MacOS: [Pine64 Updater](https://github.com/pine64/pine64_updater) is an easy-to-use GUI app. It is fast and automatically fetches the newest stable version of IronOS from GitHub. It can also be used to load custom boot logo art.
- Recommended Updater for Linux/MacOS: [PineFlash](https://github.com/Spagett1/PineFlash) is an easy-to-use GUI app. It is fast and automatically fetches the newest stable version of IronOS from Github. It can also be used to load custom boot logo art.

- Troubleshooting: if you have issues using the Pine64 Updater or your install fails, please go to troubleshooting tips below.
- The [Pinecil Wiki](https://wiki.pine64.org/wiki/Pinecil) is a great resource for all things Pinecil.
- Community chat: if troubleshooting doesn't work, then join the Pine64 > Pinecil channel [here](https://wiki.pine64.org/wiki/Pinecil#Live_Community_Chat). There are knowledgeable members in Discord/Telegram/Matrix. Discord has a bridge bot connection to Telegram and Matrix so that all pine64 volunteers/members can see advice for Pinecil and related items or just get tips on which power supply to purchase.
- One advantage of Pinecil is that you cannot permanently damage it doing a firmware update (because DFU is in ROM); an update could render Pinecil temporarily inoperable if you flash an invalid firmware. But no worries, simply re-flashing with a working firmware copy will fix everything.
- USB-C cable is required to do an update. Generally, all USB controllers work, but some hubs have issues, so it is preferred to avoid USB hubs for updates.
- Alternate Update Methods: if your OS is not currently supported by the [Pine64 Updater](https://github.com/pine64/pine64_updater) or it does not meet your needs, i.e., you want to install a beta version, the below manual methods may be used.

## Linux and Mac

### Steps

⛔ Do not use the DC barrel jack while updating firmware or you may destroy your PC. ⛔

1. Highly recommend updating `dfu-util` to the newest version before starting.
2. Download and extract the firmware package from GitHub [IronOS Releases](https://github.com/Ralim/IronOS/releases).
3. Enter DFU mode: press and hold (`-`) button at the back of the iron before you connect the USB-C cable.
4. Connect USB to PC, and USB-C to back of Pinecil, keep holding (`-`) button down.
5. Once the USB cable is connected at two ends, wait ~10 seconds more, then release the (`-`) button.
6. The screen will stay **black/off** to indicate the Pinecil is in DFU mode. This is normal.
7. Using `dfu-util` you can flash the firmware using a command line like this:

```
dfu-util -D Pinecil_EN.dfu
```

Choose the file name from the folder with the appropriate 2-letter country code for your chosen language (i.e., EN = English).

### Troubleshooting:

- If you get a message stating that `More than one DFU capable USB device found!` when running the above command you probably have an old version of `dfu-util` installed. Might be worth updating. You can still install on the old version, but you will have to specify which DFU interface to flash to. Running the command `dfu-util -l` will show you if there are several DFU devices detected. Example:

```
Found DFU: [28e9:0189] ver=0100, devnum=48, cfg=1, intf=0, path="1-1", alt=1, name="@Option Bytes  /0x1FFFF800/01*016Be", serial="??"
Found DFU: [28e9:0189] ver=0100, devnum=48, cfg=1, intf=0, path="1-1", alt=0, name="@Internal Flash  /0x08000000/128*001Kg", serial="??"
```

In this example we see that more than one part of the Pinecil is detected as a DFU interface and we need to specify which one we want to flash to. We want the `Internal Flash` so in this case we can use `alt=0` to identify which interface to target. The command would then look like this:

```
dfu-util -D Pinecil_EN.dfu -a 0
```

- Note: if you use an older release of `dfu-util` and do not see `alt=0, name="@Internal Flash /0x08000000/128*001Kg"` when running `dfu-util -l` you likely will not be able to update without first updating 'dfu-util'.
- If your update is crashing part-way into the update, there is sometimes an issue with older/fussy USB controllers (they can show up/disappear/then show up again)
  - Try a direct connection to the USB port, do not use a USB hub, and use shorter cable. If possible, pick a port connected to the main board.
  - Switch to a different PC/Laptop and use different ports. USB-C ports are recommended but some have also reported having a fussy C port.
  - Hold down the (-) button for the entire firmware update, do not release until near the end.
- `DC Low` message: a pc/laptop cannot fully power Pinecil, it generally can only get 5 V (non-PD) to communicate for firmware updates and Pinecil will report 'DC Low'. This is normal.
- If `dfu-util` aborts with an error like
  ```
  dfu-util: Cannot open DFU device 28e9:0189 found on devnum 42 (LIBUSB_ERROR_IO)
  ```
  and `dmesg` reports USB errors like these
  ```
  kernel: usb 1-1: reset full-speed USB device number 42 using xhci_hcd
  kernel: usb 1-1: device descriptor read/64, error -71
  kernel: usb 1-1: device descriptor read/64, error -71
  kernel: usb 1-1: reset full-speed USB device number 42 using xhci_hcd
  kernel: usb 1-1: device descriptor read/64, error -71
  kernel: usb 1-1: device descriptor read/64, error -71
  kernel: usb 1-1: reset full-speed USB device number 42 using xhci_hcd
  kernel: usb 1-1: Device not responding to setup address.
  kernel: usb 1-1: Device not responding to setup address.
  kernel: usb 1-1: device not accepting address 42, error -71
  ```
  then try to disable USB autosuspend.
  This can be done with a set of udev rules specifically for the Pinecil:
  ```udev
  SUBSYSTEM=="usb", ATTR{idVendor}=="28e9", ATTR{idProduct}=="0189", MODE:="0660"
  SUBSYSTEM=="usb", ATTR{idVendor}=="28e9", ATTR{idProduct}=="0189", GROUP="plugdev"
  SUBSYSTEM=="usb", ATTR{idVendor}=="28e9", ATTR{idProduct}=="0189", TEST=="power/control", ATTR{power/control}="on"
  ```

## Windows

Two Options for Windows

### Option 1: use command line

### Steps

⛔ Do not use the DC barrel jack while updating firmware or you may destroy your PC. ⛔

1. Using command line `dfu-util` is similar to above for Linux / Mac.
2. Highly recommend updating `dfu-util` to the newest version.
3. Download and extract the firmware package from GitHub [IronOS Releases](https://github.com/Ralim/IronOS/releases).
4. Enter DFU mode: press and hold (-) button at the back of the iron (do not release).
5. Connect USB to PC, and USB-C to the back of Pinecil, keep holding (-) button down.
6. Screen will stay **black/off** to indicate the Pinecil is in DFU mode. This is normal.
7. After the USB cable is connected at both ends, wait ~10 seconds more, then release the (-) button.
8. Open PowerShell or Command window.
9. Change to the directory of the unzipped firmware files
10. Using `dfu-util,` flash the firmware using a command like this:

```
dfu-util -D Pinecil_EN.dfu
```

- If you have errors, see Troubleshooting above.

### Option 2: use the GUI tool from chip vendor

### Steps

⛔ Do not use the DC barrel jack while updating firmware or you may destroy your PC. ⛔

1. If you are uncomfortable with the command line, then this chip vendor supplied GUI tool/drivers is an option.
2. Download and extract the firmware package from GitHub [IronOS Releases](https://github.com/Ralim/IronOS/releases).
3. Download both the `GD32 MCU DFU TOOL` and the `GD32 Dfu Drivers`.
   - GD32 DFU Tool [here](http://www.gd32mcu.com/en/download?kw=GD32+MCU+Dfu+Tool&lan=en). If the link breaks, search for "GD32 MCU Dfu Tool" at this [link](http://www.gd32mcu.com/en/download/).
   - GD32 DFU Drivers [here](http://www.gd32mcu.com/en/download?kw=GD32+Dfu+Drivers&lan=en). If the link breaks, search for "GD32 Dfu Drivers" at this [link](http://www.gd32mcu.com/en/download/).
   - Check properties of both downloads, tick Unblock if needed, then Unzip
4. Install the drivers and the GD32 DFU tool (ignore prompts to update the tool).
5. Enter DFU mode: press and hold (`-`) button at the back of Pinecil (do not release).
6. Connect Pinecil to a PC via USB cable (do not release the (`-`) yet).
7. Screen will stay **black/off** to indicate the Pinecil is in DFU mode. This is normal.
8. You may hear a beep from Windows as it connects to Pinecil in DFU mode.
9. If you see windows notification that it `does not recognize USB device`, then you didn't connect, repeat step 3-8.
10. Open the GD32 DFU Tool (ignore prompts to update tool).
11. At the top of the DFU tool, you should see `GD DFU DEVICE 1` appear if you successfully connected Pinecil.
12. If DFU Device box at top is blank, then Pinecil is not connected in DFU mode, repeat steps 3-11.
13. If it has been more than 10 seconds since you connected the USB cable, Release the (`-`) button. (don't use Upload from Device section)
14. Select `Download to device` > Open > Browse to folder you unzipped in step 2.
15. Select the `hex` file for language. English is Pinecil_EN.hex , tick `Verify after download`.
16. Click `OK` at bottom. After a few minutes you will see 0-100%, Download successfully! Click `Leave DFU` at the top.
17. Disconnect Pinecil cable from PC, plug it into a power supply.
18. Do not need to press any buttons, a new screen should appear.
19. To confirm upgrade, hold the minus (`-`) button down for a few seconds, it then shows new firmware version v2.xx.x....date

- If you have errors, see Troubleshooting above.
