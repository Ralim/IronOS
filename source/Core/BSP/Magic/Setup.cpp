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

void setup_slow_PWM();
void setup_adc(void);
void hardware_init() {
  gpio_set_mode(OLED_RESET_Pin, GPIO_OUTPUT_MODE);
  gpio_set_mode(KEY_A_Pin, GPIO_INPUT_PD_MODE);
  gpio_set_mode(KEY_B_Pin, GPIO_INPUT_PD_MODE);
  setup_slow_PWM();
  setup_adc();
  I2C_ClockSet(I2C0_ID, 400000);
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

void setup_slow_PWM() {

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
  return; // TODO

  MSG((char *)"Setting up FUSB IRQ\r\n");
  gpio_set_mode(FUSB302_IRQ_Pin, GPIO_SYNC_FALLING_TRIGER_INT_MODE);
  MSG((char *)"Setting up FUSB IRQ1r\n");
  CPU_Interrupt_Disable(GPIO_INT0_IRQn);
  MSG((char *)"Setting up FUSB IRQ2\r\n");
  Interrupt_Handler_Register(GPIO_INT0_IRQn, GPIO_IRQHandler);
  MSG((char *)"Setting up FUSB IRQ3\r\n");
  CPU_Interrupt_Enable(GPIO_INT0_IRQn);

  MSG((char *)"Setting up FUSB IRQ4\r\n");
  gpio_irq_enable(FUSB302_IRQ_Pin, ENABLE);
}

void vAssertCalled(void) {
  MSG((char *)"vAssertCalled\r\n");

  while (1)
    ;
}
