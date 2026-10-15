# Miniware TS21 port (experimental)

IronOS port for the Miniware TS21 soldering iron (210-series cartridges, 0.91 in OLED, two
buttons, front work light; [product page](https://e-design.com.cn/ProductDetail/10747118.html)).
Discussion: [#2122](https://github.com/Ralim/IronOS/issues/2122).

The default `ts21-ironos` build has the heater compiled out. `TS21_HEATER_TEST=1` turns it
on. `ts21-bringup` is a standalone diagnostic with no IronOS and no heater path. All of
them need `TS21_EXPERIMENTAL=1`; TS21 is not in the default `ALL_MODELS` and CI does not
build it. The heater protections below are software checks on uncalibrated ADC readings,
not a hardware current limit, and they do not cover a failed power stage.

## Hardware

There is no schematic. Pins, peripherals and ADC transfer functions come from the stock
v1.5.0 image ([Provenance](#provenance)) and bench work on one unit; the PCB revision is
unknown. The MCU is a WCH CH32V203-class RISC-V part (`-march=rv32imac`) with 20 KiB RAM;
the exact part is unconfirmed. IronOS runs it from HSI x 8 at 64 MHz (APB1 32 MHz, ADC
8 MHz) with an 8 MHz timebase and a 1 kHz RTOS tick. The diagnostic stays on the 8 MHz HSI.

The display is a 128x32 OLED at I2C address 0x3C on a bit-banged open-drain bus (PB10 SCL,
PB11 SDA); the IronOS builds do not blank it on inactivity. Buttons are active low; A is
PA7 (bottom, `BUTTON_F`) and B is PA5 (top) in the IronOS builds. The work light is PA9,
active high, set under Settings -> User interface -> Work light and independent of the
heater.

The tip is a thermocouple on ADC1 channel 1, read twice, with PA0 low for temperature and
with PA0 high as a probe; the difference scales to element resistance. Channel 2 is the
handle NTC and channel 9 the input voltage. As in stock, PB15 is high while sampling
(function unidentified) and channel 4 is read after it; nothing uses channel 4, and
nothing measures heater current. Drive is TIM3 CH1 on PA6 at 100 kHz, gated by a TIM1 CH3
one-pulse output on PA10. `TEMP_DIRECT_ADC` is set, so the microvolt debug fields read
"unavailable" and CJC calibration is not offered. PID mass and inertia are the Sequre
S60/S60P C210 values.

Not implemented: motion sensing, sleep (`NO_ACCEL`, `NO_SLEEP_MODE`) and USB-PD, although
Miniware lists an accelerometer with auto-sleep and PD input. USART2 receives frames on PA3
at 9600 baud from an unidentified sender; only the diagnostic displays them.

## Build

The tested toolchain is GCC 8.2.0 (prefix `riscv-none-embed-`) from
[Community-PIO-CH32V/toolchain-riscv-linux](https://github.com/Community-PIO-CH32V/toolchain-riscv-linux)
at commit `e8e7ba93c9c1ea30366d98f28318d4ad09b369e2`. Clone it to `$HOME/ts21-toolchain` on
a Linux filesystem (copies made on Windows lose its symlinks); on Windows run make under
WSL. General setup is in [Development.md](../../../../Documentation/Development.md).

```sh
export CROSS_COMPILE=$HOME/ts21-toolchain/bin/riscv-none-embed-
export TS21_EXPERIMENTAL=1

make ts21-ironos        # source/Hexfile/TS21_EN.hex, heater compiled out
make ts21-heater-test   # source/Hexfile/TS21-heater-test/TS21_font_compressed_EN.hex
make ts21-bringup       # source/Objects/TS21-bringup/TS21_BRINGUP_NOHEAT.elf (+ .map)
make ts21-bringup-hex   # the same, plus a .hex
make test-ts21          # host tests; needs neither the toolchain nor the gate
```

`source/Makefile` rejects `model=TS21` unless `TS21_EXPERIMENTAL=1` and adds TS21 to
`ALL_MODELS` only then. `TS21_HEATER_TEST` must be 0 or 1; 1 builds into
`Objects/TS21-heater-test` and `Hexfile/TS21-heater-test`. The IronOS targets always
rebuild (`make -B`) and leave each ELF next to its hex. The BSP Makefile's `probe` and
`probe-hex` need `EXPERIMENTAL=1`, which the root targets pass through. TS21 uses
`Translation_brieflz_font` because its fixed flash region needs both lossless compression
formats. Every hex is checked by `tools/inspect_image.py <elf> --hex <hex>` (ELF layout,
entry jump, vectors, flash bounds, stack, ELF/HEX equality), which prints
`layout-valid; hardware-unvalidated`.

## Flashing

There is no flash make target; the stock bootloader loads a `.hex` from a USB volume.

1. Unplug the iron, let it cool and remove the cartridge, and keep it out for the first
   boot of any image. With the heater image, check before fitting one that PA6 and PA10
   are low after reset and on a fault, and that PA10 falls 120 ms after a command.
2. Hold the A button (vendor procedure) while connecting a USB data cable. A FAT volume
   labelled `TS21_DFU` appears (USB `1A86:FE10`, "CH20xUDisk", 1 MiB); drive letters
   vary, so identify it by label and USB ID.
3. Copy exactly one `.hex` to its root under a short 8.3 name such as `TS21NFS.hex` (every
   image accepted so far had one). Never copy an ELF, a directory or several images.
4. The bootloader renames the file to `.rdy` on acceptance and `.err` on rejection; a
   finished copy says nothing either way. `.rdy` means accepted, not running. Disconnect
   USB and power the iron normally.

To go back to stock, or after `.err`, a blank display or a reset loop, re-enter DFU and
copy the official `TS21APPV1.5.0EN.hex` ([Provenance](#provenance)) alone; fetch it before
flashing anything. It is the vendor application, not a dump of the device, its settings or
its bootloader. An `.err` says this image was rejected, not that the previous one survived.
Going back to stock over this firmware has not been tried, and DFU is the only recovery
route known.

## Memory map

```
0x08000000-0x08006FFF  stock bootloader, 28 KiB, never written
0x08007000-0x0800FBFF  FLASH in ts21.ld, 0x8C00 bytes (35 KiB)
  0x08007000           .init, exactly one 32-bit jump
  0x08007004           .vectors, 256 bytes
0x0800FC00-0x0800FCFF  settings, one 256-byte page
0x0800FD00-0x0800FFFF  unused (the stock settings are at 0x0800FF00)
0x20000000-0x20004FFF  RAM, 20 KiB
  0x20004800-0x20004FFF  reserved 2 KiB stack, __stack_top = 0x20005000
```

This follows the stock v1.5.0 layout: a 32-bit jump at 0x08007000, vectors at 0x08007004
and the stack top at 0x20005000. `ts21.ld` asserts the `.init` size, the vector address and
size, and that `.bss` ends below the stack. The bootloader may hand over inside an
interrupt, so `startup.S` leaves that context through `mepc`/`mret` before starting
FreeRTOS, as the stock startup does. `IronOS/flash.cpp` masks interrupts, inhibits the
heater, erases with the WCH 256-byte fast page erase (a normal erase would wipe 4 KiB),
reads back with `memcmp` and calls `ts21_fault(TS21_ERROR_FLASH)` on failure. The write is
not power-loss atomic.

## Heater and faults

All of this exists only with `TS21_HEATER_TEST=1`. Without it `setTipPWM()` always inhibits
output (PA6 and PA10 low, TIM3 off) and the power limit is 0.

The heater is off at boot, and `showBootLogo()` forces `AutoStartMode`, `BoostTemp` and
`KeepAwakePulse` to 0 and `PowerLimit` to 100 W in RAM. On the home screen with no inhibit
error set, pressing and releasing A alone arms the heater, and the same short press starts
soldering. Power is permitted only while armed, in Soldering mode, with a target of
1-500 C and `heaterThermalRunawayCounter` at 8 or less (`ts21_heater_permitted()`).

The ceiling is 100 W (`TS21_HEATER_POWER_X10`), Miniware's published rating. No USB-PD
driver is compiled (`POW_PD_EXT 0`) and no contract is negotiated; the input voltage is
only read back on ADC1. Inputs outside 4.5-24.0 V (`TS21_HEATER_MAX_INPUT_X10` is 240) are
rejected, and the upper bound leaves room for the tolerance and load-release overshoot of
a 20 V supply.

`setTipPWM()` withholds a command unless the heater is permitted, no inhibit bit is set and
the latest sample is good and below 500 C; each sample serves one command. The compare
value is limited to 100 W at the sampled voltage and element resistance and to 85% duty.
TIM1 CH3 on PA10 is a 120 ms one-pulse gate, added by this port, that falls in hardware
even if the CPU, interrupts or PID stall. `ts21_heater_check()` also inhibits output when
permission is lost or the last command is 150 ms old, and an IWDG of about 500 ms runs
only in the heater build.

`heater_record_sample()` classifies each sample, first match wins:

```
sensor  tip ADC 0 or full scale, or handle outside the NTC table   latching
open    tip or probe ADC >= 4090, or element above 10.0 ohm        inhibit only
short   element below 1.0 ohm                                      latching
supply  input below 4.5 V or above 24.0 V                          inhibit only
temp    tip delta plus handle temperature >= 500 C                 inhibit only
```

A bad sample inhibits output at once and is counted per class: `heater_fault_error` holds
the class, the count restarts when it changes, and a good sample clears both and
refreshes the power cap. Sensor and short are in `TS21_HEATER_LATCHING_ERRORS` (which also
holds ADC, see below). After `TS21_HEATER_FAULT_SAMPLES` (4) consecutive samples of one of
them while the heater is permitted, `ts21_heater_trip()` inhibits, records the bit,
disarms, zeroes the target and sets the runaway counter to 255, which opens the stop
screen. Open, supply and over-temperature readings set no bit and never trip; they
withhold output while they last and resume on the next good sample.

ADC faults are not debounced: `ts21_adc_read()` sets `TS21_ERROR_ADC` on the first failed
conversion, and `getInputVoltageX10()` sets `TS21_ERROR_SENSOR` as soon as the last sample
sequence was invalid (`sampleSensors()` itself waits for four). Both inhibit output at once
and stay set until acknowledged, but neither calls `ts21_heater_trip()`.

The stop screen reads "Heating stopped" plus the inhibit bits as eight hex digits
(`00000200` heater short, `00000100` sensor, `00000002` ADC, `00010000` shared monitor,
meaning the upstream runaway detector tripped with no other bit set; the rest are in
`ts21.h`). From the home screen, A opens it while any
inhibit bit is set. It stays until a button is pressed and everything is released;
`ts21_heater_acknowledge()` then disarms, zeroes the target and takes a fresh sample, and
clears the recoverable bits only if that sample is healthy. Otherwise the sample's error is
recorded and the stop persists. Heating restarts only with a fresh press.

Every error bit except UART inhibits output. Clock, I2C, argument, trap and flash bits are
never cleared by acknowledgment, and clock, I2C-bus, trap and flash failures end in
`ts21_fault()`, which masks interrupts and loops with the heater inhibited and the
watchdog refreshed.

## Tests and emulator

`make test-ts21` builds and runs the native test programs against mock MMIO, then
`python3 -m unittest discover -s tests -p '*_test.py'` (image inspector, HEX validation,
font generator). It needs native `cc` and `c++`, python3 and PyYAML, but neither the cross
compiler nor the gate. CI runs it and lints this directory with black and flake8.

`tools/emulate_boot.py` smoke-tests startup and task switching of a built ELF under
Unicorn. Peripheral values and interrupt delivery are simulated and instruction counts are
only an approximate timebase, so a pass says nothing about real timing or the watchdog.

```sh
pip install -r source/Core/BSP/TS21/tools/requirements-emulation.txt   # unicorn==2.1.4
python3 source/Core/BSP/TS21/tools/emulate_boot.py \
    source/Hexfile/TS21-heater-test/TS21_font_compressed_EN.elf \
    --prefix "$HOME/ts21-toolchain/bin/riscv-none-embed-" \
    --inherited-interrupt --max-frame-us 60000 --check-heater short --instructions 160000000
```

`--inherited-interrupt` starts with an interrupt already active, the case `startup.S`
handles with `mret`; run with and without it. `--max-frame-us` fails the run if a simulated
display frame is longer. With no check option the script verifies boot (four tasks, two
display frames, 100 timer interrupts, font and translation buffers that match a host
BriefLZ decompression, PA6 and PA10 never driven high); `source/Hexfile/TS21_EN.elf` is
the heater-free ELF.

`--check-heater` needs the heater-test ELF. `short` and `monitor` (which sets the shared
runaway counter to 255) must latch, hold the stop code until acknowledged and refuse a
restart while the fault persists; `recover` clears the fault first and expects a fresh
press to restart heating within the power limit. `open`, `supply`, `overtemp`, `continuous`
and `range` withhold output without latching and resume on their own, the last two after
first running at target. `uart` injects a USART2 overrun, which must be retained without
blocking soldering, then a short. `startup` checks that saved autostart, boost and
keep-alive cannot start heating, and `stall` that the 120 ms gate expires with the CPU
halted.

`--check-cold-display` checks that the display stays on after inactivity,
`--check-worklight off|on|menu` the PA9 restore or the menu checkbox, and `--fahrenheit`
boots with saved Fahrenheit settings. The first two cannot be combined with each other or
with `--check-heater`. Exit status is 0 only if the final line starts with `PASS`. Budgets
that have been needed: `short` 160000000 instructions, `--check-worklight menu` 200000000,
`recover` and `range` 400000000, `continuous` 600000000 (default 40000000).

## Status

One unit accepted the images and ran the full 128x32 UI, both buttons, the work light and a
temperature readout that follows external warming. On a 20 V supply the heater build drove
the tip and the readout climbed.

Nothing was measured. The readout has not been compared against an independent reference,
delivered power and thermal behaviour at full load were not instrumented, and fault
shutdown has not been tried under load. The polarity of the PA10 gate is inferred from the
stock image rather than probed. Only one PCB revision has been seen.

## Provenance

The stock reference is Miniware's
[TS21APPV1.5.0.zip](https://forum.minidso.com/forumdownload/TS21APPV1.5.0.zip), SHA-256
`24c74454ac876b8fb2d43b2b5cfe4de3fdcb3109d6f2b6349fc5d676a9a6082b`; the
`TS21APPV1.5.0EN.hex` inside it has SHA-256
`786ae2d3bb5702b50ce2d628891b3bf07a4580bfc9a2ea974db69e46ab8882c8`. Pin assignments, ADC
transfer functions and the memory layout were derived from it. Neither it nor the
toolchain is in the tree, and the build does not read it.

The [WCH CH32V20x SDK](https://github.com/openwch/ch32v20x/tree/baef0054588f4826548429ff4e7a9257d752ef2f)
was the reference for startup and the FreeRTOS port; `IronOS/Port/` keeps its FreeRTOS
notices (MIT, Copyright (C) 2021 Amazon.com).
