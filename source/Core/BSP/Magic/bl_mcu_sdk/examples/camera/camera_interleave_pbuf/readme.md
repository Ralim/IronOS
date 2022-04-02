**bsp_common/image_sensor/bsp_image_sensor.c** 中**IMAGE_SENSOR_USE** 选择**IMAGE_SENSOR_GC0308** (目前暂时没有 BF2013 sensor 验证)
**bsp_common/image_sensor/bsp_image_sensor.c** 中**FORMAT_SEL** 选择 **YUYV**
**bsp_common/image_sensor/bsp_image_sensor.c** 中**CAM_MODE** 选择 **CAM_USING_INT_MODE**模式
sensor 选择 GC0308
此 case 用于测 camera 使用 pingpong buf 出图，验证此模式出图是否正常；
首先，当第一个 buf 填满后，会触发中断，通过 memcopy 或者 DMA 将图片原数据搬运到 PSRAM 中存储，
当第二个 buf 填满后，也会触发中断，将图像原数据紧接着上一帧的地址继续存储；

两个 buf 会依次循环，填充 psram 直到一张完整的图像填完。

当一张图填完后，通过串口打印出来（使用串口打印时，注意将无关的 log 关闭，否则会影响图像数据，其次串口波特率要设置低一些，建议 512000、115200 等不然会丢包）
使用 YUV 图像查看工具即可查看图像是否正常


```bash

$ make APP=camera_interleave_pbuf BOARD=bl706_avb SUPPORT_FLOAT=y

```