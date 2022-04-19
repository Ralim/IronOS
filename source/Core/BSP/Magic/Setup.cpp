/*
 * Setup.c
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Setup.h"
#include "BSP.h"
#include "Debug.h"
#include "FreeRTOSConfig.h"
#include "Pins.h"

#include "IRQ.h"
#include "history.hpp"
#include <string.h>
#define ADC_NORM_SAMPLES 16
#define ADC_FILTER_LEN   4
uint16_t ADCReadings[ADC_NORM_SAMPLES]; // room for 32 lots of the pair of readings

// Functions

void setup_timer_scheduler(void);
void setup_pwm(void);
void setup_adc(void);
void hardware_init() {
  gpio_set_mode(OLED_RESET_Pin, GPIO_OUTPUT_MODE);
  gpio_set_mode(KEY_A_Pin, GPIO_INPUT_PD_MODE);
  gpio_set_mode(KEY_B_Pin, GPIO_INPUT_PD_MODE);

  setup_timer_scheduler();
  setup_adc();
  setup_pwm();
  I2C_ClockSet(I2C0_ID, 400000); // Sets clock to around 375kHz
}
void setup_pwm(void) {
  // Setup PWM we use for driving the tip
  PWM_CH_CFG_Type cfg = {
      PWM_Channel,       // channel
      PWM_CLK_XCLK,      // Clock
      PWM_STOP_GRACEFUL, // Stop mode
      PWM_POL_NORMAL,    // Normal Polarity
      50,                // Clock Div
      100,               // Period
      0,                 // Thres 1 - start at beginng
      50,                // Thres 2 - turn off at 50%
      0,                 // Interrupt pulse count
  };

  BL_Err_Type err     = PWM_Channel_Init(&cfg);
  uint32_t    pwm_clk = peripheral_clock_get(PERIPHERAL_CLOCK_PWM);
  MSG((char *)"PWM Setup returns %d %d\r\n", err, pwm_clk);
  PWM_Channel_Disable(PWM_Channel);
}
void setup_adc(void) {
  MSG((char *)"Setting up ADC\r\n");
  //
  ADC_CFG_Type      adc_cfg      = {};
  ADC_FIFO_Cfg_Type adc_fifo_cfg = {};

  CPU_Interrupt_Disable(GPADC_DMA_IRQn);

  ADC_IntMask(ADC_INT_ALL, MASK);

  adc_cfg.clkDiv         = ADC_CLK_DIV_32;
  adc_cfg.vref           = ADC_VREF_3P2V;
  adc_cfg.resWidth       = ADC_DATA_WIDTH_16_WITH_256_AVERAGE;
  adc_cfg.inputMode      = ADC_INPUT_SINGLE_END;
  adc_cfg.v18Sel         = ADC_V18_SEL_1P82V;
  adc_cfg.v11Sel         = ADC_V11_SEL_1P1V;
  adc_cfg.gain1          = ADC_PGA_GAIN_NONE;
  adc_cfg.gain2          = ADC_PGA_GAIN_NONE;
  adc_cfg.chopMode       = ADC_CHOP_MOD_AZ_PGA_ON;
  adc_cfg.biasSel        = ADC_BIAS_SEL_MAIN_BANDGAP;
  adc_cfg.vcm            = ADC_PGA_VCM_1V;
  adc_cfg.offsetCalibEn  = DISABLE;
  adc_cfg.offsetCalibVal = 0;

  ADC_Disable();
  ADC_Enable();
  ADC_Reset();
  ADC_Init(&adc_cfg);
  adc_fifo_cfg.dmaEn         = DISABLE;
  adc_fifo_cfg.fifoThreshold = ADC_FIFO_THRESHOLD_16;
  ADC_FIFO_Cfg(&adc_fifo_cfg);
  // Enable FiFo IRQ
  MSG((char *)"Int Enable\r\n");
  Interrupt_Handler_Register(GPADC_DMA_IRQn, adc_fifo_irq);
  ADC_IntMask(ADC_INT_FIFO_READY, UNMASK);
  CPU_Interrupt_Enable(GPADC_DMA_IRQn);
  MSG((char *)"Start\r\n");
  start_adc_misc();
  MSG((char *)"Started\r\n");
}

struct device *timer0;

void setup_timer_scheduler() {

  timer_register(TIMER0_INDEX, "timer0");

  timer0 = device_find("timer0");

  if (timer0) {

    device_open(timer0, DEVICE_OFLAG_INT_TX); /* 1s,2s,3s timing*/
    // Set interrupt handler
    device_set_callback(timer0, timer0_irq_callback);
    // Enable both interrupts (0 and 1)
    device_control(timer0, DEVICE_CTRL_SET_INT, (void *)(TIMER_COMP0_IT | TIMER_COMP1_IT | TIMER_COMP2_IT));

    TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_0, 255 + 10 - 2); // Channel 0 is used to trigger the ADC
    TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_1, 128 - 2);
    TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_2, (255 + 10 + 10) - 2); // We are using compare 2 to set the max duration of the timer
    TIMER_SetPreloadValue(TIMER_CH0, 0);
    TIMER_SetCountMode(TIMER_CH0, TIMER_COUNT_PRELOAD);
  } else {
    MSG((char *)"timer device open failed! \n");
  }
}

void setupFUSBIRQ() {

  gpio_set_mode(FUSB302_IRQ_Pin, GPIO_SYNC_FALLING_TRIGER_INT_MODE);
  CPU_Interrupt_Disable(GPIO_INT0_IRQn);
  Interrupt_Handler_Register(GPIO_INT0_IRQn, GPIO_IRQHandler);
  CPU_Interrupt_Enable(GPIO_INT0_IRQn);
  gpio_irq_enable(FUSB302_IRQ_Pin, ENABLE);
}

void vAssertCalled(void) {
  MSG((char *)"vAssertCalled\r\n");

  while (1)
    ;
}
