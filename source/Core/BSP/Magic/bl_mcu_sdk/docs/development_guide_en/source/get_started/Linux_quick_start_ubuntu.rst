Linux OR WSL environment development guide
============================================

This document introduces how to install and configure the software development tools needed for BL702 series MCUs in Linux. The installation and configuration method under WSL system is the same as under linux, please install WSL system by yourself. The difference is that one runs on a pure linux system and the other runs on windows. If you don't want to install a virtual machine or a linux system, you can choose WSL.

**Windows Subsystem for Linux** (WSL) is a compatibility layer that can run native Linux binary executable files (ELF format) on Windows 10. It was developed by Microsoft and Canonical in cooperation. Its goal is to enable the pure Ubuntu image to be downloaded and decompressed to the user's local computer, and the tools in the image can run on this subsystem. Therefore, the operation mode under WSL is exactly the same as the operation mode under linux.

Software and hardware environment
------------------------------------

- A mini USB data cable
- A USB-TTL serial port module
- Several Dupont lines

Configure RISC-V toolchain
-----------------------------

.. code-block:: bash
   :linenos:
   :emphasize-lines: 4-6

   $ cd ~
   $ wget -c https://dev.bouffalolab.com/media/upload/download/riscv64-elf-x86_64-20210120.tar.gz
   $ mkdir -p riscv64-elf-20210120
   $ tar -zxvf riscv64-elf-x86_64-20210120.tar.gz -C riscv64-elf-20210120
   $ sudo cp -rf ~/riscv64-elf-20210120  /usr/bin
   $ echo "export PATH=\"$PATH:/usr/bin/riscv64-elf-20210120/bin\""  >> ~/.bashrc
   $ source ~/.bashrc

Configure cmake & make tools
-------------------------------

.. code-block:: bash
   :linenos:
   :emphasize-lines: 5-7

   $ sudo apt update
   $ sudo apt install make
   $ cd ~
   $ wget -c https://cmake.org/files/v3.19/cmake-3.19.3-Linux-x86_64.tar.gz
   $ tar -zxvf cmake-3.19.3-Linux-x86_64.tar.gz
   $ sudo cp -rf ~/cmake-3.19.3-Linux-x86_64  /usr/bin
   $ echo "export PATH=\"$PATH:/usr/bin/cmake-3.19.3-Linux-x86_64/bin\""  >> ~/.bashrc
   $ source ~/.bashrc


Hardware connection
----------------------

- For the connection of the board, please refer to :ref:`connect_hardware`
- Please make sure that the board is set correctly before proceeding to the following steps (Serial connection is recommended under Linux)

Get bl_mcu_sdk
---------------------------

- Open the terminal and enter the following command to get bl_mcu_sdk

.. code-block:: bash
   :linenos:

    $ cd ~
    $ git clone https://gitee.com/bouffalolab/bl_mcu_sdk.git  --recursive


Test Hello World project
------------------------------

Open Hello World
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- After obtaining the SDK, enter ``examples/hellowd/helloworld`` in the SDK, open ``main.c``, and then edit the related code of helloworld.

.. code-block:: bash
   :linenos:

    $ cd ~/bl_mcu_sdk/examples/hellowd/helloworld
    $ vi main.c

- After editing, save the changes and close the file, and then compile

Compile Hello World
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash
   :linenos:

    $ cd ~/bl_mcu_sdk
    $ make build BOARD=bl706_iot APP=helloworld


Program Hello World
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Please confirm the programming method first. If you use serial programming, please press and hold the ``boot`` key on the board and don't release it. At this time, press the ``rst`` key, and then release the two keys. The board enters the boot_rom state.
- At this time, enter the following command in the terminal to program

.. code-block:: bash
   :linenos:

    $ cd ~/bl_mcu_sdk
    $ make download INTERFACE=uart COMx=/dev/ttyUSB1

- If the download fails, please check:

   - 1. Whether the serial port is used for programming, whether the development board is powered, and whether the hardware connection is correct.
   - 2. Is the programming command executed in the ``bl_mcu_sdk`` directory
   - 3. Whether to enter boot_rom mode
   - 4. Whether the serial port is occupied, and whether your available serial port is selected correctly, if your serial port is not ``ttyUSB1``, then please specify the correct serial port

Run Hello World
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Open a new terminal, install and run the serial port tool

.. code-block:: bash
   :linenos:

    $ sudo apt install picocom   # Ignore if it is already installed
    $ picocom -b 2000000 /dev/ttyUSB1 # Pay attention to your available serial port number (if you use the serial port of Sipeed RV-debugger Plus, it will be ``ttyUSB1``)

- Press the ``rst`` key on the board, you can see ``hello world!`` in the serial terminal.

.. figure:: img/linux1.png
   :alt:

   helloworld!


Debug Hello World
^^^^^^^^^^^^^^^^^^^^^^^^^^^^



