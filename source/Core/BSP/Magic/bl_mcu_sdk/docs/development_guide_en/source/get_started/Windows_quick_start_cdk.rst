.. _windows_cdk_quick_start:

Guide to using CDK (like MDK Keil) under Windows
===================================================

This document introduces the use of the CDK developed by `T-HEAD Semiconductor <https://www.t-heah.cn/about>`_ under Windows to complete the related software development of BL702 series MCU.

Regarding T-HEAD CDK, this is an integrated development environment developed by T-HEAD. It has operations and settings that are very similar to the traditional MCU development environment, and aims to fully access cloud development resources without changing user development habits, combined with graphical debugging and analysis tools such as OSTracer and Profiling, to accelerate user product development.

Software and hardware environment
--------------------------------------

- T-HEAD CDK software
- One USB Type-A data cable, one Type-C data cable
- A CK-Link emulator or a Sipeed RV-Debugger Plus debugger
- A USB-TTL serial port module
- Several Dupont lines


Download CDK
-----------------------------

- `CDK <https://occ.t-head.cn/development/series/download?id=3864775351511420928&type=kind&softPlatformType=4#sticky>`_ software can be obtained from the official website of T-HEAD OCC

- After the download is complete, unzip, double-click ``setup.exe``, follow the prompts, and complete the software installation

Download bl_mcu_sdk
----------------------------

- Download bl_mcu_sdk from the open source community.

   - You can use ``git clone`` or directly ``download`` to download the SDK
   - Before using ``git clone``, please make sure that ``git`` has been installed correctly, open a terminal that supports ``git`` and enter the following command to get the latest SDK

   .. code-block:: bash
      :linenos:
      :emphasize-lines: 1

      $ git clone https://gitee.com/bouffalolab/bl_mcu_sdk.git  --recursive


Hardware connection
-----------------------

- For specific connection methods, please refer to :ref:`connect_hardware`
- Please make sure that the board is set up correctly before proceeding to the following steps

Test the Hello World project
------------------------------

**When using Sipeed RV-Debugger Plus to debug a project, please follow the steps below:**

.. toctree::
   :maxdepth: 1

   Use Sipeed RV-Debugger <cdk_rv_debugger_plus>

**When using CK-Link to debug the project, please follow the steps below:**

.. toctree::
   :maxdepth: 1

   Use CK-Link <cdk_ck_link>


