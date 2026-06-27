# BSP — Alientek T90 (Nations N32L40x)

Board support for the Alientek T90 soldering iron. The T90 pairs a Nations **N32L40x**
(Cortex-M4F) MCU with a **JBC T245 / C245** cartridge, a **GC9-family 160x40 RGB565 colour
SPI LCD**, a **CH224Q** USB-PD sink and a **QST QMA6100P** accelerometer. It ships a closed
HID "update-mode" bootloader; there is no exposed SWD header by default, so the port was
brought up almost entirely from reverse engineering of the stock firmware.

`model=T90` selects this BSP (`make model=T90 firmware-EN`). See
[Documentation/Flashing/Alientek T90.md](../../../../Documentation/Flashing/Alientek%20T90.md)
for build + flash instructions and `Tools/atktool/` for the `.atk` packer / HID flasher.

## Hardware map

| Function          | Pin  | Peripheral                                  |
| ----------------- | ---- | ------------------------------------------- |
| Heater PWM        | PA0  | TIM2_CH1 (AF) -> MOSFET gate, ~30 kHz       |
| Tip thermocouple  | PA3  | ADC ch4, injected group (op-amp amplified)  |
| Cold-junction NTC | PA2  | ADC ch3 (regular)                           |
| Input voltage Vin | PA1  | ADC ch2 (regular), ~11:1 divider            |
| Output current    | PA4  | ADC ch5 (reserved, not sampled)             |
| Buzzer            | PA7  | TIM1_CH1N (complementary, BDTR/MOE)         |
| Button A          | PB4  | active-low, polled                          |
| Button B          | PA15 | active-low, polled                          |
| LCD SPI1          | PB3/PB5/PA10/PD0/PA6 | SCK/MOSI/CS/DC/backlight (BL active-low) |
| CH224Q + QMA6100P | PB6/PB7 | shared software (bit-bang) I2C bus       |

**ADC channel convention (N32L40x):** channel `N` samples pad `PA(N-1)`
(`ADC_CH_2_PA1` ... `ADC_CH_6_PA5`, from `n32l40x_adc.h`). The single ADC has no DMA scan here:
the injected group (4 ranks on the tip) is hardware-triggered in the heater-off window, and Vin /
NTC are converted on demand on the regular group, serialised by a FreeRTOS mutex.

## Reverse-engineering approach

The T90 is not a documented IronOS target, so every value below was recovered empirically rather
than from a datasheet or schematic:

1. **Boot / flash protocol** — the stock `.atk` image format and the HID update-mode protocol were
   recovered by byte-exact round-trip against two real factory images and the related T80P tool, and
   re-implemented in `Tools/atktool/`. The commit opcode (`SetBoot` 0x15, *not* `SetStartAPP` 0x16)
   was found by sweeping the bootloader's function table on hardware. See
   [t90b HID flash protocol] in the porting notes.
2. **Pinmap + display** — SPI1 wiring, the GC9 init sequence and the analog front-end were read out
   of the disassembled stock firmware and confirmed against on-screen probes (a tri-colour I2C
   scanner identified the accelerometer chip-ID).
3. **First-boot crash (the hard one)** — a deterministic `UsageFault`/`INVSTATE` at startup was traced
   to an **inherited FPU exception frame**: the hard-float factory bootloader hands off via a branch
   (not a reset) with `CONTROL.FPCA` still set, while IronOS is a soft-float build on the FreeRTOS
   **ARM_CM3** port that never saves FP context. The fix (`preRTOS.cpp`) clears `FPCA`, disables
   `FPCCR.ASPEN/LSPEN` and denies CP10/11 before the scheduler starts, so every exception uses the
   8-word basic frame the CM3 port expects. General lesson: any soft-float / CM3-port firmware
   launched from a hard-float bootloader via branch must do this.
4. **Temperature calibration** — the factory tip function returns an **absolute** temperature via a
   piecewise-linear `lut_interp(opamp_input)` table; it does **not** add a live cold-junction term at
   soldering temperatures (the breakpoints are absolute, anchored at a ~38 degC cold junction). This
   drove the calibration model below.

## Key engineering decisions

- **ADC channel map (`Pins.h`)** — the initial pinmap applied a wrong `ch_n = PA_n` convention and
  shifted every analog signal up one pad, so "Vin" sampled the NTC node and "tip" sampled the
  current shunt (a bogus temperature that drove full-power runaway). Corrected to the map above.
- **Tip calibration (`ThermoModel.cpp`, `configuration.h`)** — `OP_AMP_GAIN_STAGE = 250`,
  `VOLTAGE_DIV = 370` and a `uV -> degC` lookup, all RE'd from the factory curve. The lookup stores
  the factory absolute breakpoints minus the ~38 degC op-amp intercept so it fits IronOS's delta
  model (`getTipInC = convertuVToDegC(uV) + getHandleTemperature()`).
- **Fixed cold junction (`BSP.cpp getHandleTemperature`)** — returns a fixed `38 degC`. The on-board
  NTC runs through an uncharacterised divider; a live reading injected tens of degrees of error (the
  original overheating). Returning the factory's baked-in 38 degC anchor reproduces the factory
  reading exactly. *TODO:* reverse the NTC curve for a live cold junction once a reference
  thermometer is available.
- **Tip presence (`BSP.cpp isTipDisconnected`)** — the open thermocouple does not rail to the ADC
  ceiling (so the classic rail-high test is useless) and the current node floats. Detected instead by
  an idle-temperature test: while idle (no setpoint) an empty handle reads the op-amp open-circuit
  output (~360 degC) whereas a fitted tip reads ambient.
- **Cross-thread ADC safety (`Setup.cpp`)** — the single ADC's regular group is reprogrammed on
  demand from two threads; reads are serialised with a **static** FreeRTOS mutex
  (`configSUPPORT_DYNAMIC_ALLOCATION = 0`), with a discard-first settling pass and a TIM4 `OC2REF`
  holdoff so the injected tip sample lands after the heater blanks.
- **Settings persistence (`flash.c`, linker)** — the top of the 128K die is owned by the HID
  bootloader and is wiped on power-cycle, so settings never survived there. They were relocated to a
  dedicated page at `0x08018000` (logo at `0x08018800`) inside the retained application-flash region,
  and the linker `FLASH` window shortened to `0x08005000..0x08018000` (76K) so app code cannot reach
  them. Programming is word-only with erase/program error handling.
- **Colour display (`GC9Display.cpp`, `ColorUI.*`, `ColorTheme.*`)** — IronOS renders a 1bpp mono
  framebuffer; the GC9 shim expands it to RGB565 on the fly (no 12.8K full-frame buffer fits the 20K
  SRAM), heat-maps the foreground and draws a live power bar. A native RGB565 "hero" soldering screen
  is streamed straight to the panel; every other mode falls back to the mono pipeline so all icons,
  warnings, menus and the boot logo render normally. The full-frame `Transmit` is serialised with a
  display mutex (the motion task also issues a full-frame transmit via `setRotation`).
- **Software framebuffer rotation (`OLED.hpp` + `FRAMEBUFFER_ROTATION`)** — the shim rotates the whole
  framebuffer, so the UI draws a single un-rotated layout: `getRotation()` is forced false while
  `getRawRotation()` exposes the true left/right-hand state. Without this the per-screen rotation
  branches double-apply and the home-screen icons scatter.
- **Compact menu help text (`MENU_DESCRIPTION_SMALL_FONT`)** — on the wide/short panel the large help
  font is unreadable; descriptions are word-wrapped at build time and rendered in the small font in
  the free area under the setting value, so the name/value/scrollbar stay visible instead of the
  upstream full-screen scrolling takeover.

## Known limitations / TODO

- Calibration is fitted to the factory curve, not yet to a reference thermometer; room-temperature
  readings sit at ~38 degC (the fixed cold junction). Trim with the on-device tip calibration; a live
  cold junction (reversing the NTC curve) is pending a reference thermometer.
- The output-current node (PA4) is wired and configured but not sampled.
- The thermal-mass / inertia / max-wattage constants in `configuration.h` are first-cut and want a
  final fit on hardware.

[t90b HID flash protocol]: ../../../../Tools/atktool/README.md
