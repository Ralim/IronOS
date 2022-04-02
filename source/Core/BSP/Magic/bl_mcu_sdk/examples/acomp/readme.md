**board/bl706_lp/pinmux_config.h** 中 **CONFIG_GPIO7_FUNC** 选择 **GPIO_FUN_ADC**，ACOMP 会使用ADC_CH6
**board/bl706_lp/pinmux_config.h** 中 **CONFIG_GPIO17_FUNC** 选择 **GPIO_FUN_ADC**，ACOMP 会使用ADC_CH3

```bash

$ make APP=acomp BOARD=bl706_lp

```