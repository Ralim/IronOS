Shell 命令行调试
====================

本 demo 将演示如何使用 shell 通过串口进行命令行调试。

准备工具
-----------------------

- pc控制端使用串口终端软件：xshell 或者 mobaxterm
- 连接介质：usb转串口 or 网络 or usb

硬件连接
-----------------------------

本 demo 基于 BL706_IOT 开发板，连接方式如下：

.. list-table::
    :widths: 30 30
    :header-rows: 1

    * - GPIO function
      - GPIO pin
    * - UART0_TX
      - GPIO14
    * - UART0_RX
      - GPIO15

软件实现
-------------------------

shell 移植到串口
^^^^^^^^^^^^^^^^^^^^

-  软件代码见 ``examples/shell``

.. code-block:: C
    :linenos:

    #define BSP_UART_CLOCK_SOURCE  ROOT_CLOCK_SOURCE_PLL_96M
    #define BSP_UART_CLOCK_DIV  0

-  配置 ``UART`` 设备时钟源，见 ``bsp/board/bl706_iot/clock_config.h``

.. code-block:: C
    :linenos:

    #define CONFIG_GPIO14_FUNC GPIO_FUN_UART0_TX
    #define CONFIG_GPIO15_FUNC GPIO_FUN_UART0_RX

-  配置 ``UART`` 设备复用引脚，见 ``bsp/board/bl706_iot/pinmux_config.h``

.. code-block:: C
    :linenos:

    #define BSP_USING_UART0

    #if defined(BSP_USING_UART0)
    #ifndef UART0_CONFIG
    #define UART0_CONFIG \
    {   \
    .id = 0, \
    .baudrate = 2000000,\
    .databits = UART_DATA_LEN_8, \
    .stopbits = UART_STOP_ONE, \
    .parity = UART_PAR_NONE, \
    .fifo_threshold = 1, \
    }
    #endif
    #endif

-  使能 ``BSP_USING_UART0`` 并配置 ``UART`` 设备配置，见 ``bsp/board/bl706_iot/peripheral_config.h``

.. code-block:: C
    :linenos:

    bflb_platform_init();

-  在 ``bflb_platform_init`` 函数中，我们已经注册并且打开了一个调试用的串口设备，给用户实现一个 ``MSG`` 的基本功能用作打印输出报文。具体实现如下

.. code-block:: C
    :linenos:

        uart_register(board_get_debug_uart_index(), "debug_log");
        struct device *uart = device_find("debug_log");

        if (uart)
        {
            device_open(uart, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_INT_RX);
            device_set_callback(uart, NULL);
            device_control(uart, DEVICE_CTRL_CLR_INT, (void *)(UART_RX_FIFO_IT));
        }

- 首先调用 ``uart_register`` 函数注册  ``UART`` 设备,当前注册 ``UART0``
- 然后通过 ``find`` 函数找到设备对应的句柄，保存于 ``uart`` 句柄中
- 最后使用 ``device_open`` 以轮询发送和中断接收来打开 ``uart`` 设备，默认关闭中断并且不注册接收中断回调函数

.. code-block:: C
    :linenos:

    struct device *uart = device_find("debug_log");
    if (uart) {
        device_set_callback(uart, shell_irq_callback);
        device_control(uart, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT));
    }

-  通过 ``device_set_callback`` 函数，为 ``UART0`` 注册接收中断服务函数。通过 ``device_control`` 函数打开 ``UART_RX_FIFO_IT`` 中断

.. code-block:: C
    :linenos:

    void shell_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
    {
        uint8_t data;
        if (state == UART_EVENT_RX_FIFO) {
            for (size_t i = 0; i < size; i++) {
                data = *(uint8_t *)(args + i);
                shell_handler(data);
            }
        }
    }

-  中断回调函数中，判断 ``state`` 是否是 ``UART_EVENT_RX_FIFO``,是的话就将接收的字节传入 ``shell_handler`` 函数。


.. code-block:: C
    :linenos:

    shell_init();

-   调用 ``shell_init`` 初始化 shell 组件。


SHELL 命令注册
^^^^^^^^^^^^^^^^^^^^

以下两种注册方式任选

.. code-block:: C
    :linenos:

    void hellowd()
    {
        MSG("hello World\r\n");
    }

    int echo(int argc, char *argv[])
    {
        MSG("%dparameter(s)\r\n", argc);

        for (uint8_t i = 1; i < argc; i++) {
            MSG("%s\r\n", argv[i]);
        }

        return 0;
    }

    SHELL_CMD_EXPORT(hellowd, hellowd test)
    SHELL_CMD_EXPORT(echo, echo test)

.. code-block:: C
    :linenos:

    void hellowd()
    {
        MSG("hello World\r\n");
    }

    int cmd_echo(int argc, char *argv[])
    {
        MSG("%dparameter(s)\r\n", argc);

        for (uint8_t i = 1; i < argc; i++) {
            MSG("%s\r\n", argv[i]);
        }

        return 0;
    }

    SHELL_CMD_EXPORT_ALIAS(hellowd, hellwd,hellowd test)
    SHELL_CMD_EXPORT_ALIAS(cmd_echo, echo,echo test)


编译和烧录
-----------------------------

-  **CDK 编译**

   打开项目中提供的工程文件：shell.cdkproj

   参照 :ref:`windows_cdk_quick_start` 的步骤编译下载即可

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=shell

-  **烧录**

   详见 :ref:`bl_dev_cube`


实验现象
-----------------------------

.. figure:: img/shell_demo.gif
   :alt:

shell test

