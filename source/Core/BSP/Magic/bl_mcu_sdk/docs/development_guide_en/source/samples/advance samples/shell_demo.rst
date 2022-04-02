SHELL Command Debugging
=========================

In order to facilitate the user to use the pc or other controllers to debug the functions of the development board (non-emulator debugging), we provide users with a shell command component, which is similar to the command operation under linux. The user sends commands on the PC or other control terminals, and sends the data to the shell of the development board through serial port, usb, Ethernet, Bluetooth, wifi, etc. The shell will read the received commands for analysis and scan the registered internal functions. After scanning the matching function, execute the matching function, and return the incoming key value and the result of the function execution to the pc or control terminal in real time . It should be noted that the controller side needs to send the key value of the standard keyboard.
This demo will demonstrate how to use **shell** to debug commands through the serial port.

This shell component has the following functions:

- Support standard keyboard character control
- Support command auto completion
- Support up and down keys to view historical commands
- Support left and right keys to modify commands
- Support file system and network system debugging

Prepare
-----------------------

- PC control terminal uses serial terminal software: xshell or mobaxterm
- Connection medium: usb to serial port or network or usb

Hardware Connection
-----------------------------

This demo is based on BL706_IOT and the connection method is as follows

::

       GPIO function         GPIO pin
    ----------------------------------
        UART0_TX      <-->     GPIO14
        UART0_RX      <-->     GPIO15


Software Implementation
--------------------------

Shell porting to serial port
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- See ``examples/shell`` for the software code

.. code-block:: C
    :linenos:

    #define BSP_UART_CLOCK_SOURCE  ROOT_CLOCK_SOURCE_PLL_96M
    #define BSP_UART_CLOCK_DIV  0

- Configure the ``UART`` device clock source, see ``bsp/board/bl706_iot/clock_config.h``

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

- Enable ``BSP_USING_UART0`` and configure ``UART`` device configuration, see ``bsp/board/bl706_iot/peripheral_config.h``

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

    struct device *uart = device_find("debug_log");
    if (uart) {
        device_set_callback(uart, shell_irq_callback);
        device_control(uart, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT));
    }

- Register the receive interrupt service function for ``UART0`` through the ``device_set_callback`` function. Open the ``UART_RX_FIFO_IT`` interrupt via the ``device_control`` function

.. code-block:: C
    :linenos:

    void shell_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
    {
        uint8_t data;
        if (state == UART_EVENT_RX_FIFO) {
            data = *(uint8_t *)args;
            shell_handler(data);
        }
    }

-In the interrupt callback function, judge whether the ``state`` is ``UART_EVENT_RX_FIFO``, and if it is, pass the received byte to the ``shell_handler`` function.

.. code-block:: C
    :linenos:

    shell_init();

- Call ``shell_init`` to initialize the shell components.


SHELL Command Registration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Shell command registration uses the following two macros

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



Compile and Program
-----------------------------

-  **CDK compile**

   Open project：shell.cdkproj

   Refer to the steps of :ref:`windows_cdk_quick_start` to compile and download

-  **Command compilation**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=shell SUPPORT_SHELL=y

-  **Program**

   See :ref:`bl_dev_cube`


Experimental Phenomena
-----------------------------

.. figure:: img/shell_demo.gif
   :alt:

shell test

