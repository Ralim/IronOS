**board/bl706_iot/clock_config.h** 中,

**BSP_QDEC_KEYSCAN_CLOCK_SOURCE** 设置成 **ROOT_CLOCK_SOURCE_32K_CLK**
**BSP_QDEC_KEYSCAN_CLOCK_DIV** 设置成 **0**

**board/bl706_iot/pinmux_config.h** 中, 以下宏设置成 **GPIO_FUN_KEY_SCAN_ROW**

- **CONFIG_GPIO16_FUNC**
- **CONFIG_GPIO17_FUNC**
- **CONFIG_GPIO18_FUNC**
- **CONFIG_GPIO19_FUNC**

**board/bl706_iot/pinmux_config.h** 中, 以下宏设置成 **GPIO_FUN_KEY_SCAN_COL**

- **CONFIG_GPIO0_FUNC**
- **CONFIG_GPIO1_FUNC**
- **CONFIG_GPIO2_FUNC**
- **CONFIG_GPIO3_FUNC**

```bash

$ make APP=keyscan_it BOARD=bl706_iot

```