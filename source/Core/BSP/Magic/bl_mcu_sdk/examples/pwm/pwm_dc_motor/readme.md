**board/bl706_iot/pinmux_config.h** 中 **CONFIG_GPIO20_FUNC**、**CONFIG_GPIO21_FUNC** 选择 **GPIO_FUN_PWM**

**board/bl706_iot/clock_config.h** 中

- **BSP_PWM_CLOCK_SOURCE** 选择 **ROOT_CLOCK_SOURCE_32K_CLK**,也可选择其他时钟源，用其他时钟源时应注意 case 中的设置要匹配
- **BSP_PWM_CLOCK_DIV** 设置成 **0**

将两个通道分别接到 H 桥或半桥的两个通道

```bash

$ make APP=pwm_dc_motor BOARD=bl706_iot

```

**正常运行后，GPIO20 和 GPIO21 引脚分别先后输出 PWM 波**