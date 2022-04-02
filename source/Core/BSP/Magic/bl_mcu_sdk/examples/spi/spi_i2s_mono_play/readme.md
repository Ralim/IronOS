## 修改程序

-- **board/bl706_avb/pinmux_config.h** 中 **CONFIG_GPIO10_FUNC** 、 **CONFIG_GPIO19_FUNC** 、 **CONFIG_GPIO20_FUNC** 、 **CONFIG_GPIO21_FUNC**选择 **GPIO_FUN_SPI**

-- **board/bl706_avb/pinmux_config.h** 中 **CONFIG_GPIO11_FUNC** 、 **CONFIG_GPIO16_FUNC**选择 **GPIO_FUN_I2C**

-- **board/bl706_avb/pinmux_config.h** 中 **CONFIG_GPIO6_FUNC** 选择 **GPIO_FUN_CLK_OUT**

-- **board/bl706_avb/clock_config.h** 中 **BSP_AUDIO_PLL_CLOCK_SOURCE** 选择 **ROOT_CLOCK_SOURCE_AUPLL_12288000_HZ**

## 编译命令
```bash

$ make APP=spi_i2s_mono_play BOARD=bl706_avb

```

## 硬件环境
- **BL706_AVB** 开发板上的 **FUN2** 接上跳线帽;
- 准备音频模组 **ES8388** 或者 **WM8978** ;
- 连线方式：

|  AVB_Board | Audio_Module |
|  :------  | :------  |
| I2C-SCL  | I2C-SCL |
| I2C-SDA  | I2C-SDA |
| SPI-CS   | I2S-FS |
| SPI-SCK  | I2S-BCLK |
| SPI-MISO | I2S-DO/SDA |
| I2S-MCLK  | I2S-MCLK |
