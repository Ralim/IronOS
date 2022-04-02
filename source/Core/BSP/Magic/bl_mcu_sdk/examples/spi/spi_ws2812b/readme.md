将 **board/bl706_iot/peripheral_config.h** 中 **BSP_USING_SPI0** 的 spi 外设的 **clk** 配置为 **8000000**; 由于 WS2812B 的时序要求，所以 spi 发送时 clk 必须选择 **8000000**

**board/bl706_iot/pinmux_config.h** 中 **CONFIG_GPIO3_FUNC 、CONFIG_GPIO4_FUNC 、CONFIG_GPIO5_FUNC、CONFIG_GPIO6_FUNC** 选择 **GPIO_FUN_SPI**

```bash

$ make APP=spi_ws2812b BOARD=bl706_iot

```