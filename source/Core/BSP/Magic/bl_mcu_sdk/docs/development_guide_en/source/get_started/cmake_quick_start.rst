New Project Guide based on cmake framework
=============================================

This document will introduce how to create a new project based on this SDK.

Examples directory structure
------------------------------------

There are two levels of subdirectories under ``bl_mcu_sdk/examples``, the first level is the folders of different peripherals. The second level is a specific test case of the peripheral, and the directory usually contains a ``CMakeList.txt`` and the source code related to the case.


Add a single source file project
-----------------------------------

.. note::
   The source file must contain the c program entry, usually the ``main`` function, the source file may not be called ``main.c``

- Create a new ``my_case`` folder under ``examples`` to store your case
- Create a new folder that needs to be tested in the ``my_case`` directory, such as ``gpio_case``
- Then add the ``main.c`` and ``CMakeLists.txt`` files in the ``gpio_case`` directory


The directory structure is as follows:

.. code-block:: bash
   :linenos:

   bl_mcu_sdk
   ├── examples
       ├── my_case
           ├── gpio_case
           │   ├──CMakeLists.txt
           │   └──main.c
           └── xxxx_case

`CMakeLists.txt`:

.. code-block:: cmake
   :linenos:
   :emphasize-lines: 1-2

   set(mains main.c)
   generate_bin()

- After writing the code in main.c, compile it in the bl_mcu_sdk directory. The compilation command is as follows:

.. code-block:: bash
   :linenos:

   make BOARD=bl706_iot APP=gpio_case


Add multiple source file projects
-------------------------------------

.. note::
    One of the source files must contain the c program entry, usually the ``main`` function, the source file does not need to be called ``main.c``. This source file depends on other source files.

- The steps for adding a multi-source file project are the same as the basic steps for adding a single source file project
- Add ``test1.c``, ``test1.h``, ``test2.c``, ``test2.h`` and other source files and header files that need to be relied on in the ``gpio_case`` directory

The directory structure is as follows:

.. code-block:: bash
   :linenos:

   bl_mcu_sdk
   ├── examples
       ├── my_case
           ├── gpio_case
           │   ├──CMakeLists.txt
           │   ├──test1.c
           │   ├──test1.h
           │   ├──test2.c
           │   ├──test2.h
           │   └──main.c
           └── xxxx_case

- At this time, the ``CMakeLists.txt`` file needs to add the corresponding dependent source file, the content is as follows:

`CMakeLists.txt`:

.. code-block:: cmake
   :linenos:
   :emphasize-lines: 2

   set(mains main.c)
   set(TARGET_REQUIRED_SRCS test1.c test2.c)
   generate_bin()

- After writing the code, compile it in the bl_mcu_sdk directory. The compilation command is as follows:

.. code-block:: bash
   :linenos:

   make BOARD=bl706_iot APP=gpio_case


Add a new project with dependent libraries
---------------------------------------------

- The steps for adding a new project with dependent libraries are the same as the basic steps for adding a single source file project
- If the dependent library used already exists in this SDK, you only need to modify CMakeLists.txt

   - If the dependent library does not exist, you need to add it yourself, please refer to the follow-up instructions for details


If it already exists, the directory structure is as follows:

.. code-block:: bash
   :linenos:

   bl_mcu_sdk
   ├── examples
       ├── my_case
           ├── gpio_case
           │   ├──CMakeLists.txt
           │   └──main.c
           └── xxxx_case

- At this time, the ``CMakeLists.txt`` file needs to add the corresponding dependent library files. For example, we add the FreeRTOS component library, the content is as follows:

`CMakeLists.txt`:

.. code-block:: cmake
   :linenos:
   :emphasize-lines: 1

   set(TARGET_REQUIRED_LIBS freertos)
   set(mains main.c)
   generate_bin()

- After writing the code, compile it in the bl_mcu_sdk directory. The compilation command is as follows:

.. code-block:: bash
   :linenos:

   make BOARD=bl706_iot APP=gpio_case  SUPPORT_FREERTOS=y

Add a new project and set the private compilation option (gcc option)
-------------------------------------------------------------------------

- The steps of adding a new project are basically the same as adding a single source file project
- Mainly modify the CMakeLists.txt file and add private compilation options

`CMakeLists.txt`:

.. code-block:: cmake
   :linenos:
   :emphasize-lines: 2

   set(mains main.c)
   set(TARGET_REQUIRED_PRIVATE_OPTIONS -Ofast)
   generate_bin()

- After writing the code, compile it in the bl_mcu_sdk directory. The compilation command is as follows:

.. code-block:: bash
   :linenos:

   make BOARD=bl706_iot APP=gpio_case


Add a new project and set up a private link script (ld)
------------------------------------------------------------

- The steps of adding a new project are basically the same as adding a single source file project
- Add a private link script file, such as ``gpio_test_case.ld``

The directory structure is as follows:

.. code-block:: bash
   :linenos:

   bl_mcu_sdk
   ├── examples
       ├── my_case
           ├── gpio_case
           │   ├──CMakeLists.txt
           │   ├──gpio_test_case.ld
           │   └──main.c
           └── xxxx_case

- Modify the CMakeLists.txt file and add private link script settings

`CMakeLists.txt`:

.. code-block:: cmake
   :linenos:
   :emphasize-lines: 1

   set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/gpio_test_case.ld)
   set(mains main.c)
   generate_bin()

- After writing the code, compile it in the bl_mcu_sdk directory. The compilation command is as follows:

.. code-block:: bash
   :linenos:

   make BOARD=bl706_iot APP=gpio_case


Add a new project and its dependent source files and library files
----------------------------------------------------------------------


