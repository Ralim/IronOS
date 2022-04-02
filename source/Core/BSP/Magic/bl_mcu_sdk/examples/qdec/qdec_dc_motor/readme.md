**board/bl706_iot/pinmux_config.h** 中 **CONFIG_GPIO20_FUNC**、**CONFIG_GPIO21_FUNC** 选择 **GPIO_FUN_PWM**

**board/bl706_iot/pinmux_config.h** 中 **CONFIG_GPIO3_FUNC**、**CONFIG_GPIO4_FUNC** 选择 **GPIO_FUN_QDEC**


**board/bl706_iot/clock_config.h** 中

- **BSP_PWM_CLOCK_SOURCE** 选择 **ROOT_CLOCK_SOURCE_32K_CLK**,也可选择其他时钟源，用其他时钟源时应注意 case 中的设置要匹配
- **BSP_PWM_CLOCK_DIV** 设置成 **1**

- **BSP_QDEC_KEYSCAN_CLOCK_SOURCE** 选择 **ROOT_CLOCK_SOURCE_XCLK**，也可选择其他时钟源，用其他时钟源时应注意 case 中的设置要匹配
- **BSP_QDEC_KEYSCAN_CLOCK_DIV**  设置成  **31**

将两个通道分别接到 H 桥或半桥的两个通道

```bash

$ make APP=qdec_dc_motor BOARD=bl706_iot

```

**正常运行后，可以 QDEC 外设通过读旋转编码器的旋转位置控制 GPIO20 和 GPIO21 引脚分别先后输出 PWM 波，从而电机转速及正反转**
