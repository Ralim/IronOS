BOOT2 IAP
============================

IAP (In Application Programming) is to program some areas of User Flash during the running of the user program. The purpose is to update the firmware program in the product through the reserved communication port after the product is released.

If you need to realize the IAP function, that is, it will be automatically updated when the user program is running. Need to write two project codes when designing the firmware program. The first project program does not perform normal functional operations, but only receives programs or data through some communication channel (such as USB, USART) to update the second part of the code, and the second project code is the real function Code.

Bouffalo Lab provides the boot2_iap.bin file and releases it simultaneously with the Dev Cube software package. Users can use Dev Cube to program boot2_iap.bin into the target board. After programming once, the user code can be updated online through the IAP function.

bl_mcu_sdk contains the source code of boot2_iap, users can check the code in examples/boot2_iap, and complete the compilation and programming. For the compilation and programming process, please refer to the introduction of "Quick Development Guide".

Prepare
-----------------------

- The latest version of Dev Cube
- Bl706
- TTL-USB


Experimental phenomena
-----------------------------

For the specific steps of using Dev Cube to complete the IAP function, please refer to `DevCube User Manual <https://dev.bouffalolab.com/media/upload/doc/DevCube%E7%94%A8%E6%88% B7%E6%89%8B%E5%86%8Cv1.3.pdf>`_ "IAP Program Download".
