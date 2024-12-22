# Startup Logo / Animation

When the device starts, you can have it optionally show either a static image or an animation. You can also set if these should stay on the screen or dismiss after some amount of time.
These can be an elegant way to personalise your device or just mark it as your one at a meetup where there may be multiple.

All devices supported by IronOS support this logo, and follow a similar process for setting one up. Please read the below general information as well as any model specific notes.

Bootup logos are stored at the end of the flash storage in the Iron; next to the user settings. By locating them at the end of storage they are not erased during the normal firmware upgrade process. Once a logo is set it should stay (unless we need to change things in the main firmware); so to erase your logo you will also find that we generate an erase file. Alternatively your method of flashing _may_ support doing a full erase flash which will also work for this.

## Generating the Logo files

Because logos are stored at a fixed location in the device's internal flash; we can use the same method to flash these as you would normal firmware.
This does also mean that we need to convert the image/animation file into the format that IronOS understands.

IronOS uses a pre-processed file format to dramatically reduce the amount of space required to store the image; allowing for animations and saving space.

In the [IronOS-Meta](https://github.com/Ralim/IronOS-Meta) repository is a `python` script to convert images into this pre-processed file format.
Additionally, memebers of the community have contributed back their logo images as well. We provide these pre-converted for all models and ready to use in [IronOS-Meta/releases](https://github.com/Ralim/IronOS-Meta/releases).
Download the zip for Pinecil or Miniware and then install using the instructions in the Flashing section below.

If you want to make custom art then it needs to be converted with the Python script.
You can checkout the repository or use the download-as-zip button in the Github web interface to download the code.

Inside the download code is a `Boot Logos` folder, inside here is the python script required for logo conversion.
It is easiest if you copy your logo file to be converted into this folder too, in order to keep commands shorter.

The image can be in color and any size, but it will be resized and converted to 1-bit color. However, it looks best if you create a 96x16 image (`png` or `bmp`) in any image editor and color the pixels black & white manually. The thresholding used for converting colour to B&W may not always work as well as one would hope.

The converter requires at least Python3 and Pillow apps. Follow online instructions for installing Python and Pillow on your machine. Any reasonably recent version should work well.

When running the script on the Windows operating system; it is recommended to use `Powershell` rather than the old `Command Prompt`.

For installing pillow; you can install it via your package manager (Debian and similar distros) or via pip. To install via pip the command should be `python -m pip install pillow`.

In your shell you can now execute `python img2logo.py input.png out -m ${model}` to convert the file `input.png` and create output files in the folder `out`.
The model should be replaced by one of the following options:

- `miniware` for older Miniware Irons -> TS100, TS80, TS80P
- `pinecilv1` for the Pinecil V1
- `pinecilv2` for the Pinecil V2
- `ts101` for the Miniware TS101 [^1] [^2]
- `s60` for the Sequre S60 [^1]
- `mhp30` for the Miniware MHP30

Different models are used for different flash locations for the image storage.
This means that files are **not** interchangeable between devices. If you are flashing multiple devices you will need to create a different file for different models.

After processing its expected to have a `.hex` and `.dfu` file created to be used. Which one to use will depend on your device.

Note: make sure your image file is in the same folder as script files (img2logo.py, output_dfu.py, output_hex.py).

[^1] Note that these devices have larger resolution screens that the logo system supports right now. Fixes are coming for this soon, roughly scheduled for 2.23.
[^2] The TS101 requires extra steps, see below.

### TS101 Quirks

When Miniware designed the TS101 they cut cost by using an STM32 clone with some odd quirks. They also re-wrote their USB bootloader, which has introduced new bugs for us to deal with.
Their bootloader appears to have kept the existing limit of not being able to flash small hex files, but they no longer fall for the older "just repeat the content" trick and instead reject the file.
Additionally, while the MCU in use has 128K of flash, their bootloader (at least for me) fails to write to anything above 99K. It _looks_ like a watchdog reset or hard crash. Unsure.

This has flow on effects, where the settings can still be located in the upper ~28K of flash, but it cant be used for anything we flash over USB.
Of that 100K we can use, they waste 32K of it for their bootloader (Old bootloaders were 16K).
This means the main "app" of IronOS is limited to around 67K (100K-32K for bootloader, -1K for logo).

For this device the Logo is not located at the end of flash but instead at the last writable page (99K).

Additionally, as we need to do a large write, to avoid having to waste more flash space; the logo is merged with the normal firmware. This means that the firmware and logo are flashed together once.
Future updates can be done without merging as it will leave the logo data there as normal firmware doesnt touch that area of flash.

To do this, download the latest version of IronOS and merge it with the logo using the `--merge` command line argument.

To create the logo file for a TS101 the full command looks like `python3 img2logo.py <image file path> <output folder path> -m ts101 --merge <Path to main firmware>`.

For this reason, there are no TS101 logo's generated by the IronOS-Meta repo.

## Flashing the Logo

### Upload via virtual disk (TS100,TS101,TS80,TS80P,S60,MHP30)

If you normally update your firmware by having your device show up as a flash drive this is the method for you.
This applies to all Miniware + S60 devices running the stock DFU bootloader.

Place your device into update mode (usually by holding the B button when connecting your device to your pc via USB).
Upload the `.hex` file you created earlier as if it was a firmware update. Do any normal tricks required for firmware flashing if any are required.
Afterwards the firmware should indicate that it has worked (often by creating a `.rdy` file).

At this point unplug your iron and re-connect it to power to start normally and the logo should welcome you.

### Upload via GUI flash tool (PinecilV1/V2)

If you normally upload your firmware using a helper application, they should accept the files from the bootlogo the same as the normal firmware.
Try the `.dfu` file first and then the `.hex`. If neither work then the application may not be updated to be able to handle boot logos. And you may need to use a different/newer tool.

### Upload via dfu-util (PinecilV1/IronOS-DFU)

For the PinecilV1 and for any devices that have been converted to use `IronOS-DFU` as the bootloader you can flash these via the `dfu-util` command line tool.
For these flash as per usual using the `.dfu` file. Afterwards power cycle and the logo should show up.

### Upload via blisp (PinecilV2)

For the PinecilV2 we suggest `blisp` as the command line tool to use if you are not using a GUI tool. `blsip` has been updated to accept `.dfu` files as well as the `.bin` files it historically used. As such you use the `.dfu` file for the logo and flash as per normal otherwise and it will work and reboot at the end. It should show you your new logo after flashing.
