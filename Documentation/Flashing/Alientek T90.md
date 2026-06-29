# Alientek T90

The T90 ships a closed **USB HID "update-mode" bootloader** (it does **not** enumerate as a USB mass
storage drive like the Miniware irons, and there is no exposed SWD header by default). IronOS is
flashed with the small `atktool` HID utility in this repository, which packs the firmware into the
Alientek `.atk` image format and streams it over the bootloader protocol.

> Flashing is hardware-affecting and only works while the iron is in update-mode. Always `probe`
> first and confirm before `flash`. If anything goes wrong, recovery is reflashing the factory
> `t90-factory.atk`.

## 1. Get the firmware

Download `T90.zip` from the [releases page](https://github.com/Ralim/IronOS/releases) (or the build
artifacts for a mainline build) and extract the `T90_<Language-Code>.atk` file.

To build it yourself, from the repository root:

```
docker run --rm -v "$PWD":/build/ironos -w /build/ironos/source ironos-build:latest \
  make model=T90 -j8 firmware-EN
```

The output is `source/Hexfile/T90_EN.atk` (plus `.bin/.hex/.elf`). `firmware-<LANG>` selects other
languages. The application links to flash base `0x08005000` (the 20K bootloader sits below it).

## 2. Build `atktool` (once)

`atktool` lives in `Tools/atktool/` (a small C CLI with a Makefile; it needs `hidapi`). Build it with
`make` in that directory if a binary is not already present.

## 3. Enter update-mode and probe

To enter update-mode: **hold button B, plug in USB, and keep power for ~4 seconds** — the screen shows
`Updating...`. Confirm over HID:

```
atktool probe
```

A good reply shows `found device at VID 19F5 PID 3245`, a valid 21-byte GET_INFO reply and
`PROTOCOL CONFIRMED`. If `probe` finds nothing, the iron is not in update-mode — redo the
B + USB + power sequence.

## 4. Flash

```
atktool flash source/Hexfile/T90_EN.atk --force
```

It streams the image in 32-byte chunks and commits with **SetBoot (fn 0x15)**, which finalises the
freshly written image and reboots into it. On success it prints `flashed N bytes in M chunks` and the
iron boots the new firmware.

## 5. Recovery

If the new app does not boot (black/blank screen, or stuck on `Updating...`), reflash the factory
image to recover:

```
atktool flash t90-factory.atk --force
```

## Notes

- `atktool unpack <in.atk> <out.bin>` strips the 13-byte header and de-obfuscates the payload
  (XOR 0xFF) to the raw application, which maps to `0x08005000` — useful for inspecting the stock
  firmware. `atktool info <file.atk>` prints the header and vector table without unpacking.
- The T90A / T90B / T90C are different cartridge families (C210 / C245 / T65) and are **not**
  interchangeable. `model=T90` targets the C245-based variant.
- A black screen or a stuck `Updating...` after flashing usually means the commit did not boot the app
  or the app faulted at startup — reflash the factory image and recheck the build.
