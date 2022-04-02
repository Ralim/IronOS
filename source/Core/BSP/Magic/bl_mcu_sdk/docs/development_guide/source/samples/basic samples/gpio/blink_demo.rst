GPIO 输出 - 流水灯
====================

本 demo 基于 GPIO 外设的输出模式编写。

硬件连接
-----------------------------

本 demo 基于 BL706_IOT 开发板，连接方式如下：

.. list-table::
    :widths: 30 30
    :header-rows: 1

    * - GPIO function
      - GPIO pin
    * - D0
      - GPIO22
    * - D1
      - GPIO29
    * - D2
      - GPIO30
    * - D3
      - GPIO31

.. figure:: img/blink_breath_sch.png
    :alt:

软件实现
-----------------------------

-  软件代码见 ``examples/gpio/gpio_blink``

.. code-block:: C
    :linenos:

    gpio_set_mode(GPIO_PIN_22, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_29, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_30, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_31, GPIO_OUTPUT_PP_MODE);

-  使用上述代码将 ``GPIO22`` ``GPIO29`` ``GPIO30`` ``GPIO31`` 配置为输出上拉模式。

.. code-block:: C
    :linenos:

    gpio_write(GPIO_PIN_22, 0);

-  使用上述代码修改输出的电平值。

编译和烧录
-----------------------------

-  **CDK 编译**

   打开项目中提供的工程文件：gpio_blink.cdkproj

   参照 :ref:`windows_cdk_quick_start` 的步骤编译下载即可

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=gpio_blink

-  **烧录**

   详见 :ref:`bl_dev_cube`


实验现象
-----------------------------

见视频展示：

.. raw:: html

    <iframe src="//player.bilibili.com/player.html?aid=887712205&bvid=BV1xK4y1P7ur&cid=326226616&page=3" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"> </iframe>
