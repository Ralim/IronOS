- 将 **CONFIG_GPIO11_FUNC** 设置成 **GPIO_FUN_DAC**
- 如果使用外部参考电压，将 **CONFIG_GPIO7_FUNC** 设置成 **GPIO_FUN_ADC**
- 设置 **BSP_DAC_CLOCK_SOURCE**  为 **ROOT_CLOCK_SOURCE_AUPLL_12288000_HZ**
- 设置 **BSP_DAC_CLOCK_DIV**  为 **23**，能够得到16KHZ

```bash

$ make APP=dac_mono_play_form_flash BOARD=bl706_iot

```