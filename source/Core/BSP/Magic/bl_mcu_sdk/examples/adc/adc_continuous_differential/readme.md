# 编译指令
```bash

$ make APP=adc_continuous_differential BOARD=bl706_iot

```

# 基本配置

此示例需要将对应board目录下的pinmux_config文件中的GPIO11与GPIO12配置为ADC复用功能，
并且将对应board目录下的clock_config.h文件中的ADC时钟配置为相应的时钟。
# 基本原理

此示例会通过将GPIO11与GPIO12用作ADC的差分输入端，实时将差分结果打印到串口。
# 关键参数

正端通道配置 posChList 数组
负端通道配置 negChList 数组

通道与GPIO映射表，
bl70x系列如下

| GPIO          |  Channel |
| ------------- | -------- |
| GPIO_PIN_8,   |  CH0 IO  |
| GPIO_PIN_15,  |  CH1 IO  |
| GPIO_PIN_17,  |  CH2 IO  |
| GPIO_PIN_11,  |  CH3 IO  |
| GPIO_PIN_12,  |  CH4 IO  |
| GPIO_PIN_14,  |  CH5 IO  |
| GPIO_PIN_7,   |  CH6 IO  |
| GPIO_PIN_9,   |  CH7 IO  |
| GPIO_PIN_18,  |  CH8 IO  |
| GPIO_PIN_19,  |  CH9 IO  |
| GPIO_PIN_20,  |  CH10 IO |
| GPIO_PIN_21,  |  CH11 IO |


# 操作流程

1、 首先注册ADC设备，将其命名为adc_one_diff

2、 配置ADC设备的基本参数，配置为差分连续模式

3、 判断通道对应的gpio是否配置为ADC复用功能

4、 开始ADC转换

5、 读取ADC的结果并打印在LOG口

# 运行结果

差分转换结果会打印在UART的LOG口