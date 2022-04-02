**先将本 demo 的 bin 烧录到芯片后，将 gpio15 引脚拉高复位进入 u 盘升级，如果不想升级，将 gpio15 拉低即可**

```bash

$ make APP=usb_msc_flash_boot BOARD=bl706_iot

```
