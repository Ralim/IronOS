Clock tree
=========================

Introduction
------------------------

The BL series chips have a lot of clock source selection and provide a clock tree configuration table to facilitate user configuration. Users do not need to call the clock setting interface. Users only need to care about the final system clock and peripheral clock frequency. The clock configuration table is located in the ``clock_config.h`` file under the ``bsp/board/xxx_board`` directory.

Clock Frequency Acquisition Interface
---------------------------------------

**system_clock_get**
^^^^^^^^^^^^^^^^^^^^^^^^

``system_clock_get`` is used to get the system clock frequency.

.. code-block:: C

    uint32_t system_clock_get(enum system_clock_type type);

- **type** the type of system clock frequency

``type`` provide the following types

.. code-block:: C

    enum system_clock_type
    {
        SYSTEM_CLOCK_ROOT_CLOCK = 0,
        SYSTEM_CLOCK_FCLK,
        SYSTEM_CLOCK_BCLK,
        SYSTEM_CLOCK_XCLK,
        SYSTEM_CLOCK_32K_CLK,
        SYSTEM_CLOCK_AUPLL,
    };


**peripheral_clock_get**
^^^^^^^^^^^^^^^^^^^^^^^^

``peripheral_clock_get`` is used to get the peripheral clock frequency.

.. code-block:: C

    uint32_t peripheral_clock_get(enum peripheral_clock_type type);

- **type** peripheral clock frequency type

``type`` provide the following types

.. code-block:: C

    enum peripheral_clock_type
    {
        PERIPHERAL_CLOCK_UART = 0,
        PERIPHERAL_CLOCK_SPI,
        PERIPHERAL_CLOCK_I2C,
        PERIPHERAL_CLOCK_ADC,
        PERIPHERAL_CLOCK_DAC,
        PERIPHERAL_CLOCK_I2S,
        PERIPHERAL_CLOCK_PWM,
        PERIPHERAL_CLOCK_CAM,
    };