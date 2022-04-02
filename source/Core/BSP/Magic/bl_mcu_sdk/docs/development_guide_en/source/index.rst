.. bl_mcu_sdk_development_guide documentation master file, created by
   sphinx-quickstart on Fri Mar 12 16:32:56 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

========================================================
BL MCU SDK development guide
========================================================

The `BL702 <https://www.bouffalolab.com/bl70X>`_ series products are general-purpose microcontrollers based on "SiFive E24 Core" `RISC-V <https://en.wikipedia.org/wiki/RISC-V>`_ , with network functions such as BLE 5.0, zigbee and Ethernet, as well as other rich peripherals.
It can be used in a wide range of `IoT <https://en.wikipedia.org/wiki/Internet_of_things>`_ and other low-power applications.
Supports programming and debugging of the chip through `JTAG <https://en.wikipedia.org/wiki/JTAG>`_ , and also supports through `UART <https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter>`_ programming.
BL MCU SDK will provide users with comprehensive support for BL70X series MCU development.

.. toctree::
   :maxdepth: 1
   :caption: Quick Development Guide
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
   :caption: API Manuals
   :numbered:

   api_reference/api_overview
   api_reference/peripheral/index
   #api_reference/shell/api_shell
   #api_reference/usb stack/api_usb_stack
   api_reference/bluetooth/api_ble

.. toctree::
   :maxdepth: 1
   :caption: Basic Peripheral Samples
   :numbered:

   samples/basic samples/gpio/index
   samples/basic samples/uart/index
   samples/basic samples/pwm/index
   samples/basic samples/mtimer/index
   samples/basic samples/dma/index
   samples/basic samples/spi/index
   samples/basic samples/i2c/index
   #samples/basic samples/i2s/index
   samples/basic samples/adc/index
   samples/basic samples/timer/index


.. toctree::
   :maxdepth: 1
   :caption: Advance Samples
   :numbered:

   samples/advance samples/shell_demo
   samples/advance samples/fatfs_demo
   #samples/advance samples/usb/index
   samples/advance samples/lowpower_demo
   samples/advance samples/boot2_iap_info
   samples/advance samples/ble_scan_demo

