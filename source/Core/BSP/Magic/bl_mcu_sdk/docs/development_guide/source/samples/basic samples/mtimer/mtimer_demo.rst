MTIMER - 系统定时器
====================

本 demo 基于 risc-v 内核自带的一个 64 位定时器（MTIMER）编写。本 demo 可以为后面 os tick 提供参考。

硬件连接
-----------------------------
无

软件实现
-----------------------------

-  软件代码见 ``examples/systick``

.. note:: ``mtimer`` 时钟默认经过分频以后为 1M，方便后面使用，减少计算时间。

.. code-block:: C
    :linenos:

    void systick_isr()
    {
        static uint32_t tick=0;
        tick++;
        MSG("tick:%d\r\n",tick);
    }

    bflb_platform_set_alarm_time(1000000,systick_isr);

-  使用上述代码设置 mtimer 定时时间为 1s，并且注册中断回调函数。

编译和烧录
-----------------------------

-  **CDK 编译**

   打开项目中提供的工程文件：systick.cdkproj

   参照 :ref:`windows_cdk_quick_start` 的步骤编译下载即可

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=systick

-  **烧录**

   详见 :ref:`bl_dev_cube`

实验现象
-----------------------------

``tick`` 值每秒自增 1 并通过串口打印。