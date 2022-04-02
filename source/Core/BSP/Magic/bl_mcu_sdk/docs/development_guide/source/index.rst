.. bl_mcu_sdk_development_guide documentation master file, created by
   sphinx-quickstart on Fri Mar 12 16:32:56 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

========================================================
BL MCU SDK 开发指南
========================================================

博流 `BL702 <https://www.bouffalolab.com/bl70X>`_ 系列产品是基于 “SiFive E24 Core” `RISC-V <https://en.wikipedia.org/wiki/RISC-V>`_ 处理器的通用微控制器，具有 BLE 5.0、zigbee 和以太网等网络功能，同时还具备其它丰富的外设。
可被广泛应用于 `IoT <https://en.wikipedia.org/wiki/Internet_of_things>`_ 和其他低功耗领域中。
支持通过 `JTAG <https://en.wikipedia.org/wiki/JTAG>`_ 对芯片进行烧写和调试，也支持通过 `UART <https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter>`_ 进行程序烧录。

BL MCU SDK 将为您提供博流 BL70X 系列 MCU 开发的全方位支持。

.. toctree::
   :maxdepth: 1
   :caption: 快速开发指南
   :numbered: 3

   get_started/get_started
   get_started/connecting_hardware
   get_started/index
   get_started/cmake_quick_start
   get_started/cdk_new_project_quick_start
   get_started/bl_dev_cube
   get_started/board



.. toctree::
   :maxdepth: 1
   :caption: API 手册
   :numbered:

   api_reference/api_overview
   api_reference/peripheral/index
   api_reference/shell/api_shell
   api_reference/usb stack/api_usb_stack
   api_reference/bluetooth/api_ble

.. toctree::
   :maxdepth: 1
   :caption: 基础外设例程
   :numbered:

   samples/basic samples/gpio/index
   samples/basic samples/uart/index
   samples/basic samples/pwm/index
   samples/basic samples/mtimer/index
   samples/basic samples/dma/index
   samples/basic samples/spi/index
   samples/basic samples/i2c/index
   samples/basic samples/i2s/index
   samples/basic samples/adc/index
   samples/basic samples/timer/index
   samples/basic samples/flash/index


.. toctree::
   :maxdepth: 1
   :caption: 进阶例程
   :numbered:

   samples/advance samples/shell_demo
   samples/advance samples/fatfs_demo
   samples/advance samples/usb/index
   samples/advance samples/lowpower_demo
   samples/advance samples/boot2_iap_info
   samples/advance samples/ble_scan_demo
   samples/advance samples/ble_pds
   samples/advance samples/emac/index

