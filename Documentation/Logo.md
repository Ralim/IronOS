# Startup Logos

This firmware supports a user created bootup logo.
By default, there is _not_ one included in the firmware. This means that once flashed they generally stay. If you want no logo again, you would have to flash a blank image to the bootup logo. 

## Generating the Logo files

There are community logo's already converted and ready to use in [IronOS-Meta/releases](https://github.com/Ralim/IronOS-Meta/releases).
Download the zip for Pinecil or Miniware and then install using the instructions in the Flashing section below.

If you want to make custom art then it needs to be converted with a Python script. The script and other needed files are in [IronOS-Meta](https://github.com/Ralim/IronOS-Meta/). Go to that folder, then it is easiest to select the green Code button (upper right), then Download Zip. This way you get all the files you need and some extras. You only need what is inside Boot Logos. Put your custom image inside the Boot Logos folder with all python script files already there.

The Python script converts an image passed into it on the command line into both a `.hex` file and a `.dfu` to be uploaded to the iron in DFU mode. The image can be in color and any size, but it will be resized and converted to 1-bit color. However, it looks best if you create a 96x16 image (Png or Bmp) in any image editor and color the pixels black & white manually.

The converter requires at least Python3 and Pillow apps. Follow online instructions for installing Python and Pillow.

For Windows, it is recommended to use Windows PowerShell instead of Command.
Open Powershell (run as administrator), type python to install it, it will open microsoft store where you can install it free.
Go back to Powershell and install Pillow. What works can vary, but this command may work:

    python -m pip install Pillow
or 
    python3 -m pip install pillow

If the above does not work, see [this page](https://stackoverflow.com/a/20061019/6705343) on StackOverflow about installing Pillow.
Now that Python and Pillow are successfuly installed, you can convert an image.

Go back to Powershell and type this command (change infile.png to the name of your image):

- `python img2logo.py infile.png out -m`  for Miniware
- `python img2logo.py infile.png out -p`  for Pinecil

Run `python img2logo.py --help` to see available options. Replace the word python with python3 if you have multiple versions of python installed. 

Note: make sure your image file is in the same folder as script files (img2logo.py, output_dfu.py, output_hex.py).

## Flashing the Logo

### Miniware (TS100/TS80/TS80P)

Upload the HEX file to the iron in DFU mode and, if the file's extension changes to .RDY, your custom splash screen should show up on startup.
You perform this the same way as if you were flashing a new firmware, and all the existing notes around this apply.

If you have flashed the `IronOS-dfu` alternative bootloader, you should use the `.dfu` files instead

### Pinecil V1

For Pinecil V1, we require using dfu-util to flash the logo art (Pinecil does not use hex).
[Pine64 Updater](https://github.com/pine64/pine64_updater/releases) is the easiest way to load the Bootup logo onto Pinecil as it already includes the necessary DFU library. Connect Pinecil to a PC, and open the Updater the same as updating firmware.
  Select Custom > Browse to the DFU image file you just made > Update to install.

The bootup logo is stored in a separate location than the IronOS firmware and you do not have to worry about it changing or breaking the IronOS.

You could also use dfu-util and use Command line to install it.

- `dfu-util -D logo_file.dfu`
