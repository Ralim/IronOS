# WCH CH57x vendor files

From the WCH CH573 EVT package (2022.04, http://wch.cn), which covers the CH571:

- `CH57x/`: peripheral headers and drivers (only the ones used), `ISP573.h` and `libISP573.a`.
- `RVMSIS/core_riscv.h`, `Startup/startup_CH571.S`: core header and startup code.
- `Startup/ch571.ld`: based on the EVT `Link.ld`, resized for the CH571 and IronOS.
- `FreeRTOS/`: FreeRTOS port adapted from the WCH QingKe V4A port (CH583 EVT).

The sources are Apache-2.0 (see `LICENSE`). `libISP573.a` is WCH's prebuilt data-flash and flash-ROM library from the same package; it ships as a binary only.

Local changes: `startup_CH571.S` drives the heater pin low at reset and sends faults to `fault_halt`; formatting.
