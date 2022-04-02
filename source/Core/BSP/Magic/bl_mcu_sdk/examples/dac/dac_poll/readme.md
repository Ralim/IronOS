- 将 **CONFIG_GPIO11_FUNC** 设置成 **GPIO_FUN_DAC**
- 如果使用外部参考电压，将 **CONFIG_GPIO7_FUNC** 设置成 **GPIO_FUN_ADC**
- 设置 **BSP_DAC_CLOCK_SOURCE**  为 **ROOT_CLOCK_SOURCE_AUPLL_24000000_HZ**
- 设置 **BSP_DAC_CLOCK_DIV**  为 **2**，能够得到500KHZ

```bash

$ make APP=dac_poll BOARD=bl706_iot

```