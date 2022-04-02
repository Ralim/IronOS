FatFs 文件系统读写(SD 卡)
===========================

本 demo 主要演示使用 spi 驱动 sd 卡并挂载 fatfs 文件系统的例子。其中 fatfs api 见 `Fatfs 官网 <http://elm-chan.org/fsw/ff/00index_e.html>`_。

准备工具
-----------------------

- 一张 class10 的 sd 卡和 sd 卡转spi 转接板

硬件连接
-----------------------------

本 demo 基于 BL706_AVB 开发板，连接方式如下：

.. list-table::
    :widths: 30 30
    :header-rows: 1

    * - GPIO function
      - GPIO pin
    * - SD_CS
      - GPIO10
    * - SPI_SCK
      - GPIO19
    * - SPI_MISO
      - GPIO20
    * - SPI_MOSI
      - GPIO21

软件实现
-----------------------------

-  软件代码见 ``examples/spi/spi_dma_sd``

.. code-block:: C
    :linenos:

    #define BSP_SPI_CLOCK_SOURCE  ROOT_CLOCK_SOURCE_BCLK
    #define BSP_SPI_CLOCK_DIV  0

-  配置 ``SPI`` 设备时钟源，见 ``bsp/board/bl706_avb/clock_config.h``

.. code-block:: C
    :linenos:

    #define CONFIG_GPIO19_FUNC GPIO_FUN_SPI
    #define CONFIG_GPIO20_FUNC GPIO_FUN_SPI
    #define CONFIG_GPIO21_FUNC GPIO_FUN_SPI

-  配置 ``SPI`` 设备复用引脚，见 ``bsp/board/bl706_avb/pinmux_config.h``,，如有改动，请自行修改

.. note:: 为适配 bl702_avb 硬件, SPI 的 MOSI 和 MISO 默认进行了调换，如果想要恢复默认，修改 ``drivers/bl702_driver/hal_drv/default_config/spi_config.h`` 中 ``SPI_SWAP_ENABLE`` 为 0

.. code-block:: C
    :linenos:

    #define BSP_USING_SPI0

    #if defined(BSP_USING_SPI0)
    #ifndef SPI0_CONFIG
    #define SPI0_CONFIG \
    {   \
    .id = 0, \
    .clk = 18000000,\
    .mode = SPI_MASTER_MODE, \
    .direction = SPI_MSB_BYTE0_DIRECTION_FIRST, \
    .clk_polaraity = SPI_POLARITY_LOW, \
    .clk_phase = SPI_PHASE_1EDGE, \
    .datasize = SPI_DATASIZE_8BIT, \
    .fifo_threshold = 4, \
    }
    #endif
    #endif

-  使能 ``BSP_USING_SPI0`` 并配置 ``SPI`` 设备配置，见 ``bsp/board/bl706_avb/peripheral_config.h``

.. note:: sd卡的频率不能过高，所以设置成 18M 比较合适

.. code-block:: C
    :linenos:

    #define BSP_USING_DMA0_CH3
    #define BSP_USING_DMA0_CH4

    #if defined(BSP_USING_DMA0_CH3)
    #ifndef DMA0_CH3_CONFIG
    #define DMA0_CH3_CONFIG                             \
        {                                               \
            .id = 0,                                    \
            .ch = 3,                                    \
            .direction = DMA_MEMORY_TO_PERIPH,          \
            .transfer_mode = DMA_LLI_ONCE_MODE,         \
            .src_req = DMA_REQUEST_NONE,                \
            .dst_req = DMA_REQUEST_SPI0_TX,             \
            .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,  \
            .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE, \
            .src_burst_size = DMA_BURST_1BYTE,          \
            .dst_burst_size = DMA_BURST_1BYTE,          \
            .src_width = DMA_TRANSFER_WIDTH_8BIT,       \
            .dst_width = DMA_TRANSFER_WIDTH_8BIT,       \
        }
    #endif
    #endif

    #if defined(BSP_USING_DMA0_CH4)
    #ifndef DMA0_CH4_CONFIG
    #define DMA0_CH4_CONFIG                             \
        {                                               \
            .id = 0,                                    \
            .ch = 4,                                    \
            .direction = DMA_PERIPH_TO_MEMORY,          \
            .transfer_mode = DMA_LLI_ONCE_MODE,         \
            .src_req = DMA_REQUEST_SPI0_RX,             \
            .dst_req = DMA_REQUEST_NONE,                \
            .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE, \
            .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,  \
            .src_burst_size = DMA_BURST_1BYTE,          \
            .dst_burst_size = DMA_BURST_1BYTE,          \
            .src_width = DMA_TRANSFER_WIDTH_8BIT,       \
            .dst_width = DMA_TRANSFER_WIDTH_8BIT,       \
        }
    #endif
    #endif

-  使能 ``BSP_USING_DMA0_CH3``， ``BSP_USING_DMA0_CH4`` 并配置 ``DMA`` 设备配置，见 ``bsp/board/bl706_avb/peripheral_config.h``

.. code-block:: C
    :linenos:

    void fatfs_sd_driver_register(void)
    {
        FATFS_DiskioDriverTypeDef sdDiskioDriver;

        memset(&sdDiskioDriver, 0, sizeof(FATFS_DiskioDriverTypeDef));

        sdDiskioDriver.MMC_disk_status = sd_disk_status;
        sdDiskioDriver.MMC_disk_initialize = sd_disk_initialize;
        sdDiskioDriver.MMC_disk_write = sd_disk_write;
        sdDiskioDriver.MMC_disk_read = sd_disk_read;
        sdDiskioDriver.MMC_disk_ioctl = sd_disk_ioctl;
        sdDiskioDriver.Translate_Result_Code = Translate_Result_Code;
        disk_driver_callback_init(&sdDiskioDriver);
    }

- 实现 fatfs 需要的底层接口

剩余代码见 main.c。

编译和烧录
-----------------------------

-  **CDK 编译**

   打开项目中提供的工程文件：spi_dma_sd.cdkproj

   参照 :ref:`windows_cdk_quick_start` 的步骤编译下载即可

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_avb APP=spi_dma_sd

-  **烧录**

   详见 :ref:`bl_dev_cube`


实验现象
-----------------------------

