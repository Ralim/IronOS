**board/bl706_lp/pinmux_config.h** 中 **CONFIG_GPIO12_FUNC** 选择 **GPIO_FUN_WAKEUP**
**board/bl706_lp/pinmux_config.h** 中 **CONFIG_GPIO3_FUNC** 选择 **GPIO_FUN_WAKEUP**
**board/bl706_lp/pinmux_config.h** 中 **CONFIG_GPIO10_FUNC** 选择 **GPIO_FUN_GPIO_EXTI_FALLING_EDGE**

```bash

$ make APP=pds_mode_wakeup BOARD=bl706_lp

```