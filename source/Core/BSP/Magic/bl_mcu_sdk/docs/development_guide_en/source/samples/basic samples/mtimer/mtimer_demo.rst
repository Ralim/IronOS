MTIMER - System Timer
========================

This demo is based on a 64-bit timer (MTIMER) that comes with the risc-v kernel. This demo can provide reference for os tick.

Hardware Connection
-----------------------------
None

Software Implementation
-----------------------------

- See ``examples/systick`` for the software code

.. note::
    The ``mtimer`` clock defaults to 1M after frequency division, which is convenient for later use and reduces calculation time.

.. code-block:: C
    :linenos:

    void systick_isr()
    {
        static uint32_t tick=0;
        tick++;
        MSG("tick:%d\r\n",tick);
    }

    bflb_platform_set_alarm_time(1000000,systick_isr);

- Use the above code to set the mtimer timing time to 1s, and register the interrupt callback function.

Compile and Program
-----------------------------

-  **CDK compilation**

   Open project:systick.cdkproj

   Refer to the steps of :ref:`windows_cdk_quick_start` to compile and download

-  **Command compilation**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=systick

-  **Program**

   See :ref:`bl_dev_cube`

Experimental Phenomena
-----------------------------

The ``tick`` value is incremented by 1 per second and printed through the serial port.
