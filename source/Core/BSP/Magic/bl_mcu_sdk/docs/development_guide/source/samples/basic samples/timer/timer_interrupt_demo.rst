TIMER - 秒中断定时
====================

本 demo 基于 TIMER 中断模式秒中断定时。

软件实现
-----------------------------

-  软件代码见 ``examples/timer/timer_int``

.. code-block:: C
    :linenos:

    #define BSP_TIMER0_CLOCK_SOURCE ROOT_CLOCK_SOURCE_FCLK
    #define BSP_TIMER0_CLOCK_DIV    0

-  配置 ``TIMER`` 设备时钟源，见 ``bsp/board/bl706_iot/clock_config.h``

.. code-block:: C
    :linenos:

    #define BSP_USING_TIMER0

    #if defined(BSP_USING_TIMER0)
    #ifndef TIMER0_CONFIG
    #define TIMER0_CONFIG                           \
        {                                           \
            .id = 0,                                \
            .cnt_mode = TIMER_CNT_PRELOAD,          \
            .trigger = TIMER_PRELOAD_TRIGGER_COMP2, \
            .reload = 0,                            \
            .timeout1 = 1000000,                    \
            .timeout2 = 2000000,                    \
            .timeout3 = 3000000,                    \
        }
    #endif
    #endif

-  使能 ``BSP_USING_TIMER0`` 并配置 ``TIMER0`` 设备配置，见 ``bsp/board/bl706_iot/peripheral_config.h``

.. code-block:: C
    :linenos:

    timer_register(TIMER0_INDEX, "timer0");

    timer0 = device_find("timer0");

    if (timer0) {
        device_open(timer0, DEVICE_OFLAG_INT_TX); /* 1s,2s,3s timing*/
        device_set_callback(timer0, timer0_irq_callback);
        device_control(timer0, DEVICE_CTRL_SET_INT, (void *)(TIMER_COMP0_IT | TIMER_COMP1_IT | TIMER_COMP2_IT));
    } else {
        MSG("timer device open failed! \n");
    }

- 调用 ``timer_register`` 函数注册  ``TIMER`` 设备，当前注册 ``TIMER0``
- 然后通过 ``find`` 函数找到设备对应的句柄，保存于 ``timer0`` 句柄中
- 最后使用 ``device_open`` 以中断模式来打开 ``timer0`` 设备
- 调用 ``device_set_callback`` 函数，注册用户指定的 ``TIMER0`` 中断服务函数。调用 ``device_control`` 函数使能中断和配置定时周期。

.. code-block:: C
    :linenos:

    void timer0_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
    {
        if (state == TIMER_EVENT_COMP0) {
            MSG("timer event comp0! \r\n");
        } else if (state == TIMER_EVENT_COMP1) {
            MSG("timer event comp1! \r\n");
        } else if (state == TIMER_EVENT_COMP2) {
            BL_CASE_SUCCESS;
            timer_timeout_cfg_t cfg = { 2, 12000000 }; /*modify compare id 2 timeout 12s*/
            device_write(dev, 0, &cfg, sizeof(timer_timeout_cfg_t));
            MSG("timer event comp2! \r\n");
        }
    }

-  此函数是示例的中断服务函数，作用是判断具体是哪个 COMP 触发的中断和打印 COMP0 触发中断的次数。
- ``device_write`` 则是在达到 comp2 超时时间时，修改 comp2 的超时时间为 12s。

编译和烧录
-----------------------------

-  **CDK 编译**

   打开项目中提供的工程文件：timer_int.cdkproj

   参照 :ref:`windows_cdk_quick_start` 的步骤编译下载即可

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=timer_int

-  **烧录**

   详见 :ref:`bl_dev_cube`


实验现象
-----------------------------

