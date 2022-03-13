# Startup Logos

This firmware supports a user created bootup logo.
By default there is _not_ one included in the firmware, as this means that once flashed they generally stay.

## Generating the Logo files

The [Python script](https://github.com/Ralim/ts100/blob/master/Bootup%20Logo/python_logo_converter/img2ts100.py) converts an image passed into it on the command line to a HEX file to be uploaded to the iron in DFU mode (similar to the process described above). The image can be in color and any size, but it will be resized and converted to 1-bit color. However, it looks best if you create a 96x16 image in an image editor and color the pixels black or white manually.

The converter requires at least Python 3 and Pillow (if you don't have it, it will tell you to install PIL, which is an old version of the same thing). See [this page](https://stackoverflow.com/a/20061019/6705343) on StackOverflow about installing it.

What works can vary, but this command may work:

`python3 -m pip install Pillow`

Then, to convert an image:

`python3 img2ts100.py infile.png outfile.hex`

Run `python3 img2ts100.py --help` to see available options.

## Flashing the Logo

### Miniware (TS100/TS80/TS80P)

Upload the HEX file to the iron in DFU mode and, if the file's extension changes to .RDY, your custom splash screen should show up on startup.
You perform this the same way as if you were flashing a new firmware, and all of the existing notes around this apply.

### Pinecil

For Pinecil, we require using dfu-util instead to flash the logo art (Pinecil does not use hex).
To flash the logo, use the following steps to convert the PNG image into a .dfu file and then install the .dfu file.

 - `python3 img2ts100.py input.png logo.dfu`
 - `dfu-util -D logo.dfu`

 The converter will create a DFU instead of a HEX file if the .dfu extension is used. 
