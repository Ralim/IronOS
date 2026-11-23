# Flashing the Alientek T90B

IronOS `model=T90` supports the **T90B with a C245 cartridge**. The T90A and T90C use different cartridges.

USB flashing with `atk-flash` has been tested on **macOS**. Linux is expected to work through hidapi and still needs a device test. Windows flashing is untested, including use of Alientek’s Windows updater with an IronOS image.

## Get the firmware and flasher

Extract `T90_<language>.atk` from the T90 build artifact for the revision you want to install. To build the English image from the repository root with the IronOS toolchain, run

```sh
make -C source model=T90 firmware-EN
```

The build writes `source/Hexfile/T90_EN.atk`. Install the standalone USB flasher with HID support and make the `atk-flash` command available on your PATH. [atk-flash downloads and build instructions](https://github.com/vshuraeff/atk-flash)

## Prepare for recovery

Before updating, keep a compatible T90B stock application image in `.atk` format. Check the model and package on the [Alientek T90 download page](http://www.openedv.com/docs/tool/hanjie/T90.html). The `t90-factory.atk` used during development was a dump of the developer’s own iron.

PanKleszcz’s `t90_msdmem` utility produces a full flash backup (`FLASH.BIN`). The recovery command below takes a T90B application image in `.atk` format. [Backup utility discussion](https://github.com/Ralim/IronOS/issues/2195#issuecomment-3962598616)

## Install IronOS

1. Hold button **B** while connecting the iron with a USB-C data cable to the computer that runs `atk-flash`. Keep it held for about four seconds, until the screen shows the update mode.
2. Run `atk-flash probe`. It looks for the update-mode device (USB VID `19F5`, PID `3245`). Continue only if it ends with `ready to flash`.
3. Flash the image. For an English build, run this command from the repository root. For a downloaded artifact, substitute the extracted file’s path.

   ```sh
   atk-flash flash source/Hexfile/T90_EN.atk --force
   ```

After the update, check that the iron boots into IronOS. To restore stock firmware, re-enter update mode, run `atk-flash probe`, then supply the saved T90B recovery image.

```sh
atk-flash flash /path/to/t90b-recovery.atk --force
```
