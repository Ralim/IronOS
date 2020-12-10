# Notes on RISC-V

## Pinmap

| Pin Number | Name | Function         | Notes       |
| ---------- | ---- | ---------------- | ----------- |
| 17         | PB2  | BOOT2            | Pulldown    |
| 32         |      | IMU INT 1        | N/A         |
| 30         |      | IMU INT 2        | N/A         |
|            | PA4  | Handle Temp      | ADC Input ? |
|            | PA1  | Tip Temp         | ADC Input ? |
|            | PB1  | B Button         | Active High |
|            | PB0  | A Button         | Active High |
|            | PA11 | USB D-           | -           |
|            | PA12 | USB D+           | -           |
|            | PA6  | Tip PWM Out      | -  |
|            | PA0  | Input DC V Sense | ADC Input ? |
|            | PA9  | OLED Reset       |             |
|            | PB7  | SDA              | I2C0_SDA    |
|            | PB6  | SCL              | I2C0_SCL    |

## ADC Configuration

For now running in matching mode for TS100

- X channels DMA in background
- Sample tip using "Intereted" channels using TIMER 0,1,3 TRGO or timer0,1,2 channels
- Using just 12 bit mode for now and move to hardware oversampling later
- use DMA for normal samples and 4x16 bit regs for tip temp
- It has dual ADC's so run them in pair mode

## Timers

### Timer 2

Timer 2 CH0 is tip drive PWM out.
This is fixed at 50% duty cycle and used via the cap to turn on the heater tip.
This should toggle relatively quickly.
