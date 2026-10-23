# WCH CH57x vendor files

These files come from the WCH CH573 EVT package (2022.04, http://wch.cn). This package also applies to the CH571.

- `CH57x/`: peripheral headers and drivers (only the ones IronOS uses), `ISP573.h` and `libISP573.a`.
- `RVMSIS/core_riscv.h`, `Startup/startup_CH571.S`: core header and startup code.
- `Startup/ch571.ld`: from the EVT `Link.ld`, with sizes changed for the CH571 and IronOS.
- `FreeRTOS/`: FreeRTOS port, from the WCH QingKe V4A port (CH583 EVT).

The sources are Apache-2.0 (refer to `LICENSE`). `libISP573.a` is the WCH data-flash and flash-ROM library from the same package. WCH supplies it as a binary only.

Local changes:

- `startup_CH571.S` sets the heater pin low at reset and sends faults to `fault_halt`.
- Formatting.
