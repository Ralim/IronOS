BOOT2 IAP
============================

IAP 是 In Application Programming 的首字母缩写，IAP 是用户自己的程序在运行过程中对 User Flash 的部分区域进行烧写，
目的是为了在产品发布后可以方便地通过预留的通信口对产品中的固件程序进行更新升级。

通常在用户需要实现 IAP 功能时，即用户程序运行中作自身的更新操作，需要在设计固件程序时编写两个项目代码，
第一个项目程序不执行正常的功能操作，而只是通过某种通信管道(如 USB、USART )接收程序或数据，
执行对第二部分代码的更新；第二个项目代码才是真正的功能代码。

Bouffalo Lab 为 IAP 功能应用提供了相应的 boot2_iap.bin 文件，并将其与 Dev Cube 软件包同步发布。
用户可以配合 Dev Cube 图形界面烧写软件，将已有的 boot2_iap.bin 文件烧录进目标板，烧录
一次之后，就可以通过 IAP 功能，在线更新用户代码。

bl_mcu_sdk 开源了 boot2_iap 的软件源码，用户可以到 ``examples/boot2_iap`` 查阅代码，并完成编译烧写。编译烧写过程可以参考《快速开发指南》介绍。

准备工具
-----------------------

- 最新版本的 Dev Cube
- BL706 开发板
- TTL-USB 电平转换芯片


实验现象
-----------------------------

使用 Dev Cube 完成 IAP 功能的具体步骤，请参阅 `DevCube用户手册 <https://dev.bouffalolab.com/media/upload/doc/DevCube%E7%94%A8%E6%88%B7%E6%89%8B%E5%86%8Cv1.3.pdf>`_ <IAP 程序下载>章节。