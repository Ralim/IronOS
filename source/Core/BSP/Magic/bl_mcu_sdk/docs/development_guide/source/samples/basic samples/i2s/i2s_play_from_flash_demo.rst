I2S - flash 内置音频播放
=========================

本 demo 演示 I2S 播放flash内置的音频数据, 使用的音频芯片 ES8388 解码, 另外需要使用 I2C 对 ES8388 配置。
若使用其他音频解码芯片，请自行配置，本文档仅重点讲述 I2S 的使用，对音频芯片不做过多介绍。

硬件连接
-----------------------------

本 demo 基于 BL706_AVB 开发板，需要用到 ES8388 音频子板，连接方式如下：

.. list-table::
    :widths: 30 30
    :header-rows: 1

    * - GPIO function
      - GPIO pin
    * - CLK_OUT(MCLK)
      - GPIO6
    * - I2S_BCLK
      - GPIO4
    * - I2S_FS
      - GPIO29
    * - I2S_DO
      - GPIO30
    * - I2S_DI
      - GPIO3
    * - I2C_SCL
      - GPIO16
    * - I2C_SDA
      - GPIO11

如果使用的是BL706_AVB板，请确保FUN1 FUN2两个跳帽处于短接的状态,如下图所示

  .. figure:: img/tiaomao.png
    :alt:


本 demo 用到的内部外设资源如下：

.. list-table::
    :widths: 10 40
    :header-rows: 1

    * - peripheral
      - role
    * - I2S
      - 标准音频数据接口，用于向 ES8388 音频子板传送音频数据
    * - I2C
      - 一种串行通讯总线，用于配置 ES8388 的寄存器设置
    * - DMA-CH2
      - 直接存储访问技术，用于配合 I2S 高效发送音频数据，减少对 CPU 的负载
    * - CLK_OUT
      - 引脚复用对外输出指定时钟信号，用作提供MCLK时钟

软件实现
-----------------------------

-  软件代码见 ``examples/i2s/i2s_play_form_flash``

本文档不再详细介绍 I2C 与 DMA，仅介绍 I2S 相关配置，其他外设可以查看对应的文档。

配置 ``I2S, I2C, CLK_OUT(MCLK)`` 相关复用引脚，见 ``bsp/board/bl706_iot/pinmux_config.h`` 中的宏定义选项:

.. code-block:: C
    :linenos:

    #define CONFIG_GPIO3_FUNC GPIO_FUN_I2S
    #define CONFIG_GPIO4_FUNC GPIO_FUN_I2S
    #define CONFIG_GPIO6_FUNC GPIO_FUN_CLK_OUT
    #define CONFIG_GPIO11_FUNC GPIO_FUN_I2C
    #define CONFIG_GPIO14_FUNC GPIO_FUN_UART0_TX
    #define CONFIG_GPIO15_FUNC GPIO_FUN_UART0_RX
    #define CONFIG_GPIO16_FUNC GPIO_FUN_I2C
    #define CONFIG_GPIO29_FUNC GPIO_FUN_I2S
    #define CONFIG_GPIO30_FUNC GPIO_FUN_I2S

配置 ES8388 的参数，初始化 ES8388 ，其中会用到 I2C 外设，具体过程可见 ``bsp/bsp_common/es8388/es8388.c``

.. code-block:: C
    :linenos:

    static ES8388_Cfg_Type ES8388Cfg = {
        .work_mode = ES8388_CODEC_MDOE,          /*!< ES8388 work mode */
        .role = ES8388_SLAVE,                    /*!< ES8388 role */
        .mic_input_mode = ES8388_DIFF_ENDED_MIC, /*!< ES8388 mic input mode */
        .mic_pga = ES8388_MIC_PGA_3DB,           /*!< ES8388 mic PGA */
        .i2s_frame = ES8388_LEFT_JUSTIFY_FRAME,  /*!< ES8388 I2S frame */
        .data_width = ES8388_DATA_LEN_16,        /*!< ES8388 I2S dataWitdh */
    };

    /* init ES8388 Codec */
    ES8388_Init(&ES8388Cfg);
    ES8388_Set_Voice_Volume(60);

I2S 的配置与使能:

-  使能 ``BSP_USING_I2S0`` 并配置 ``I2S`` 设备配置，见 ``bsp/board/bl706_iot/peripheral_config.h`` 中：

.. code-block:: C
    :linenos:

    #define BSP_USING_I2S0

    #if defined(BSP_USING_I2S0)
    #ifndef I2S0_CONFIG
    #define I2S0_CONFIG                          \
    {                                            \
        .id = 0,                                 \
        .iis_mode = I2S_MODE_MASTER,             \
        .interface_mode = I2S_MODE_LEFT,         \
        .sampl_freq_hz = 16 * 1000,              \
        .channel_num = I2S_FS_CHANNELS_NUM_MONO, \
        .frame_size = I2S_FRAME_LEN_16,          \
        .data_size = I2S_DATA_LEN_16,            \
        .fifo_threshold = 4,                     \
    }

- 先调用 ``i2s_register`` 函数注册  ``I2S`` 设备
- 然后通过 ``find`` 函数找到设备对应的句柄，保存于 ``i2s`` 中
- 最后填写配置参数后，使用 ``device_open`` 来打开 ``I2S`` 设备
- 如果不填写配置参数，会默认使用 ``bsp/board/bl706_iot/peripheral_config.h`` 中预设的参数

.. code-block:: C
    :linenos:

    /* register & open i2s device */
    i2s_register(I2S0_INDEX, "I2S");
    i2s = device_find("I2S");
    if (i2s) {
        I2S_DEV(i2s)->iis_mode = I2S_MODE_MASTER;
        I2S_DEV(i2s)->interface_mode = I2S_MODE_LEFT;
        I2S_DEV(i2s)->sampl_freq_hz = 16 * 1000;
        I2S_DEV(i2s)->channel_num = I2S_FS_CHANNELS_NUM_MONO;
        I2S_DEV(i2s)->frame_size = I2S_FRAME_LEN_16;
        I2S_DEV(i2s)->data_size = I2S_DATA_LEN_16;
        I2S_DEV(i2s)->fifo_threshold = 4;
        device_open(i2s, DEVICE_OFLAG_DMA_TX | DEVICE_OFLAG_DMA_RX);
    }

DMA 的配置与使能
- 为了 I2S 更高效，减少对 CPU 的占用，需要配置 DMA 来搬运数据，具体配置可见 DMA 文档，代码如下：

.. code-block:: C
    :linenos:

    /* register & open dma device */
    dma_register(DMA0_CH2_INDEX, "dma_ch2_i2s_tx", DEVICE_OFLAG_RDWR);
    dma_ch2 = device_find("dma_ch2_i2s_tx");
    if (dma_ch2) {
        DMA_DEV(dma_ch2)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dma_ch2)->transfer_mode = DMA_LLI_CYCLE_MODE;
        DMA_DEV(dma_ch2)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch2)->dst_req = DMA_REQUEST_I2S_TX;
        DMA_DEV(dma_ch2)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch2)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch2)->src_width = DMA_TRANSFER_WIDTH_16BIT;
        DMA_DEV(dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_16BIT;
        DMA_DEV(dma_ch2)->src_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch2)->dst_burst_size = DMA_BURST_4BYTE;
        device_open(dma_ch2, 0);

        /* connect i2s device and dma device */
        device_control(i2s, DEVICE_CTRL_ATTACH_TX_DMA, (void *)dma_ch2);
    }

最后启动 I2S-DMA ，发送包含在程序里烧录到flash里的音频数据：

.. code-block:: C
    :linenos:

    /* start send */
    device_write(i2s, 0, fhm_onechannel_16k_20, sizeof(fhm_onechannel_16k_20));

    MSG("Play Music start \r\n");

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }


编译和烧录
-----------------------------

-  **CDK 编译**

打开项目中提供的工程文件：i2s_play_form_flash.cdkproj

参照 :ref:`windows_cdk_quick_start` 的步骤编译下载即可

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=i2s_play_form_flash

-  **烧录**
	详见 :ref:`bl_dev_cube`

实验现象
-----------------------------

会播放一段约为 20 秒的歌曲