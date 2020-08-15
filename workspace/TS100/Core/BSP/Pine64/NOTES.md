# Notes on RISC-V

## Pinmap

| Pin Number | Name | Function         | Notes       |
| ---------- | ---- | ---------------- | ----------- |
| 17         | PB2  | BOOT2            | Pulldown    |
| 32         |      | IMU INT 1        | N/A         |
| 30         |      | IMU INT 2        | N/A         |
| 14         | PA7  | Handle Temp      | ADC Input 7 |
| 15         | PB0  | Tip Temp         | ADC Input 8 |
| 13         | PA6  | B Button         | Active Low  |
| 21         | PA9  | A Button         | Active Low  |
| 23         | PA11 | USB D-           | -           |
| 24         | PA12 | USB D+           | -           |
| 31         | PB4  | Tip PWM Out      | TIMER2_CH0  |
| 16         | PB1  | Input DC V Sense | ADC Input 9 |
| 20         | PA8  | OLED Reset       |             |
| 34         | PB7  | SDA              | I2C0_SDA    |
| 33         | PB6  | SCL              | I2C0_SCL    |

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
