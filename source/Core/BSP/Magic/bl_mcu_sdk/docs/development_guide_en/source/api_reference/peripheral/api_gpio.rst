GPIO
=========================

Introduction
------------------------

The full name of GPIO is General Purpose Input Output. The GPIO peripherals of BL series chips mainly have the following functions.

- General input and output pull-up and pull-down
- Multiplex function pull-up and pull-down
- Simulation function
- External interrupt (rising edge, falling edge, high level, low level)
- Hardware eliminate jitter
- Drive capacity control

The pin configuration of bl mcu sdk is divided into two kinds.

- GPIO multiplexing function is through a special **pinmux table**, users only need to modify the functions of related pins in the table, and the program will automatically configure these pins. **pinmux table** is located in the ``pinmux_config.h`` file under the ``bsp/board/xxx_board`` directory.
- Configure the pins through the standard GPIO device interface. The disadvantage is that only common input and output and interrupt functions can be configured. It is recommended to use the table to configure the multiplexing functions.

GPIO Device Interface
------------------------

**gpio_set_mode**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_set_mode`` is used to configure the mode of gpio.

.. code-block:: C

    void gpio_set_mode(uint32_t pin, uint32_t mode);

- **pin** the pin to be configured
- **mode** pin function to be configured

``mode`` provides the following types

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

``gpio_write`` is used to set pin level

.. code-block:: C

    void gpio_write(uint32_t pin, uint32_t value);


- **pin** the pin to be set
- **value** the level to be set

**gpio_toggle**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_toggle``: is used to toggle pin level

.. code-block:: C

    void gpio_toggle(uint32_t pin);

- pin: the pin to be toggled

**gpio_read**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_read`` is used to read pin level

.. code-block:: C

    int  gpio_read(uint32_t pin);


- **pin** the pin to read the level
- **return** 0 is low level, 1 is high level

**gpio_attach_irq**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_attach_irq`` is used to attache an interrupt callback function to the interrupt pin

.. code-block:: C

    void gpio_attach_irq(uint32_t pin, void (*cbfun)(uint32_t pin));

- **pin** the pin to which the interrupt callback is attached
- **cbfun** register interrupt callback

**gpio_irq_enable**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_irq_enable`` is used to enable gpio interrupt

.. code-block:: C

    void gpio_irq_enable(uint32_t pin,uint8_t enabled);

- **pin** the pin to turn on or off the interrupt
- **enabled** 0 is to close the interrupt, 1 is to open the interrupt
