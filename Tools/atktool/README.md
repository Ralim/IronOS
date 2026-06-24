# atktool

A small C CLI toolkit for the Alientek **T90** (Nations N32L40x) soldering iron firmware images.

It packs/unpacks the device's `.atk` image format and (experimentally) flashes over USB, so you can
build and load IronOS on the T90 without the official Windows updater.

## Build

```sh
cd Tools/atktool
make                 # pack/unpack/info always build; probe/flash need hidapi
```

`probe`/`flash` require **hidapi**:
- macOS: `brew install hidapi`
- Debian/Ubuntu: `apt install libhidapi-dev`

then `make` again (it auto-detects hidapi via `pkg-config`).

## Commands

```
atktool pack    <in.bin> <out.atk> [--date YYYY-MM-DD]   wrap a raw app image into an .atk
atktool unpack  <in.atk> <out.bin>                        extract the raw app image
atktool info    <file.atk>                                print header fields + vector table
atktool probe   [--vid V] [--pid P]                       find the iron in update mode (hidapi)
atktool flash   <in.atk> [--vid V] [--pid P] --force      flash over USB (hidapi, EXPERIMENTAL)
```

## .atk format

Reverse-engineered and verified by byte-exact round-trip against the official factory `t90.atk`
and PanKleszcz's `t90_msdmem.atk`:

| offset | size | field |
|--------|------|-------|
| 0      | 6    | magic `08 02 0A 01 A0 20` |
| 6      | 4    | payload size (uint32 little-endian) |
| 10     | 1    | year − 2000 (build date, metadata only) |
| 11     | 1    | month |
| 12     | 1    | day |
| 13     | N    | application image, **each byte XOR 0xFF** |

There is no payload checksum or signature; the application is linked at `0x08005000` (the 20 KB
bootloader lives below it). The older 32-byte ASCII-name header (`dozed-dev/atk-fw-util`) is the
**T80P** format and does NOT work on the T90.

## Flashing (read before using `flash`)

Entering update mode: **hold the B button, plug in USB-C, power on** — after ~4 s the screen shows
`Upgrade...` and the device enumerates on the USB bootloader (assumed VID `0x413D` / PID `0x2107`,
same family as the T80P; override with `--vid/--pid` if yours differs — check `atktool probe`).

The USB protocol is the Alientek HID bootloader protocol reverse-engineered from the T80P
(`dozed-dev/atk-fw-util`): 64-byte reports `[0xFB, fn, seq, len, data…, crc16]` (CRC-16/MODBUS),
function codes `0x10 get-info, 0x11 set-fw-info, 0x12 start, 0x13 data, 0x14 end`, 58-byte chunks.

**`flash` is EXPERIMENTAL on the T90** — the update-mode VID/PID and exact chunking are not yet
fully confirmed, so it refuses to run without `--force`. Before using it:
1. Keep the **factory `t90.atk`** so you can always re-flash and recover.
2. Ideally confirm the protocol with a USB capture of the official updater.
3. First-boot a freshly flashed iron **without a soldering tip installed** until the firmware is
   validated, so a wrong heater configuration cannot heat anything.

## Examples

```sh
# build a flashable image from the IronOS T90 build
atktool pack ../../source/Hexfile/T90_EN.bin T90_EN.atk

# inspect an image
atktool info T90_EN.atk

# recover the factory firmware
atktool flash t90-factory.atk --force
```
