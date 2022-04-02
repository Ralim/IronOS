DMA - RAM间数据搬运
====================

本 demo 基于 DMA，方向为 memory to memory 模式，进行数据的搬运。

硬件连接
-----------------------------
无

软件实现
-----------------------------

-  软件代码见 ``examples/dma/dma_m2m``

.. code-block:: C
    :linenos:

    #define BSP_USING_DMA0_CH0

    #if defined(BSP_USING_DMA0_CH0)
    #ifndef DMA0_CH0_CONFIG
    #define DMA0_CH0_CONFIG                            \
        {                                              \
            .id = 0,                                   \
            .ch = 0,                                   \
            .direction = DMA_MEMORY_TO_MEMORY,         \
            .transfer_mode = DMA_LLI_ONCE_MODE,        \
            .src_req = DMA_REQUEST_NONE,               \
            .dst_req = DMA_REQUEST_NONE,               \
            .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE, \
            .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE, \
            .src_burst_size = DMA_BURST_4BYTE,         \
            .dst_burst_size = DMA_BURST_4BYTE,         \
            .src_width = DMA_TRANSFER_WIDTH_32BIT,     \
            .dst_width = DMA_TRANSFER_WIDTH_32BIT,     \
        }
    #endif
    #endif

-  使能 ``BSP_USING_DMA0_CH0`` 并配置 ``DMA`` 设备配置，见 ``bsp/board/bl706_iot/peripheral_config.h``

.. code-block:: C
    :linenos:

    dma_register(DMA0_CH0_INDEX, "dma_ch0");

    struct device *dma_ch0 = device_find("dma_ch0");

    if (dma_ch0) {
        DMA_DEV(dma_ch0)->direction = DMA_MEMORY_TO_MEMORY;
        DMA_DEV(dma_ch0)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch0)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch0)->dst_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch0)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch0)->dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch0)->src_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch0)->dst_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch0)->src_width = DMA_TRANSFER_WIDTH_32BIT;
        DMA_DEV(dma_ch0)->dst_width = DMA_TRANSFER_WIDTH_32BIT;
        device_open(dma_ch0, 0);
        device_set_callback(dma_ch0, dma_transfer_done);
        device_control(dma_ch0, DEVICE_CTRL_SET_INT, NULL);
    }

- 首先调用 ``dma_register`` 函数注册 ``DMA`` 设备的一个通道,当前注册 ``DMA_CH0``
- 然后通过 ``find`` 函数找到设备对应的句柄，保存于 ``dma_ch0`` 句柄中
- 最后使用 ``device_open`` 以默认模式来打开 ``dma_ch0`` 设备，调用 ``device_set_callback`` 注册一个 dma 通道0中断回调函数，调用 ``device_control`` 开启dma传输完成中断

.. code-block:: C
    :linenos:

    dma_reload(dma_ch0,(uint32_t)dma_src_buffer,(uint32_t)dma_dst_buffer,8000);
    dma_channel_start(dma_ch0);

- 调用 ``dma_reload`` 函数对 dma 通道0的配置进行补充，``DMA0_CH0_CONFIG`` 中已经补充了一部分配置，这边主要补充源数据地址和目标数据地址以及传输总长度
- 调用 ``dma_channel_start`` 开启 dma 传输

.. code-block:: C
    :linenos:

    void dma_transfer_done(struct device *dev, void *args, uint32_t size, uint32_t state)
    {
        uint32_t index=0;

        if(!state)
        {
            MSG("dma transfer task done\r\n");

            for(index=0;index<8000;index++){
                if(dma_dst_buffer[index]!=0xff){
                    MSG("dma transfer error\r\n");
                }
            }

            MSG("dma transfer success\r\n");
        }

    }

- 在中断函数中判断数据传输是否正确

编译和烧录
-----------------------------

-  **CDK 编译**

   打开项目中提供的工程文件：dma_m2m.cdkproj

   参照 :ref:`windows_cdk_quick_start` 的步骤编译下载即可

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=dma_m2m

-  **烧录**

   详见 :ref:`bl_dev_cube`


实验现象
-----------------------------
``dma_src_buffer`` 数组中的数据通过 DMA 通道 0 ，以源 32 位宽、目标 32 位宽传输到 ``dma_dst_buffer`` 数组中，数据传输完成并完整，串口打印 ``dma transfer success``。