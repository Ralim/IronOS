Use CDK + Sipeed RV-Debugger Plus to compile and debug
==========================================================

Open Hello World
------------------------------

- After obtaining the SDK, enter the ``examples/hellowd/helloworld/cdk`` directory and double-click ``helloworld.cdkproj`` to open the ``Helloworld`` project

Compile Hello World
------------------------------

.. figure:: img/cdk1.png
   :alt:

   helloworld.cdkproj

- Select the ``OpenOCD_Debug`` project in the drop-down menu. Since Sipeed RV-Debugger Plus uses OpenOCD for debugging, this tutorial is based on the ``OpenOCD_Debug`` project;
- If Sipeed RV-Debugger Plus does not install the driver correctly, please refer to :ref:`sipeed_rv_debugger_plus`, set up the driver, and then proceed to the following steps

- In the CDK toolbar, click the compile icon to compile the project

   - Click ``1`` ``Build Project`` to compile the currently selected project
   - Click ``2`` ``Clean Project`` to clear the results of the last compilation
   - Click ``3`` ``Flash Download`` to download the compiled code to the chip (**Flash download function cannot be used with OpenOCD Debug**)
   - Click ``5`` ``Start/Stop Debug whitout Download`` to debug directly without loading the current bin file
   - You can also right-click the project name in ``Project``, and compile the project through the options in the right-click menu


Program Hello World
----------------------------

- When using the OpenOCD mode debugging method in the CDK, it is not currently supported to directly use the CDK related ``flash`` tool to download the code, so please use the BL Dev Cube tool to program, please refer to :ref:`bl_dev_cube`
- Use CDK to debug after the code is programmed

Run Hello World
----------------------------

- From the menu bar of the CDK ``View->Serial Pane``, open the serial port panel, right-click in the opened ``Serial Pane``, set the serial port, select your corresponding serial port number and baud rate

.. figure:: img/cdk4.png
.. figure:: img/cdk3.png
   :alt:

   CDK Serial Pane setting

- Press the ``RST`` key on the board, you can see the result of the code in the serial port

.. figure:: img/cdk6.png
   :alt:

   HelloWorld！


Debug Hello World
----------------------------

- Click the ``Start/Stop Debugger`` button at the top of the toolbar to enter the debug interface, as shown in the figure below

.. figure:: img/cdk10.png
   :alt:

   Debug HelloWorld！

- In the debug interface, the ``Register`` window can view the ``CPU`` internal register data; the ``Peripherals`` peripheral panel, you can view the corresponding peripheral register data, the top menu bar ``Peripherals-> System Viewer`` can select peripherals; click the relevant debugging button in the upper toolbar to perform operations such as breakpoint setting, single-step debugging, single-step instruction, and full-speed operation. Of course, these operations have corresponding shortcut keys and shortcut setting methods. For details, please refer to ``CDK Help``.

- We click the single step button to run the code, and we can see that the cursor moves to the next sentence of code, and we can see our output ``Hello World!`` displayed in the serial port panel. 
