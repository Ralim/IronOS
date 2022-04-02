I2C - AT24CXX read and write
================================

Hardware Connection
-----------------------------

This demo is based on BL706_IOT, add AT24CXX circuit by yourself, the connection method is as follows

::

       GPIO function         GPIO pin
    ----------------------------------
        I2C_SCL      <-->     GPIO11
        I2C_SDA      <-->     GPIO16


Software Implementation
-----------------------------

- See ``examples/i2c/i2c_at24cxx`` for the software code

.. code-block:: C
    :linenos:

    #define BSP_I2C_CLOCK_SOURCE  ROOT_CLOCK_SOURCE_BCLK
    #define BSP_I2C_CLOCK_DIV  0

- Configure the ``I2C`` device clock source, see ``bsp/board/bl706_iot/clock_config.h``

.. code-block:: C
    :linenos:

    #define CONFIG_GPIO11_FUNC GPIO_FUN_I2C
    #define CONFIG_GPIO16_FUNC GPIO_FUN_I2C

- Configure ``I2C`` device multiplexing pins, see ``bsp/board/bl706_iot/peripheral_config.h``

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

- Enable ``BSP_USING_I2C0`` and configure ``I2C`` device, see ``bsp/board/bl706_iot/peripheral_config.h``

.. code-block:: C
    :linenos:

    i2c_register(I2C0_INDEX, "i2c", DEVICE_OFLAG_RDWR);
    struct device *i2c0 = device_find("i2c");

    if (i2c0)
    {
        MSG("device find success\r\n");
        device_open(i2c0, 0);
    }

- First call the ``i2c_register`` function to register the ``I2C`` device, currently register ``I2C0``
- Then use the ``find`` function to find the handle corresponding to the device and save it in the ``i2c0`` handle
- Finally use ``device_open`` to open the ``I2C0`` device in the default mode

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

- Call ``i2c_transfer`` to transfer two ``msg``, one ``msg`` represents writing 8-byte data to eeprom, and one ``msg`` represents reading 8-byte data from eeprom

Compile and Program
-----------------------------

-  **CDK compilation**

   Open project:i2c_at24cxx.cdkproj

   Refer to the steps of :ref:`windows_cdk_quick_start` to compile and download

-  **Command compilation**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=i2c_at24cxx

-  **Program**

   See :ref:`bl_dev_cube`

Experimental Phenomena
-----------------------------
