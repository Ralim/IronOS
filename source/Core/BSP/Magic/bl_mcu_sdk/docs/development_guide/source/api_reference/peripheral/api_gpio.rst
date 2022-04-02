GPIO 设备
=========================

简介
------------------------

GPIO 全称 General Purpose Input Output（通用输入 / 输出），博流系列芯片的 GPIO 外设主要有以下功能。

- 普通输入输出带上下拉
- 复用功能带上下拉
- 模拟功能
- 外部中断（上升沿、下降沿、高电平、低电平）
- 硬件消抖
- 驱动能力控制

**bl mcu sdk** 的引脚配置方式分为两种。

- GPIO 复用功能通过专门的 **pinmux table** ，用户只需要修改 table 中的相关引脚的功能，程序会自动配置这些引脚。**pinmux table** 位于 ``bsp/board/xxx_board`` 目录下 ``pinmux_config.h`` 文件。
- 通过标准的 GPIO 设备接口配置引脚，缺点是只能配置普通的输入输出和中断功能，复用功能建议还是使用 table 进行配置。

GPIO 设备接口
------------------------

**gpio_set_mode**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_set_mode`` 用来配置 gpio 的模式。

.. code-block:: C

    void gpio_set_mode(uint32_t pin, uint32_t mode);

- **pin** 要配置的引脚
- **mode** 要配置的引脚功能

``mode`` 提供以下几种类型

.. code-block:: C

    #define GPIO_OUTPUT_MODE                        0
    #define GPIO_OUTPUT_PP_MODE                     1
    #define GPIO_OUTPUT_PD_MODE                     2
    #define GPIO_INPUT_MODE                         3
    #define GPIO_INPUT_PP_MODE                      4
    #define GPIO_INPUT_PD_MODE                      5
    #define GPIO_ASYNC_RISING_TRIGER_INT_MODE       6
    #define GPIO_ASYNC_FALLING_TRIGER_INT_MODE      7
    #define GPIO_ASYNC_HIGH_LEVEL_INT_MODE          8
    #define GPIO_ASYNC_LOW_LEVEL_INT_MODE           9
    #define GPIO_SYNC_RISING_TRIGER_INT_MODE        10
    #define GPIO_SYNC_FALLING_TRIGER_INT_MODE       11
    #define GPIO_SYNC_HIGH_LEVEL_INT_MODE           12
    #define GPIO_SYNC_LOW_LEVEL_INT_MODE            13

**gpio_write**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_write`` 设置引脚电平

.. code-block:: C

    void gpio_write(uint32_t pin, uint32_t value);


- **pin** 要设置的引脚
- **value** 要设置的电平

**gpio_toggle**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_toggle`` 翻转引脚电平

.. code-block:: C

    void gpio_toggle(uint32_t pin);

- **pin** 要翻转的引脚

**gpio_read**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_read`` 读取引脚电平

.. code-block:: C

    int  gpio_read(uint32_t pin);


- **pin** 要读取电平的引脚
- **return** 0 为低电平，1 为高电平

**gpio_attach_irq**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_attach_irq`` 为中断引脚附加中断回调函数

.. code-block:: C

    void gpio_attach_irq(uint32_t pin, void (*cbfun)(uint32_t pin));

- **pin** 要附加中断回调的引脚
- **cbfun** 要注册的中断回调函数

**gpio_irq_enable**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_irq_enable`` 开启gpio某个引脚的中断

.. code-block:: C

    void gpio_irq_enable(uint32_t pin,uint8_t enabled);

- **pin** 要开启或者关闭中断的引脚
- **enabled** 0 为关闭中断，1 为打开中断
