[![License](https://img.shields.io/badge/License-Apache--2.0-brightgreen)](LICENSE)
[![Release](https://img.shields.io/github/v/tag/bouffalolab/bl_mcu_sdk?color=s&label=release)]()

[English Version](README.md)

# 简介

**bl mcu sdk** 是博流智能科技团队专为 BL602/BL604,BL702/BL704/BL706 以及未来其他系列芯片提供的 MCU 软件开发包。

## 代码框架

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

- **bsp/board** : 存放板级描述文件 `clock_config.h`(描述时钟配置文件) `pinmux_config.h`(描述io功能文件) `peripheral_config.h` (描述外设默认配置文件) 这几个文件共同形成 board 文件夹描述特定应用的所有硬件信息。
- **bsp/bsp_common** : 存放一些板级相关的常用外设驱动代码
- **common** : 存放一些常用的函数和宏（芯片驱动需要）
- **components** : 存放第三方库公共组件
- **drivers** : 存放博流智能系列芯片驱动
- **examples** : 存放官方提供的示例代码
- **tools** : 存放编译下载相关的工具包
- **docs** : 存放教程文档以及其他帮助信息
- **build**: 存放 cmake 缓存文件
- **out** : 中间文件，用来存放编译链接后的产生的bin文件

## 层次结构

[![层次结构](https://z3.ax1x.com/2021/06/18/RpUVoj.png)](https://imgtu.com/i/RpUVoj)

# 资源

## 硬件资源

- BL706 IOT 开发板
[![BL706_IOT](https://z3.ax1x.com/2021/08/06/fnPcLT.png)](https://imgtu.com/i/fnPcLT)

- BL706 AVB 开发板
[![BL706_AVB](https://z3.ax1x.com/2021/11/16/IWPuVJ.png)](https://imgtu.com/i/IWPuVJ)


目前暂时没有提供官方的购买渠道，如果想获得上述开发板，可到[论坛申请贴](https://bbs.bouffalolab.com/d/88)申请。

## 芯片手册

芯片数据手册和参考手册见 [文档](https://dev.bouffalolab.com/document)。

## 文档教程

获取更多 bl mcu sdk 开发相关的教程，如 api 手册、外设 demo 等，请参考：

- [bl mcu sdk 文档教程](https://dev.bouffalolab.com/media/doc/sdk/bl_mcu_sdk_zh/index.html)

## 视频教程

- [BL706 MCU 开发系列视频教程](https://www.bilibili.com/video/BV1xK4y1P7ur)

## 开发工具

### 命令行开发

命令行开发需要的工具，具体使用方法参考 [linux 开发指南](https://dev.bouffalolab.com/media/doc/sdk/bl_mcu_sdk_zh/get_started/Linux_quick_start_ubuntu.html)

- [cmake 3.19](https://cmake.org/files/v3.19/)， cmake 编译工具，建议使用 cmake v3.15 以上版本
- [riscv64-unknown-elf-gcc](https://gitee.com/bouffalolab/toolchain_gcc_sifive_linux), risc-v linux 端工具链，下载方式：

```

git clone https://gitee.com/bouffalolab/toolchain_gcc_sifive_linux.git

```

### Eclipse开发

Eclipse 开发需要的工具，具体使用方法参考 [Eclipse 开发指南](https://dev.bouffalolab.com/media/doc/sdk/bl_mcu_sdk_zh/get_started/Windows_quick_start_eclipse.html)

- [Eclipse](https://dev.bouffalolab.com/media/upload/download/BouffaloLab_eclipse_x86_64_win.zip) Windows 下 eclipse 开发免安装包
- [riscv64-unknown-elf-gcc](https://gitee.com/bouffalolab/toolchain_gcc_sifive_windows), risc-v windows 端工具链，下载方式：

```

git clone https://gitee.com/bouffalolab/toolchain_gcc_sifive_windows.git

```

- [J-Link v10](https://www.segger.com/downloads/jlink)， J-Link 调试器，用于在线调试芯片，建议使用 J-Link V10 以上版本硬件，软件驱动建议使用 V6.98 版本

### CDK开发

CDK 开发需要的工具，具体使用方法参考 [CDK 开发指南](https://dev.bouffalolab.com/media/doc/sdk/bl_mcu_sdk_zh/get_started/Windows_quick_start_cdk.html)

- [CDK](https://occ.t-head.cn/development/activities/cdk)，平头哥剑池 CDK 集成开发环境，建议使用 CDK v2.8.4 以上版本

### 烧写工具

除了可以使用 CK-link、J-link 以及命令行烧写以外，还支持图形化的烧写工具。
博流提供的图形化烧写工具：

- [Bouffalo Lab Dev Cube](https://dev.bouffalolab.com/download)

### Board Config Wizard

我们提供了[BL Config Wizard](https://dev.bouffalolab.com/media/config/index.html)，可以在线生成  `clock_config.h`、 `pinmux_config.h` 和 `peripheral_config.h` 文件。

## 如何将 SDK 作为 submodule 使用

首先使用添加子模块的命令添加到你自己的工程下，然后将 gitmodules 文件提交到远程仓库。

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

最终目录呈现的结果如下：

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

### 命令行编译方式

```
    cd bl_mcu_sdk
    make APP=xxx APP_DIR=../user_code
```

## 论坛

博流开发者交流论坛: [https://bbs.bouffalolab.com/](https://bbs.bouffalolab.com/)

# 许可协议

**bl mcu sdk** 完全开源，遵循 Apache License 2.0 开源许可协议，可以免费在商业产品中使用，并且不需要公开私有代码。

```
/*
 * Copyright (c) 2021 Bouffalolab team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 ```