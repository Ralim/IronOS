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
#include "IRQ.h"
#include "Pins.h"
#include "bl702_sec_eng.h"
#include "history.hpp"
#include <string.h>
#define ADC_NORM_SAMPLES 16
#define ADC_FILTER_LEN   4
uint16_t ADCReadings[ADC_NORM_SAMPLES]; // room for 32 lots of the pair of readings

// Heap

extern uint8_t      _heap_start;
extern uint8_t      _heap_size; // @suppress("Type cannot be resolved")
static HeapRegion_t xHeapRegions[] = {
    {&_heap_start, (unsigned int)&_heap_size},
    {NULL, 0}, /* Terminates the array. */
    {NULL, 0}  /* Terminates the array. */
};

// Functions

void setup_timer_scheduler(void);
void setup_pwm(void);
void setup_adc(void);
void hardware_init() {

  vPortDefineHeapRegions(xHeapRegions);
  HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);

  // Set capcode
  {
    uint32_t tmpVal = 0;
    tmpVal          = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal          = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_IN_AON, 33);
    tmpVal          = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_OUT_AON, 33);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);
  }

  Sec_Eng_Trng_Enable();

  gpio_set_mode(OLED_RESET_Pin, GPIO_OUTPUT_MODE);
  gpio_set_mode(KEY_A_Pin, GPIO_INPUT_PD_MODE);
  gpio_set_mode(KEY_B_Pin, GPIO_INPUT_PD_MODE);
  gpio_set_mode(TMP36_INPUT_Pin, GPIO_INPUT_MODE);
  gpio_set_mode(TIP_TEMP_Pin, GPIO_INPUT_MODE);
  gpio_set_mode(VIN_Pin, GPIO_INPUT_MODE);

  gpio_set_mode(TIP_RESISTANCE_SENSE, GPIO_OUTPUT_MODE);
  gpio_write(TIP_RESISTANCE_SENSE, 0);

  MSG((char *)"Pine64 Pinecilv2 Starting\r\n");
  setup_timer_scheduler();
  setup_adc();
  setup_pwm();
  I2C_SetSclSync(I2C0_ID, 1);
  I2C_SetDeglitchCount(I2C0_ID, 1); // Turn on de-glitch
  // Note on I2C clock rate @ 100Khz the screen update == 20ms which is too long for USB-PD to work
  // 200kHz and above works
  I2C_ClockSet(I2C0_ID, 300000); // Sets clock to around 25 kHz less than set here
  TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_1, 0);
}
void setup_pwm(void) {
  // Setup PWM we use for driving the tip
  PWM_CH_CFG_Type cfg = {
      PWM_Channel,     // channel
      PWM_CLK_XCLK,    // Clock
      PWM_STOP_ABRUPT, // Stop mode
      PWM_POL_NORMAL,  // Normal Polarity
      60,              // Clock Div
      100,             // Period
      0,               // Thres 1 - start at beginng
      50,              // Thres 2 - turn off at 50%
      0,               // Interrupt pulse count
  };

  PWM_Channel_Init(&cfg);
  PWM_Channel_Disable(PWM_Channel);
}

const ADC_Chan_Type adc_tip_pos_chans[]
    = {TIP_TEMP_ADC_CHANNEL, TMP36_ADC_CHANNEL, TIP_TEMP_ADC_CHANNEL, VIN_ADC_CHANNEL, TIP_TEMP_ADC_CHANNEL, TMP36_ADC_CHANNEL, TIP_TEMP_ADC_CHANNEL, VIN_ADC_CHANNEL};
const ADC_Chan_Type adc_tip_neg_chans[] = {ADC_CHAN_GND, ADC_CHAN_GND, ADC_CHAN_GND, ADC_CHAN_GND, ADC_CHAN_GND, ADC_CHAN_GND, ADC_CHAN_GND, ADC_CHAN_GND};
static_assert(sizeof(adc_tip_pos_chans) == sizeof(adc_tip_neg_chans));

void setup_adc(void) {
  //
  ADC_CFG_Type      adc_cfg      = {};
  ADC_FIFO_Cfg_Type adc_fifo_cfg = {};

  CPU_Interrupt_Disable(GPADC_DMA_IRQn);

  ADC_IntMask(ADC_INT_ALL, MASK);

  adc_cfg.clkDiv         = ADC_CLK_DIV_4;
  adc_cfg.vref           = ADC_VREF_3P2V;
  adc_cfg.resWidth       = ADC_DATA_WIDTH_14_WITH_64_AVERAGE;
  adc_cfg.inputMode      = ADC_INPUT_SINGLE_END;
  adc_cfg.v18Sel         = ADC_V18_SEL_1P72V;
  adc_cfg.v11Sel         = ADC_V11_SEL_1P1V;
  adc_cfg.gain1          = ADC_PGA_GAIN_NONE;
  adc_cfg.gain2          = ADC_PGA_GAIN_NONE;
  adc_cfg.chopMode       = ADC_CHOP_MOD_AZ_ON;
  adc_cfg.biasSel        = ADC_BIAS_SEL_MAIN_BANDGAP;
  adc_cfg.vcm            = ADC_PGA_VCM_1P6V;
  adc_cfg.offsetCalibEn  = ENABLE;
  adc_cfg.offsetCalibVal = 0;

  ADC_Disable();
  ADC_Enable();
  ADC_Reset();

  ADC_Init(&adc_cfg);
  adc_fifo_cfg.dmaEn         = DISABLE;
  adc_fifo_cfg.fifoThreshold = ADC_FIFO_THRESHOLD_8;
  ADC_FIFO_Cfg(&adc_fifo_cfg);
  ADC_MIC_Bias_Disable();
  ADC_Tsen_Disable();

  // Enable FiFo IRQ
  Interrupt_Handler_Register(GPADC_DMA_IRQn, adc_fifo_irq);
  ADC_IntMask(ADC_INT_FIFO_READY, UNMASK);
  CPU_Interrupt_Enable(GPADC_DMA_IRQn);
  ADC_Stop();
  ADC_FIFO_Clear();
  ADC_Scan_Channel_Config(adc_tip_pos_chans, adc_tip_neg_chans, sizeof(adc_tip_pos_chans) / sizeof(ADC_Chan_Type), DISABLE);
}

void setup_timer_scheduler() {
  TIMER_Disable(TIMER_CH0);

  TIMER_CFG_Type cfg = {
      TIMER_CH0,                           // Channel
      TIMER_CLKSRC_32K,                    // Clock source
      TIMER_PRELOAD_TRIG_COMP0,            // Trigger; reset after trigger 0
      TIMER_COUNT_PRELOAD,                 // Counter mode
      22,                                  // Clock div
      (uint16_t)(powerPWM + holdoffTicks), // CH0 compare (adc)
      0,                                   // CH1 compare (pwm out)
      0,                                   // CH2 compare not used
      0,                                   // Preload
  };
  TIMER_Init(&cfg);

  Timer_Int_Callback_Install(TIMER_CH0, TIMER_INT_COMP_0, timer0_comp0_callback);
  Timer_Int_Callback_Install(TIMER_CH0, TIMER_INT_COMP_1, timer0_comp1_callback);

  TIMER_ClearIntStatus(TIMER_CH0, TIMER_COMP_ID_0);
  TIMER_ClearIntStatus(TIMER_CH0, TIMER_COMP_ID_1);

  TIMER_IntMask(TIMER_CH0, TIMER_INT_COMP_0, UNMASK);
  TIMER_IntMask(TIMER_CH0, TIMER_INT_COMP_1, UNMASK);
  CPU_Interrupt_Enable(TIMER_CH0_IRQn);
  TIMER_Enable(TIMER_CH0);
}

void setupFUSBIRQ() {

  gpio_set_mode(FUSB302_IRQ_Pin, GPIO_SYNC_FALLING_TRIGER_INT_MODE);
  CPU_Interrupt_Disable(GPIO_INT0_IRQn);
  Interrupt_Handler_Register(GPIO_INT0_IRQn, GPIO_IRQHandler);
  CPU_Interrupt_Enable(GPIO_INT0_IRQn);
  gpio_irq_enable(FUSB302_IRQ_Pin, ENABLE);
}
