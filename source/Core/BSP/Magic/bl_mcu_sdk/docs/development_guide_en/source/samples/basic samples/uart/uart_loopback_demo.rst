UART - Loopback
====================

This demo is based on UART polling sending and receiving FIFO interrupt mode.

Hardware Connection
-----------------------------

This demo is based on BL706_IOT and the connection method is as follows

::

       GPIO function         GPIO pin
    ----------------------------------
        UART0_TX      <-->     GPIO14
        UART0_RX      <-->     GPIO15

Software Implementation
-----------------------------

- See ``examples/uart/uart_echo`` for the software code

.. code-block:: C
    :linenos:

    #define BSP_UART_CLOCK_SOURCE  ROOT_CLOCK_SOURCE_PLL_96M
    #define BSP_UART_CLOCK_DIV  0

-Configure the ``UART`` device clock source, see ``bsp/board/bl706_iot/clock_config.h``

.. code-block:: C
    :linenos:

    #define CONFIG_GPIO14_FUNC GPIO_FUN_UART0_TX
    #define CONFIG_GPIO15_FUNC GPIO_FUN_UART0_RX

- Configure ``UART`` device multiplexing pins, see ``bsp/board/bl706_iot/pinmux_config.h``

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

- Enable ``BSP_USING_UART0`` and configure the ``UART`` device, see ``bsp/board/bl706_iot/peripheral_config.h``

.. code-block:: C
    :linenos:

    bflb_platform_init();

- In the ``bflb_platform_init`` function, we have registered and opened a serial port device for debugging, to provide users with a basic function of ``MSG`` for printing out messages. The specific implementation is as follows

.. code-block:: C
    :linenos:

        uart_register(board_get_debug_uart_index(), "debug_log", DEVICE_OFLAG_RDWR);
        struct device *uart = device_find("debug_log");

        if (uart)
        {
            device_open(uart, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_INT_RX);
            device_set_callback(uart, NULL);
            device_control(uart, DEVICE_CTRL_CLR_INT, (void *)(UART_RX_FIFO_IT));
        }

- First call the ``uart_register`` function to register the ``UART`` device, currently register ``UART0``
- Then use the ``find`` function to find the handle corresponding to the device and save it in the ``uart`` handle
- Finally use ``device_open`` to open the ``uart`` device with polling sending and interrupt receiving, the interrupt is closed by default and the receiving interrupt callback function is not registered

.. code-block:: C
    :linenos:

    if (uart)
    {
        device_set_callback(uart, uart_irq_callback);
        device_control(uart, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT|UART_RTO_IT));
    }

- Register the user-specified ``UART0`` receiving interrupt service function through the ``device_set_callback`` function. Open the ``RX_FIFO`` and ``RTO`` interrupts through the ``device_control`` function

.. code-block:: C
    :linenos:

    void uart_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
    {
        if (state == UART_EVENT_RX_FIFO)
        {
            device_write(dev,0,(uint8_t *)args,size);
        }
        else if (state == UART_EVENT_RTO)
        {
            device_write(dev,0,(uint8_t *)args,size);
        }
    }


- This function is the interrupt service function of the example, and its function is to send out the received data.

     - ``state`` will return the interrupt type of the ``UART`` device
     - ``args`` contains the return data pointer
     - ``size`` contains the length of the returned data
     - ``dev`` is the handle of the interrupted ``uart`` device

- When an interrupt occurs, the ``device_write`` function will be called to send the received data back.

Compile and Program
-----------------------------

-  **CDK compilation**

   Open project:uart_echo.cdkproj

   Refer to the steps of :ref:`windows_cdk_quick_start` to compile and download

-  **Command compilation**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=uart_echo

-  **Program**

   See :ref:`bl_dev_cube`


Experimental Phenomena
-----------------------------

Video display:

.. raw:: html

    <iframe src="https://player.bilibili.com/player.html?aid=887712205&bvid=BV1xK4y1P7ur&cid=330261457&page=5" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"> </iframe>

