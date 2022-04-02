GPIO 中断 - 按键检测
====================

本 demo 基于 GPIO 外设外部中断模式编写。

硬件连接
-----------------------------

本 demo 基于BL706_IOT开发板，自行添加按键电路，连接方式如下：

.. list-table::
    :widths: 30 30
    :header-rows: 1

    * - GPIO function
      - GPIO pin
    * - SW1
      - GPIO11

.. figure:: img/button_sch.png
    :alt:

软件实现
-----------------------------

-  软件代码见 ``examples/gpio/gpio_int``

.. code-block:: C
    :linenos:

    static void gpio11_int_callback(uint32_t pin)
    {
        MSG("gpio rising trigger !\r\n");
    }

    gpio_set_mode(GPIO_PIN_11,GPIO_SYNC_RISING_TRIGER_INT_MODE);
    gpio_attach_irq(GPIO_PIN_11,gpio11_int_callback);
    gpio_irq_enable(GPIO_PIN_11,ENABLE);

-  使用上述代码将 ``GPIO11`` 配置为GPIO上升沿中断触发模式，并注册中断回调函数。

编译和烧录
-----------------------------

-  **CDK 编译**

   打开项目中提供的工程文件：gpio_int.cdkproj

   参照 :ref:`windows_cdk_quick_start` 的步骤编译下载即可

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=gpio_int

-  **烧录**

   详见 :ref:`bl_dev_cube`


实验现象
-----------------------------

当按键按下时，串口会打印 ``"gpio rising trigger !"``