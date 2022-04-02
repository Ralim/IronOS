XIP FLASH - 数据读写
======================

本 demo 主要演示 xip flash 基本的数据读写功能。

硬件连接
-----------------------------

- 内部 flash 不需要额外电路
- 外部 flash 需要连接 flash 控制器接口

软件实现
-----------------------------

-  软件代码见 ``examples/flash/flash_rw``

.. code-block:: C
    :linenos:

    /* erase 0x00010000 16k flash */
    flash_erase(0x00010000, 16 * 1024);

    /* write 0x00010000 flash data */
    flash_write(0x00010000, writeTestData, sizeof(writeTestData));

    memset(readTestData, 0, 256);

    /* read 0x00010000 flash data */
    flash_read(0x00010000, readTestData, sizeof(readTestData));

-  擦除 flash 物理地址 0x00010000 开始的 16K 内容
-  从flash 物理地址 0x00010000 开始写入用户数据
-  从flash 物理地址 0x00010000 开始读取用户数据

编译和烧录
-----------------------------

-  **CDK 编译**

   打开项目中提供的工程文件：flash_rw.cdkproj

   参照 :ref:`windows_cdk_quick_start` 的步骤编译下载即可

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=flash_rw

-  **烧录**

   详见 :ref:`bl_dev_cube`

实验现象
-----------------------------

打开串口，如果显示 case success 表示读写成功，否则读写错误