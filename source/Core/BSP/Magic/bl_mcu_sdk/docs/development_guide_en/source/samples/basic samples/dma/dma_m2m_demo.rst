DMA - Data Transfer Between RAM
=================================

This demo is based on the memory to memory mode of DMA for data transfer.

Hardware Connection
-----------------------------
None

Software Implementation
-----------------------------

- For the code see ``examples/dma/dma_m2m``

.. code-block:: C
    :linenos:

    #define BSP_USING_DMA0_CH0

    #if defined(BSP_USING_DMA0_CH0)
    #ifndef DMA0_CH0_CONFIG
    #define DMA0_CH0_CONFIG \
    {   \
    .id = 0, \
    .ch = 0,\
    .direction = DMA_MEMORY_TO_MEMORY,\
    .transfer_mode = DMA_LLI_ONCE_MODE, \
    .src_req = DMA_REQUEST_NONE, \
    .dst_req = DMA_REQUEST_NONE, \
    .src_width = DMA_TRANSFER_WIDTH_32BIT , \
    .dst_width = DMA_TRANSFER_WIDTH_32BIT , \
    }
    #endif
    #endif

- Enable ``BSP_USING_DMA0_CH0`` and configure the ``DMA`` device, see ``bsp/board/bl706_iot/peripheral_config.h``

.. code-block:: C
    :linenos:

    dma_register(DMA0_CH0_INDEX, "DMA", DEVICE_OFLAG_RDWR);

    struct device *dma = device_find("DMA");

    if (dma)
    {
        device_open(dma, 0);
        device_set_callback(dma, dma_transfer_done);
        device_control(dma, DEVICE_CTRL_SET_INT, NULL);
    }

- First call the ``dma_register`` function to register a channel of the ``DMA`` device, currently register ``DMA_CH0``
- Then use the ``find`` function to find the handle corresponding to the device and save it in the ``dma`` handle
- Finally use ``device_open`` to open the ``dma`` device in the default mode, call ``device_set_callback`` to register a dma channel 0 interrupt callback function, and call ``device_control`` to open the dma transmission completion interrupt

.. code-block:: C
    :linenos:

    dma_reload(dma,(uint32_t)dma_src_buffer,(uint32_t)dma_dst_buffer,8000);
    dma_channel_start(dma);

- Call the ``dma_reload`` function to supplement the configuration of dma channel 0. A part of the configuration has been supplemented in ``DMA0_CH0_CONFIG``. Here we mainly supplement the source data address, destination data address and total transmission length
- Call ``dma_channel_start`` to start dma transmission

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

- Check whether the data transmission is correct in the interrupt function

Compile and Program
-----------------------------

-  **CDK compilation**

   Open project:dma_m2m.cdkproj

   Refer to the steps of :ref:`windows_cdk_quick_start` to compile and download

-  **Command compilation**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=dma_m2m

-  **Program**

   详见 :ref:`bl_dev_cube`


Experimental Phenomena
-----------------------------
The data in the ``dma_src_buffer`` array is transferred to the ``dma_dst_buffer`` array through DMA channel 0 with a source 32-bit width and a target 32-bit width. After the data transfer is completed, the serial port prints ``dma transfer success``.
