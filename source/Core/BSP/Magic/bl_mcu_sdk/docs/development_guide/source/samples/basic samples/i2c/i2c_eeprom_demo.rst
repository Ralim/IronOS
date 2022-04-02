I2C - AT24CXX 读写
====================

硬件连接
-----------------------------

本 demo 基于 BL706_IOT 开发板，自行添加 AT24CXX 电路，连接方式如下：

.. list-table::
    :widths: 30 30
    :header-rows: 1

    * - GPIO function
      - GPIO pin
    * - I2C_SCL
      - GPIO11
    * - I2C_SDA
      - GPIO16

软件实现
-----------------------------

-  软件代码见 ``examples/i2c/i2c_at24cxx``

.. code-block:: C
    :linenos:

    #define BSP_I2C_CLOCK_SOURCE  ROOT_CLOCK_SOURCE_BCLK
    #define BSP_I2C_CLOCK_DIV  0

-  配置 ``I2C`` 设备时钟源，见 ``bsp/board/bl706_iot/clock_config.h``

.. code-block:: C
    :linenos:

    #define CONFIG_GPIO11_FUNC GPIO_FUN_I2C
    #define CONFIG_GPIO16_FUNC GPIO_FUN_I2C

-  配置 ``I2C`` 设备复用引脚，见 ``bsp/board/bl706_iot/pinmux_config.h``

.. code-block:: C
    :linenos:

    #define BSP_USING_I2C0

    #if defined(BSP_USING_I2C0)
    #ifndef I2C0_CONFIG
    #define I2C0_CONFIG \
    {   \
    .id = 0, \
    .mode = I2C_HW_MODE,\
    .phase = 15, \
    }
    #endif
    #endif

-  使能 ``BSP_USING_I2C0`` 并配置 ``I2C`` 设备配置，见 ``bsp/board/bl706_iot/peripheral_config.h``

.. code-block:: C
    :linenos:

    i2c_register(I2C0_INDEX, "i2c");
    struct device *i2c0 = device_find("i2c");

    if (i2c0)
    {
        MSG("device find success\r\n");
        device_open(i2c0, 0);
    }

- 首先调用 ``i2c_register`` 函数注册  ``I2C`` 设备,当前注册 ``I2C0``
- 然后通过 ``find`` 函数找到设备对应的句柄，保存于 ``i2c0`` 句柄中
- 最后使用 ``device_open`` 以默认模式来打开 ``I2C0`` 设备

.. code-block:: C
    :linenos:

    i2c_msg_t msg[2];
    uint8_t buf[8] = {0};

    msg[0].buf = buf;
    msg[0].flags = SUB_ADDR_1BYTE | I2C_WR;
    msg[0].len = 8;
    msg[0].slaveaddr = 0x50;
    msg[0].subaddr = 0x00;

    msg[1].buf = buf;
    msg[1].flags = SUB_ADDR_1BYTE | I2C_RD;
    msg[1].len = 8;
    msg[1].slaveaddr = 0x50;
    msg[1].subaddr = 0x00;
    if (i2c_transfer(i2c0, &msg[0], 2) == 0)
        MSG("\r\n read:%0x\r\n", msg[1].buf[0] << 8 | msg[1].buf[1]);

- 调用 ``i2c_transfer`` 传输两个 ``msg``，一个 ``msg`` 代表向 eeprom 写入 8 字节数据，一个 ``msg`` 代表从 eeprom 读取 8 字节数据，

编译和烧录
-----------------------------

-  **CDK 编译**

   打开项目中提供的工程文件：i2c_at24cxx.cdkproj

   参照 :ref:`windows_cdk_quick_start` 的步骤编译下载即可

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=i2c_at24cxx

-  **烧录**

   详见 :ref:`bl_dev_cube`

实验现象
-----------------------------
