**board/bl706_lp/pinmux_config.h** 中 **CONFIG_GPIO12_FUNC** 选择 **GPIO_FUN_WAKEUP**

**board/bl706_lp/pinmux_config.h** 中 **CONFIG_GPIO11_FUNC** 选择 **GPIO_FUN_ADC**，ACOMP0 会使用ADC_CH3


```bash

$ make APP=hbn_mode_wakeup BOARD=bl706_lp

```