[![License](https://img.shields.io/badge/License-Apache--2.0-brightgreen)](LICENSE)
[![Release](https://img.shields.io/github/v/tag/bouffalolab/bl_mcu_sdk?color=s&label=release)]()

[中文版](README_zh.md)

# Introduction

**bl mcu sdk** is an MCU software development kit provided by the Bouffalo Lab Team for BL602/BL604, BL702/BL704/BL706 and other series of chips in the future.

## Code Framework

```

bl_mcu_sdk
├── bsp
│   ├── board
│   │   ├── bl602
│   │   └── bl702
│   └── bsp_common
├── build
├── common
│   ├── bl_math
│   ├── device
│   ├── list
│   ├── memheap
│   ├── misc
│   ├── partition
│   ├── pid
│   ├── ring_buffer
│   ├── soft_crc
│   └── timestamp
├── components
│   ├── ble
│   ├── fatfs
│   ├── freertos
│   ├── lvgl
│   ├── lwip
│   ├── mbedtls
│   ├── nmsis
│   ├── romfs
│   ├── rt-thread
│   ├── shell
│   ├── tflite
│   ├── tiny_jpeg
│   ├── usb_stack
│   └── xz
├── docs
│   ├── chipSpecification
│   ├── development_guide
│   └── development_guide_en
├── drivers
│   ├── bl602_driver
│   └── bl702_driver
├── examples
│   ├── acomp
│   ├── adc
│   ├── audio_cube
│   ├── ble
│   ├── boot2_iap
│   ├── camera
│   ├── coremark
│   ├── cxx
│   ├── dac
│   ├── dma
│   ├── dsp
│   ├── emac
│   ├── flash
│   ├── freertos
│   ├── gpio
│   ├── hellowd
│   ├── i2c
│   ├── i2s
│   ├── keyscan
│   ├── lvgl
│   ├── mbedtls
│   ├── memheap
│   ├── nn
│   ├── pka
│   ├── pm
│   ├── psram
│   ├── pwm
│   ├── qdec
│   ├── rt-thread
│   ├── rtc
│   ├── shell
│   ├── spi
│   ├── systick
│   ├── tensorflow
│   ├── timer
│   ├── uart
│   └── usb
├── out
└── tools
    ├── bflb_flash_tool
    ├── cdk_flashloader
    ├── cmake
    └── openocd
```

- **bsp/board** : store the board-level description file such as `clock_config.h` (describes the clock configuration file) `pinmux_config.h` (describes the io function file) `peripheral_config.h` (describes the default configuration file of the peripheral) , These files together describe the board hardware information.
- **bsp/bsp_common** : store some common peripheral driver codes related to the board.
- **common** : store some common functions and macros that chip drivers will need.
- **components** : store the third-party library public component library.
- **drivers** : store bouffalo series of chip drivers.
- **examples** : store the official sample code.
- **tools** : store toolkits related to compiling and downloading.
- **docs** : store tutorial documents and other help information.
- **build** : store cmake cache files.
- **out** : store the bin and map file generated after compiling and linking.

## Hierarchy

[![Hierarchy](https://z3.ax1x.com/2021/06/18/RpUVoj.png)](https://imgtu.com/i/RpUVoj)

# Resources

## Hardware Resources

- BL706 IOT Development board
[![BL706_IOT](https://z3.ax1x.com/2021/08/06/fnPcLT.png)](https://imgtu.com/i/fnPcLT)

- BL706 AVB Development Board
[![BL706_AVB](https://z3.ax1x.com/2021/11/16/IWPuVJ.png)](https://imgtu.com/i/IWPuVJ)


There is currently no official purchase channel. If you want to get the above development board, you can apply for it in [Forum  post](https://bbs.bouffalolab.com/d/88).

## Chip Manual

**Chip Reference Manual** and **Chip Data Manual** are listed on [document](https://dev.bouffalolab.com/document)

## Documentation Tutorial

To get more bl mcu sdk documentation tutorial, like api manual or peripheral demo and so on, please visit:

- [bl mcu sdk documentation tutorial](https://dev.bouffalolab.com/media/doc/sdk/bl_mcu_sdk_en/index.html)

## Video Tutorial

- [BL706 MCU Development Series Video Tutorial](https://www.bilibili.com/video/BV1xK4y1P7ur)

## Development Tools

### Command Line Development

For the tools needed for command line development, please refer to [linux development guide](https://dev.bouffalolab.com/media/doc/sdk/bl_mcu_sdk_en/get_started/Linux_quick_start_ubuntu.html)

- [cmake 3.19](https://cmake.org/files/v3.19/), cmake compilation tool, it is recommended to use cmake v3.15 or above
- [riscv64-unknown-elf-gcc](https://gitee.com/bouffalolab/toolchain_gcc_sifive_linux), risc-v linux toolchain，download command:

```

git clone https://gitee.com/bouffalolab/toolchain_gcc_sifive_linux.git

```

### Eclipse Development

For the tools needed for Eclipse development, please refer to [Eclipse Development Guide](https://dev.bouffalolab.com/media/doc/sdk/bl_mcu_sdk_en/get_started/Windows_quick_start_eclipse.html)

- [Eclipse](https://dev.bouffalolab.com/media/upload/download/BouffaloLab_eclipse_x86_64_win.zip) eclipse development free installation package under Windows
- [riscv64-unknown-elf-gcc](https://gitee.com/bouffalolab/toolchain_gcc_sifive_windows), risc-v windows toolchain，download command:

```

git clone https://gitee.com/bouffalolab/toolchain_gcc_sifive_windows.git

```

- [J-Link v10](https://www.segger.com/downloads/jlink), J-Link debugger, used to debug the chip online, it is recommended to use the hardware of J-Link V10 or above, and the software driver is recommended to use V6 .98 version

### CDK Development

For tools needed for CDK development, please refer to  [CDK Development Guide](https://dev.bouffalolab.com/media/doc/sdk/bl_mcu_sdk_en/get_started/Windows_quick_start_cdk.html)

- [CDK](https://occ.t-head.cn/development/activities/cdk), Jianchi CDK integrated development environment of T-Head , it is recommended to use CDK v2.8.4 or above

### Flash Tool

In addition to using CK-link, J-link and command line programming, it also supports graphical programming tools.
Graphical programming tools provided by Bouffalolab:

- [Bouffalo Lab Dev Cube](https://dev.bouffalolab.com/download)

### Board Config Wizard

We provide [BL Config Wizard](https://dev.bouffalolab.com/media/config/index.html) to generate `clock_config.h`、 `pinmux_config.h` and `peripheral_config.h` file online.

## How to make sdk as submodule

First add bl_mcu_sdk to your own project using the add submodule command, and then commit the gitmodules file to the remote repo.


```

git submodule add https://gitee.com/bouffalolab/bl_mcu_sdk.git bl_mcu_sdk
cd bl_mcu_sdk
git pull --rebase
cd ..
git add .gitmodules
git add bl_mcu_sdk
git commit -m "xxx"
git push

```

The final catalog presents the following results:

```
.
├── hardware
├── xxxx
├── xxxx
├── xxxx
├── bl_mcu_sdk
├── user_code
│   └── gpio
│       ├── gpio_blink
│       ├── gpio_dht11
│       └── gpio_int

```

### Command Line compile

```
    cd bl_mcu_sdk
    make APP=xxx APP_DIR=../user_code
```

## Forum

Bouffalolab Developer Forum: [https://bbs.bouffalolab.com/](https://bbs.bouffalolab.com/)

# License

**bl mcu sdk** is completely open source and follows the Apache License 2.0 open source license agreement. It can be used in commercial products for free and does not require public private code.

```
/*
 * Copyright (c) 2021 Bouffalolab team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 ```