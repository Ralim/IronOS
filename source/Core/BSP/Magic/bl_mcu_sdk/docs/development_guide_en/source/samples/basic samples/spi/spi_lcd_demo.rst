SPI - TFT LCD Display
=======================

Hardware Connection
-----------------------------

This demo is based on BL706_AVB, and the connection method is as follows

::

       GPIO function         GPIO pin
    ----------------------------------
        LCD_CS      <-->     GPIO10
        LCD_DC      <-->     GPIO22
        SPI_SCK     <-->     GPIO19
        SPI_MISO    <-->     GPIO20
        SPI_MOSI    <-->     GPIO21


Software Implementation
-----------------------------

- See ``examples/spi/spi_lcd`` for the software code

.. code-block:: C
    :linenos:

    #define BSP_SPI_CLOCK_SOURCE  ROOT_CLOCK_SOURCE_BCLK
    #define BSP_SPI_CLOCK_DIV  0

- Configure the ``SPI`` device clock source, see ``bsp/board/bl706_avb/clock_config.h``

.. code-block:: C
    :linenos:

    #define CONFIG_GPIO19_FUNC GPIO_FUN_SPI
    #define CONFIG_GPIO20_FUNC GPIO_FUN_SPI
    #define CONFIG_GPIO21_FUNC GPIO_FUN_SPI

- Configure ``SPI`` device multiplexing pins, see ``bsp/board/bl706_avb/pinmux_config.h``

.. note::
    ``bsp/board/bl706_avb/pinmux_config.h`` is currently used by all demo demos, so you need to select ``PINMUX_SELECT`` as ``PINMUX_LVGL``, and open one of the demos

.. note::
    In order to adapt to the bl702_avb hardware, the MOSI and MISO of SPI have been swapped by default. If you want to restore the default, modify ``SPI_SWAP_ENABLE`` in ``drivers/bl702_driver/hal_drv/default_config/spi_config.h`` to 0

.. code-block:: C
    :linenos:

    #define BSP_USING_SPI0

    #if defined(BSP_USING_SPI0)
    #ifndef SPI0_CONFIG
    #define SPI0_CONFIG \
    {   \
    .id = 0, \
    .clk = 36000000,\
    .mode = SPI_MASTER_MODE, \
    .direction = SPI_MSB_BYTE0_DIRECTION_FIRST, \
    .clk_polaraity = SPI_POLARITY_LOW, \
    .clk_phase = SPI_PHASE_1EDGE, \
    .datasize = SPI_DATASIZE_8BIT, \
    .fifo_threshold = 4, \
    }
    #endif
    #endif

- Enable ``BSP_USING_SPI0`` and configure ``SPI`` device, see ``bsp/board/bl706_avb/peripheral_config.h``

.. code-block:: C
    :linenos:

    gpio_set_mode(LCD_CS_PIN,GPIO_OUTPUT_MODE);
    gpio_set_mode(LCD_DC_PIN,GPIO_OUTPUT_MODE);
    gpio_write(LCD_CS_PIN,1); //CS1
    gpio_write(LCD_DC_PIN,1); //DC

    spi0 = device_find("spi0");
    if(spi0)
    {
        device_close(spi0);
    }
    else{
        spi_register(SPI0_INDEX,"spi0",DEVICE_OFLAG_RDWR);
        spi0 = device_find("spi0");
    }
    if(spi0)
    {
        device_open(spi0,DEVICE_OFLAG_STREAM_TX|DEVICE_OFLAG_STREAM_RX);
    }

- Configure the ``LCD_CS`` and ``LCD_DC`` pins as output mode and pull up
- Call ``spi_register`` function to register ``SPI`` device, currently register ``SPI0``
- Then use the ``find`` function to find the handle corresponding to the device and save it in the ``spi0`` handle
- Finally use ``device_open`` to open the ``spi0`` device in polling sending mode

.. code-block:: C
    :linenos:

    void LCD_WR_Byte(uint8_t data)
    {
        CS1_LOW;
        DC_HIGH;
        spi_transmit(spi0,&data,1,SPI_TRANSFER_TYPE_8BIT);
        CS1_HIGH;
    }

    void LCD_WR_HalfWord(uint16_t data)
    {
        CS1_LOW;
        DC_HIGH;
        spi_transmit(spi0,&data,1,SPI_TRANSFER_TYPE_16BIT);
        CS1_HIGH;
    }

    void LCD_WR_Word(uint32_t data)
    {
        CS1_LOW;
        DC_HIGH;
        spi_transmit(spi0,&data,1,SPI_TRANSFER_TYPE_32BIT);
        CS1_HIGH;
    }

- Provide interface for LCD display driver

Compile and Program
-----------------------------

-  **CDK compilation**

   Open project:spi_lcd.cdkproj

   Refer to the steps of :ref:`windows_cdk_quick_start` to compile and download

-  **Command compilation**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_avb APP=spi_lcd

-  **Program**

   See :ref:`bl_dev_cube`

Experimental Phenomena
-----------------------------

.. figure:: img/spi_lcd.png
   :alt:

spi display!
