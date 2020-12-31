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

For the Pinecil, we require to flash the logo using dfu-util instead, which will only take `.bin` files rather than `.hex`.
To flash the logo, use the following steps:


 - `python3 img2ts100.py input.png logo.hex`
 - `riscv-nuclei-elf-objcopy -I ihex -O binary logo.hex logo.bin`
 - `dfu-util -d 28e9:0189 -a 0 -D logo.bin -s 0x0800f800`

 This will use the objcopy tool to convert the hex to a binary file, and then use dfu-util to flash it in the right location.
 If you do not have `riscv-nuclei-elf-objcopy` installed, you can generally use any objcopy tool from any toolchain you do have.
