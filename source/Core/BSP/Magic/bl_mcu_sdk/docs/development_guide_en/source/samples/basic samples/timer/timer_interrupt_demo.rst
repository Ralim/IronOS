TIMER - Second Timing Interrupt
==================================

    This demo is based on TIMER interrupt mode with second timing.

Hardware Connection
-----------------------------
None

Software Implementation
-----------------------------

-  See ``examples/timer/timer_int`` for the software code

.. code-block:: C
    :linenos:

    #define BSP_TIMER0_CLOCK_SOURCE ROOT_CLOCK_SOURCE_FCLK
    #define BSP_TIMER0_CLOCK_DIV    0

-  Configure ``TIMER`` device clock source,see ``bsp/board/bl706_iot/clock_config.h``

.. code-block:: C
    :linenos:

    #define BSP_USING_TIMER0

    #if defined(BSP_USING_TIMER0)
    #ifndef TIMER0_CONFIG
    #define TIMER0_CONFIG                           \
        {                                           \
            .id = 0,                                \
            .cnt_mode = TIMER_CNT_PRELOAD,          \
            .trigger = TIMER_PRELOAD_TRIGGER_COMP2, \
            .reload = 0,                            \
            .timeout1 = 1000000,                    \
            .timeout2 = 2000000,                    \
            .timeout3 = 3000000,                    \
        }
    #endif
    #endif

-  Enable ``BSP_USING_TIMER0`` and configure ``TIMER0`` device,see ``bsp/board/bl706_iot/peripheral_config.h``

.. code-block:: C
    :linenos:

    timer_register(TIMER0_INDEX, "timer0");

    timer0 = device_find("timer0");

    if (timer0) {
        device_open(timer0, DEVICE_OFLAG_INT_TX); /* 1s,2s,3s timing*/
        device_set_callback(timer0, timer0_irq_callback);
        device_control(timer0, DEVICE_CTRL_SET_INT, (void *)(TIMER_COMP0_IT | TIMER_COMP1_IT | TIMER_COMP2_IT));
    } else {
        MSG("timer device open failed! \n");
    }

- Call ``timer_register`` function to register ``TIMER`` device, currently register ``TIMER0``
- Then use the ``find`` function to find the handle corresponding to the device and save it in the ``timer0`` handle
- Finally use ``device_open`` to open the ``timer0`` device in interrupt mode
- Call ``device_set_callback`` to register irq callback named ``timer0_irq_callback``. Call ``device_control`` to enable irq and configure timing period.

.. code-block:: C
    :linenos:

    void timer0_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
    {
        if (state == TIMER_EVENT_COMP0) {
            MSG("timer event comp0! \r\n");
        } else if (state == TIMER_EVENT_COMP1) {
            MSG("timer event comp1! \r\n");
        } else if (state == TIMER_EVENT_COMP2) {
            BL_CASE_SUCCESS;
            timer_timeout_cfg_t cfg = { 2, 12000000 }; /*modify compare id 2 timeout 12s*/
            device_write(dev, 0, &cfg, sizeof(timer_timeout_cfg_t));
            MSG("timer event comp2! \r\n");
        }
    }

- In irq callback,try to determine whether compare id flag is coming.
- Call ``device_write`` to modify compare id 2 timeout with 12s.

Compile and Program
-----------------------------

-  **CDK compilation**

   Open project:timer_int.cdkproj

   Refer to the steps of :ref:`windows_cdk_quick_start` to compile and download

-  **Command compilation**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=timer_int

-  **Program**

   See :ref:`bl_dev_cube`


Experimental Phenomena
-----------------------------

